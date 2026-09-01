#pragma once
#include <cstddef>
#include <mdr-c/Connection.h>

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
     * @breif Locate platform-specific font binary data
     * @param outData Pointer to output font data. Must not be freed by caller.
     * @return Size of font data in bytes, 0 if not available - can be retried.
     */
    extern int clientPlatformLocateFontBinary(const char** outData);
#ifdef __EMSCRIPTEN__
    /**
     * @brief Download bytes through the browser.
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
