#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>

#define MDR_DEFAULT_ERROR_STRING "Nothing to see here"

/**
 * @brief Converts UUID string to byte array
 * @param szSrc Zero-terminated UUID string e.g. 956C7B26-D49A-4BA8-B03F-B17D393CB6E2
 * @param dst 16-byte buffer to store the UUID in bytes
 *            Expect 0x95, 0x6C, 0x7B, 0x26, 0xD4, 0x9A, 0x4B, 0xA8, 0xB0, 0x3F, 0xB1, 0x7D, 0x39, 0x3C, 0xB6, 0xE2 for the example.
 * @return 0 on success, -1 on failure.
 */
inline int serviceUUIDtoBytes(const char* szSrc, uint8_t* dst)
{
    if (strlen(szSrc) != 36)
        return -1;
    unsigned int buf[16];
    int ret = sscanf(szSrc,
                     "%2x%2x%2x%2x-%2x%2x-%2x%2x-%2x%2x-%2x%2x%2x%2x%2x%2x",
                     &buf[0], &buf[1], &buf[2], &buf[3],
                     &buf[4], &buf[5], &buf[6], &buf[7],
                     &buf[8], &buf[9], &buf[10], &buf[11],
                     &buf[12], &buf[13], &buf[14], &buf[15]);

    if (ret != 16)
        return -1;
    for (int i = 0; i < 16; ++i)
        dst[i] = static_cast<uint8_t>(buf[i]);
    return 0;
}

/**
* @breif Converts a column-seperated Mac address (00:01:02:03:04:05) into a 8-byte int,
*        in little-endian.
* @returns For the example, in LE: 0x0000000102030405
*/
inline unsigned long long macAddressToULL(const char* szMacAddres) {
    unsigned int buf[6];
    int ret =
        sscanf(szMacAddres, "%2x:%2x:%2x:%2x:%2x:%2x", &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5]);
    if (ret != 6)
        return ~0ULL;
    union
    {
        unsigned long long ULL;
        unsigned char b[8];
    } u;
    for (int i = 0; i < 6; i++)
        u.b[i] = static_cast<unsigned char>(buf[i]);
    return u.ULL;
}
