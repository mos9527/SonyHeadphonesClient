#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <variant>
#include <cstring>
#include "Constants.hpp"
namespace mdr
{
    typedef uint8_t UInt8;
    struct Int32BE
    {
        int32_t value;

        Int32BE() : value(0) {}
        Int32BE(int32_t v) : value(v) {}

        static uint32_t Swap(uint32_t v) // Compiles into bswap
        {
            return ((v & 0x000000FF) << 24) |
                   ((v & 0x0000FF00) << 8) |
                   ((v & 0x00FF0000) >> 8) |
                   ((v & 0xFF000000) >> 24);
        }
        operator int32_t() const { return Swap(value); }
        Int32BE &operator=(int32_t v)
        {
            value = Swap(v);
            return *this;
        }
    };
    struct Int16BE
    {
        int16_t value;

        Int16BE() : value(0) {}
        Int16BE(int16_t v) : value(v) {}

        static uint16_t Swap(uint16_t v)
        {
            return ((v & 0x000000FF) << 8) |
                   ((v & 0x0000FF00) >> 8);
        }
        operator int16_t() const { return Swap(value); }
        Int16BE &operator=(int16_t v)
        {
            value = Swap(v);
            return *this;
        }
    };

    template<typename T>
    concept MDRIsSerializable = requires(T const& a) {
        { T::Serialize(a, std::declval<UInt8*>()) } -> std::same_as<size_t>;
        { T::Deserialize(std::declval<UInt8*>(), std::declval<T&>()) } -> std::same_as<void>;
    };
    template<typename T>
    concept MDRIsTrivial = std::is_standard_layout_v<T> && std::is_trivially_copyable_v<T>;
    template<typename T>
    concept MDRIsReadWritable = requires {
        { T::Read(std::declval<UInt8**>(), std::declval<T&>(), std::declval<size_t>()) } -> std::same_as<void>;
        { T::Write(std::declval<T const&>(), std::declval<UInt8**>()) } -> std::same_as<size_t>;
    };
    /**
     * @brief POD wrapper to enable @ref Read and @ref Write methods for trivial types.
     *        For completely trivial types, consider using @ref MDR_DEFINE_TRIVIAL_SERIALIZATION instead.
     */
    struct MDRPod
    {
        // Read a POD type from/to a buffer, advancing the buffer pointer.
        // Throws std::runtime_error if there is not enough data to read.
        template<typename T> static void Read(UInt8** ppSrcBuffer, T& value, size_t maxSize = ~0LL)
        {
            static_assert(MDRIsTrivial<T>, "MDRPod::Read requires trivial type T");
            if (sizeof(T) > maxSize)
                throw std::runtime_error("Not enough data to read");
            std::memcpy(&value, *ppSrcBuffer, sizeof(T));
            *ppSrcBuffer += sizeof(T);
        }
        // Write a POD type from/to a buffer, advancing the buffer pointer.
        // Throws std::runtime_error if there is not enough data to read.
        template<typename T> static size_t Write(T const& value, UInt8** ppDstBuffer)
        {
            static_assert(MDRIsTrivial<T>, "MDRPod::Write requires trivial type T");
            std::memcpy(*ppDstBuffer, &value, sizeof(T));
            *ppDstBuffer += sizeof(T);
            return sizeof(T);
        }
    };

    /**
     * @brief String prefixed with a length byte. Max len=128
     */
    struct MDRPrefixedString
    {
        std::string value;
        static void Read(UInt8** ppSrcBuffer, MDRPrefixedString& str, size_t maxSize = ~0LL)
        {
            UInt8 length = **ppSrcBuffer++;
            if (length > 128 || length > maxSize - 1)
                throw std::runtime_error("Invalid string length");
            str.value.resize(length);
            std::memcpy(str.value.data(), *ppSrcBuffer, length);
            *ppSrcBuffer += length;
        }
        static size_t Write(MDRPrefixedString const& str, UInt8** ppDstBuffer)
        {
            if (str.value.length() > 128)
                throw std::runtime_error("String too long to write");
            **ppDstBuffer++ = static_cast<UInt8>(str.value.length());
            std::memcpy(*ppDstBuffer, str.value.data(), str.value.length());
            *ppDstBuffer += str.value.length();
            return str.value.length() + 1;
        }
    };

