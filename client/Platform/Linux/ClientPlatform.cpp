#include <mdr-bt/ConnectionLinux.h>

extern "C" {
int clientPlatformLocateFontBinary(const char** outData)
{
    // TODO: This would be hell.
    *outData = nullptr;
    return 0;
}

static MDRConnectionLinux* gConn = nullptr;

int clientPlatformConnectionInit(int flags)
{
    if (flags & MDR_INIT_BT_BLE)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    gConn = mdrConnectionLinuxCreate();
    return MDR_RESULT_OK;
}

void clientPlatformConnectionDestroy()
{
    if (gConn) { mdrConnectionLinuxDestroy(gConn); gConn = nullptr; }
}

MDRConnection* clientPlatformConnectionGet()
{
    if (gConn) return mdrConnectionLinuxGet(gConn);
    [[unlikely]] return nullptr;
}

void clientPlatformDestroy()
{
    clientPlatformConnectionDestroy();
    // TODO
}
}
