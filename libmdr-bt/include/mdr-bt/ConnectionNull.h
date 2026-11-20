// NOTE: Available with CMake option MDR_BT_NULL_BLUETOOTH
// This builds a stub BT backend with no bluetooth support.
#pragma once
#include <mdr-c/Connection.h>

#ifdef __cplusplus
extern "C" {
#endif

// Classic Bluetooth stub connection.
struct MDRConnectionNull;
MDR_API struct MDRConnectionNull* mdrConnectionNullCreate(void);
MDR_API MDRConnection* mdrConnectionNullGet(struct MDRConnectionNull* conn);
MDR_API void mdrConnectionNullDestroy(struct MDRConnectionNull* conn);

#ifdef MDR_BT_BLE_SUPPORT
// Bluetooth Low Energy stub connection.
struct MDRConnectionNullBLE;
MDR_API struct MDRConnectionNullBLE* mdrConnectionNullBLECreate(void);
MDR_API MDRConnection* mdrConnectionNullBLEGet(struct MDRConnectionNullBLE* conn);
MDR_API void mdrConnectionNullBLEDestroy(struct MDRConnectionNullBLE* conn);
#endif

#ifdef __cplusplus
}
#endif
