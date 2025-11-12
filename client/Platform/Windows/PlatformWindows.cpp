#include "../Platform.hpp"
#include <mdr-c/Platform/PlatformWindows.h>

MDRConnectionWindows* gConn;
extern "C" {
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
    int clientPlatformLocateFontBinary(const char** outData)
    {
        // TODO
        *outData = nullptr;
        return 0;
    }
}