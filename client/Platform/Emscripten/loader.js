'use strict';

const appBase = new URL('./', location.href);
const fontCacheName = `SonyHeadphonesClient-fonts:${appBase.pathname}`;
const elements = Object.fromEntries([
    'preload', 'canvas', 'status-text', 'progress-bar',
    'retry', 'skip-font', 'pwa-notice', 'pwa-text', 'pwa-retry', 'update', 'dismiss-notice'
].map(id => [id, document.getElementById(id)]));
const resources = Object.fromEntries(['js', 'wasm', 'font'].map(id => [id, {
    loaded: 0, total: 0, state: 'waiting'
}]));
let failed = false;
let runtimeReady = false;
let registration;
let updateRequested = false;
let fontSaved = false;
let fontController;
let noticeTimer;

function renderProgress() {
    const required = [resources.js, resources.wasm];
    if (startupAppLocale > 0 && resources.font.state !== 'skipped') required.push(resources.font);
    const loaded = required.reduce((sum, resource) => sum + resource.loaded, 0);
    const known = required.every(resource => resource.total > 0);
    const total = required.reduce((sum, resource) => sum + resource.total, 0);
    if (required.every(resource => resource.state === 'done')) {
        elements['progress-bar'].value = 1;
    } else if (known) {
        elements['progress-bar'].value = Math.min(loaded / total, 1);
    } else {
        elements['progress-bar'].removeAttribute('value');
    }
}

function setStatus(text) {
    if (!failed && text) elements['status-text'].textContent = text;
}

function fail(error) {
    if (failed) return;
    failed = true;
    console.error('Application startup failed', error);
    elements['canvas'].hidden = true;
    elements['preload'].hidden = false;
    elements['status-text'].textContent = `Unable to start: ${error.message || error} ${navigator.onLine ? 'Please retry. If this continues, check that the deployment contains all app files.' : 'You are offline. Connect once to finish saving the app, then retry.'}`;
    elements['retry'].hidden = false;
    elements['skip-font'].hidden = true;
    fontController?.abort(new DOMException('Font loading cancelled because application startup failed', 'AbortError'));
}

async function download(url, id, signal, resource = resources[id]) {
    resource.state = 'loading';
    renderProgress();
    try {
        const response = await fetch(url, { signal });
        if (!response.ok) throw new Error(`${id === 'font' ? 'Language font' : url.pathname.split('/').pop()}: HTTP ${response.status}`);
        const encoding = response.headers.get('content-encoding');
        resource.total = !encoding || encoding === 'identity' ? Number(response.headers.get('content-length')) || 0 : 0;
        renderProgress();
        let buffer;
        if (response.body) {
            const reader = response.body.getReader();
            const chunks = [];
            while (true) {
                const { done, value } = await reader.read();
                if (done) break;
                chunks.push(value);
                resource.loaded += value.byteLength;
                if (resource.total && resource.loaded > resource.total) resource.total = 0;
                renderProgress();
            }
            buffer = new Uint8Array(resource.loaded);
            let offset = 0;
            for (const chunk of chunks) {
                buffer.set(chunk, offset);
                offset += chunk.byteLength;
            }
        } else {
            buffer = new Uint8Array(await response.arrayBuffer());
            resource.loaded = buffer.byteLength;
        }
        if (!buffer.byteLength) throw new Error(`Empty ${id} download`);
        resource.total = resource.loaded;
        resource.state = 'done';
        renderProgress();
        return buffer;
    } catch (error) {
        resource.state = id === 'font' ? 'skipped' : 'error';
        renderProgress();
        throw error;
    }
}

const appLocales = Object.freeze(['default', 'sc', 'tc', 'jp', 'kr']);
function getPreferredAppLocale() {
    const languages = navigator.languages?.length ? navigator.languages : [navigator.language];
    for (const tag of languages) {
        const locale = (tag || '').toLowerCase().replace(/_/g, '-');
        const language = locale.split('-')[0];
        if (language === 'zh') {
            if (/(?:^|-)hans(?:-|$)/.test(locale)) return 1;
            return /(?:^|-)(?:hant|tw|hk|mo)(?:-|$)/.test(locale) ? 2 : 1;
        }
        if (language === 'ja') return 3;
        if (language === 'ko') return 4;
    }
    return 0;
}
const configuredLocale = new URL(location.href).searchParams.get('locale') ||
    globalThis.SonyHeadphonesClientConfig.locale;
const startupAppLocale = appLocales.includes(configuredLocale) ?
    appLocales.indexOf(configuredLocale) : getPreferredAppLocale();

