#include "../Platform.hpp"
#include <mdr/Protocol.hpp>
#include <mdr-c/Platform/PlatformMacOS.h>

MDRConnectionMacOS* gConn = nullptr;
extern "C" {
    int clientPlatformConnectionInit(int flags)
    {
        MDR_CHECK_MSG(gConn == nullptr, "Platform already initialized. You MUST call clientPlatformDestroy() before initializing again.");
        if (flags & MDR_INIT_BT_BLE)
        {
            gConn = nullptr;
            return MDR_RESULT_ERROR_NOT_SUPPORTED;
        }
        gConn = mdrConnectionMacOSCreate();
        return MDR_RESULT_OK;
    }
    void clientPlatformConnectionDestroy()
    {
        if (gConn)
        {
            mdrConnectionMacOSDestroy(gConn);
            gConn = nullptr;
        }
    }
    MDRConnection* clientPlatformConnectionGet()
    {
        if (gConn)
            return mdrConnectionMacOSGet(gConn);
        [[unlikely]] return nullptr;
    }
    int clientPlatformLocateFontBinary(const char** outData)
    {
        *outData = nullptr;
        return 0;
    }
    void clientPlatformDestroy()
    {
        clientPlatformConnectionDestroy();
        // TODO
    }
}
