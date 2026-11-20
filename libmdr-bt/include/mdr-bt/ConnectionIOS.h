/**
 * @file ConnectionIOS.h
 * @brief C ABI for the iOS (External Accessory) Bluetooth backend of libmdr-bt.
 * @note  The host app's Info.plist must list `jp.co.sony.songpal.mdr.link` and `jp.co.sony.songpal.mdr.link2`
 *        in `UISupportedExternalAccessoryProtocols`. Device addresses are derived from the accessory's model and
 *        serial numbers, and are not Bluetooth MAC addresses.
 */
#pragma once
#include <mdr-c/Connection.h>
#include <mdr-c/Base.h>

#ifdef __cplusplus
extern "C" {
#endif

struct MDRConnectionIOS;
MDR_API struct MDRConnectionIOS* mdrConnectionIOSCreate();
MDR_API MDRConnection* mdrConnectionIOSGet(struct MDRConnectionIOS* conn);
MDR_API void mdrConnectionIOSDestroy(struct MDRConnectionIOS* conn);

#ifdef __cplusplus
}
#endif
