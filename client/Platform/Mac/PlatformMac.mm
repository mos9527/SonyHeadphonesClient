#import <AppKit/AppKit.h>
#include "../Platform.hpp"
#include <mdr-c/Platform/PlatformMac.h>

MDRConnectionMac* gConn;
extern "C" {
    void clientPlatformInit()
    {
        gConn = mdrConnectionMacCreate();
    }
    void clientPlatformDestroy()
    {
        mdrConnectionMacDestroy(gConn);
    }
    MDRConnection* clientPlatformConnectionGet()
    {
        return mdrConnectionMacGet(gConn);
    }
    int clientPlatformLocateFontBinary(const char** outData)
    {
        *outData = nullptr;
        return 0;
    }
}
