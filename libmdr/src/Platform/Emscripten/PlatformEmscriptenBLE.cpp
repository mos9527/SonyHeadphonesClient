#include <string>
#include <mdr-c/Platform/PlatformEmscriptenBLE.h>

#include <emscripten.h>

EM_JS(void, em_js_ble_init, (void* user), {
    navigator.em_js_ble_user = user;
    navigator.em_js_ble_set_last_error = Module.cwrap(
      'em_js_ble_set_last_error',
      null,
      ['number', 'string']
    );
    navigator.ble_set_last_error = function (err)
    {
        navigator.em_js_ble_set_last_error(navigator.em_js_ble_user, err);
    };
    navigator.em_js_ble_poll_result = 0; // OK
    navigator.em_js_ble_recv_buffer = new Uint8Array();
    navigator.em_js_ble_send_promise = null;
    navigator.em_js_ble_device = null;
    navigator.em_js_ble_server = null;
    navigator.em_js_ble_service = null;
    navigator.em_js_ble_write_char = null;
    navigator.em_js_ble_notify_char = null;
    navigator.em_js_ble_write_no_resp = false;
    navigator.em_js_ble_value_listener = null;
    navigator.em_js_ble_disconnect_listener = null;
});

EM_JS(void, em_js_ble_connect, (const char* serviceUUID), {
    const uuid = UTF8ToString(serviceUUID).toLowerCase();
    navigator.ble_set_last_error(`Requesting Web Bluetooth device for service ${uuid}`);
    navigator.em_js_ble_poll_result = 1; // INPROGRESS

    async function async_connect()
    {
        try
        {
            if (!!!navigator.bluetooth)
                throw "Your browser does not support Web Bluetooth API. Check README for more information.";

            // Prompt the user to pick a device that advertises the target service.
            // Must be called synchronously from a user-gesture callstack (we are).
            console.log(`Requesting device ${uuid}`);
            const device = await navigator.bluetooth.requestDevice({
                acceptAllDevices: true,
                optionalServices: [uuid]
            });
            navigator.em_js_ble_device = device;
            navigator.ble_set_last_error(`Connecting GATT on "${device.name || device.id}"`);

            // Track unexpected server-side disconnects.
            navigator.em_js_ble_disconnect_listener = function () {
                navigator.ble_set_last_error('GATT server disconnected');
                navigator.em_js_ble_poll_result = 5; // ERROR_NET
            };
            device.addEventListener('gattserverdisconnected',
                navigator.em_js_ble_disconnect_listener);

            const server = await device.gatt.connect();
            navigator.em_js_ble_server = server;

            const service = await server.getPrimaryService(uuid);
            navigator.em_js_ble_service = service;

            const chars = await service.getCharacteristics();

            // Pick first writable char (prefer WriteWithoutResponse for lower latency)
            // and first notifiable/indicatable char -- matches the Windows BLE backend.
            let writeChar = null;
            let writeNoResp = false;
            let notifyChar = null;
            for (const ch of chars)
            {
                const p = ch.properties;
                if (writeChar == null && (p.write || p.writeWithoutResponse))
                {
                    writeChar = ch;
                    writeNoResp = !!p.writeWithoutResponse;
                }
                if (notifyChar == null && (p.notify || p.indicate))
                    notifyChar = ch;
            }
            if (writeChar == null)
                throw "No writable GATT characteristic found in service";
            if (notifyChar == null)
                throw "No notifiable GATT characteristic found in service";

            navigator.em_js_ble_write_char = writeChar;
            navigator.em_js_ble_write_no_resp = writeNoResp;
            navigator.em_js_ble_notify_char = notifyChar;

            // Route notifications into em_js_ble_recv_buffer.
            navigator.em_js_ble_value_listener = function (event) {
                const dv = event.target.value; // DataView
                const bytes = new Uint8Array(dv.buffer, dv.byteOffset, dv.byteLength);
                navigator.em_js_ble_recv_buffer = new Uint8Array(
                    [...navigator.em_js_ble_recv_buffer, ...bytes]);
            };
            notifyChar.addEventListener('characteristicvaluechanged',
                navigator.em_js_ble_value_listener);
            await notifyChar.startNotifications();

            navigator.ble_set_last_error('Connected via BLE GATT');
            navigator.em_js_ble_poll_result = 0; // OK
        } catch (error)
        {
            navigator.ble_set_last_error(`${error}`);
            navigator.em_js_ble_poll_result = 5; // ERROR_NET
        }
    }
    navigator.em_js_ble_connect_promise = async_connect();
});

EM_JS(int /* bytes sent */, em_js_ble_send, (const char* src, int size), {
    if (navigator.em_js_ble_send_promise)
        return 0;
    if (navigator.em_js_ble_write_char == null)
        return 0;

    // HEAPU8.slice returns a plain ArrayBuffer-backed Uint8Array copy -- safe
    // to hand off to an async writer after the HEAP view is invalidated.
    const buf = Module.HEAPU8.slice(src, src + size);

    async function async_send()
    {
        try
        {
            const ch = navigator.em_js_ble_write_char;
            if (navigator.em_js_ble_write_no_resp && ch.writeValueWithoutResponse)
                await ch.writeValueWithoutResponse(buf);
            else
                await ch.writeValue(buf);
            navigator.em_js_ble_send_promise = null;
        } catch (error)
        {
            navigator.ble_set_last_error(`${error}`);
            navigator.em_js_ble_poll_result = 5; // ERROR_NET
            navigator.em_js_ble_send_promise = null;
        }
    }
    navigator.em_js_ble_send_promise = async_send();
    return size;
});

