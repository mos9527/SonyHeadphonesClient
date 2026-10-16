#pragma once
#include <cstddef>
#include <cstdint>
#include <mdr-c/Connection.h>

#include "../I18N/Strings.hpp"

struct SDL_Window;
struct SDL_Renderer;

extern AppLocale clientGetAppLocale();
extern void clientSetAppLocale(AppLocale locale);
extern bool clientInit(float displayScale, const char* fontPath = nullptr);
/**
 * The worker thread run this. This will be the sole callsite for @ref mdrHeadphonesPoll()
 * @return true if the @ref MDRHeadphones state has changed.
 */
extern bool clientOnUpdate();
/**
 * Main thread, and everything UI.
 * @return true if the UI wants to be redrawn.
 */
extern bool clientOnFrame(SDL_Window* window);
extern void clientDestroy();
extern void clientSetPauseMediaOnRemove(bool enabled);
extern float clientPlatformGetDisplayScale();
extern const char* clientPlatformGetProcessorArchitecture();
/**
 * Platform specific SDL_Renderer setup
 * - Called (renderer=nullptr) before graphics init
 * - Called (renderer!=nullptr) after graphics init

 * @return true if the platform-specific graphics setup succeeded.
 
 * Currently:
 * - Apple platforms: This requests double-buffering instead of triple-buffering
 * - GL platforms (Web/Android/OHOS/Linux): This disables useless depth FBO allocation
 */
extern bool clientPlatformInitGraphics(SDL_Renderer* renderer);
/**
 * @brief Initialize platform resources before command-line parsing and SDL video initialization.
 */
extern bool clientPlatformInit();
extern bool clientPlatformWaitEventTimeout(int timeoutMS);
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
     * @param path Saved path, or suggested filename for a dispatched download/share.
     * @param error Error details on failure, or dialog error on fallback success; otherwise NULL.
     * @note Strings are valid only during the callback. It may run on a different thread.
     *       DISPATCHED only means the OS/browser accepted the handoff, not that the file was saved.
     */
    typedef void (*ClientPlatformSaveBlobCallback)(
        void* userdata, ClientPlatformSaveBlobResult result, const char* path, const char* error);
    /**
     * @brief Export bytes through the platform's save dialog, share panel, or browser download.
     * @param filename UTF-8 suggested filename, including its extension; no directory separators.
     * @param data Non-empty input bytes.
     * @param dataSize Input size in bytes.
     * @param mimeType File MIME type.
     * @param window Required parent window.
     * @param callback Called exactly once if the request is accepted; may run synchronously or on another thread.
     * @param userdata Callback context; must survive until completion.
     * @return MDR_RESULT_OK if accepted, otherwise an error code with no callback.
     * @note Call on the SDL main thread. Input strings and bytes are copied/consumed before return.
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
