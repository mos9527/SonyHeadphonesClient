#include "../Platform.hpp"
#include <mdr-c/Platform/PlatformEmscripten.h>


MDRConnectionEmscripten* gConn;

void clientPlatformInit()
{
    gConn = mdrConnectionEmscriptenCreate();
}
void clientPlatformDestroy()
{
    mdrConnectionEmscriptenDestroy(gConn);
}
MDRConnection* clientPlatformConnectionGet()
{
    return mdrConnectionEmscriptenGet(gConn);
}