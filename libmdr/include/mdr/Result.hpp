#pragma once

#include <mdr-c/Base.h>

#include <type_traits>
#include <utility>

#if defined(_MSC_VER)
#define MDR_TRAP() __debugbreak()
#elif defined(__clang__) && __has_builtin(__builtin_debugtrap)
#define MDR_TRAP() __builtin_debugtrap()
#else
#define MDR_TRAP() __builtin_trap()
#endif

#define MDR_CHECK(expr) do { \
if (!(expr)) [[unlikely]] { \
MDR_TRAP(); \
std::abort(); \
} \
} while (false);

namespace mdr
{    
    template <typename T>
    struct [[nodiscard]] MDRResult
    {
        T value{};
        int error{MDR_RESULT_OK};
        const char* errMessage{nullptr};

        [[nodiscard]] static MDRResult Success(T value)
        {
            return {std::move(value), MDR_RESULT_OK};
        }

        [[nodiscard]] static MDRResult Failure(int error, const char* errMessage = nullptr)
        {
            return {T{}, error, errMessage};
        }

        [[nodiscard]] constexpr bool HasValue() const noexcept
        {
            return error == MDR_RESULT_OK;
        }

        [[nodiscard]] constexpr explicit operator bool() const noexcept
        {
            return HasValue();
        }
    };

    template <>
    struct [[nodiscard]] MDRResult<void>
    {
        int error{MDR_RESULT_OK};
        const char* errMessage{nullptr};

        [[nodiscard]] static constexpr MDRResult Success() noexcept
        {
            return {};
        }

        [[nodiscard]] static constexpr MDRResult Failure(int error, const char* errMessage = nullptr) noexcept
        {
            return {error, errMessage};
        }

        [[nodiscard]] constexpr bool HasValue() const noexcept
        {
            return error == MDR_RESULT_OK;
        }

        [[nodiscard]] constexpr explicit operator bool() const noexcept
        {
            return HasValue();
        }
    };
}

#define MDR_TRY(ResultType, ...) \
    do { \
        const auto mdrResult = (__VA_ARGS__); \
        if (!mdrResult) \
        { \
            MDR_TRAP(); \
            return ::mdr::MDRResult<ResultType>::Failure(mdrResult.error,                                              \
                                                         mdrResult.errMessage ? mdrResult.errMessage : #__VA_ARGS__); \
        } \
    } while (false)


#define MDR_TRY_SIZE(ResultType, ...) \
    do { \
        const auto mdrResult = (__VA_ARGS__); \
        if (!mdrResult) \
        { \
            MDR_TRAP(); \
            return ::mdr::MDRResult<ResultType>::Failure(mdrResult.error,                                              \
                                                         mdrResult.errMessage ? mdrResult.errMessage : #__VA_ARGS__); \
        } \
        maxSize -= mdrResult.value; \
    } while (false)

#define MDR_VALIDATE(...) \
    do { \
        if (!(__VA_ARGS__)) \
        { \
            MDR_TRAP(); \
            return ::mdr::MDRResult<void>::Failure(MDR_RESULT_ERROR_MALFORMED_PAYLOAD, "Validation failed: " #__VA_ARGS__); \
        } \
    } while (false)
