/**
 * @file ConnectionMacOS.h
 * @brief C ABI for the macOS Bluetooth backend of libmdr-bt.
 */
#pragma once
#include <mdr-c/Connection.h>
#include <mdr-c/Base.h>

#ifdef __cplusplus
extern "C" {
#endif

struct MDRConnectionMacOS;
MDR_API struct MDRConnectionMacOS* mdrConnectionMacOSCreate();
MDR_API MDRConnection* mdrConnectionMacOSGet(struct MDRConnectionMacOS* conn);
MDR_API void mdrConnectionMacOSDestroy(struct MDRConnectionMacOS* conn);

#ifdef __cplusplus
}
#endif
