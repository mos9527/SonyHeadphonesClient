#pragma once
#include <cstddef>
#include <mdr-c/Connection.h>

#include "../I18N/Strings.hpp"

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
#ifdef __EMSCRIPTEN__
    /**
     * @brief Download bytes through the browser. Emscripten only.
     * @return Non-zero when the browser download was started.
     */
    extern int clientPlatformDownloadFile(
        const char* filename,
        const unsigned char* data,
        size_t dataSize,
        const char* mimeType);
#endif
    /**
     * @brief Master clean up function.
     * This will destroy all connections, and ensures the client is quit without leaking resources.
     */
    extern void clientPlatformDestroy();
}
