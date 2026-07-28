#include "../Platform.hpp"
#include <mdr-c/Platform/PlatformLinux.h>

MDRConnectionLinux* gConn;
extern "C" {
    int clientPlatformConnectionInit(int flags)
    {
        if (flags & MDR_INIT_BT_BLE)
        {
            gConn = nullptr;
            return MDR_RESULT_ERROR_NOT_SUPPORTED;
        }
        gConn = mdrConnectionLinuxCreate();
        return MDR_RESULT_OK;
    }
    void clientPlatformConnectionDestroy()
    {
        if (gConn)
            mdrConnectionLinuxDestroy(gConn);
    }
    MDRConnection* clientPlatformConnectionGet()
    {
        if (gConn)
            return mdrConnectionLinuxGet(gConn);
        [[unlikely]] return nullptr;
    }
    int clientPlatformLocateFontBinary(const char** outData)
    {
        // TODO: This would be hell.
        *outData = nullptr;
        return 0;
    }
    void clientPlatformDestroy()
    {
        clientPlatformConnectionDestroy();
        // TODO
    }
}