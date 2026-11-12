#include <mdr-bt/ConnectionEmscripten.h>

#include <emscripten.h>

#include "mdr/Protocol.hpp"

// JS state is owned by a backend instance, and each connection attempt gets its own
// session. Async callbacks retain only that session and cannot publish into a later
// connection (or call back into WASM after the instance has been destroyed).
// clang-format off
EM_JS(void, em_js_init, (void* user), {
    const connections = Module.mdrSerialConnections || (Module.mdrSerialConnections = new Map());
    // A destroyed/recreated backend must also wait before reopening the same port.
    const portClosures = Module.mdrSerialPortClosures || (Module.mdrSerialPortClosures = new WeakMap());
    const setLastError = Module.cwrap('em_js_set_last_error', null, ['number', 'string']);
    const OK = 0, INPROGRESS = 1, TIMEOUT = 4, NET = 5, NO_CONNECTION = 6, NOT_SUPPORTED = 8;
    const capacity = 65536, writeTimeoutMS = 10000, maxReadErrors = 3;
    const owner = { user, generation: 0, session: null, closing: Promise.resolve(null) };
    connections.set(user, owner);

    owner.active = (session) => owner.user !== 0 && owner.session === session &&
        owner.generation === session.generation && !session.stopped;
    owner.report = (session, message) => {
        if (owner.active(session)) setLastError(owner.user, message);
    };
    owner.trace = (session, message) => {
        // Opt in via Module.mdrSerialDebug. Counters remain inspectable on the session.
        if (Module.mdrSerialDebug)
            console.debug('[mdr Web Serial]', session.generation, message, { ...session.stats });
    };

    owner.stop = (session) => {
        if (session.closePromise) return session.closePromise;
        session.stopped = true;
        session.rx.length = 0;
        session.rxBytes = session.rxOffset = 0;
        if (session.write) clearTimeout(session.write.timer);
        if (session.onDisconnect) {
            navigator.serial.removeEventListener('disconnect', session.onDisconnect);
            session.onDisconnect = null;
        }
        // Defer cleanup so even a synchronous open/read/write exception has finished
        // assigning its promise before we collect outstanding operations.
        session.closePromise = Promise.resolve().then(async () => {
            if (session.openPromise) {
                try { await session.openPromise; } catch (_) { /* Open never succeeded. */ }
            }
            const reader = session.reader, writer = session.writer;
            const cancelRead = (async () => {
                if (!reader) return;
                try { await reader.cancel(); }
                catch (error) { owner.trace(session, `Reader cancellation: ${error}`); }
            })();
            const abortWrite = (async () => {
                if (!writer) return;
                // Do not wait for an indefinitely blocked write before requesting abort.
                try { await writer.abort(); }
                catch (error) { owner.trace(session, `Writer abort: ${error}`); }
            })();
            await Promise.all([cancelRead, abortWrite, session.readPromise,
                session.write ? session.write.promise : null]);
            // The read loop releases its reader in finally, after the read settles.
            if (writer) writer.releaseLock();
            session.writer = null;
            if (session.opened) {
                await session.port.close();
                session.opened = false;
            }
            owner.trace(session, 'Port closed');
            return null;
        }).catch((error) => {
            // Never call WASM from detached cleanup. Keep failed closes as barriers:
            // another backend must not reopen a port whose close did not complete.
            console.warn('[mdr Web Serial] Port cleanup failed:', error);
            return `${error}`;
        });
        if (session.port) {
            const port = session.port, previous = portClosures.get(port);
            const closing = previous ? Promise.all([previous, session.closePromise])
                .then((errors) => errors.find((error) => error) || null) : session.closePromise;
            portClosures.set(port, closing);
            closing.then((error) => {
                if (!error && portClosures.get(port) === closing) portClosures.delete(port);
            });
        }
        owner.closing = Promise.all([owner.closing, session.closePromise])
            .then((errors) => errors.find((error) => error) || null);
        return session.closePromise;
    };

    owner.fail = (session, result, message) => {
        if (!owner.active(session) || (session.result !== OK && session.result !== INPROGRESS)) return;
        session.result = result;
        owner.report(session, `${message} (RX ${session.stats.rxBytes}, ` +
            `TX accepted ${session.stats.txAccepted}, writes completed ${session.stats.txCompleted} bytes)`);
        console.warn('[mdr Web Serial]', message, {
            generation: session.generation, ...session.stats,
            rxBuffered: session.rxBytes,
            writeAgeMS: session.write ? performance.now() - session.write.startedAt : 0
        });
        owner.stop(session);
    };

    owner.check = () => {
        const session = owner.session;
        if (!session) return NO_CONNECTION;
        if (session.result !== OK) return session.result;
        if (session.stopped) return NO_CONNECTION;
        if (!session.opened || session.port.connected === false ||
            !session.port.readable || !session.port.writable) {
            owner.fail(session, NO_CONNECTION, 'Web Serial port is no longer available');
        } else if (session.write && performance.now() - session.write.startedAt >= writeTimeoutMS) {
            owner.fail(session, NET, 'Web Serial write timed out; delivery is unknown');
        }
        return session.result;
    };

    owner.readLoop = async (session) => {
        let consecutiveErrors = 0;
        while (owner.active(session)) {
            const stream = session.port.readable;
            if (!stream) {
                owner.fail(session, NO_CONNECTION, 'Web Serial receive stream disappeared');
                return;
            }
            let reader = null;
            try {
                reader = stream.getReader();
                session.reader = reader;
                while (owner.active(session)) {
                    session.stats.lastReadStartedAt = performance.now();
                    const { value, done } = await reader.read();
                    if (!owner.active(session)) return;
                    if (done) {
                        owner.fail(session, NO_CONNECTION, 'Unexpected EOF on Web Serial receive stream');
                        return;
                    }
                    if (!value || !value.length) continue;
                    consecutiveErrors = 0;
                    if (value.length > capacity - session.rxBytes) {
                        owner.fail(session, NET, 'Web Serial receive queue overflow');
                        return;
                    }
                    session.rx.push(value);
                    session.rxBytes += value.length;
                    session.stats.rxBytes += value.length;
                    session.stats.lastRxAt = performance.now();
                    owner.trace(session, `Received ${value.length} bytes`);
                }
            } catch (error) {
                if (!owner.active(session)) return;
                // Nonfatal errors leave a replacement stream available; fatal ones
                // do not. Finally releases the old reader before we read the replacement.
                if (!reader || !session.port.readable || ++consecutiveErrors > maxReadErrors) {
                    owner.fail(session, NET, `Web Serial receive failed: ${error}`);
                    return;
                }
                session.stats.readRecoveries++;
                owner.report(session, `Recovering Web Serial receive stream: ${error}`);
                console.warn('[mdr Web Serial] Recovering receive stream:', error);
            } finally {
                if (reader) {
                    try { reader.releaseLock(); }
                    catch (error) { owner.fail(session, NET, `Unable to release Web Serial reader: ${error}`); }
                    if (session.reader === reader) session.reader = null;
                }
            }
        }
    };

    owner.connect = (uuid) => {
        if (owner.session) owner.stop(owner.session);
        const previousClose = owner.closing;
        const session = {
            generation: ++owner.generation, result: INPROGRESS, stopped: false,
            port: null, opened: false, openPromise: null, closePromise: null,
            reader: null, writer: null, readPromise: null, connectPromise: null,
            onDisconnect: null, write: null, rx: [], rxBytes: 0, rxOffset: 0,
            stats: { rxBytes: 0, txAccepted: 0, txCompleted: 0, readRecoveries: 0,
                lastReadStartedAt: null, lastRxAt: null, lastWriteStartedAt: null, lastTxAt: null }
        };
        owner.session = session;
        owner.report(session, `Requesting Web Serial interface on ${uuid}`);
        if (!navigator.serial) {
            owner.fail(session, NOT_SUPPORTED, 'Your browser does not support Web Serial API. Check README for more information.');
            return session.result;
        }
        let selection;
        try {
            // requestPort MUST run before any await to retain the user's activation.
            selection = navigator.serial.requestPort({ allowedBluetoothServiceClassIds: [uuid] });
        } catch (error) {
            owner.fail(session, NET, `Unable to request Web Serial port: ${error}`);
            return session.result;
        }
        session.connectPromise = (async () => {
            try {
                const port = await selection;
                if (!owner.active(session)) return; // A cancelled picker must not open anything.
                const previousError = await previousClose;
                if (!owner.active(session)) return;
                if (previousError) throw new Error(`Previous port close failed: ${previousError}`);
                const closing = portClosures.get(port);
                if (closing) {
                    const closeError = await closing;
                    if (!owner.active(session)) return;
                    if (closeError) throw new Error(`Selected port close failed: ${closeError}`);
                }
                session.port = port;
                session.onDisconnect = (event) => {
                    if (event.target === port)
                        owner.fail(session, NO_CONNECTION, 'Web Serial device disconnected');
                };
                navigator.serial.addEventListener('disconnect', session.onDisconnect);
                owner.report(session, 'Opening Web Serial port');
                session.openPromise = port.open({ baudRate: 9600, bufferSize: capacity })
                    .then(() => { session.opened = true; });
                await session.openPromise;
                if (!owner.active(session)) return; // Cleanup waits for this open to settle.
                if (!port.readable || !port.writable)
                    throw new Error('Opened port has no readable/writable streams');
                session.writer = port.writable.getWriter();
                session.result = OK;
                owner.report(session, 'Web Serial port ready');
                // Drain data independently of C++ polling; never leave only one
                // read scheduled per rendered frame. The queue remains bounded.
                session.readPromise = owner.readLoop(session).catch((error) => {
                    owner.fail(session, NET, `Web Serial receive loop failed: ${error}`);
                });
                owner.trace(session, 'Port opened');
            } catch (error) {
                owner.fail(session, NET, `Unable to open Web Serial port: ${error}`);
            }
        })();
        return session.result;
    };

    owner.send = (src, size, sent) => {
        const result = owner.check();
        if (result !== OK) return result;
        const session = owner.session;
        if (size === 0) return OK;
        if (session.write) return INPROGRESS;
        const count = Math.min(size, capacity);
        const buffer = Module.HEAPU8.slice(src, src + count);
        const operation = { startedAt: performance.now(), promise: null, timer: null };
        session.write = operation;
        session.stats.txAccepted += count;
        session.stats.lastWriteStartedAt = operation.startedAt;
        operation.timer = setTimeout(() => {
            if (owner.active(session) && session.write === operation)
                owner.fail(session, NET, 'Web Serial write timed out; delivery is unknown');
        }, writeTimeoutMS);
        operation.promise = (async () => {
            try {
                await session.writer.write(buffer);
                if (!owner.active(session)) return;
                session.stats.txCompleted += count;
                session.stats.lastTxAt = performance.now();
                owner.trace(session, `Write completed (${count} bytes; device ACK still required)`);
            } catch (error) {
                owner.fail(session, NET, `Web Serial write failed; delivery is unknown: ${error}`);
            } finally {
                clearTimeout(operation.timer);
                if (session.write === operation) session.write = null;
            }
        })();
        if (session.result !== OK) return session.result;
        // Acceptance, not write completion. The copy above outlives the caller's buffer.
        setValue(sent, count, 'i32');
        owner.trace(session, `Write accepted (${count} bytes)`);
        return OK;
    };

    owner.recv = (dst, size, received) => {
        const result = owner.check();
        if (result !== OK) return result;
        const session = owner.session;
        const count = Math.min(size, session.rxBytes);
        let copied = 0;
        while (copied < count) {
            const chunk = session.rx[0];
            const length = Math.min(count - copied, chunk.length - session.rxOffset);
            Module.HEAPU8.set(chunk.subarray(session.rxOffset, session.rxOffset + length), dst + copied);
            session.rxOffset += length;
            copied += length;
            if (session.rxOffset === chunk.length) {
                session.rx.shift();
                session.rxOffset = 0;
            }
        }
        session.rxBytes -= count;
        setValue(received, count, 'i32');
        return count || size === 0 ? OK : INPROGRESS;
    };

    owner.poll = () => {
        const result = owner.check();
        if (result !== OK) return result;
        // Readable OR writable: a pending write must not prevent draining RX/ACKs.
        return owner.session.rxBytes || !owner.session.write ? OK : TIMEOUT;
    };
    owner.disconnect = () => {
        const session = owner.session;
        if (!session) return;
        session.result = NO_CONNECTION;
        owner.report(session, 'Web Serial port disconnected');
        owner.stop(session);
    };
});

