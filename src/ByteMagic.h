#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <climits>
#include <cstdint>
#include "Constants.h"

inline unsigned int byteOrderSwap(unsigned int num)
{
    return (num >> 24) |
        ((num << 8) & 0x00FF0000) |
        ((num >> 8) & 0x0000FF00) |
        (num << 24);
}

inline unsigned short byteOrderSwap(unsigned short num)
{
    return (num >> 8) | (num << 8);
}

inline int bytesToIntBE(const unsigned char* buf) {
    return ((int)buf[3] & 0xFF) | (((int)buf[2] << 8) & 0x0000FF00) | (((int)buf[1] << 16) & 0x00FF0000) | (((int)buf[0] << 24) & 0xFF000000);
}

inline std::vector<unsigned char> intToBytesBE(unsigned int num)
{
    std::vector<unsigned char> ret(sizeof(num));
    for (size_t i = 0; i < sizeof(num); i++)
    {
        ret[i] = (num >> (CHAR_BIT * (3 - i))) & 0xff;
    }
    return ret;
}

inline long long MACStringToLong(const std::string& str)
{
    if (str.size() != MAC_ADDR_STR_SIZE)
    {
        throw std::runtime_error("Invalid MAC address size (" + str + "): " + std::to_string(str.size()) + " != " + std::to_string(MAC_ADDR_STR_SIZE));
    }

    std::istringstream iss(str);
    uint64_t b;
    uint64_t result(0);
    iss >> std::hex;
    while (iss >> b) {
        result = (result << 8) + b;
        auto eaten = iss.get();
        if (eaten != '-' && eaten != ':' && eaten != EOF)
        {
            throw std::runtime_error("Invalid MAC address format: " + str);
        }
    }
    return result;
}


constexpr size_t MAC_ADDR_BYTES_LEN = 6;
inline std::string MACBytesToString(unsigned char addr[])
{
    // Create hex string from the bytes.
    std::stringstream mac_addr_str;
    for (int i = 0; i < MAC_ADDR_BYTES_LEN; i++)
    {
        mac_addr_str << std::hex << (((addr[MAC_ADDR_BYTES_LEN - 1 - i] & 0xF0) >> 4) & 0xF);
        mac_addr_str << std::hex << (addr[MAC_ADDR_BYTES_LEN - 1 - i] & 0x0F);
        mac_addr_str << ':';
    }

    // Pop the redundant ':'
    auto ret = mac_addr_str.str();
    ret.pop_back();

    return ret;
}
