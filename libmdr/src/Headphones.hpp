#pragma once
#include "Command.hpp"
#include "ProtocolV2T1.hpp"
#include "ProtocolV2T2.hpp"
#include "mdr/Headphones.h"

#include <deque>
#include <coroutine>

using namespace mdr;
// Coroutine task boilerplate from https://github.com/mos9527/coro
// NOLINTBEGIN
struct MDRTask
{
    struct promise_type
    {
        std::exception_ptr exception;
        std::coroutine_handle<> next;
        static std::suspend_always initial_suspend() noexcept { return {}; }
        void unhandled_exception() noexcept { exception = std::current_exception(); }

        struct final_awaiter
        {
            static bool await_ready() noexcept { return false; }

            std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> other) noexcept
            {
                if (auto& p = other.promise(); p.next)
                    return p.next;
                return std::noop_coroutine();
            }

            static void await_resume() noexcept
            {
            }
        };

        static final_awaiter final_suspend() noexcept { return final_awaiter{}; }
        MDRTask get_return_object();

        static void return_void() noexcept
        {
        }
    };

    using value_type = void;
    std::coroutine_handle<promise_type> coroutine{nullptr};

    MDRTask() = default;

    explicit MDRTask(std::coroutine_handle<promise_type> handle) :
        coroutine(handle)
    {
    };
    MDRTask(const MDRTask&) = delete;

    MDRTask& operator=(MDRTask&& other)
    {
        std::swap(coroutine, other.coroutine), other.coroutine = nullptr;
        return *this;
    }

    MDRTask& operator=(MDRTask const&) = delete;
    MDRTask(MDRTask&& other) noexcept { *this = std::move(other); }
    constexpr operator bool() const noexcept { return coroutine != nullptr; }

    ~MDRTask()
    {
        if (coroutine)
            coroutine.destroy();
    }

    bool await_ready() const noexcept { return !coroutine || coroutine.done(); }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> next) noexcept
    {
        coroutine.promise().next = next;
        return coroutine;
    }

    void await_resume()
    {
        auto& p = coroutine.promise();
        if (p.exception)
            std::rethrow_exception(p.exception);
    }
};

inline MDRTask MDRTask::promise_type::get_return_object()
{
    return MDRTask{std::coroutine_handle<MDRTask::promise_type>::from_promise(*this)};
}

// NOLINTEND

struct MDRHeadphones
{
    enum AwaitType
    {
        // Wait for an immediate ACK from the device on the current task
        AWAIT_ACK = 0,
        AWAIT_PROTOCOL_INFO = 1,
        AWAIT_SUPPORT_FUNCTION = 2,
        AWAIT_NUM_TYPES = 3
    };
    // NOLINTBEGIN
    struct Awaiter
    {
        MDRHeadphones* self;
        AwaitType type;
        static bool await_ready() noexcept { return false; }

        void await_suspend(std::coroutine_handle<> handle) noexcept
        {
            auto& dst = self->mAwaiters[static_cast<size_t>(type)];
            if (dst) [[unlikely]]
                std::terminate(); // This is your fault.
                                  // How'd you get more than one task at a time running?
            if (handle)
                dst = handle;
        }

        static void await_resume() noexcept {
        }
    };

    // NOLINTEND
    std::string mLastError{"Nothing here to see"};

    std::deque<UInt8> mRecvBuf, mSendBuf;
    MDRConnection* mConn;

    MDRCommandSeqNumber mSeqNumber{0};

    std::array<std::coroutine_handle<>, AWAIT_NUM_TYPES> mAwaiters{};
    MDRTask mTask;

    explicit MDRHeadphones(MDRConnection* conn) :
        mConn(conn)
    {
    }

    // Pinned.
    MDRHeadphones(MDRHeadphones const&) = delete;
    MDRHeadphones(MDRHeadphones&&) = delete;

    int Receive();
    int Send();
    /**
     * @brief Dequeues a _complete_ command payload and spawns appropriate coroutines - and advances them _here_.
     * @note  This is a no-op if buffer is incomplete and no complete command payload can be produced.
     * @note  Non-blocking. Need @ref Receive, @ref Sent to be polled periodically.
     * @note  This is your best friend.
     */
    void MoveNext();
    /**
     * @brief Executes the task, and _may_ schedule it later if it wants to @ref Await on something
     * @return @ref MDR_RESULT_OK if task has been resumed _at least_ once, @ref MDR_RESULT_INPROGRESS if _another_ task
     *         is still being executed.
     */
    int Invoke(MDRTask&& task);
    /**
     * @brief This does what you think it does.
     */
    [[nodiscard]] const char* GetLastError() const { return mLastError.c_str(); }

#pragma region Tasks
    MDRTask RequestInit();
#pragma endregion

private:
    // XXX: So, very naive. We just die if anything bad happens.
    void ExceptionHandler(auto&& func)
    {
        try
        {
            func();
        }
        catch (std::runtime_error& exc)
        {
            fmt::println("!!! Exception: {}", exc.what());
            mLastError = exc.what();
            mConn->disconnect(mConn->user);
        }
    }

    /**
     * @brief This does what you think it does.
     */
    Awaiter Await(AwaitType type);
    /**
     * @note Queues a command payload to be sent through @ref Send. You generally don't need to call this directly.
     * @note Non-blocking. Need @ref Sent to be polled periodically.
     */
    void SendCommandImpl(Span<const UInt8> command, MDRDataType type, MDRCommandSeqNumber seq);
    void SendACK(MDRCommandSeqNumber seq);
    /**
     * @note Queues a command payload to be sent through @ref Send, with a @ref AWAIT_ACK wait
     * @note Non-blocking. Need @ref Sent to be polled periodically.
     */
    template<MDRIsSerializable T, typename ...Args>
    MDRTask SendCommandACK(Args&&... args)
    {
        UInt8 buf[kMDRMaxPacketSize];

        MDRDataType type = MDRTraits<T>::kDataType;
        T command(std::forward<Args>(args)...);
        T::Validate(command); // <- Throws if something's bad
        size_t size = T::Serialize(command, buf);
        SendCommandImpl({ buf, buf + size}, type, mSeqNumber);
        co_await Await(AWAIT_ACK);
    }
    /**
     * @brief Check if the coroutine frame has been completed - and if so, resets the current @ref mTask
     *        and allow subsequent @ref Invoke calls to take effect.
     */
    bool TaskMoveNext();
    void Handle(Span<const UInt8> command, MDRDataType type, MDRCommandSeqNumber seq);
    void HandleAck(MDRCommandSeqNumber seq);
    void HandleCommandV2T1(Span<const UInt8> command, MDRCommandSeqNumber seq);
    void HandleCommandV2T2(Span<const UInt8> command, MDRCommandSeqNumber seq);
};
