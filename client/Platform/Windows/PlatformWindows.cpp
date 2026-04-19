#include "../Platform.hpp"
#include <mdr/Protocol.hpp>
#include <mdr-c/Platform/PlatformWindows.h>
#include <mdr-c/Platform/PlatformWindowsBLE.h>

static MDRConnectionWindows* gConnClassic = nullptr;
static MDRConnectionWindowsBLE* gConnBLE = nullptr;
extern "C" {
    int clientPlatformConnectionInit(int flags)
    {
        MDR_CHECK_MSG(gConnBLE == nullptr && gConnClassic == nullptr, "Platform already initialized. You MUST call clientPlatformDestroy() before initializing again.");
        if (flags & MDR_INIT_BT_BLE)
            gConnBLE = mdrConnectionWindowsBLECreate(), gConnClassic = nullptr;
        else
            gConnClassic = mdrConnectionWindowsCreate(), gConnBLE = nullptr;
        return MDR_RESULT_OK;
    }

    void clientPlatformConnectionDestroy()
    {
        if (gConnClassic)
            mdrConnectionWindowsDestroy(gConnClassic), gConnClassic = nullptr;
        if (gConnBLE)
            mdrConnectionWindowsBLEDestroy(gConnBLE), gConnBLE = nullptr;
    }

    MDRConnection* clientPlatformConnectionGet()
    {
        if (gConnClassic)
            return mdrConnectionWindowsGet(gConnClassic);
        if (gConnBLE)
            return mdrConnectionWindowsBLEGet(gConnBLE);
        [[unlikely]] return nullptr;
    }

    int clientPlatformLocateFontBinary(const char** outData)
    {
        // TODO
        *outData = nullptr;
        return 0;
    }
    void clientPlatformDestroy()
    {
        clientPlatformConnectionDestroy();
        // TODO
    }
}
