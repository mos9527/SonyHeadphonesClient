#pragma once
#include "Base.h"
#include "Connection.h"
/**
 * @brief Opaque pointer to @ref mdr::MDRHeadphones
 */
typedef struct MDRHeadphones MDRHeadphones;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Create a new MDRHeadphones instance.
 * @return A pointer to the new MDRHeadphones instance, or NULL on failure.
 */
MDRHeadphones* mdrHeadphonesCreate(MDRConnection*);
/**
 * @brief Destroy an MDRHeadphones instance and free its resources.
 */
void mdrHeadphonesDestroy(MDRHeadphones*);
/**
 * @breif Receive commands and process events. This is non-blocking, and should be
 *        run in - for example - your UI loop.
 * @note  This is your best friend. Use it.
 * @note  This function does not block. To not burn cycles for fun - poll on your @ref MDRConnection
 *        with @ref mdrConnectionPoll is recommended
 * @return One of MDR_HEADPHONES_* values
 */
int mdrHeadphonesPollEvents(MDRHeadphones*);
/**
 * @brief Check if @ref MDRHeadphones is ready to do more requests.
 * @return @ref MDR_RESULT_OK is true, @ref MDR_RESULT_INPROGRESS if occupied.
 */
int mdrHeadphonesRequestIsReady(MDRHeadphones*);
/**
 * @brief Send initialization payloads to the headphones.
 * @note  The official app does the same things - so you should too.
 * @return @ref MDR_RESULT_OK if scheduled, @ref MDR_RESULT_INPROGRESS if another request is in progress.
 *         In @ref mdrHeadphonesPollEvents, @ref MDR_HEADPHONES_INITIALIZED can be polled
 *         upon completion.
 */
int mdrHeadphonesRequestInit(MDRHeadphones*);
/**
 * @brief Send query payloads to the headphones, for values that don't automatically update
 *        (e.g. Battery levels)
 * @return @ref MDR_RESULT_OK if scheduled, @ref MDR_RESULT_INPROGRESS if another request is in progress.
 *         In @ref mdrHeadphonesPollEvents, @ref MDR_HEADPHONES_SYNC_COMPLETED can be polled
 *         upon completion.
 */
int mdrHeadphonesRequestSync(MDRHeadphones*);
/**
 * @brief Get a string describing the last error that occurred.
 * @return A null-terminated string describing the last error.
 */
const char* mdrHeadphonesGetLastError(MDRHeadphones*);
#ifdef __cplusplus
}
#endif
