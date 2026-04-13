#include "../Platform.hpp"
#include <mdr-c/Platform/PlatformWindows.h>
#include <mdr-c/Platform/PlatformWindowsBLE.h>
#include <cstdio>

static MDRConnectionWindows* gConnClassic = nullptr;
static MDRConnectionWindowsBLE* gConnBLE = nullptr;
static bool gUseBLE = false;

// Callback that logs GATT enumeration results to stderr
static void gattEnumCallback(void* ctx, const char* serviceUUID, const char* charUUID, int properties)
{
    (void)ctx;
    (void)serviceUUID;
    (void)charUUID;
    (void)properties;
    // Logging is already handled inside PlatformWindowsBLE.cpp
}

extern "C" {
    void clientPlatformInit()
    {
        gConnClassic = mdrConnectionWindowsCreate();
        gConnBLE = mdrConnectionWindowsBLECreate();
        fprintf(stderr, "[CLIENT] Platform initialized (Classic + BLE backends)\n");
    }

    void clientPlatformDestroy()
    {
        mdrConnectionWindowsDestroy(gConnClassic);
        mdrConnectionWindowsBLEDestroy(gConnBLE);
        gConnClassic = nullptr;
        gConnBLE = nullptr;
        fprintf(stderr, "[CLIENT] Platform destroyed\n");
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
            fprintf(stderr, "[CLIENT] Switching to %s backend\n", useBLE ? "BLE" : "Classic");
            gUseBLE = useBLE;
        }
    }

    bool clientPlatformGetUseBLE()
    {
        return gUseBLE;
    }

    int clientPlatformBLEEnumerateGatt(const char* macAddress)
    {
        fprintf(stderr, "[CLIENT] BLE GATT enumeration requested for %s\n", macAddress);
        return mdrConnectionBLEEnumerateGatt(gConnBLE, macAddress, gattEnumCallback, nullptr);
    }

    int clientPlatformLocateFontBinary(const char** outData)
    {
        // TODO
        *outData = nullptr;
        return 0;
    }
}
