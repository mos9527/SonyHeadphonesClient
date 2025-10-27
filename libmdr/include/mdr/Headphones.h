#pragma once
// MDR_RESULT...
#define MDR_RESULT_OK 0
#define MDR_RESULT_INPROGRESS 1
#define MDR_RESULT_ERROR_GENERAL 2
#define MDR_RESULT_ERROR_NOT_FOUND 3
#define MDR_RESULT_ERROR_TIMEOUT 4
#define MDR_RESULT_ERROR_NET 5
#define MDR_RESULT_ERROR_NO_CONNECTION 6
#define MDR_RESULT_ERROR_BAD_ADDRESS 7
// MDR_HEADPHONES...
#define MDR_HEADPHONES_NO_EVENT 0
#define MDR_HEADPHONES_ERROR 1
#define MDR_HEADPHONES_INITIALIZED 2
#define MDR_HEADPHONES_SYNC_COMPLETED 3
#define MDR_HEADPHONES_FEATURE_UNSUPPORTED 4
#define MDR_HEADPHONES_STATE_UPDATE 5
// Service UUIDs
// XM5s and newer
#define MDR_SERVICE_UUID_XM5 "956C7B26-D49A-4BA8-B03F-B17D393CB6E2"
/**
 * @brief Structs for your devices.
 */
typedef struct MDRDeviceInfo
{
    char szDeviceName[128];
    char szDeviceMacAddress[18];
} MDRDeviceInfo;

/**
 * @brief MDR RFCOMM Socket VTable.
 * @note  You _can_ use these as-is. But you should check out @ref mdrConnectionConnect and co first.
 */
typedef struct MDRConnection
{
    // Pointer to whatever your backend uses. See @ref MDRConnectionLinux and co for reference.
    void* user;
    // @ref mdrConnectionConnect
    int (*connect)(void* user, const char* macAddress, const char* serviceUUID);
    // @ref mdrConnectionDisconnect
    void (*disconnect)(void* user);
    // @ref mdrConnectionRecv
    int (*recv)(void* user, char* dst, int size, int* pReceived);
    // @ref mdrConnectionSend
    int (*send)(void* user, const char* src, int size, int* pSent);
    // @ref mdrConnectionPoll
    int (*poll)(void* user, int timeout);
    // @ref mdrConnectionGetDevicesList
    int (*getDevicesList)(void* user, MDRDeviceInfo** ppList, int* pCount);
    // @ref mdrConnectionFreeDevicesList
    int (*freeDevicesList)(void* user, MDRDeviceInfo** ppList);
    // @ref mdrConnectionGetLastError
    const char* (*getLastError)(void* user);
} MDRConnection;

typedef struct MDRHeadphones MDRHeadphones;

#ifdef __cplusplus
extern "C" {
#endif

#pragma region mdr[...]String
/**
 * @brief Format MDR_RESULT_... error codes as null-terminated strings
 */
const char* mdrResultString(int err);
/**
 * @brief Format MDR_HEADPHONES_... error codes as null-terminated strings
 */
const char* mdrHeadphonesString(int err);
#pragma endregion

#pragma region mdrConnection
/**
 * @brief Connect to a device using the specified MAC address and service UUID.
 * @param conn The connection object created by platform-specific backends.
 *             e.g. @ref mdrConnectionWindowsCreate, @ref mdrConnectionLinuxCreate
 * @param macAddress The MAC address of the device to connect to. e.g. "AA:BB:CC:DD:EE:FF"
 * @param serviceUUID The service UUID to connect to. e.g."956C7B26-D49A-4BA8-B03F-B17D393CB6E2" (XM5s and newer)
 * @return @ref MDR_RESULT_INPROGRESS on success - do @ref mdrConnectionPoll to wait for it to be _actually_ available.
 *         Other MDR_RESULT_... codes on error.
 */
int mdrConnectionConnect(MDRConnection* conn, const char* macAddress, const char* serviceUUID);
/**
 * @brief Disconnect from the device.
 * @param conn The connection object.
 */
void mdrConnectionDisconnect(MDRConnection* conn);
/**
 * @brief Receive data from the device.
 * @param conn The connection object.
 * @param dst Buffer to store the received data.
 * @param size The maximum number of bytes to receive.
 * @param pReceived Pointer to an integer that will be set to the number of bytes actually received.
 * @return @ref MDR_RESULT_OK on success, or an error code on failure.
 */
int mdrConnectionRecv(MDRConnection* conn, char* dst, int size, int* pReceived);
/**
 * @brief Send data to the device.
 * @param conn The connection object.
 * @param src Buffer containing the data to send.
 * @param size The number of bytes to send.
 * @param pSent Pointer to an integer that will be set to the number of bytes actually sent.
 * @return @ref MDR_RESULT_OK on success, or an error code on failure.
 */
int mdrConnectionSend(MDRConnection* conn, const char* src, int size, int* pSent);
/**
 * @brief Poll for @ref mdrConnectionSend or @ref mdrConnectionRecv to be available
 * @param conn The connection object.
 * @param timeout Non-zero for timeout in milliseconds. Zero to return immediately. Negative values to wait forever.
 * @return @ref MDR_RESULT_OK on ready, @ref MDR_RESULT_ERROR_TIMEOUT on timeout, or an error code on failure.
 */
int mdrConnectionPoll(MDRConnection* conn, int timeout);
/**
 * @brief Get a list of available devices.
 * @param conn The connection object.
 * @param ppList Pointer to a pointer that will be set to the list of devices. The caller is responsible for freeing this list with @ref mdrConnectionFreeDevicesList.
 * @param pCount Pointer to an integer that will be set to the number of devices in the list.
 * @return @ref MDR_RESULT_OK on success, or an error code on failure.
 */
int mdrConnectionGetDevicesList(MDRConnection* conn, MDRDeviceInfo** ppList, int* pCount);
/**
 * @brief Free a list of devices obtained from @ref mdrConnectionGetDevicesList.
 * @param conn The connection object.
 * @param ppList Pointer to the list of devices to free.
 * @return @ref MDR_RESULT_OK on success, or an error code on failure.
 */
int mdrConnectionFreeDevicesList(MDRConnection* conn, MDRDeviceInfo** ppList);
/**
 * @brief Get error information from the last @ref MDRConnection
 *        operation.
 *        MAY be locale dependent - but, ALWAYS return a null-terminated UTF-8 string.
 * @note  Implementations MUST guarantee to NEVER return a nullptr.
 * @return Null-terminated string describing the error.
 */
const char* mdrConnectionGetLastError(MDRConnection* conn);
#pragma endregion

#pragma region mdrHeadphones
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
 * @return One of MDR_HEADPHONES_* event types
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
#pragma endregion
#ifdef __cplusplus
}
#endif
