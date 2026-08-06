/**
 * @file ConnectionLinux.h
 * @brief C ABI for the Linux (BlueZ/DBus) Bluetooth backend of libmdr-bt.
 */
#pragma once
#include <mdr-c/Connection.h>
#include <mdr-c/Base.h>

#ifdef __cplusplus
extern "C" {
#endif

struct MDRConnectionLinux;
MDR_API struct MDRConnectionLinux* mdrConnectionLinuxCreate();
MDR_API MDRConnection* mdrConnectionLinuxGet(struct MDRConnectionLinux* conn);
MDR_API void mdrConnectionLinuxDestroy(struct MDRConnectionLinux* conn);

#ifdef __cplusplus
}
#endif
