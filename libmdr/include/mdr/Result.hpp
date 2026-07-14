#pragma once

#include <mdr-c/Base.h>

#include <type_traits>
#include <utility>

namespace mdr
{
    template <typename T>
    struct [[nodiscard]] MDRResult
    {
        T value{};
        int error{MDR_RESULT_OK};

        [[nodiscard]] static MDRResult Success(T value)
        {
            return {std::move(value), MDR_RESULT_OK};
        }

        [[nodiscard]] static MDRResult Failure(int error)
        {
            return {T{}, error};
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

        [[nodiscard]] static constexpr MDRResult Success() noexcept
        {
            return {};
        }

        [[nodiscard]] static constexpr MDRResult Failure(int error) noexcept
        {
            return {error};
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
            return ::mdr::MDRResult<ResultType>::Failure(mdrResult.error); \
    } while (false)

#define MDR_TRY_SIZE(ResultType, ...) \
    do { \
        const auto mdrResult = (__VA_ARGS__); \
        if (!mdrResult) \
            return ::mdr::MDRResult<ResultType>::Failure(mdrResult.error); \
        maxSize -= mdrResult.value; \
    } while (false)

#define MDR_VALIDATE(...) \
    do { \
        if (!(__VA_ARGS__)) \
            return ::mdr::MDRResult<void>::Failure(MDR_RESULT_ERROR_MALFORMED_PAYLOAD); \
    } while (false)
