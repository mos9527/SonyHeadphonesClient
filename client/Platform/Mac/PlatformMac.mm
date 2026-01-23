#import <AppKit/AppKit.h>
#include "../Platform.hpp"
#include <mdr-c/Platform/PlatformMac.h>

MDRConnectionMac* gConn = nullptr;
extern "C" {
    void clientPlatformInit()
    {
        gConn = mdrConnectionMacCreate();
    }
    void clientPlatformDestroy()
    {
        if (gConn)
        {
            mdrConnectionMacDestroy(gConn);
            gConn = nullptr;
        }
    }
    MDRConnection* clientPlatformConnectionGet()
    {
        return gConn ? mdrConnectionMacGet(gConn) : nullptr;
    }
    int clientPlatformLocateFontBinary(const char** outData)
    {
        *outData = nullptr;
        return 0;
    }
}