EM_JS(int, em_js_connect, (void* user, const char* serviceUUID), {
    const owner = Module.mdrSerialConnections.get(user);
    return owner ? owner.connect(UTF8ToString(serviceUUID).toLowerCase()) : 6;
});
EM_JS(int, em_js_send, (void* user, const char* src, int size, int* sent), {
    const owner = Module.mdrSerialConnections.get(user);
    return owner ? owner.send(src, size, sent) : 6;
});
EM_JS(int, em_js_read, (void* user, char* dst, int size, int* received), {
    const owner = Module.mdrSerialConnections.get(user);
    return owner ? owner.recv(dst, size, received) : 6;
});
EM_JS(int, em_js_poll, (void* user), {
    const owner = Module.mdrSerialConnections.get(user);
    return owner ? owner.poll() : 6;
});
EM_JS(void, em_js_disconnect, (void* user), {
    const owner = Module.mdrSerialConnections.get(user);
    if (owner) owner.disconnect();
});
EM_JS(void, em_js_destroy, (void* user), {
    const connections = Module.mdrSerialConnections;
    const owner = connections.get(user);
    if (!owner) return;
    // Invalidate the callback target before native memory can be freed/reused.
    connections.delete(user);
    owner.user = 0;
    owner.disconnect();
});
// clang-format on

