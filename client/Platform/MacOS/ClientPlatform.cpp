#include "../Platform.hpp"
#include "ConnectionMacOS.hpp"

MDRConnectionMacOS* gConn = nullptr;
extern "C" {
    int clientPlatformConnectionInit(int flags)
    {
        if (flags & MDR_INIT_BT_BLE)
        {
            gConn = nullptr;
            return MDR_RESULT_ERROR_NOT_SUPPORTED;
        }
        gConn = clientPlatformMacOSConnectionCreate();
        return MDR_RESULT_OK;
    }
    void clientPlatformConnectionDestroy()
    {
        if (gConn)
            clientPlatformMacOSConnectionDestroy(gConn), gConn = nullptr;
    }
    MDRConnection* clientPlatformConnectionGet()
    {
        if (gConn)
            return clientPlatformMacOSConnectionGet(gConn);
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