    /**
     * @brief POD Array prefixed with a length byte of POD type. Max len=255
     * @tparam T POD type
     */
    template<typename T>
    struct MDRPodArray
    {
        std::vector<T> value;
        static void Read(UInt8** ppSrcBuffer, MDRPodArray& value, size_t maxSize = ~0LL)
        {
            UInt8 count = **ppSrcBuffer++;
            size_t size = sizeof(T) * count;
            if (size > maxSize)
                throw std::runtime_error("Invalid array length");
            value.value.resize(size);
            std::memcpy(value.value.data(), *ppSrcBuffer, size);
            *ppSrcBuffer += size;
        }
        static size_t Write(MDRPodArray const& value, UInt8** ppDstBuffer)
        {
            size_t size = sizeof(T) * value.value.size();
            if (size > 255)
                throw std::runtime_error("Array too long to write");
            **ppDstBuffer++ = static_cast<UInt8>(value.value.size());
            std::memcpy(*ppDstBuffer, &size, sizeof(size));
            *ppDstBuffer += sizeof(size);
            return size + 1;
        }
    };

    /**
     * @brief Non-POD Array prefixed with a length byte of non-POD type. Max len=255
     * @tparam T Type that implements @ref Read and @ref Write methods.
     */
    template<typename T>
    struct MDRArray
    {
        static_assert(MDRIsReadWritable<T>, "MDRArray requires T to implement Read and Write methods of consistent signatures");
        std::vector<T> value;
        static void Read(UInt8** ppSrcBuffer, MDRArray& value, size_t maxSize = ~0LL)
        {
            UInt8 count = **ppSrcBuffer++;
            value.value.resize(count);
            for (T& elem : value.value)
                T::Read(ppSrcBuffer, elem, maxSize);
        }
        static size_t Write(MDRArray const& value, UInt8** ppDstBuffer)
        {
            UInt8* ptr = *ppDstBuffer;
            if (value.value.size() > 255)
                throw std::runtime_error("Array too long to write");
            **ppDstBuffer++ = static_cast<UInt8>(value.value.size());
            for (const T& elem : value.value)
                T::Write(elem, ppDstBuffer);
            return *ppDstBuffer - ptr;
        }
    };

    /**
     * @brief Alias for std::variant. This does not map to any specific protocol type directly.
     */
    template<typename ...Args> using Variant = std::variant<Args...>;
    /**
     * @brief Alias for std::array. This does not map to any specific protocol type directly.
     */
    template<typename T, size_t Size> using Array = std::array<T, Size>;
    /**
     * @breif Alias for std::vector. This does not map to any specific protocol type directly.
     */
    template<typename T> using Vector = std::vector<T>;
    /**
     * @brief Macro for POD types that can be serialized/deserialized via std::memcpy.
     *
     * @note  This defines @ref Serialize and @ref Deserialize in the current scope,
     *        which must be a struct.
     */
    #define MDR_DEFINE_TRIVIAL_SERIALIZATION(Type) \
    static size_t Serialize(const Type &data, UInt8* out) { \
        static_assert(MDRIsTrivial<Type> && "Non-trivial layout attempted with trivial (memcpy) serialization"); \
        const UInt8 *ptr = reinterpret_cast<const UInt8*>(&data); \
        std::memcpy(out, ptr, sizeof(Type)); \
        return sizeof(Type); \
    } \
    static void Deserialize(UInt8* data, Type &out) { \
        static_assert(MDRIsTrivial<Type> && "Non-trivial layout attempted with trivial (memcpy) serialization"); \
        std::memcpy(&out, data, sizeof(Type)); \
    }
    /**
     * @brief Macro to declare external serialization methods for non-trivial types.
     *
     * @note This declares @ref Serialize and @ref Deserialize in the current scope,
     *       which must be a struct.
     *
     * @note The implementations must be provided elsewhere, ideally in a corresponding
     *       translation unit, which may or may not be generated.
     */
    #define MDR_DEFINE_EXTERN_SERIALIZATION(Type) \
    static size_t Serialize(const Type &data, UInt8* out); \
    static void Deserialize(UInt8* data, Type &out);
    /**
     * @brief Macro to mark the struct to implement bespoke serialization logic.
     */
    #define MDR_CODEGEN_IGNORE_SERIALIZATION
}
