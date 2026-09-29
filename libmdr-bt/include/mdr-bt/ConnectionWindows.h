/**
 * @file ConnectionWindows.h
 * @brief C ABI for the Windows (WinRT) Bluetooth backend of libmdr-bt.
 */
#pragma once
#include <mdr-c/Connection.h>
#include <mdr-c/Base.h>

#ifdef __cplusplus
extern "C" {
#endif

// Classic (RFCOMM/SDP) Windows connection.
struct MDRConnectionWindows;
MDR_API struct MDRConnectionWindows* mdrConnectionWindowsCreate();
MDR_API MDRConnection* mdrConnectionWindowsGet(struct MDRConnectionWindows* conn);
MDR_API void mdrConnectionWindowsDestroy(struct MDRConnectionWindows* conn);

#ifdef MDR_BLE
// Bluetooth Low Energy Windows connection.
struct MDRConnectionWindowsBLE;
MDR_API struct MDRConnectionWindowsBLE* mdrConnectionWindowsBLECreate();
MDR_API MDRConnection* mdrConnectionWindowsBLEGet(struct MDRConnectionWindowsBLE* conn);
MDR_API void mdrConnectionWindowsBLEDestroy(struct MDRConnectionWindowsBLE* conn);
#endif

#ifdef __cplusplus
}
#endif
