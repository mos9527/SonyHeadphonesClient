#include "../Platform.hpp"
#include <mdr-c/Platform/PlatformLinux.h>

MDRConnectionLinux* gConn;

void clientPlatformInit()
{
    gConn = mdrConnectionLinuxCreate();
}
void clientPlatformDestroy()
{
    mdrConnectionLinuxDestroy(gConn);
}
MDRConnection* clientPlatformConnectionGet()
{
    return mdrConnectionLinuxGet(gConn);
}