EM_JS(int /* bytes read */, em_js_ble_read, (const char* dst, int size), {
    const have = navigator.em_js_ble_recv_buffer.length;
    if (have == 0)
        return 0;

    const n = Math.min(size, have);
    const slice = navigator.em_js_ble_recv_buffer.slice(0, n);
    navigator.em_js_ble_recv_buffer = navigator.em_js_ble_recv_buffer.slice(n);
    Module.HEAPU8.set(slice, dst);
    return n;
});

EM_JS(int, em_js_ble_poll, (), {
    return navigator.em_js_ble_poll_result;
});

EM_JS(void, em_js_ble_disconnect, (), {
    try
    {
        if (navigator.em_js_ble_notify_char)
        {
            if (navigator.em_js_ble_value_listener)
            {
                navigator.em_js_ble_notify_char.removeEventListener(
                    'characteristicvaluechanged',
                    navigator.em_js_ble_value_listener);
            }
            // Best-effort: stopNotifications() returns a promise, but we don't
            // await it here -- the device will be torn down anyway.
            try { navigator.em_js_ble_notify_char.stopNotifications(); } catch (e) {}
        }
        if (navigator.em_js_ble_device)
        {
            if (navigator.em_js_ble_disconnect_listener)
            {
                navigator.em_js_ble_device.removeEventListener(
                    'gattserverdisconnected',
                    navigator.em_js_ble_disconnect_listener);
            }
            if (navigator.em_js_ble_device.gatt &&
                navigator.em_js_ble_device.gatt.connected)
                navigator.em_js_ble_device.gatt.disconnect();
        }
    } catch (e)
    {
        console.log(`[em_js_ble_disconnect] ${e}`);
    }
    em_js_ble_init(navigator.em_js_ble_user);
    console.log('[em_js_ble_disconnect] Disconnected');
});

struct MDRConnectionEmscriptenBLE
{
    MDRConnection mdrConn;
    std::string lastError;

    MDRConnectionEmscriptenBLE() : mdrConn({.user = this,
                                .connect = Connect,
                                .disconnect = Disconnect,
                                .recv = Recv,
                                .send = Send,
                                .poll = Poll,
                                .getDevicesList = GetDevicesList,
                                .freeDevicesList = FreeDevicesList,
                                .getLastError = GetLastError})
    {
        em_js_ble_init(this);
    }

    // macAddress is intentionally ignored: Web Bluetooth hides the peer BD_ADDR
    // and handles device selection via its own browser-chrome picker.
    static int Connect(void*, const char* /*macAddress*/, const char* uuid) noexcept
    {
        em_js_ble_connect(uuid);
        return MDR_RESULT_INPROGRESS;
    }
    static void Disconnect(void*) noexcept
    {
        em_js_ble_disconnect();
    }
    static int Recv(void*, char* dst, int size, int* pReceived) noexcept
    {
        *pReceived = em_js_ble_read(dst, size);
        if (*pReceived == 0)
            return MDR_RESULT_INPROGRESS;
        return MDR_RESULT_OK;
    }
    static int Send(void*, const char* src, int size, int* pSent) noexcept
    {
        *pSent = em_js_ble_send(src, size);
        if (*pSent)
            return MDR_RESULT_OK;
        return MDR_RESULT_INPROGRESS;
    }
    static int Poll(void*, int) noexcept
    {
        return em_js_ble_poll();
    }
    static int GetDevicesList(void*, MDRDeviceInfo** ppList, int* pCount) noexcept
    {
        // Web Bluetooth does not enumerate devices ahead of time -- selection
        // happens inside navigator.bluetooth.requestDevice()'s browser picker.
        // Return a single placeholder so the UI "Connect" button is enabled.
        static MDRDeviceInfo sInfo{
            .szDeviceName = "Web Bluetooth device",
            .szDeviceMacAddress = "00:00:09:05:02:07"
        };
        *ppList = &sInfo, *pCount = 1;
        return MDR_RESULT_OK;
    }
    static int FreeDevicesList(void*, MDRDeviceInfo**) noexcept
    {
        // No-op -- the device list is a static placeholder.
        return MDR_RESULT_OK;
    }
    static const char* GetLastError(void* user) noexcept
    {
        auto* ptr = static_cast<MDRConnectionEmscriptenBLE*>(user);
        return ptr->lastError.c_str();
    }
};

extern "C"{
    EMSCRIPTEN_KEEPALIVE
    void em_js_ble_set_last_error(void* user, const char* error)
    {
        auto* ptr = static_cast<MDRConnectionEmscriptenBLE*>(user);
        ptr->lastError = error;
    }
    MDRConnectionEmscriptenBLE* mdrConnectionEmscriptenBLECreate() { return new MDRConnectionEmscriptenBLE(); }
    MDRConnection* mdrConnectionEmscriptenBLEGet(MDRConnectionEmscriptenBLE* p) { return &p->mdrConn; }
    void mdrConnectionEmscriptenBLEDestroy(MDRConnectionEmscriptenBLE* p)
    {
        if (p)
        {
            em_js_ble_disconnect();
            delete p;
        }
    }
}
