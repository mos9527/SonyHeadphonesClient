#pragma once

#include <utility>
#include <mdr/Protocol.hpp>

namespace mdr
{
    template <typename T>
    struct MDRProperty
    {
        enum class State
        {
            CLEAN, // No pending changes
            STAGED, // Changes staged and not yet submitted
            SUBMITTED // Changes in flight to the device
        };

        T mDesired{};
        T mCurrent{};
        T mSubmitted{};
        State mState{State::CLEAN};

        void stage(const T& value)
        {
            if (mDesired == value)
                return;
            mDesired = value;
            mState = State::STAGED;
        }

        void stage(T&& value)
        {
            if (mDesired == value)
                return;
            mDesired = std::move(value);
            mState = State::STAGED;
        }

        void overwrite(const T& value)
        {
            mCurrent = value;
            if (mState == State::CLEAN)
                mDesired = value;
        }

        void submit()
        {
            mSubmitted = mDesired;
            if (mState == State::STAGED)
                mState = State::SUBMITTED;
        }

        [[nodiscard]] constexpr bool dirty() const noexcept { return mState != State::CLEAN; }
        [[nodiscard]] constexpr bool pending() const noexcept { return mState == State::SUBMITTED; }

        void commit()
        {
            mState = State::CLEAN;
        }

        void override(const T& value)
        {
            mCurrent = value;
            mDesired = value;
            mSubmitted = value;
            mState = State::CLEAN;
        }
    };
} // namespace mdr
