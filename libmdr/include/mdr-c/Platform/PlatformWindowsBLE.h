#pragma once
#include "../Connection.h"

typedef struct MDRConnectionWindowsBLE MDRConnectionWindowsBLE;

// Callback for GATT service/characteristic enumeration.
// Called once per characteristic found on the device.
// properties is a bitmask: 1=Read, 2=Write, 4=WriteWithoutResponse, 8=Notify, 16=Indicate
typedef void (*MDRBLEGattEnumCallback)(
    void* ctx,
    const char* serviceUUID,
    const char* characteristicUUID,
    int properties
);

#ifdef __cplusplus
extern "C" {
#endif

MDRConnectionWindowsBLE* mdrConnectionWindowsBLECreate();
MDRConnection* mdrConnectionWindowsBLEGet(MDRConnectionWindowsBLE*);
void mdrConnectionWindowsBLEDestroy(MDRConnectionWindowsBLE*);

// Enumerate all GATT services and characteristics on a BLE device.
// Results are delivered via callback and also logged to stderr.
// macAddress: "XX:XX:XX:XX:XX:XX" format
// Returns MDR_RESULT_OK on success, error code on failure.
int mdrConnectionBLEEnumerateGatt(
    MDRConnectionWindowsBLE* pConn,
    const char* macAddress,
    MDRBLEGattEnumCallback callback,
    void* ctx
);

#ifdef __cplusplus
}
#endif
