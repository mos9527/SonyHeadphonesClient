#include "../Platform.hpp"
#include <mdr-c/Platform/PlatformMacOS.h>

MDRConnectionMacOS* gConn = nullptr;
extern "C" {
    int clientPlatformConnectionInit(int flags)
    {
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
            mdrConnectionMacOSDestroy(gConn);
    }
    MDRConnection* clientPlatformConnectionGet()
    {
        if (gConn)
            mdrConnectionMacOSGet(gConn);
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
