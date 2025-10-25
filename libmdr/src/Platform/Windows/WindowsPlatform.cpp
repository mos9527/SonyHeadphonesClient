#include <mdr/WindowsPlatform.h>
#include "../Platform.hpp"
struct MDRConnectionWindows
{
    MDRConnection mdrConn;
    char const* lastError;

    MDRConnectionWindows() noexcept :
        lastError(""), mdrConn({.user = this,
                                .connect = Connect,
                                .disconnect = Disconnect,
                                .recv = Recv,
                                .send = Send,
                                .list = List,
                                .freeList = FreeList,
                                .getLastError = GetLastError})
    {
    }

    static int Connect(void* user, const char* macAddress, const char* serviceUUID) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindows*>(user);        
        return MDR_RESULT_OK;
    }

    static void Disconnect(void* user) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindows*>(user);
    }

    static int Recv(void* user, char* dst, int size, int* pReceived) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindows*>(user);
        return MDR_RESULT_OK;
    }

    static int Send(void* user, const char* src, int size, int* pSent) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindows*>(user);
        return MDR_RESULT_OK;
    }

    static int List(void* user, MDRDeviceInfo** ppList, int* pCount) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindows*>(user);
        return MDR_RESULT_OK;
    }

    static int FreeList(void*, MDRDeviceInfo** ppList) noexcept
    {
        delete[] *ppList;
        *ppList = nullptr;
        return MDR_RESULT_OK;
    }

    static const char* GetLastError(void* user) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindows*>(user);
        return ptr->lastError;
    }
};
extern "C" {

MDRConnectionWindows* mdrConnectionWindowsCreate() { return new MDRConnectionWindows(); }

void mdrConnectionWindowsDestroy(MDRConnectionWindows* instance) { delete instance; }

MDRConnection* mdrConnectionWindowsGet(MDRConnectionWindows* instance) { return &instance->mdrConn; }
}
