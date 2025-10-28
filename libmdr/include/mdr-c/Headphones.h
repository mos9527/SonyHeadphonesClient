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
 * @brief Send initialization payloads to the device.
 * @note  The official app does the same things - so you should too.
 * @return @ref MDR_RESULT_OK if scheduled, @ref MDR_RESULT_INPROGRESS if another request is in progress.
 *         In @ref mdrHeadphonesPollEvents, @ref MDR_HEADPHONES_TASK_INIT_OK can be polled
 *         upon completion.
 */
int mdrHeadphonesRequestInit(MDRHeadphones*);
/**
 * @brief Send query payloads to the device, for values that don't automatically update
 *        (e.g. Battery levels)
 * @return @ref MDR_RESULT_OK if scheduled, @ref MDR_RESULT_INPROGRESS if another request is in progress.
 *         In @ref mdrHeadphonesPollEvents, @ref MDR_HEADPHONES_TASK_SYNC_OK can be polled
 *         upon completion.
 */
int mdrHeadphonesRequestSync(MDRHeadphones*);
/**
 * @brief Sends all changed properties up until this point for them to be set on the device.
 * @return @ref MDR_RESULT_OK if scheduled, @ref MDR_RESULT_INPROGRESS if another request is in progress.
 *         In @ref mdrHeadphonesPollEvents, @ref MDR_HEADPHONES_TASK_COMMIT_OK can be polled
 *         upon completion.
 */
int mdrHeadphonesRequestCommit(MDRHeadphones*);
/**
 * @brief Checks if there's any property to be set.
 * @return @ref MDR_RESULT_OK if not, @ref MDR_RESULT_INPROGRESS if there's anything that
 *         should be committed with @ref mdrHeadphonesRequestCommit
 */
int mdrHeadphonesIsDirty(MDRHeadphones*);
/**
 * TODO:
 *  Figure out how we can expose the bajillion of Headphone @ref MDRProperty to the C interface.
 *  And basically this _sucks_. Almost all of the properties has their own bespoke structs.
 *  Thus a monolithic export isn't an option (types!) - so codegen every single one of those
 *  into their own exports?
 *  Someone (my future self) planning to make FFI bindings will have to figure this out.
 */
/**
 * @brief Get a string describing the last error that occurred.
 * @return A null-terminated string describing the last error.
 */
const char* mdrHeadphonesGetLastError(MDRHeadphones*);
#ifdef __cplusplus
}
#endif
