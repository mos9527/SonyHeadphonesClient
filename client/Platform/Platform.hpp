#pragma once
#include <mdr-c/Connection.h>

extern "C" {
    extern void clientPlatformInit();
    extern MDRConnection* clientPlatformConnectionGet();
    // Platform data
    /**
     * @breif Locate platform-specific font binary data
     * @param outData Pointer to output font data. Must not be freed by caller.
     * @return Size of font data in bytes, 0 if not available - can be retried.
     */
    extern int clientPlatformLocateFontBinary(const char** outData);
    extern void clientPlatformDestroy();
    // Switch between Bluetooth Classic (RFCOMM) and Bluetooth LE (GATT) backends
    extern void clientPlatformSetUseBLE(bool useBLE);
    extern bool clientPlatformGetUseBLE();
    // Enumerate GATT services/characteristics on a BLE device (for discovery)
    extern int clientPlatformBLEEnumerateGatt(const char* macAddress);
}
