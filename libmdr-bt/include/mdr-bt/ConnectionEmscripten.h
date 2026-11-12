/**
 * @file ConnectionEmscripten.h
 * @brief C ABI for the Emscripten Web Serial / Bluetooth RFCOMM backend of libmdr-bt.
 * @note Poll is non-blocking regardless of its timeout argument. Disconnect/destroy
 *       detach the session immediately and finish closing the port asynchronously.
 *       Reopening waits for the previous close; selecting a port requires a user gesture.
 * @note Send reports bytes accepted into the backend, not delivery to the device.
 *       Only one bounded write is accepted at a time; a write pending for 10 seconds
 *       fails the session with MDR_RESULT_ERROR_NET (delivery is unknown).
 *       Poll's MDR_RESULT_ERROR_TIMEOUT is reserved for temporarily unavailable I/O.
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
