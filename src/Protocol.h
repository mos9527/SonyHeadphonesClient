#pragma once

#include <cstring>
#include <memory>
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

#define VARIABLE_SIZE_PAYLOAD_ONE_ARRAY_AT_END(elementType, maxSize) \
    void* operator new(size_t baseSize, size_t extraBytes) \
    { \
        return ::operator new(baseSize + extraBytes); \
    } \
    void operator delete(void* p) noexcept \
    { \
        ::operator delete(p); \
    } \
    void operator delete(void* p, size_t) noexcept \
    { \
        ::operator delete(p); \
    } \
    using TEndArrayElement = elementType; \
    static constexpr size_t ARRAY_AT_END_MAX_SIZE = maxSize; \
    static constexpr bool VARIABLE_SIZE_ONE_ARRAY_AT_END = true; \
    template <typename TPayload, typename... TArgs> \
    friend std::unique_ptr<TPayload> (::createVariableSizePayloadOneArrayAtEnd)( \
        size_t* outSize, const std::span<const typename TPayload::TEndArrayElement>& elements, TArgs&&... args); \
    template <typename TPayload, typename... TArgs> \
    friend std::unique_ptr<TPayload> (::createVariableSizePayloadOneArrayAtEnd_CommandType)( \
        size_t* outSize, CommandType ct, const std::span<const typename TPayload::TEndArrayElement>& elements, TArgs&&... args);

template <typename TPayload, typename... TArgs>
std::unique_ptr<TPayload> createVariableSizePayloadOneArrayAtEnd(
    size_t* outSize, const std::span<const typename TPayload::TEndArrayElement>& elements, TArgs&&... args)
{
    if constexpr (!TPayload::VARIABLE_SIZE_ONE_ARRAY_AT_END)
        static_assert(sizeof(TPayload) == 0, "TPayload must be a variable size payload");
    if (elements.size() > TPayload::ARRAY_AT_END_MAX_SIZE)
        throw std::runtime_error("Too many elements for variable size payload");
    *outSize = sizeof(TPayload) + sizeof(typename TPayload::TEndArrayElement) * elements.size();
    size_t extra = *outSize - sizeof(TPayload);
    return std::unique_ptr<TPayload>(new(extra) TPayload(elements, std::forward<TArgs>(args)...));
}

template <typename TPayload, typename... TArgs>
std::unique_ptr<TPayload> createVariableSizePayloadOneArrayAtEnd_CommandType(
    size_t* outSize, CommandType ct, const std::span<const typename TPayload::TEndArrayElement>& elements, TArgs&&... args)
{
    if constexpr (!TPayload::VARIABLE_SIZE_ONE_ARRAY_AT_END)
        static_assert(sizeof(TPayload) == 0, "TPayload must be a variable size payload");
    if (elements.size() > TPayload::ARRAY_AT_END_MAX_SIZE)
        throw std::runtime_error("Too many elements for variable size payload");
    *outSize = sizeof(TPayload) + sizeof(typename TPayload::TEndArrayElement) * elements.size();
    size_t extra = *outSize - sizeof(TPayload);
    return std::unique_ptr<TPayload>(new(extra) TPayload(ct, elements, std::forward<TArgs>(args)...));
}
