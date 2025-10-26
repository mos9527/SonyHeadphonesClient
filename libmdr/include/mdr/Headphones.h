#pragma once

#define MDR_RESULT_OK 0
#define MDR_RESULT_INPROGRESS 1
#define MDR_RESULT_ERROR_GENERAL 2
#define MDR_RESULT_ERROR_NOT_FOUND 3
#define MDR_RESULT_ERROR_TIMEOUT 4
#define MDR_RESULT_ERROR_NET 5
#define MDR_RESULT_ERROR_NO_CONNECTION 6
#define MDR_RESULT_ERROR_BAD_ADDRESS 7

#define MDR_SERVICE_UUID_XM5 "956C7B26-D49A-4BA8-B03F-B17D393CB6E2"

typedef struct MDRDeviceInfo
{
    char szDeviceName[128];
    char szDeviceMacAddress[18];
} MDRDeviceInfo;

typedef struct MDRConnection
{
    void* user;

    int (*connect)(void* user, const char* macAddress, const char* serviceUUID);
    void (*disconnect)(void* user);

    int (*recv)(void* user, char* dst, int size, int* pReceived);
    int (*send)(void* user, const char* src, int size, int* pSent);

    int (*list)(void* user, MDRDeviceInfo** ppList, int* pCount);
    int (*freeList)(void* user, MDRDeviceInfo** ppList);
    /**
     * @brief Get error information from the last @ref MDRConnection
     *        operation.
     *        MAY be locale dependent - but, ALWAYS return a null-terminated UTF-8 string.
     * @note  Implementations MUST guarantee to NEVER return a nullptr.
     */
    const char* (*getLastError)(void* user);
} MDRConnection;

typedef struct MDRHeadphones MDRHeadphones;

#define MDR_HEADPHONES_NO_EVENT 0

#ifdef __cplusplus
extern "C" {
#endif

const char* mdrResultString(int err);

MDRHeadphones* mdrHeadphonesCreate(MDRConnection*);
void mdrHeadphonesDestroy(MDRHeadphones*);

const char* mdrHeadphonesEventString(int evt);
/**
 * @breif Receive commands and process events. This is non-blocking, and should be
 *        run in a tight loop.
 * @return One of MDR_HEADPHONES_* event types
 */
int mdrHeadphonesPollEvents(MDRHeadphones*);
int mdrHeadphonesRequestInit(MDRHeadphones*);

const char* mdrHeadphonesGetLastError(MDRHeadphones*);
#ifdef __cplusplus
}
#endif
