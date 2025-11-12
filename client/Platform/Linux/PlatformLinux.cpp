#include "../Platform.hpp"
#include <mdr-c/Platform/PlatformLinux.h>

MDRConnectionLinux* gConn;
extern "C" {
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
    int clientPlatformLocateFontBinary(const char** outData)
    {
        // TODO: This would be hell.
        *outData = nullptr;
        return 0;
    }
}