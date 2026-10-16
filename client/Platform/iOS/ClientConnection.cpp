#include "../Platform.hpp"
#include <mdr-bt/ConnectionIOS.h>

static MDRConnectionIOS* gConn = nullptr;

extern "C" {
int clientPlatformConnectionInit(int flags)
{
    if (flags & MDR_INIT_BT_BLE)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    gConn = mdrConnectionIOSCreate();
    return MDR_RESULT_OK;
}

void clientPlatformConnectionDestroy()
{
    if (gConn) { mdrConnectionIOSDestroy(gConn); gConn = nullptr; }
}

MDRConnection* clientPlatformConnectionGet()
{
    if (gConn) return mdrConnectionIOSGet(gConn);
    [[unlikely]] return nullptr;
}
}
