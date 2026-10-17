/**
 * @file ConnectionEmscripten.h
 * @brief C ABI for the Emscripten (Web Bluetooth) Bluetooth backend of libmdr-bt.
 */
#pragma once
#include <mdr-c/Connection.h>
#include <mdr-c/Base.h>

#ifdef __cplusplus
extern "C" {
#endif

struct MDRConnectionEmscripten;
MDR_API struct MDRConnectionEmscripten* mdrConnectionEmscriptenCreate();
MDR_API MDRConnection* mdrConnectionEmscriptenGet(struct MDRConnectionEmscripten* conn);
MDR_API void mdrConnectionEmscriptenDestroy(struct MDRConnectionEmscripten* conn);

#ifdef __cplusplus
}
#endif