const platformFonts = globalThis.SonyHeadphonesClientFonts = {
    entries: new Map(),
    destroyed: false,
    load(locale) {
        if (this.destroyed || !Number.isInteger(locale) || locale <= 0 || locale >= appLocales.length)
            return Promise.resolve();
        const previous = this.entries.get(locale);
        if (previous) return previous.promise;
        const entry = {
            state: 'loading', data: null, ptr: 0, size: 0,
            controller: new AbortController(), promise: null
        };
        this.entries.set(locale, entry);
        entry.promise = loadFont(locale, entry);
        return entry.promise;
    }
};

async function loadFont(locale, entry) {
    const startup = locale === startupAppLocale && !runtimeReady;
    const resource = startup ? resources.font : { loaded: 0, total: 0, state: 'waiting' };
    const controller = entry.controller;
    const signal = controller.signal;
    if (startup) {
        fontController = controller;
        elements['skip-font'].hidden = false;
    }
    const configuredTimeout = globalThis.SonyHeadphonesClientConfig.fontTimeoutMs;
    const timeoutMs = Number.isInteger(configuredTimeout) && configuredTimeout > 0 && configuredTimeout <= 0x7fffffff ?
        configuredTimeout : 20000;
    const timeout = setTimeout(() => controller.abort(
        new DOMException(`Font loading timed out after ${timeoutMs / 1000} seconds`, 'TimeoutError')
    ), timeoutMs);
    let onAbort;
    const aborted = new Promise((resolve, reject) => {
        onAbort = () => reject(signal.reason);
        if (signal.aborted) onAbort();
        else signal.addEventListener('abort', onAbort, { once: true });
    });
    try {
        const region = appLocales[locale];
        const url = new URL(globalThis.SonyHeadphonesClientConfig.fonts[region], appBase);
        const buffer = await Promise.race([aborted, (async () => {
            let cache;
            try {
                cache = await caches.open(fontCacheName);
                const cached = await cache.match(url.href);
                if (cached) {
                    const data = new Uint8Array(await cached.arrayBuffer());
                    signal.throwIfAborted();
                    if (data.byteLength) {
                        if (startup) fontSaved = true;
                        return data;
                    }
                }
            } catch (error) {
                signal.throwIfAborted();
                console.warn('Font cache unavailable', error);
            }
            signal.throwIfAborted();
            const data = await download(url, 'font', signal, resource);
            signal.throwIfAborted();
            if (cache) {
                void cache.put(url.href, new Response(data, { headers: { 'Content-Type': 'font/otf' } }))
                    .then(() => { if (startup) fontSaved = true; })
                    .catch(error => console.warn('Unable to save language font for offline use', error));
            }
            return data;
        })()]);
        signal.throwIfAborted();
        if (!buffer.byteLength || buffer.byteLength > 0x7fffffff)
            throw new Error('Invalid font size');
        entry.data = buffer;
        entry.state = 'ready';
        resource.loaded = resource.total = buffer.byteLength;
        resource.state = 'done';
    } catch (error) {
        entry.state = 'unavailable';
        resource.state = 'skipped';
        const cause = signal.aborted ? signal.reason : error;
        const reason = cause?.message || String(cause);
        const intentional = signal.aborted && cause?.name === 'AbortError';
        const message = `Continuing with the built-in font (${appLocales[locale]}): ${reason}`;
        if (intentional) console.info(message, cause);
        else console.warn(message, cause);
        if (!intentional && !failed && !platformFonts.destroyed) {
            showNotice(`Language font (${appLocales[locale]}) could not be loaded: ${reason}. Continuing with the built-in font; some characters may be missing. Reload to retry.`);
        }
    } finally {
        clearTimeout(timeout);
        signal.removeEventListener('abort', onAbort);
        if (startup) elements['skip-font'].hidden = true;
        renderProgress();
    }
}

function showNotice(text, retry = false, update = false) {
    clearTimeout(noticeTimer);
    elements['pwa-text'].textContent = text;
    elements['pwa-notice'].hidden = false;
    elements['pwa-retry'].hidden = !retry;
    elements.update.hidden = !update;
    if (!retry && !update && runtimeReady) noticeTimer = setTimeout(() => { elements['pwa-notice'].hidden = true; }, 10000);
}

function checkOfflineReady() {
    if (registration?.active) registration.active.postMessage({ type: 'CHECK_OFFLINE' });
}

function watchWorker(worker) {
    if (!worker) return;
    const changed = () => {
        if (worker.state === 'installed' && navigator.serviceWorker.controller) {
            showNotice('An update is ready. Reload when you are ready to reconnect your headphones.', false, true);
        } else if (worker.state === 'activated') {
            checkOfflineReady();
        } else if (worker.state === 'redundant') {
            showNotice('Offline setup did not finish. Keep this page online and retry.', true);
        }
    };
    worker.addEventListener('statechange', changed);
    changed();
}

