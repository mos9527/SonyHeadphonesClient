#include <mdr-c/Base.h>
#include <mdr-c/Platform/PlatformMac.h>

struct MDRConnectionMac
{
    MDRConnection mdrConn;
    const char* lastError;
    MDRConnectionMac() noexcept :
        mdrConn({
            .user = this,
            .connect = Connect,
            .disconnect = Disconnect,
            .recv = Recv,
            .send = Send,
            .poll = Poll,
            .getDevicesList = GetDevicesList,
            .freeDevicesList = FreeDevicesList,
            .getLastError = GetLastError
        }),
        lastError("Not implemented")
    {
    }

    static int Connect(void* user, const char*, const char*) noexcept
    {
        auto* ptr = static_cast<MDRConnectionMac*>(user);
        ptr->lastError = "macOS connection backend not implemented";
        return MDR_RESULT_ERROR_NET;
    }

    static void Disconnect(void*) noexcept
    {
    }

    static int Recv(void* user, char*, int, int*) noexcept
    {
        auto* ptr = static_cast<MDRConnectionMac*>(user);
        ptr->lastError = "macOS connection backend not implemented";
        return MDR_RESULT_ERROR_NO_CONNECTION;
    }

    static int Send(void* user, const char*, int, int*) noexcept
    {
        auto* ptr = static_cast<MDRConnectionMac*>(user);
        ptr->lastError = "macOS connection backend not implemented";
        return MDR_RESULT_ERROR_NO_CONNECTION;
    }

    static int Poll(void* user, int) noexcept
    {
        auto* ptr = static_cast<MDRConnectionMac*>(user);
        ptr->lastError = "macOS connection backend not implemented";
        return MDR_RESULT_ERROR_NO_CONNECTION;
    }

    static int GetDevicesList(void*, MDRDeviceInfo** ppList, int* pCount) noexcept
    {
        *ppList = nullptr;
        *pCount = 0;
        return MDR_RESULT_OK;
    }

    static int FreeDevicesList(void*, MDRDeviceInfo**) noexcept
    {
        return MDR_RESULT_OK;
    }

    static const char* GetLastError(void* user) noexcept
    {
        auto* ptr = static_cast<MDRConnectionMac*>(user);
        return ptr->lastError;
    }
};

extern "C" {
MDRConnectionMac* mdrConnectionMacCreate() { return new MDRConnectionMac(); }
MDRConnection* mdrConnectionMacGet(MDRConnectionMac* instance) { return &instance->mdrConn; }
void mdrConnectionMacDestroy(MDRConnectionMac* instance) { delete instance; }
}
