#include "../Platform.hpp"
#include <mdr-c/Platform/PlatformMacOS.h>

MDRConnectionMacOS* gConn;
extern "C" {
    void clientPlatformInit()
    {
        gConn = mdrConnectionMacOSCreate();
    }
    void clientPlatformDestroy()
    {
        mdrConnectionMacOSDestroy(gConn);
    }
    MDRConnection* clientPlatformConnectionGet()
    {
        return mdrConnectionMacOSGet(gConn);
    }
    int clientPlatformLocateFontBinary(const char** outData)
    {
        *outData = nullptr;
        return 0;
    }
    void clientPlatformSetUseBLE(bool) {}
    bool clientPlatformGetUseBLE() { return false; }
    int clientPlatformBLEEnumerateGatt(const char*) { return -1; }
}
