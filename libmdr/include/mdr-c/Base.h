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
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Format MDR_RESULT_... error codes as null-terminated strings
 */
const char* mdrResultString(int err);
/**
 * @brief Format MDR_HEADPHONES_... error codes as null-terminated strings
 */
const char* mdrHeadphonesString(int err);
#ifdef __cplusplus
}
#endif
