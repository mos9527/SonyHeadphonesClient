'use strict';

importScripts('config.js');

const CACHE_VERSION = globalThis.SonyHeadphonesClientConfig.commit;
const FILES_TO_CACHE = [
    'index.html',
    'config.js',
    'loader.js',
    'manifest.json',
    'icon-192.png',
    'icon-512.png',
    'SonyHeadphonesClient.js',
    'SonyHeadphonesClient.wasm'
];
const APP_BASE = new URL('./', self.location.href);
const CACHE_PREFIX = `SonyHeadphonesClient-assets:${APP_BASE.pathname}:`;
const CACHE_NAME = `${CACHE_PREFIX}${CACHE_VERSION}`;
const ASSETS = FILES_TO_CACHE.map(path => new URL(path, APP_BASE).href);
const INDEX_URL = new URL('index.html', APP_BASE).href;

async function precache() {
    const cache = await caches.open(CACHE_NAME);
    await cache.addAll(ASSETS.map(url => new Request(url, { cache: 'reload' })));
}

async function offlineReady() {
    if (!ASSETS.length) return false;
    const cache = await caches.open(CACHE_NAME);
    const responses = await Promise.all(ASSETS.map(url => cache.match(url)));
    return responses.every(Boolean);
}

self.addEventListener('install', event => {
    event.waitUntil(precache());
});

self.addEventListener('activate', event => {
    event.waitUntil((async () => {
        const keys = await caches.keys();
        await Promise.all(keys.filter(key => key.startsWith(CACHE_PREFIX) && key !== CACHE_NAME).map(key => caches.delete(key)));
        await self.clients.claim();
    })());
});

self.addEventListener('message', event => {
    if (event.data?.type === 'ACTIVATE_UPDATE') {
        event.waitUntil(self.skipWaiting());
    } else if (event.data?.type === 'CHECK_OFFLINE' || event.data?.type === 'REPAIR_OFFLINE') {
        event.waitUntil((async () => {
            try {
                if (event.data.type === 'REPAIR_OFFLINE') await precache();
                event.source?.postMessage({ type: 'OFFLINE_STATUS', ready: await offlineReady() });
            } catch (error) {
                console.warn('Offline cache unavailable', error);
                event.source?.postMessage({ type: 'OFFLINE_STATUS', ready: false });
            }
        })());
    }
});

self.addEventListener('fetch', event => {
    const url = new URL(event.request.url);
    if (event.request.method !== 'GET' || url.origin !== APP_BASE.origin || !url.pathname.startsWith(APP_BASE.pathname)) return;
    const navigation = event.request.mode === 'navigate';
    if (navigation && url.pathname !== APP_BASE.pathname && url.pathname !== new URL(INDEX_URL).pathname) return;
    url.search = '';
    url.hash = '';
    if (!navigation && !ASSETS.includes(url.href)) return;
    event.respondWith((async () => {
        const cache = await caches.open(CACHE_NAME);
        const cached = await cache.match(navigation ? INDEX_URL : url.href);
        return cached || fetch(event.request);
    })());
});
