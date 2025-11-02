#pragma once

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>
#include <span>
#include <array>
#include <source_location>

#include <fmt/format.h>
#define MDR_CHECK_MSG(expr, format_str, ...) \
    { \
        auto const& srcloc = std::source_location::current(); \
        if(!(expr)) throw std::runtime_error(fmt::format("{}.\nExpression: " #expr "\nFunction: {}\nSource:{}#L{}",fmt::format(format_str __VA_OPT__(,) __VA_ARGS__), srcloc.function_name(), srcloc.file_name(), srcloc.line())); \
    }
#define MDR_CHECK(expr) \
    { \
    auto const& srcloc = std::source_location::current(); \
    if(!(expr)) throw std::runtime_error(fmt::format("Check Failure.\nExpression: " #expr "\nFunction: {}\nSource:{}#L{}", srcloc.function_name(), srcloc.file_name(), srcloc.line())); \
    }

#define MDR_LOG_STREAM stderr
#define MDR_LOG(str, ...) \
    fprintf(MDR_LOG_STREAM, "%s\n", fmt::format((str) __VA_OPT__(,) __VA_ARGS__).c_str());
#ifdef MDR_DEBUG
#define MDR_LOG_DEBUG(...) \
    MDR_LOG(__VA_ARGS__);
#else
#define MDR_LOG_DEBUG(...)
#endif
namespace mdr
{
    typedef uint8_t UInt8;
    typedef int8_t Int8;

    enum class MDRDataType : UInt8
    {
        DATA = 0,
        ACK = 1,
        DATA_MC_NO1 = 2,
        DATA_ICD = 9,
        DATA_EV = 10,
        DATA_MDR = 12,
        DATA_COMMON = 13,
        DATA_MDR_NO2 = 14,
        SHOT = 16,
        SHOT_MC_NO1 = 18,
        SHOT_ICD = 25,
        SHOT_EV = 26,
        SHOT_MDR = 28,
        SHOT_COMMON = 29,
        SHOT_MDR_NO2 = 30,
        LARGE_DATA_COMMON = 45,
        UNKNOWN = 0xff
    };
#pragma pack(push,1)
    struct Int16BE
    {
        int16_t value; // Big-endian

        Int16BE() :
            value(0)
        {
        }

        Int16BE(int16_t v) :
            value(Swap(v))
        {
        }

        static uint16_t Swap(uint16_t v)
        {
            return ((v & 0x000000FF) << 8) |
                ((v & 0x0000FF00) >> 8);
        }

        operator int16_t() const { return Swap(value); }

        Int16BE& operator=(int16_t v)
        {
            value = Swap(v);
            return *this;
        }
    };

    struct Int24BE
    {
        uint8_t low;
        uint8_t mid;
        uint8_t high;

        Int24BE() :
            low(0), mid(0), high(0)
        {
        }


        Int24BE(int32_t v)
        {
            this->operator=(v);
        }

        operator int32_t() const { return low << 16u | mid << 8u | high; }

        Int24BE& operator=(int32_t v)
        {
            low = v & 0xFF;
            mid = (v >> 8) & 0xFF;
            high = v & 0xFF;
            return *this;
        }
    };

    struct Int32BE
    {
        int32_t value; // Big-endian

        Int32BE() :
            value(0)
        {
        }

        Int32BE(int32_t v) :
            value(Swap(v))
        {
        }

        static uint32_t Swap(uint32_t v) // Compiles into bswap
        {
            return ((v & 0x000000FF) << 24) |
                ((v & 0x0000FF00) << 8) |
                ((v & 0x00FF0000) >> 8) |
                ((v & 0xFF000000) >> 24);
        }

        operator int32_t() const { return Swap(value); }

        Int32BE& operator=(int32_t v)
        {
            value = Swap(v);
            return *this;
        }
    };

#pragma pack(pop)

    template <typename T>
    concept MDRIsSerializable = requires(T const& a)
    {
        { T::Serialize(a, std::declval<UInt8*>(), std::declval<size_t>()) } -> std::same_as<size_t>;
        { T::Deserialize(std::declval<const UInt8*>(), std::declval<T&>(), std::declval<size_t>()) } -> std::same_as<void>;
        { T::Validate(a) } -> std::same_as<bool>;
    };
    template <typename T>
    concept MDRIsTrivial = std::is_standard_layout_v<T> && std::is_trivially_copyable_v<T>;
    template <typename T>
    concept MDRIsReadWritable = requires
    {
        { T::Read(std::declval<const UInt8**>(), std::declval<T&>(), std::declval<size_t>()) } -> std::same_as<size_t>;
        { T::Write(std::declval<T const&>(), std::declval<UInt8**>(), std::declval<size_t>()) } -> std::same_as<size_t>;
    };

    /**
     * @brief Traits for payload structs
     * @note  Don't worry - these are always automatically generated.
     */
    template <typename T>
    struct MDRTraits
    {
        static constexpr MDRDataType kDataType = MDRDataType::UNKNOWN;
    };

    /**
     * @brief POD wrapper to enable @ref Read and @ref Write methods for trivial types.
     *        For completely trivial types, consider using @ref MDR_DEFINE_TRIVIAL_SERIALIZATION instead.
     */
    struct MDRPod
    {
        // Read a POD type from/to a buffer, advancing the buffer pointer.
        // Throws std::runtime_error if there is not enough data to read.
        template <typename T>
        static size_t Read(const UInt8** ppSrcBuffer, T& value, size_t maxSize)
        {
            static_assert(MDRIsTrivial<T>, "MDRPod::Read requires trivial type T");
            MDR_CHECK_MSG(sizeof(T) <= maxSize, "Not enough data to read");
            std::memcpy(&value, *ppSrcBuffer, sizeof(T));
            *ppSrcBuffer += sizeof(T);
            return sizeof(T);
        }

        // Write a POD type from/to a buffer, advancing the buffer pointer.
        template <typename T>
        static size_t Write(T const& value, UInt8** ppDstBuffer, size_t maxSize)
        {
            static_assert(MDRIsTrivial<T>, "MDRPod::Write requires trivial type T");
            MDR_CHECK_MSG(sizeof(T) <= maxSize, "Destination has not enough space to write");
            std::memcpy(*ppDstBuffer, &value, sizeof(T));
            *ppDstBuffer += sizeof(T);
            return sizeof(T);
        }
    };
    /**
     * @breif Alias for std::array. This MAY map to any specific protocol type directly as a POD type.
     */
    template <typename T, size_t Size>
    using Array = std::array<T, Size>;
    /**
     * @breif Alias for std::pair. This does not map to any specific protocol type directly.
     */
    template<typename A, typename B>
    using Pair = std::pair<A, B>;
    /**
     * @breif Alias for std::tuple. This does not map to any specific protocol type directly.
     */
    template <typename... Args>
    using Tuple = std::tuple<Args...>;
    /**
     * @breif Alias for std::string. This does not map to any specific protocol type directly.
     */
    using String = std::basic_string<char>;
    /**
     * @breif Alias for std::vector. This does not map to any specific protocol type directly.
     */
    template <typename T>
    using Vector = std::vector<T>;
    /**
     * @brief Alias for std::span w/o extents. This does not map to any specific protocol type directly.
     */
    template <typename T>
    using Span = std::span<T>;
    /**
     * @brief String prefixed with a length byte. Max len=128
     */
    struct MDRPrefixedString
    {
        String value;

        static size_t Read(const UInt8** ppSrcBuffer, MDRPrefixedString& str, size_t maxSize)
        {
            const UInt8 len = *(*ppSrcBuffer)++;
            maxSize--;
            MDR_CHECK_MSG(len < 128 && len <= maxSize, "Invalid string length");
            str.value.resize(len);
            std::memcpy(str.value.data(), *ppSrcBuffer, len);
            *ppSrcBuffer += len;
            return len + 1;
        }

        static size_t Write(MDRPrefixedString const& str, UInt8** ppDstBuffer, size_t maxSize)
        {
            MDR_CHECK_MSG(str.value.length() < 128, "String too long to write");
            MDR_CHECK_MSG(str.value.size() + 1 <= maxSize, "Destination has not enough space to write");
            *(*ppDstBuffer)++ = static_cast<UInt8>(str.value.length());
            std::memcpy(*ppDstBuffer, str.value.data(), str.value.length());
            *ppDstBuffer += str.value.length();
            return str.value.length() + 1;
        }

        [[nodiscard]] auto begin() { return value.begin(); }
        [[nodiscard]] auto end() { return value.end(); }
        [[nodiscard]] auto begin() const { return value.begin(); }
        [[nodiscard]] auto end() const { return value.end(); }
        [[nodiscard]] constexpr size_t size() const { return value.size(); }
    };

    /**
     * @brief POD Array prefixed with a length byte of POD type. Max len=255
     * @tparam T POD type
     */
    template <typename T>
    struct MDRPodArray
    {
        Vector<T> value;

        static size_t Read(const UInt8** ppSrcBuffer, MDRPodArray& value, size_t maxSize)
        {
            UInt8 count = *(*ppSrcBuffer)++;
            size_t size = sizeof(T) * count;
            MDR_CHECK_MSG(size <= maxSize, "Invalid array size");
            value.value.resize(count);
            std::memcpy(value.value.data(), *ppSrcBuffer, size);
            *ppSrcBuffer += size;
            return size + 1;
        }

        static size_t Write(MDRPodArray const& value, UInt8** ppDstBuffer, size_t maxSize)
        {
            size_t size = sizeof(T) * value.value.size();
            MDR_CHECK_MSG(size < 256, "Array too long to write");
            MDR_CHECK_MSG(size + 1 <= maxSize, "Destination has not enough space to write");
            *(*ppDstBuffer)++ = static_cast<UInt8>(value.value.size());
            std::memcpy(*ppDstBuffer, value.value.data(), size);
            *ppDstBuffer += size;
            return size + 1;
        }

        [[nodiscard]] auto begin() { return value.begin(); }
        [[nodiscard]] auto end() { return value.end(); }
        [[nodiscard]] auto begin() const { return value.begin(); }
        [[nodiscard]] auto end() const { return value.end(); }
        [[nodiscard]] constexpr size_t size() const { return value.size(); }
    };

    /**
     * @brief Non-POD Array prefixed with a length byte of non-POD type. Max len=255
     * @tparam T Type that implements @ref Read and @ref Write methods.
     */
    template <typename T>
    struct MDRArray
    {
        static_assert(MDRIsReadWritable<T>,
                      "MDRArray requires T to implement Read and Write methods of consistent signatures");
        Vector<T> value;

        static size_t Read(const UInt8** ppSrcBuffer, MDRArray& value, size_t maxSize)
        {
            const UInt8* ptr = *ppSrcBuffer;
            UInt8 count = *(*ppSrcBuffer)++;
            maxSize--;
            value.value.resize(count);
            for (T& elem : value.value)
                maxSize -= T::Read(ppSrcBuffer, elem, maxSize);
            return *ppSrcBuffer - ptr;
        }

        static size_t Write(MDRArray const& value, UInt8** ppDstBuffer, size_t maxSize)
        {
            UInt8* ptr = *ppDstBuffer;
            MDR_CHECK_MSG(value.value.size() < 256, "Array too long to write");
            maxSize--;
            *(*ppDstBuffer)++ = static_cast<UInt8>(value.value.size());
            for (const T& elem : value.value)
                maxSize -= T::Write(elem, ppDstBuffer, maxSize);
            return *ppDstBuffer - ptr;
        }

        [[nodiscard]] auto begin() { return value.begin(); }
        [[nodiscard]] auto end() { return value.end(); }
        [[nodiscard]] auto begin() const { return value.begin(); }
        [[nodiscard]] auto end() const { return value.end(); }
        [[nodiscard]] constexpr size_t size() const { return value.size(); }
    };

    /**
     * @brief Non-POD Array with a fixed size
     * @tparam T Type that implements @ref Read and @ref Write methods.
     */
    template <typename T, size_t Size>
    struct MDRFixedArray
    {
        static_assert(MDRIsReadWritable<T>,
                      "MDRFixedArray requires T to implement Read and Write methods of consistent signatures");
        Array<T, Size> value;

        static size_t Read(const UInt8** ppSrcBuffer, MDRFixedArray& value, size_t maxSize)
        {
            const UInt8* ptr = *ppSrcBuffer;
            for (T& elem : value.value)
                maxSize -= T::Read(ppSrcBuffer, elem, maxSize);
            return *ppSrcBuffer - ptr;
        }

        static size_t Write(MDRFixedArray const& value, UInt8** ppDstBuffer, size_t maxSize)
        {
            UInt8* ptr = *ppDstBuffer;
            for (const T& elem : value.value)
                maxSize -= T::Write(elem, ppDstBuffer, maxSize);
            return *ppDstBuffer - ptr;
        }

        [[nodiscard]] auto begin() { return value.begin(); }
        [[nodiscard]] auto end() { return value.end(); }
        [[nodiscard]] auto begin() const { return value.begin(); }
        [[nodiscard]] auto end() const { return value.end(); }
        [[nodiscard]] constexpr size_t size() const { return value.size(); }
    };

    /**
     * @brief Macro for POD types that can be serialized/deserialized via std::memcpy.
     *
     * @note  This defines @ref Serialize and @ref Deserialize and @ref Validate in the current scope,
     *        which must be a struct.
     */
#define MDR_DEFINE_TRIVIAL_SERIALIZATION(Type) \
    static size_t Serialize(const Type &data, UInt8* out, size_t maxSize) { \
        static_assert(alignof(Type) == 1u, "Trivial type are required to have 1-byte alignment"); \
        static_assert(MDRIsTrivial<Type> && "Non-trivial layout attempted with trivial (memcpy) serialization"); \
        MDR_CHECK_MSG(sizeof(Type) <= maxSize, "Destination has not enough space to write"); \
        const UInt8 *ptr = reinterpret_cast<const UInt8*>(&data); \
        std::memcpy(out, ptr, sizeof(Type)); \
        return sizeof(Type); \
    } \
    static void Deserialize(const UInt8* data, Type &out, size_t maxSize) { \
        static_assert(alignof(Type) == 1u, "Trivial type are required to have 1-byte alignment"); \
        static_assert(MDRIsTrivial<Type> && "Non-trivial layout attempted with trivial (memcpy) serialization"); \
        MDR_CHECK_MSG(sizeof(Type) <= maxSize, "Not enough data to read"); \
        std::memcpy(&out, data, sizeof(Type)); \
    } \
    static bool Validate(const Type& data);
    /**
     * @brief Macro to declare external serialization methods for non-trivial types.
     *
     * @note This declares @ref Serialize, @ref Deserialize and @ref Validate in the current scope,
     *       which must be a struct.
     *
     * @note The implementations must be provided elsewhere, ideally in a corresponding
     *       translation unit, which may or may not be generated.
     */
#define MDR_DEFINE_EXTERN_SERIALIZATION(Type) \
    static size_t Serialize(const Type &data, UInt8* out, size_t maxSize); \
    static void Deserialize(const UInt8* data, Type &out, size_t maxSize); \
    static bool Validate(const Type& data);
    /**
     * @brief Macro to declare external read/write methods for non-trivial types.
     *
     * @note This declares @ref Read and @ref Write in the current scope,
     *       which must be a struct.
     *
     * @note The implementations must be provided elsewhere, ideally in a corresponding
     *       translation unit, which may or may not be generated.
     */
#define MDR_DEFINE_EXTERN_READ_WRITE(SubType) \
    static size_t Read(const UInt8** ppSrcBuffer, SubType &out, size_t maxSize); \
    static size_t Write(const SubType &data, UInt8** ppDstBuffer, size_t maxSize);
    /**
     * @brief Macro to mark the struct to implement bespoke serialization logic.
     */
#define MDR_CODEGEN_IGNORE_SERIALIZATION
}
