#include "../Platform.hpp"
#include <mdr-c/Platform/PlatformWindows.h>

MDRConnectionWindows* gConn;

void clientPlatformInit()
{
    gConn = mdrConnectionWindowsCreate();
}
void clientPlatformDestroy()
{
    mdrConnectionWindowsDestroy(gConn);
}
MDRConnection* clientPlatformConnectionGet()
{
    return mdrConnectionWindowsGet(gConn);
}