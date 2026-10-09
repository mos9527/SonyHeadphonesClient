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
    fontController?.abort();
}

async function download(url, id, signal) {
    const resource = resources[id];
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

function selectFont() {
    const locale = (navigator.language || 'zh-CN').toLowerCase();
    const language = locale.split('-')[0];
    let region = 'sc';
    if (language === 'ja') region = 'jp';
    else if (language === 'ko') region = 'kr';
    else if (language === 'zh' && /(?:^|-)(?:hant|tw|hk|mo)(?:-|$)/.test(locale)) region = 'tc';
    return new URL(globalThis.SonyHeadphonesClientConfig.fonts[region], appBase);
}

async function loadFont() {
    const url = selectFont();
    navigator.externalFont = url.href;
    navigator.externalFontManaged = true;
    fontController = new AbortController();
    elements['skip-font'].hidden = false;
    const timeout = setTimeout(() => fontController.abort(), 20000);
    try {
        let cache;
        try {
            cache = await caches.open(fontCacheName);
            const cached = await cache.match(url.href);
            if (cached) {
                const buffer = new Uint8Array(await cached.arrayBuffer());
                if (buffer.byteLength) {
                    navigator.externalFontData = buffer;
                    resources.font.loaded = resources.font.total = buffer.byteLength;
                    resources.font.state = 'done';
                    fontSaved = true;
                    renderProgress();
                    return;
                }
            }
        } catch (error) {
            console.warn('Font cache unavailable', error);
        }
        const buffer = await download(url, 'font', fontController.signal);
        navigator.externalFontData = buffer;
        if (cache) {
            try {
                await cache.put(url.href, new Response(buffer, { headers: { 'Content-Type': 'font/otf' } }));
                fontSaved = true;
            } catch (error) {
                console.warn('Unable to save language font for offline use', error);
            }
        }
    } catch (error) {
        resources.font.state = 'skipped';
        renderProgress();
        console.warn('Continuing with the built-in font', error);
    } finally {
        clearTimeout(timeout);
        elements['skip-font'].hidden = true;
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
elements['skip-font'].addEventListener('click', () => fontController?.abort());
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

var Module = {
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
        loadFont()
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
