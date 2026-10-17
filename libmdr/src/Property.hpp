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
            SUBMITTED, // Changes in flight to the device
            SUBMITTED_STAGED // Changes in flight with newer changes staged
        };

        T mDesired{};
        T mCurrent{};
        T mSubmitted{};
        State mState{State::CLEAN};

        void stage(const T& value)
        {
            mDesired = value;
            mState = pending() ? State::SUBMITTED_STAGED : State::STAGED;
        }

        void stage(T&& value)
        {
            mDesired = std::move(value);
            mState = pending() ? State::SUBMITTED_STAGED : State::STAGED;
        }

        void overwrite(const T& value)
        {
            mCurrent = value;
            if (mState == State::CLEAN)
                mDesired = value;
        }

        void submit()
        {
            if (pending())
                return;
            mSubmitted = mDesired;
            if (mState == State::STAGED)
                mState = State::SUBMITTED;
        }

        [[nodiscard]] constexpr bool dirty() const noexcept { return mState != State::CLEAN; }
        [[nodiscard]] constexpr bool pending() const noexcept
        {
            return mState == State::SUBMITTED || mState == State::SUBMITTED_STAGED;
        }

        void commit()
        {
            if (mState == State::SUBMITTED)
                mState = State::CLEAN;
            else if (mState == State::SUBMITTED_STAGED)
                mState = State::STAGED;
        }

    };
} // namespace mdr
