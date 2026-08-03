#pragma once

#include "Result.hpp"

#include <array>
#include <cstdint>
#include <deque>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#include <iterator>

#include <fmt/format.h>

#define MDR_LOG_STREAM stderr
#define MDR_LOG(str, ...) \
    fprintf(MDR_LOG_STREAM, "%s\n", mdr::Format((str) __VA_OPT__(,) __VA_ARGS__).c_str());
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
            high = v & 0xFF;
            mid  = (v >> 8) & 0xFF;
            low  = (v >> 16) & 0xFF;
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

    struct UInt64BE
    {
        uint64_t value; // Big-endian

        UInt64BE() :
            value(0)
        {
        }

        UInt64BE(uint64_t v) :
            value(Swap(v))
        {
        }

        static uint64_t Swap(uint64_t v)
        {
            return ((v & 0x00000000000000FFull) << 56) |
                ((v & 0x000000000000FF00ull) << 40) |
                ((v & 0x0000000000FF0000ull) << 24) |
                ((v & 0x00000000FF000000ull) << 8) |
                ((v & 0x000000FF00000000ull) >> 8) |
                ((v & 0x0000FF0000000000ull) >> 24) |
                ((v & 0x00FF000000000000ull) >> 40) |
                ((v & 0xFF00000000000000ull) >> 56);
        }

        operator uint64_t() const { return Swap(value); }

        UInt64BE& operator=(uint64_t v)
        {
            value = Swap(v);
            return *this;
        }
    };