struct MDRConnectionEmscripten
{
    MDRConnection mdrConn;
    mdr::String lastError{"No Web Serial connection has been established"};
    MDRConnectionEmscripten() :
        mdrConn({.user = this,
                 .connect = Connect,
                 .disconnect = Disconnect,
                 .recv = Recv,
                 .send = Send,
                 .poll = Poll,
                 .getDevicesList = GetDevicesList,
                 .freeDevicesList = FreeDevicesList,
                 .getLastError = GetLastError})
    {
        em_js_init(this);
    }
    ~MDRConnectionEmscripten()
    {
        em_js_destroy(this);
    }
    static MDRResult Connect(void* user, const char*, const char* uuid) noexcept
    {
        if (!uuid || !*uuid)
            return MDR_RESULT_ERROR_INVALID_ARGUMENT;
        return em_js_connect(user, uuid);
    }
    static void Disconnect(void* user) noexcept
    {
        em_js_disconnect(user);
    }
    static MDRResult Recv(void* user, char* dst, int size, int* pReceived) noexcept
    {
        if (pReceived)
            *pReceived = 0;
        if (!pReceived || size < 0 || (!dst && size))
            return MDR_RESULT_ERROR_INVALID_ARGUMENT;
        return em_js_read(user, dst, size, pReceived);
    }
    static MDRResult Send(void* user, const char* src, int size, int* pSent) noexcept
    {
        if (pSent)
            *pSent = 0;
        if (!pSent || size < 0 || (!src && size))
            return MDR_RESULT_ERROR_INVALID_ARGUMENT;
        return em_js_send(user, src, size, pSent);
    }
    static MDRResult Poll(void* user, int) noexcept
    {
        // Never block the browser thread; readiness is published by async operations.
        return em_js_poll(user);
    }
    static MDRResult GetDevicesList(void*, MDRDeviceInfo** ppList, int* pCount) noexcept
    {
        if (!ppList || !pCount)
            return MDR_RESULT_ERROR_INVALID_ARGUMENT;
        static MDRDeviceInfo sInfo{.szDeviceName = "Web Serial API device", .szDeviceMacAddress = "00:00:09:05:02:07"};
        *ppList = &sInfo;
        *pCount = 1;
        return MDR_RESULT_OK;
    }
    static MDRResult FreeDevicesList(void*, MDRDeviceInfo** ppList) noexcept
    {
        if (!ppList)
            return MDR_RESULT_ERROR_INVALID_ARGUMENT;
        *ppList = nullptr;
        return MDR_RESULT_OK;
    }
    static const char* GetLastError(void* user) noexcept
    {
        auto* ptr = static_cast<MDRConnectionEmscripten*>(user);
        return ptr->lastError.c_str();
    }
};
extern "C" {
EMSCRIPTEN_KEEPALIVE
void em_js_set_last_error(void* user, const char* error)
{
    auto* ptr = static_cast<MDRConnectionEmscripten*>(user);
    ptr->lastError = error;
}
MDR_API MDRConnectionEmscripten* mdrConnectionEmscriptenCreate()
{
    return mdr::Construct<MDRConnectionEmscripten>();
}
MDR_API MDRConnection* mdrConnectionEmscriptenGet(MDRConnectionEmscripten* p)
{
    return p ? &p->mdrConn : nullptr;
}
MDR_API void mdrConnectionEmscriptenDestroy(MDRConnectionEmscripten* p)
{
    if (p)
        mdr::Destruct(p);
}
}
