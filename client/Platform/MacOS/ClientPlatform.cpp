#include <mdr-bt/ConnectionMacOS.h>

extern "C" {
int clientPlatformLocateFontBinary(const char** outData)
{
    *outData = nullptr;
    return 0;
}

static MDRConnectionMacOS* gConn = nullptr;

int clientPlatformConnectionInit(int flags)
{
    if (flags & MDR_INIT_BT_BLE)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    gConn = mdrConnectionMacOSCreate();
    return MDR_RESULT_OK;
}

void clientPlatformConnectionDestroy()
{
    if (gConn) { mdrConnectionMacOSDestroy(gConn); gConn = nullptr; }
}

MDRConnection* clientPlatformConnectionGet()
{
    if (gConn) return mdrConnectionMacOSGet(gConn);
    [[unlikely]] return nullptr;
}

void clientPlatformDestroy()
{
    clientPlatformConnectionDestroy();
    // TODO
}
}
