#pragma once
#include <stdint.h>

#define MDR_ABI_VERSION 5u
//      ^^^^^^^^^^^^^^^ ABI version of the header.
// To Developers: Blame this line for history ABI changes. Change this line if *you* want to break ABI.
// See DEVELOPER.md for more details.

#if !defined(MDR_API)
#if defined(_WIN32)
#if defined(MDR_BUILDING_SHARED)
#define MDR_API __declspec(dllexport)
#elif defined(MDR_USE_SHARED)
#define MDR_API __declspec(dllimport)
#else
#define MDR_API
#endif
#elif defined(__GNUC__) || defined(__clang__)
#define MDR_API __attribute__((visibility("default")))
#else
#define MDR_API
#endif
#endif


/**
 * @brief Result type used by the C/C++ API.
 */
typedef uint32_t MDRResult;

// MDR_INIT...
#define MDR_INIT_BT_BLE (1u << 0u) // Use BLE profile instead of Classic bluetooth
// MDR_RESULT...
#define MDR_RESULT_OK 0
#define MDR_RESULT_INPROGRESS 1
#define MDR_RESULT_ERROR_GENERAL 2
#define MDR_RESULT_ERROR_NOT_FOUND 3
#define MDR_RESULT_ERROR_TIMEOUT 4
#define MDR_RESULT_ERROR_NET 5
#define MDR_RESULT_ERROR_NO_CONNECTION 6
#define MDR_RESULT_ERROR_BAD_ADDRESS 7
#define MDR_RESULT_ERROR_NOT_SUPPORTED 8
#define MDR_RESULT_ERROR_BUFFER_TOO_SMALL 9
#define MDR_RESULT_ERROR_MALFORMED_PAYLOAD 10
#define MDR_RESULT_ERROR_INVALID_ARGUMENT 11
#define MDR_RESULT_ERROR_ABI_MISMATCH 12
// Service UUIDs
// MDR V2, XM5s and newer (SDP service UUID)
#define MDR_SERVICE_UUID_V2 "956C7B26-D49A-4BA8-B03F-B17D393CB6E2"
// MDR V1, XM4s and older (SDP service UUID)
#define MDR_SERVICE_UUID_V1 "96CC203E-5068-46AD-B32D-E316F5E069BA"
// XM5s and newer (BLE GATT service UUID)
#define MDR_SERVICE_UUID_BLE "5B833E20-6BC7-4802-8E9A-723CECA4BD8F"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Format MDR_RESULT_... error codes as null-terminated strings
 */
MDR_API const char* mdrResultString(MDRResult err);
#ifdef __cplusplus
}
#endif
