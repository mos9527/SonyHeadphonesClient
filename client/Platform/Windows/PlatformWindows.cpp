#include "../Platform.hpp"
#include <mdr/Protocol.hpp>
#include <mdr-c/Platform/PlatformWindows.h>
#include <mdr-c/Platform/PlatformWindowsBLE.h>

static MDRConnectionWindows* gConnClassic = nullptr;
static MDRConnectionWindowsBLE* gConnBLE = nullptr;
static bool gUseBLE = false;

extern "C" {
    void clientPlatformInit()
    {
        gConnClassic = mdrConnectionWindowsCreate();
        gConnBLE = mdrConnectionWindowsBLECreate();
        MDR_LOG("[CLIENT] Platform initialized (Classic + BLE backends)");
    }

    void clientPlatformDestroy()
    {
        mdrConnectionWindowsDestroy(gConnClassic);
        mdrConnectionWindowsBLEDestroy(gConnBLE);
        gConnClassic = nullptr;
        gConnBLE = nullptr;
        MDR_LOG("[CLIENT] Platform destroyed");
    }

    MDRConnection* clientPlatformConnectionGet()
    {
        if (gUseBLE)
            return mdrConnectionWindowsBLEGet(gConnBLE);
        return mdrConnectionWindowsGet(gConnClassic);
    }

    void clientPlatformSetUseBLE(bool useBLE)
    {
        if (gUseBLE != useBLE)
        {
            MDR_LOG("[CLIENT] Switching to {} backend", useBLE ? "BLE" : "Classic");
            gUseBLE = useBLE;
        }
    }

    bool clientPlatformGetUseBLE()
    {
        return gUseBLE;
    }

    int clientPlatformBLEEnumerateGatt(const char* macAddress)
    {
        MDR_LOG("[CLIENT] BLE GATT enumeration requested for {}", macAddress);
        return mdrConnectionBLEEnumerateGatt(gConnBLE, macAddress, nullptr, nullptr);
    }

    int clientPlatformLocateFontBinary(const char** outData)
    {
        // TODO
        *outData = nullptr;
        return 0;
    }
}
