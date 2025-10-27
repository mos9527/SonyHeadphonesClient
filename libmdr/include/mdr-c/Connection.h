#pragma once
#include "Base.h"
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

#ifdef __cplusplus
extern "C" {
#endif

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
#ifdef __cplusplus
}
#endif
