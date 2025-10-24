#pragma once

#define MDR_RESULT_OK 0
#define MDR_RESULT_INPROGRESS 1
#define MDR_RESULT_ERROR_GENERAL 2
#define MDR_RESULT_ERROR_NOT_FOUND 3
#define MDR_RESULT_ERROR_TIMEOUT 4
#define MDR_RESULT_ERROR_NET 5
#define MDR_RESULT_ERROR_NO_CONNECTION 6

#define MDR_SERVICE_UUID_XM5 { 0x95, 0x6C, 0x7B, 0x26, 0xD4, 0x9A, 0x4B, 0xA8, 0xB0, 0x3F, 0xB1, 0x7D, 0x39, 0x3C, 0xB6, 0xE2 }

struct MDRDeviceInfo
{
    char szDeviceName[128];
    char szDeviceMacAddress[18];
};

struct MDRConnection
{
    void* user;

    int (*connect)(void* user, const char* macAddress, const unsigned char* uuid);
    void (*disconnect)(void* user);

    int (*recv)(void* user, char* dst, int size, int* pReceived);
    int (*send)(void* user, const char* src, int size, int* pSent);

    int (*list)(void* user, MDRDeviceInfo** ppList, int* pCount);
    int (*freeList)(void* user, MDRDeviceInfo** ppList);

    const char* (*getLastError)(void* user);
};

struct MDRHeadphone;

#ifdef __cplusplus
extern "C" {
#endif

MDRHeadphone* mdrHeadphonesCreate(MDRConnection*);

void mdrHeadphonesDestroy(MDRHeadphone*);

int mdrHeadphonesConnect(MDRHeadphone*, const char* macAddress, const unsigned char* uuid);
int mdrHeadphonesPollEvents(MDRHeadphone*);
int mdrHeadphonesRequestInit(MDRHeadphone*);
#ifdef __cplusplus
}
#endif
