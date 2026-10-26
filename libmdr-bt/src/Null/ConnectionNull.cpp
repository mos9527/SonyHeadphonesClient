// NOTE: Available with CMake option MDR_BT_NULL_BLUETOOTH
// This builds a stub BT backend with no bluetooth support.
#include <mdr-bt/ConnectionNull.h>
#include <mdr/Protocol.hpp>

struct MDRNullConnection
{
    MDRConnection mdrConn;

    MDRNullConnection() noexcept :
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
        })
    {
    }

    static MDRResult Connect(void*, const char*, const char*) noexcept
    {
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    }

    static void Disconnect(void*) noexcept
    {
    }

    static MDRResult Recv(void*, char*, int, int* pReceived) noexcept
    {
        if (pReceived)
            *pReceived = 0;
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    }

    static MDRResult Send(void*, const char*, int, int* pSent) noexcept
    {
        if (pSent)
            *pSent = 0;
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    }

    static MDRResult Poll(void*, int) noexcept
    {
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    }

    static MDRResult GetDevicesList(void*, MDRDeviceInfo** ppList, int* pCount) noexcept
    {
        if (ppList)
            *ppList = nullptr;
        if (pCount)
            *pCount = 0;
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    }

    static MDRResult FreeDevicesList(void*, MDRDeviceInfo** ppList) noexcept
    {
        if (ppList)
            *ppList = nullptr;
        return MDR_RESULT_OK;
    }

    static const char* GetLastError(void*) noexcept
    {
        return "Bluetooth is not supported in this build (MDR_BT_NULL_BLUETOOTH)";
    }
};

#define MDR_NULL_CONNECTION(Name) \
    struct MDRConnection##Name : MDRNullConnection {}; \
    extern "C" { \
    MDR_API MDRConnection##Name* mdrConnection##Name##Create() \
    { \
        return mdr::Construct<MDRConnection##Name>(); \
    } \
    MDR_API MDRConnection* mdrConnection##Name##Get(MDRConnection##Name* conn) \
    { \
        return conn ? &conn->mdrConn : nullptr; \
    } \
    MDR_API void mdrConnection##Name##Destroy(MDRConnection##Name* conn) \
    { \
        if (conn) \
            mdr::Destruct(conn); \
    } \
    }

MDR_NULL_CONNECTION(Null)
MDR_NULL_CONNECTION(Emscripten)
MDR_NULL_CONNECTION(IOS)
MDR_NULL_CONNECTION(Linux)
MDR_NULL_CONNECTION(MacOS)
MDR_NULL_CONNECTION(Windows)

#ifdef MDR_BT_BLE_SUPPORT
MDR_NULL_CONNECTION(NullBLE)
MDR_NULL_CONNECTION(WindowsBLE)
#endif

#undef MDR_NULL_CONNECTION
