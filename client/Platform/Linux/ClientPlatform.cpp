#include "../Platform.hpp"
#include "ConnectionLinux.hpp"

MDRConnectionLinux* gConn = nullptr;
extern "C" {
    int clientPlatformConnectionInit(int flags)
    {
        if (flags & MDR_INIT_BT_BLE)
        {
            gConn = nullptr;
            return MDR_RESULT_ERROR_NOT_SUPPORTED;
        }
        gConn = clientPlatformLinuxConnectionCreate();
        return MDR_RESULT_OK;
    }
    void clientPlatformConnectionDestroy()
    {
        if (gConn)
            clientPlatformLinuxConnectionDestroy(gConn), gConn = nullptr;
    }
    MDRConnection* clientPlatformConnectionGet()
    {
        if (gConn)
            return clientPlatformLinuxConnectionGet(gConn);
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