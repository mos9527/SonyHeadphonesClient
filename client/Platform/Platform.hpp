#pragma once
#include <mdr-c/Connection.h>

extern "C" {
    /**
     * @brief Initializes platform MDRConnection* backend
     * @param flags Bit flags combinations of @ref MDR_INIT.., e.g. @ref MDR_INIT_BT_BLE
     * @return @ref MDR_RESULT_OK on success, or MDR_RESULT_ERROR_NOT_SUPPORTED if the requested configuration is not supported on this platform.
     */
    extern int clientPlatformConnectionInit(int flags);
    /**
     * @breif Get the current platform specific connection backend.
     * @note
     * @return
     */
    extern MDRConnection* clientPlatformConnectionGet();
    /**
     * @breif Locate platform-specific font binary data
     * @param outData Pointer to output font data. Must not be freed by caller.
     * @return Size of font data in bytes, 0 if not available - can be retried.
     */
    extern int clientPlatformLocateFontBinary(const char** outData);
    /**
     * @brief Destroys the current, active underlying connection.
     */
    extern void clientPlatformConnectionDestroy();
    /**
     * @brief Master clean up function.
     * This will destroy all connections, and ensures the client is quit without leaking resources.
     */
    extern void clientPlatformDestroy();
}
