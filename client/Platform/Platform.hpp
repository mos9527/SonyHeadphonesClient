#pragma once
#include <cstddef>
#include <cstdint>
#include <mdr-c/Connection.h>

#include "../I18N/Strings.hpp"

struct SDL_Window;

extern AppLocale clientGetAppLocale();
extern void clientSetAppLocale(AppLocale locale);
extern bool clientInit(float displayScale, const char* fontPath = nullptr);
// Runs on the worker thread on native platforms, or the main thread on Emscripten.
// - Polls non-blocking device I/O and updates device-reported state.
// - Schedules commits of staged property edits and synchronization requests.
// - Advances discovery/connection attempts and handles semantic events and errors.
// SDLMain serializes this with frame preparation and clientOnFrame() using a client-state mutex.
// Returns true if client state visible to the UI has changed.
extern bool clientOnUpdate();
// Always runs on the main thread; builds the UI and ImGui draw data, loads fonts, and draws the debugger.
// SDL backend NewFrame calls, rendering, and presentation are handled separately by SDLMain.
// - Normal property setters only stage local edits for clientOnUpdate() to commit.
// - Debugger sends start a raw-command task and queue TX bytes, invoking packet observers synchronously;
//   actual transport send/receive and task completion are driven by subsequent polling.
// - UI connection/discovery actions can call the platform connection backend directly.
// @returns: true if the UI wants to be redrawn.
extern bool clientOnFrame();
extern void clientDestroy();
extern void clientSetPauseMediaOnRemove(bool enabled);
extern float clientPlatformGetDisplayScale();
#ifdef MDR_CLIENT_DEBUGGER
extern void clientEnterDebuggerReplayMode();
#endif

extern "C" {
    /**
     * @brief Select and initialize the platform Bluetooth backend.
     *
     * The client is responsible for picking the backend; libmdr-bt only exposes
     * the per-platform entry points (e.g. mdrConnectionWindowsCreate /
     * mdrConnectionWindowsBLECreate). This dispatches to the matching one.
     * @param flags One or more MDR_INIT_* flags (e.g. MDR_INIT_BT_BLE).
     * @return MDR_RESULT_OK on success, or an error code (e.g. NOT_SUPPORTED).
     */
    extern int clientPlatformConnectionInit(int flags);
    extern MDRConnection* clientPlatformConnectionGet();
    extern void clientPlatformConnectionDestroy();

    /**
     * @brief Locate platform-specific font binary data.
     * @param locale Application locale used to select the default font.
     * @param outData Required output pointer. Platform-owned until clientPlatformDestroy().
     * @param outFaceIndex Required output pointer for the TTF/OTF/TTC face index.
     * @return Size in bytes, 0 if unavailable, or -1 for in-progress IO
     */
    extern int clientPlatformLocateFontBinary(AppLocale locale, const char** outData, int* outFaceIndex);
    /**
     * @brief Map a whole file read-only into memory.
     * @param path UTF-8 file path.
     * @param outAddr Required output pointer. Receives the base address, or NULL on failure.
     *                Pages are read-only; writing to them is undefined.
     * @param outSize Required output pointer. Receives the file size in bytes, or 0 on failure.
     * @return MDR_RESULT_OK, or an error code. Empty files are rejected.
     *         MDR_RESULT_ERROR_NOT_SUPPORTED on Emscripten.
     */
    extern int clientPlatformMemoryMapFile(const char* path, void** outAddr, size_t* outSize);
    /**
     * @brief Release a mapping from @ref clientPlatformMemoryMapFile. NULL is accepted and ignored.
     * @param size The size returned alongside @p addr.
     */
    extern void clientPlatformMemoryUnmapFile(void* addr, size_t size);
    /**
     * @brief Whether a Bluetooth address belongs to one of this computer's own adapters.
     * @param address Text form, "XX:XX:XX:XX:XX:XX", any case.
     * @param outIsLocal Receives 1 or 0.
     * @return MDR_RESULT_OK, or MDR_RESULT_ERROR_NOT_SUPPORTED where the platform cannot tell.
     * @note This is only implemented for the Linux platform. See https://github.com/mos9527/SonyHeadphonesClient/pull/63
     */
    extern int clientPlatformIsLocalBluetoothAddress(const char* address, int* outIsLocal);
    /**
     * @brief Pause every media player on this computer that is currently playing.
     * @return A record of what was paused, owned by the caller and handed back to
     *         @ref clientPlatformMediaResume, or NULL when nothing was playing or the platform
     *         has no media control.
     * @note This is only implemented for the Linux platform. See https://github.com/mos9527/SonyHeadphonesClient/pull/63
     */
    struct ClientMediaPause;
    extern struct ClientMediaPause* clientPlatformMediaPause();
    /**
     * @brief Resume the players in a @ref clientPlatformMediaPause record that are still paused,
     *        and free the record. NULL is accepted and ignored.
     * @note This is only implemented for the Linux platform. See https://github.com/mos9527/SonyHeadphonesClient/pull/63
     */
    extern void clientPlatformMediaResume(struct ClientMediaPause* pause);
    enum ClientPlatformSaveBlobResult
    {
        CLIENT_PLATFORM_SAVE_BLOB_SAVED,
        CLIENT_PLATFORM_SAVE_BLOB_DISPATCHED,
        CLIENT_PLATFORM_SAVE_BLOB_CANCELLED,
        CLIENT_PLATFORM_SAVE_BLOB_FAILED,
        CLIENT_PLATFORM_SAVE_BLOB_FALLBACK_SAVED
    };
    /**
     * @brief Completion callback for saving bytes.
     * @param path Saved path, or suggested filename for a dispatched download.
     * @param error Error details on failure, or dialog error on fallback success; otherwise NULL.
     * @note Strings are valid only during the callback. It may run on a different thread.
     *       DISPATCHED only means the OS/browser accepted the handoff, not that the file was saved.
     */
    typedef void (*ClientPlatformSaveBlobCallback)(
        void* userdata, ClientPlatformSaveBlobResult result, const char* path, const char* error);
    /**
     * @brief Save bytes using the platform's file dialog, download, or sharing integration.
     * @param filename Required UTF-8 suggested filename, including its extension.
     * @param data Required input bytes; empty blobs are rejected.
     * @param mimeType Required MIME type; desktop filters use the filename extension.
     * @param window Optional parent window. Desktop saves directly to filename when NULL.
     * @param callback Required completion callback, called exactly once for an accepted request.
     * @param userdata Caller-owned callback context; must survive until completion.
     * @return MDR_RESULT_OK if accepted, or an error code with no callback.
     * @note Input strings and bytes are copied/consumed before return. Completion may be synchronous.
     *       Android, iOS and OpenHarmony currently return MDR_RESULT_ERROR_NOT_SUPPORTED.
     */
    extern int clientPlatformSaveBlob(
        const char* filename,
        const unsigned char* data,
        size_t dataSize,
        const char* mimeType,
        SDL_Window* window,
        ClientPlatformSaveBlobCallback callback,
        void* userdata);
    /**
     * @brief Shared SDL implementation in SDLMain.cpp; desktop platform entry points delegate here.
     * @note Defined only when the client is compiled with SDL_DIALOG.
     */
    extern int clientSDLSaveBlob(
        const char* filename,
        const unsigned char* data,
        size_t dataSize,
        const char* mimeType,
        SDL_Window* window,
        ClientPlatformSaveBlobCallback callback,
        void* userdata);
    /**
     * @brief Master clean up function.
     * This will destroy all connections, and ensures the client is quit without leaking resources.
     */
    extern void clientPlatformDestroy();
}
