#pragma once

#include <span>
#include <stdexcept>
#include <string>
#include <vector>

inline std::string readPrefixedString(std::span<const uint8_t>& buf)
{
    if (buf.empty())
        throw std::runtime_error("Buffer too small for string length");

    uint8_t len = buf[0];
    if (len > 128)
        throw std::runtime_error("String length too large");

    if (buf.size() < 1 + len)
        throw std::runtime_error("Buffer too small for string content");

    std::string result(reinterpret_cast<const char*>(buf.data() + 1), len);
    buf = buf.subspan(1 + len);
    return result;
}

inline void writePrefixedString(std::vector<uint8_t>& buf, const std::string& str)
{
    if (str.size() > 128)
        throw std::runtime_error("String length too large");

    buf.push_back(static_cast<uint8_t>(str.size()));
    buf.insert(buf.end(), str.begin(), str.end());
}