async function setupOffline() {
    if (!('serviceWorker' in navigator) || !window.isSecureContext) {
        showNotice('Offline installation requires HTTPS (or localhost) and a browser with Service Worker support.');
        return;
    }
    try {
        registration = await navigator.serviceWorker.register(new URL('sw.js', appBase), { scope: appBase.href, updateViaCache: 'none' });
        registration.addEventListener('updatefound', () => watchWorker(registration.installing));
        watchWorker(registration.installing);
        if (registration.waiting) {
            showNotice('An update is ready. Reload when you are ready to reconnect your headphones.', false, true);
        } else if (registration.active && !registration.installing) {
            checkOfflineReady();
        }
    } catch (error) {
        console.warn('Offline setup failed', error);
        showNotice('The app can run, but offline setup failed. Check your connection and retry.', true);
    }
}

if ('serviceWorker' in navigator) {
    navigator.serviceWorker.addEventListener('message', event => {
        if (event.source !== registration?.active) return;
        if (event.data?.type === 'OFFLINE_STATUS' && !registration.waiting && !registration.installing && !event.data.ready) {
            showNotice('Offline files are incomplete. Reconnect and retry offline setup.');
        }
    });
    navigator.serviceWorker.addEventListener('controllerchange', () => {
        if (updateRequested) location.reload();
        else checkOfflineReady();
    });
}

elements.retry.addEventListener('click', () => location.reload());
elements['skip-font'].addEventListener('click', () => fontController?.abort(
    new DOMException('Language font skipped by the user', 'AbortError')
));
elements['dismiss-notice'].addEventListener('click', () => { elements['pwa-notice'].hidden = true; });
elements['pwa-retry'].addEventListener('click', async () => {
    if (!registration) return setupOffline();
    try {
        await registration.update();
        if (registration.active && !registration.installing && !registration.waiting) {
            registration.active.postMessage({ type: 'REPAIR_OFFLINE' });
        } else if (registration.waiting) {
            showNotice('An update is ready. Reload when you are ready to reconnect your headphones.', false, true);
        }
    } catch (error) {
        console.warn('Offline retry failed', error);
        showNotice('Unable to save offline files. Reconnect and retry.', true);
    }
});
elements.update.addEventListener('click', () => {
    if (!registration?.waiting) return;
    updateRequested = true;
    registration.waiting.postMessage({ type: 'ACTIVATE_UPDATE' });
});
function isBrowserShortcut(event) {
    if (/^F(?:[1-9]|1[0-2])$/.test(event.key)) return true;
    if (!(event.ctrlKey || event.metaKey)) return false;
    if (['+', '-', '=', '0'].includes(event.key)) return true;
    return event.shiftKey && ['i', 'j', 'c'].includes(event.key.toLowerCase());
}
for (const type of ['keydown', 'keyup', 'keypress']) {
    window.addEventListener(type, event => {
        if (isBrowserShortcut(event)) event.stopImmediatePropagation();
    }, { capture: true });
}
window.addEventListener('wheel', event => {
    if (event.ctrlKey || event.metaKey) event.stopImmediatePropagation();
}, { capture: true, passive: true });
elements.canvas.addEventListener('contextmenu', event => event.preventDefault());

let canvasMetrics = '';
let resizeScheduled = false;
function syncCanvasSize() {
    if (!runtimeReady || failed || resizeScheduled) return;
    resizeScheduled = true;
    requestAnimationFrame(() => {
        resizeScheduled = false;
        const rect = elements.canvas.getBoundingClientRect();
        if (rect.width <= 0 || rect.height <= 0) return;
        const metrics = `${rect.width}:${rect.height}:${window.devicePixelRatio || 1}`;
        if (metrics === canvasMetrics) return;
        canvasMetrics = metrics;
        window.dispatchEvent(new Event('resize'));
    });
}
window.addEventListener('resize', syncCanvasSize);
window.visualViewport?.addEventListener('resize', syncCanvasSize);
if (window.ResizeObserver) new ResizeObserver(syncCanvasSize).observe(elements.canvas);
function watchPixelRatio() {
    if (!window.matchMedia) return;
    const query = window.matchMedia(`(resolution: ${window.devicePixelRatio || 1}dppx)`);
    query.addEventListener('change', () => {
        syncCanvasSize();
        watchPixelRatio();
    }, { once: true });
}
watchPixelRatio();

window.addEventListener('error', event => {
    if (!runtimeReady) fail(event.error || new Error(event.message || 'Application script failed to load'));
});
window.addEventListener('unhandledrejection', event => {
    if (!runtimeReady) fail(event.reason || new Error('Application startup failed'));
});

