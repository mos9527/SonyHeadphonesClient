#include <string>
#include <mdr-c/Platform/PlatformEmscripten.h>

#include <emscripten.h>
EM_JS(void, em_js_init, (void* user), {
    navigator.em_js_user = user;
    navigator.em_js_set_last_error = Module.cwrap(
      'em_js_set_last_error',
      null,
      ['number', 'string']
    );
    navigator.set_last_error = function (err)
    {
        navigator.em_js_set_last_error(navigator.em_js_user, err);
    };
    navigator.em_js_poll_result = 0; // OK
    navigator.em_js_port_send_promise = null;
    navigator.em_js_port_recv_promise = null;
    navigator.em_js_recv_buffer = new Uint8Array();
    navigator.em_js_port = null;
    navigator.em_js_port_reader = null;
    navigator.em_js_port_writer = null;
});
EM_JS(int /* bytes sent */, em_js_send, (const char* src, int size), {

    if (navigator.em_js_port_send_promise){
        return 0;
    }
    var buf = Module.HEAPU8.slice(src, src + size);
    async function async_send()
    {
        try
        {
            await navigator.em_js_port_writer.write(buf);
            navigator.em_js_port_send_promise = null;
        } catch (error)
        {
            navigator.set_last_error(`${error}`);
            navigator.em_js_poll_result = 5; // ERROR_NET
        }
    }
    navigator.em_js_port_send_promise = async_send();
    return size;
});
EM_JS(int /* bytes read */, em_js_read, (const char* dst, int size), {

    if (navigator.em_js_recv_buffer.length == 0 && navigator.em_js_port_recv_promise != null){
        return 0;
    }
    async function async_recv()
    {
        try
        {
            const { value, done } = await navigator.em_js_port_reader.read();
            if (!done)
                navigator.em_js_recv_buffer = new Uint8Array([...navigator.em_js_recv_buffer, ...value]);
            navigator.em_js_port_recv_promise = null;

        } catch (error)
        {
            navigator.set_last_error(`${error}`);
            navigator.em_js_poll_result = 5; // ERROR_NET
        }
    }
    if (navigator.em_js_port_recv_promise == null)
        navigator.em_js_port_recv_promise = async_recv();

    size = Math.min(size, navigator.em_js_recv_buffer.length);

    var slice = navigator.em_js_recv_buffer.slice(0, size);
    navigator.em_js_recv_buffer = navigator.em_js_recv_buffer.slice(size);
    Module.HEAPU8.set(slice, dst);
    return size;
});
EM_JS(void, em_js_connect, (const char* serviceUUID), {
    async function async_request_port()
    {
        const uuid = UTF8ToString(serviceUUID).toLowerCase();
        navigator.set_last_error(`Requesting WebSerial interface on ${uuid}`);
        navigator.em_js_poll_result = 1; // INPROGRESS
        try
        {
            navigator.em_js_port = await navigator.serial.requestPort({
                allowedBluetoothServiceClassIds: [uuid]
            });
            navigator.set_last_error(`Opening Serial Port`);
            await navigator.em_js_port.open({ baudRate: 9600 });
            navigator.em_js_port_writer = navigator.em_js_port.writable.getWriter();
            navigator.em_js_port_reader = navigator.em_js_port.readable.getReader();
            navigator.em_js_poll_result = 0; // OK
        } catch (error)
        {
            navigator.set_last_error(`${error}`);
            navigator.em_js_poll_result = 5; // ERROR_NET
        }
    }
    navigator.em_js_connect_promise = async_request_port();
});
EM_JS(int, em_js_poll, (), {
    return navigator.em_js_poll_result;
});
EM_JS(void, em_js_disconnect, (), {
    if (navigator.em_js_port != null && navigator.em_js_port.connected)
    {
        if (navigator.em_js_port_reader)
        {
            navigator.em_js_port_reader.cancel();
            navigator.em_js_port_reader.releaseLock();
            navigator.em_js_port_reader = null;
        }
        if (navigator.em_js_port_writer)
        {
            navigator.em_js_port_writer.releaseLock();
            navigator.em_js_port_writer = null;
        }
        navigator.em_js_port.close();
        navigator.em_js_port = null;
    }
});
struct MDRConnectionEmscripten
{
    MDRConnection mdrConn;
    std::string lastError;
    MDRConnectionEmscripten() : mdrConn({.user = this,
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
    static int Connect(void*, const char*, const char* uuid) noexcept
    {
        em_js_connect(uuid);
        return MDR_RESULT_INPROGRESS;
    }
    static void Disconnect(void*) noexcept
    {
        em_js_disconnect();
    }
    static int Recv(void*, char* dst, int size, int* pReceived) noexcept
    {
        *pReceived = em_js_read(dst, size);
        if (*pReceived == 0)
            return MDR_RESULT_INPROGRESS;
        return MDR_RESULT_OK;
    }
    static int Send(void*, const char* src, int size, int* pSent) noexcept
    {
        *pSent = em_js_send(src, size);
        if (*pSent)
            return MDR_RESULT_OK;
        return MDR_RESULT_INPROGRESS;
    }
    static int Poll(void*, int) noexcept
    {
        return em_js_poll();
    }
    static int GetDevicesList(void*, MDRDeviceInfo** ppList, int* pCount) noexcept
    {
        static MDRDeviceInfo sInfo{
            .szDeviceName = "Emscripten RFCOMM device",
            .szDeviceMacAddress = "00:00:09:05:02:07"
        };
        *ppList = &sInfo, *pCount = 1;
        return MDR_RESULT_OK;
    }
    static int FreeDevicesList(void*, MDRDeviceInfo**) noexcept
    {
        // No-op
        return MDR_RESULT_OK;
    }
    static const char* GetLastError(void* user) noexcept
    {
        auto* ptr = static_cast<MDRConnectionEmscripten*>(user);
        return ptr->lastError.c_str();
    }
};
extern "C"{
    EMSCRIPTEN_KEEPALIVE
    void em_js_set_last_error(void* user, const char* error)
    {
        auto* ptr = static_cast<MDRConnectionEmscripten*>(user);
        ptr->lastError = error;
    }
    MDRConnectionEmscripten* mdrConnectionEmscriptenCreate() { return new MDRConnectionEmscripten(); }
    MDRConnection* mdrConnectionEmscriptenGet(MDRConnectionEmscripten* p) { return &p->mdrConn; }
    void mdrConnectionEmscriptenDestroy(MDRConnectionEmscripten* p) { delete p; }
}