#pragma pack(pop)

    template <typename T>
    concept MDRIsSerializable = requires(T const& a)
    {
        { T::Serialize(a, std::declval<UInt8*>(), std::declval<size_t>()) } -> std::same_as<MDRResult<size_t>>;
        { T::Deserialize(std::declval<const UInt8*>(), std::declval<size_t>()) } -> std::same_as<MDRResult<T>>;
        { T::Validate(a) } -> std::same_as<MDRResult<void>>;
    };
    template <typename T>
    concept MDRIsTrivial = std::is_standard_layout_v<T> && std::is_trivially_copyable_v<T>;
    template <typename T>
    concept MDRIsReadWritable = requires
    {
        { T::Read(std::declval<const UInt8**>(), std::declval<T&>(), std::declval<size_t>()) } -> std::same_as<MDRResult<size_t>>;
        { T::Write(std::declval<T const&>(), std::declval<UInt8**>(), std::declval<size_t>()) } -> std::same_as<MDRResult<size_t>>;
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
        template <typename T>
        static MDRResult<size_t> Read(const UInt8** ppSrcBuffer, T& value, size_t maxSize)
        {
            static_assert(MDRIsTrivial<T>, "MDRPod::Read requires trivial type T");
            if (sizeof(T) > maxSize)
                return MDRResult<size_t>::Failure(MDR_RESULT_ERROR_BUFFER_TOO_SMALL);
            std::memcpy(&value, *ppSrcBuffer, sizeof(T));
            *ppSrcBuffer += sizeof(T);
            return MDRResult<size_t>::Success(sizeof(T));
        }

        // Write a POD type from/to a buffer, advancing the buffer pointer.
        template <typename T>
        static MDRResult<size_t> Write(T const& value, UInt8** ppDstBuffer, size_t maxSize)
        {
            static_assert(MDRIsTrivial<T>, "MDRPod::Write requires trivial type T");
            if (sizeof(T) > maxSize)
                return MDRResult<size_t>::Failure(MDR_RESULT_ERROR_BUFFER_TOO_SMALL);
            std::memcpy(*ppDstBuffer, &value, sizeof(T));
            *ppDstBuffer += sizeof(T);
            return MDRResult<size_t>::Success(sizeof(T));
        }
    };

    /**
     * @breif `noexcept` Allocator for STL containers, simply wraps @ref malloc and @ref free.
     * @note  Terminates on OOM, etc., instead of throwing an exception.
     */
    template <typename T = void>
    struct MDRAllocator {
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;

        MDRAllocator() = default;
        template <typename U>
            constexpr MDRAllocator(const MDRAllocator<U>&) noexcept {}

        pointer allocate(size_type n) noexcept {
            pointer p = static_cast<pointer>(std::malloc(n * sizeof(T)));
            MDR_CHECK(p != nullptr && "OOM");
            return p;
        }
        void deallocate(pointer p, size_type n) noexcept {
            deallocate(p);
        }
        void deallocate(pointer p) noexcept { std::free(p); }
        friend bool operator==(const MDRAllocator& lhs, const MDRAllocator& rhs) noexcept {
            return true;
        }
        friend bool operator!=(const MDRAllocator& lhs, const MDRAllocator& rhs) noexcept {
            return false;
        }
        struct Deleter
        {
            void operator()(T* ptr) noexcept {
                MDRAllocator<T> alloc;
                MDR_CHECK(ptr != nullptr);
                std::destroy_at(ptr);
                alloc.deallocate(ptr);
            }
        };
    };
    /**
     * @brief Convenience placement new with object of type T
     * @note Using `delete`, `delete[]` on the returned pointer is undefined behaviour. @ref Destruct should ALWAYS
     *       be used for such purposes.
     */
    template <typename T, typename ...Args>
    T* Construct(Args&& ...args) {
        MDRAllocator<T> alloc;
        auto raw = alloc.allocate(1);
        return std::construct_at(raw, std::forward<Args>(args)...);
    }
    /**
     * @brief Convenience destructor for objects allocated with @ref Construct
     */
    template <typename T>
    void Destruct(T* obj) {
        typename MDRAllocator<T>::Deleter deleter;
        deleter(obj);
    }
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
    using String = std::basic_string<char, std::char_traits<char>, MDRAllocator<char>>;
    /**
     * @brief @ref mdr::String wrapper for fmt::format().
     * @note  Use this, over @ref fmt::format at all times since mdr::String is non-throwing.
     */
    template <typename... Args>
    [[nodiscard]] constexpr String Format(fmt::format_string<Args...> format, Args&&... args)
    {
        fmt::basic_memory_buffer<char, fmt::inline_buffer_size, MDRAllocator<char>> buffer;
        fmt::format_to(std::back_inserter(buffer), format, std::forward<Args>(args)...);
        return String(buffer.data(), buffer.size());
    }
    /**
     * @breif Alias for std::vector. This does not map to any specific protocol type directly.
     */
    template <typename T>
    using Vector = std::vector<T, MDRAllocator<T>>;
    /**
     * @breif Alias for std::deque. This does not map to any specific protocol type directly.
     */
    template <typename T>
    using Deque = std::deque<T, MDRAllocator<T>>;
    /**
     * @brief Alias for std::span w/o extents. This does not map to any specific protocol type directly.
     */
    template <typename T>
    using Span = std::span<T>;
    /**
     * @brief Byte-count-prefixed sequence of POD key/value pairs.
     */
    template <typename K, typename V>
    struct MDRMap
    {
        struct Entry
        {
            K key;
            V value;
        };

        Vector<Entry> entries;

        static MDRResult<size_t> Read(
            const UInt8** ppSrcBuffer, MDRMap& out, size_t maxSize
        )
        {
            const UInt8* start = *ppSrcBuffer;
            UInt8 count{};
            MDR_TRY_SIZE(size_t, MDRPod::Read(ppSrcBuffer, count, maxSize));
            out.entries.clear();
            out.entries.reserve(count);
            for (UInt8 i = 0; i < count; ++i)
            {
                Entry entry{};
                const size_t remaining = maxSize - (*ppSrcBuffer - start);
                MDR_TRY_SIZE(
                    size_t,
                    MDRPod::Read(ppSrcBuffer, entry.key, remaining)
                );
                MDR_TRY_SIZE(
                    size_t,
                    MDRPod::Read(
                        ppSrcBuffer,
                        entry.value,
                        maxSize - (*ppSrcBuffer - start)
                    )
                );
                out.entries.push_back(entry);
            }
            return MDRResult<size_t>::Success(*ppSrcBuffer - start);
        }

        static MDRResult<size_t> Write(
            const MDRMap& data, UInt8** ppDstBuffer, size_t maxSize
        )
        {
            if (data.entries.size() > UINT8_MAX)
                return MDRResult<size_t>::Failure(
                    MDR_RESULT_ERROR_MALFORMED_PAYLOAD
                );
            UInt8* start = *ppDstBuffer;
            const auto count = static_cast<UInt8>(data.entries.size());
            MDR_TRY_SIZE(
                size_t, MDRPod::Write(count, ppDstBuffer, maxSize)
            );
            for (const Entry& entry : data.entries)
            {
                MDR_TRY_SIZE(
                    size_t,
                    MDRPod::Write(
                        entry.key,
                        ppDstBuffer,
                        maxSize - (*ppDstBuffer - start)
                    )
                );
                MDR_TRY_SIZE(
                    size_t,
                    MDRPod::Write(
                        entry.value,
                        ppDstBuffer,
                        maxSize - (*ppDstBuffer - start)
                    )
                );
            }
            return MDRResult<size_t>::Success(*ppDstBuffer - start);
        }
    };
    /**
     * @brief String prefixed with a length byte. Max len=256
     */
    struct MDRPrefixedString
    {
        String value;

        static MDRResult<size_t> Read(const UInt8** ppSrcBuffer, MDRPrefixedString& str, size_t maxSize)
        {
            if (maxSize < 1)
                return MDRResult<size_t>::Failure(MDR_RESULT_ERROR_BUFFER_TOO_SMALL);
            const UInt8 len = **ppSrcBuffer;
            if (len > maxSize - 1)
                return MDRResult<size_t>::Failure(MDR_RESULT_ERROR_MALFORMED_PAYLOAD);
            (*ppSrcBuffer)++;
            str.value.resize(len);
            std::memcpy(str.value.data(), *ppSrcBuffer, len);
            *ppSrcBuffer += len;
            return MDRResult<size_t>::Success(len + 1);
        }

        static MDRResult<size_t> Write(MDRPrefixedString const& str, UInt8** ppDstBuffer, size_t maxSize)
        {
            if (str.value.length() >= 256)
                return MDRResult<size_t>::Failure(MDR_RESULT_ERROR_INVALID_ARGUMENT);
            if (str.value.size() + 1 > maxSize)
                return MDRResult<size_t>::Failure(MDR_RESULT_ERROR_BUFFER_TOO_SMALL);
            *(*ppDstBuffer)++ = static_cast<UInt8>(str.value.length());
            std::memcpy(*ppDstBuffer, str.value.data(), str.value.length());
            *ppDstBuffer += str.value.length();
            return MDRResult<size_t>::Success(str.value.length() + 1);
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

        static MDRResult<size_t> Read(const UInt8** ppSrcBuffer, MDRPodArray& value, size_t maxSize)
        {
            if (maxSize < 1)
                return MDRResult<size_t>::Failure(MDR_RESULT_ERROR_BUFFER_TOO_SMALL);
            const UInt8 count = **ppSrcBuffer;
            size_t size = sizeof(T) * count;
            if (size > maxSize - 1)
                return MDRResult<size_t>::Failure(MDR_RESULT_ERROR_MALFORMED_PAYLOAD);
            (*ppSrcBuffer)++;
            value.value.resize(count);
            std::memcpy(value.value.data(), *ppSrcBuffer, size);
            *ppSrcBuffer += size;
            return MDRResult<size_t>::Success(size + 1);
        }

        static MDRResult<size_t> Write(MDRPodArray const& value, UInt8** ppDstBuffer, size_t maxSize)
        {
            size_t size = sizeof(T) * value.value.size();
            if (value.value.size() >= 256)
                return MDRResult<size_t>::Failure(MDR_RESULT_ERROR_INVALID_ARGUMENT);
            if (size + 1 > maxSize)
                return MDRResult<size_t>::Failure(MDR_RESULT_ERROR_BUFFER_TOO_SMALL);
            *(*ppDstBuffer)++ = static_cast<UInt8>(value.value.size());
            std::memcpy(*ppDstBuffer, value.value.data(), size);
            *ppDstBuffer += size;
            return MDRResult<size_t>::Success(size + 1);
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

        static MDRResult<size_t> Read(const UInt8** ppSrcBuffer, MDRArray& value, size_t maxSize)
        {
            if (maxSize < 1)
                return MDRResult<size_t>::Failure(MDR_RESULT_ERROR_BUFFER_TOO_SMALL);
            const UInt8* const begin = *ppSrcBuffer;
            UInt8 count = *(*ppSrcBuffer)++;
            maxSize--;
            value.value.resize(count);
            for (T& elem : value.value)
                MDR_TRY_SIZE(size_t, T::Read(ppSrcBuffer, elem, maxSize));
            return MDRResult<size_t>::Success(*ppSrcBuffer - begin);
        }

        static MDRResult<size_t> Write(MDRArray const& value, UInt8** ppDstBuffer, size_t maxSize)
        {
            UInt8* const begin = *ppDstBuffer;
            if (value.value.size() >= 256)
                return MDRResult<size_t>::Failure(MDR_RESULT_ERROR_INVALID_ARGUMENT);
            if (maxSize < 1)
                return MDRResult<size_t>::Failure(MDR_RESULT_ERROR_BUFFER_TOO_SMALL);
            maxSize--;
            *(*ppDstBuffer)++ = static_cast<UInt8>(value.value.size());
            for (const T& elem : value.value)
                MDR_TRY_SIZE(size_t, T::Write(elem, ppDstBuffer, maxSize));
            return MDRResult<size_t>::Success(*ppDstBuffer - begin);
        }

        [[nodiscard]] auto begin() { return value.begin(); }
        [[nodiscard]] auto end() { return value.end(); }
        [[nodiscard]] auto begin() const { return value.begin(); }
        [[nodiscard]] auto end() const { return value.end(); }
        [[nodiscard]] constexpr size_t size() const { return value.size(); }
    };

    /**
     * @brief Non-POD Array with a fixed size. For PODs of fixed size array, just use @ref Array instead.
     * @tparam T Type that implements @ref Read and @ref Write methods.
     */
    template <typename T, size_t Size>
    struct MDRFixedArray
    {
        static_assert(MDRIsReadWritable<T>,
                      "MDRFixedArray requires T to implement Read and Write methods of consistent signatures");
        Array<T, Size> value;

        static MDRResult<size_t> Read(const UInt8** ppSrcBuffer, MDRFixedArray& value, size_t maxSize)
        {
            const UInt8* const begin = *ppSrcBuffer;
            for (T& elem : value.value)
                MDR_TRY_SIZE(size_t, T::Read(ppSrcBuffer, elem, maxSize));
            return MDRResult<size_t>::Success(*ppSrcBuffer - begin);
        }

        static MDRResult<size_t> Write(MDRFixedArray const& value, UInt8** ppDstBuffer, size_t maxSize)
        {
            UInt8* const begin = *ppDstBuffer;
            for (const T& elem : value.value)
                MDR_TRY_SIZE(size_t, T::Write(elem, ppDstBuffer, maxSize));
            return MDRResult<size_t>::Success(*ppDstBuffer - begin);
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
    static MDRResult<size_t> Serialize(const Type &data, UInt8* out, size_t maxSize) { \
        static_assert(alignof(Type) == 1u, "Trivial type are required to have 1-byte alignment"); \
        static_assert(MDRIsTrivial<Type> && "Non-trivial layout attempted with trivial (memcpy) serialization"); \
        if (sizeof(Type) > maxSize) \
            return MDRResult<size_t>::Failure(MDR_RESULT_ERROR_BUFFER_TOO_SMALL); \
        MDR_TRY(size_t, Validate(data)); \
        const UInt8 *ptr = reinterpret_cast<const UInt8*>(&data); \
        std::memcpy(out, ptr, sizeof(Type)); \
        return MDRResult<size_t>::Success(sizeof(Type)); \
    } \
    static MDRResult<Type> Deserialize(const UInt8* data, size_t maxSize) { \
        static_assert(alignof(Type) == 1u, "Trivial type are required to have 1-byte alignment"); \
        static_assert(MDRIsTrivial<Type> && "Non-trivial layout attempted with trivial (memcpy) serialization"); \
        if (sizeof(Type) > maxSize) \
            return MDRResult<Type>::Failure(MDR_RESULT_ERROR_BUFFER_TOO_SMALL); \
        Type out{}; \
        std::memcpy(&out, data, sizeof(Type)); \
        MDR_TRY(Type, Validate(out)); \
        return MDRResult<Type>::Success(std::move(out)); \
    } \
    static MDRResult<void> Validate(const Type& data);
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
    static MDRResult<size_t> Serialize(const Type &data, UInt8* out, size_t maxSize); \
    static MDRResult<Type> Deserialize(const UInt8* data, size_t maxSize); \
    static MDRResult<void> Validate(const Type& data);
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
    static MDRResult<size_t> Read(const UInt8** ppSrcBuffer, SubType &out, size_t maxSize); \
    static MDRResult<size_t> Write(const SubType &data, UInt8** ppDstBuffer, size_t maxSize);
    /**
     * @brief Macro to mark the struct to implement bespoke serialization logic.
     */
#define MDR_CODEGEN_IGNORE_SERIALIZATION
}