function installDebuggerFolderDrop() {
    if (typeof Module._clientPlatformDropDirectory !== 'function') return;
    let importing = false;
    const fs = Module.FS;
    const path = '/tmp/mdr-debugger-import';

    async function collectPackets(entry, packets) {
        if (entry.isDirectory) {
            const reader = entry.createReader();
            while (true) {
                const batch = await new Promise((resolve, reject) => reader.readEntries(resolve, reject));
                if (!batch.length) break;
                for (const child of batch) await collectPackets(child, packets);
            }
        } else if (entry.isFile && entry.name.startsWith('mdr-packet-') &&
                   entry.name.endsWith('.bin') && /-(?:rx|tx)\./.test(entry.name)) {
            packets.push(entry);
        }
    }

    elements.canvas.addEventListener('drop', event => {
        const entries = Array.from(event.dataTransfer?.items || [])
            .filter(item => item.kind === 'file').map(item => item.webkitGetAsEntry?.());
        if (!entries.some(entry => entry?.isDirectory)) return;
        event.preventDefault();
        event.stopImmediatePropagation();
        if (importing || Module._clientPlatformDropPending() || entries.length !== 1) {
            showNotice('Drop one capture folder at a time, and wait for the current import to finish.');
            return;
        }
        importing = true;
        void (async () => {
            fs.mkdirTree(path);
            for (const name of fs.readdir(path)) {
                if (name !== '.' && name !== '..') fs.unlink(`${path}/${name}`);
            }
            showNotice('Reading capture folder…');
            const packets = [];
            await collectPackets(entries[0], packets);
            const names = new Set();
            for (const packet of packets) {
                if (names.has(packet.name)) throw new Error(`Duplicate packet filename: ${packet.name}`);
                names.add(packet.name);
            }
            for (let offset = 0; offset < packets.length; offset += 32) {
                showNotice(`Importing packets: ${offset} / ${packets.length}…`);
                const results = await Promise.allSettled(packets.slice(offset, offset + 32).map(async entry => {
                    const file = await new Promise((resolve, reject) => entry.file(resolve, reject));
                    fs.writeFile(`${path}/${entry.name}`, new Uint8Array(await file.arrayBuffer()));
                }));
                const failure = results.find(result => result.status === 'rejected');
                if (failure) throw failure.reason;
            }
            if (!packets.length) throw new Error('No capture packets found in the dropped folder.');
            const error = Module.ccall('clientPlatformDropDirectory', 'string', ['string'], [path]);
            if (error) throw new Error(error);
            showNotice(`Queued ${packets.length} packet(s) for replay.`);
        })().catch(error => showNotice(`Unable to import capture: ${error.message || error}`))
            .finally(() => { importing = false; });
    }, { capture: true });
}

var Module = {
    arguments: ['--locale', appLocales[startupAppLocale]],
    canvas: elements.canvas,
    locateFile: path => new URL(path, appBase).href,
    print: (...args) => console.log(...args),
    printErr: (...args) => console.error(...args),
    setStatus,
    monitorRunDependencies: count => { if (count) setStatus(`Preparing runtime (${count} tasks remaining)`); },
    onAbort: reason => fail(new Error(`Runtime aborted: ${reason}`)),
    onExit: status => { if (status !== 0) fail(new Error(`Application exited with status ${status}`)); },
    onRuntimeInitialized: () => {
        if (failed) return;
        elements.canvas.hidden = false;
        setStatus('All done. Going home.');
        requestAnimationFrame(() => requestAnimationFrame(() => {
            if (failed) return;
            runtimeReady = true;
            elements.canvas.hidden = false;
            elements.preload.hidden = true;
            elements.canvas.focus({ preventScroll: true });
            installDebuggerFolderDrop();
            syncCanvasSize();
            void setupOffline();
        }));
    }
};

async function start() {
    if (!window.WebAssembly) throw new Error('This browser does not support WebAssembly');
    setStatus('Downloading Files');
    const [javascript, wasm] = await Promise.all([
        download(new URL('SonyHeadphonesClient.js', appBase), 'js'),
        download(new URL('SonyHeadphonesClient.wasm', appBase), 'wasm'),
        platformFonts.load(startupAppLocale)
    ]);
    if (failed) return;
    setStatus('Compiling WebAssembly');
    Module.wasmBinary = wasm;
    const url = URL.createObjectURL(new Blob([javascript], { type: 'text/javascript' }));
    const script = document.createElement('script');
    script.src = url;
    script.onload = () => URL.revokeObjectURL(url);
    script.onerror = () => {
        URL.revokeObjectURL(url);
        fail(new Error('Unable to execute the application script'));
    };
    document.body.appendChild(script);
}

renderProgress();
void start().catch(fail);
