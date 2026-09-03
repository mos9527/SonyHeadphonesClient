#pragma once

#include <mdr-c/Headphones.h>
#include <mdr/Command.hpp>
#include <mdr/Protocol.hpp>
#include "Property.hpp"
#include "DetailsV1.hpp"
#include "DetailsV2.hpp"

#include <coroutine>
#include <time.h>

namespace mdr
{
    namespace detail
    {
        template <typename T>
        bool ReadEnumTag(Span<const UInt8> command, T& out, size_t offset = 1)
        {
            if (command.size() <= offset)
                return false;
            out = static_cast<T>(command[offset]);
            return true;
        }
    } // namespace detail

    // NOLINTBEGIN
    /**
     * @brief Coroutine task boilerplate from https://github.com/mos9527/coro
     * @note The coroutine MUST return a value on the Headphones's event channel: an @ref MDREvent
     *       describing what completed, or -1 on failure. @ref SetLastError produces the latter.
     */
    struct MDRTask
    {
        struct promise_type
        {
            std::coroutine_handle<> next;
            int result;
            static std::suspend_always initial_suspend() noexcept { return {}; }
            [[noreturn]] static void unhandled_exception() noexcept { std::terminate(); }

            struct final_awaiter
            {
                static bool await_ready() noexcept { return false; }

                std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> other) noexcept
                {
                    if (auto& p = other.promise(); p.next)
                        return p.next;
                    return std::noop_coroutine();
                }

                static void await_resume() noexcept {}
            };

            static final_awaiter final_suspend() noexcept { return final_awaiter{}; }
            MDRTask get_return_object();

            void return_value(int v) { result = v; }

            // https://en.cppreference.com/w/cpp/language/coroutines#:~:text=the%20caller/resumer.-,Dynamic%20allocation,-Coroutine%20state
            void* operator new(std::size_t n) noexcept { return mdr::MDRAllocator<char>().allocate(n); }
            void operator delete(void* p, std::size_t n) noexcept
            {
                mdr::MDRAllocator<char>().deallocate(static_cast<char*>(p), n);
            }
            static MDRTask get_return_object_on_allocation_failure() noexcept
            {
                MDR_CHECK(false && "MDRTask allocation failure");
                return {};
            }
        };

        using value_type = void;
        std::coroutine_handle<promise_type> coroutine{nullptr};

        MDRTask() = default;

        explicit MDRTask(std::coroutine_handle<promise_type> handle) : coroutine(handle) {};
        MDRTask(const MDRTask&) = delete;

        MDRTask& operator=(MDRTask&& other) noexcept
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

        int await_resume() const noexcept { return coroutine ? coroutine.promise().result : -1; }
    };

    inline MDRTask MDRTask::promise_type::get_return_object()
    {
        return MDRTask{std::coroutine_handle<MDRTask::promise_type>::from_promise(*this)};
    }

    // NOLINTEND
    struct MDRHeadphones
    {
    private:
        MDRConnection* mConn;

        // TODO Make these configurable
        int mACKRetriesCount = 10u;
        int mACKRetriesTimeout = 1000u;
        int mDefaultTimeout = 5000u;
    public:
        enum AwaitType
        {
            // Wait for an immediate ACK from the device on the current task
            AWAIT_ACK = 0,
            AWAIT_PROTOCOL_INFO = 1,
            AWAIT_SUPPORT_FUNCTION = 2,
            AWAIT_MODEL_INFO = 3,
            AWAIT_NUM_TYPES = 4
        };

        enum class ProtocolFamily
        {
            UNKNOWN,
            V1,
            V2
        };

        // NOLINTBEGIN
        struct Awaiter
        {
            MDRHeadphones* self;
            AwaitType type;

            std::coroutine_handle<> h = nullptr;
            // Timepoint when Awaiter is invoked in milliseconds
            clock_t tick;
            // Timeout in milliseconds
            int timeout;
            // co_await Result on resumption
            int result = MDR_RESULT_OK;

            static bool await_ready() noexcept { return false; }

            void await_suspend(std::coroutine_handle<> handle) noexcept
            {
                if (h) [[unlikely]]
                    std::terminate(); // Misuse. Only _one_ task is allowed at a time
                if (handle)
                    h = std::move(handle), tick = clock();
            }

            int await_resume() noexcept { return result; }

            constexpr operator bool() const { return h != nullptr; }

            void resume_now(int await_result)
            {
                auto handle = h;
                h = nullptr;
                result = await_result;
                handle.resume();
            }
        };

        // NOLINTEND

        explicit MDRHeadphones(MDRConnection* conn, ProtocolFamily family) : mConn(conn), mProtocolFamily(family)
        {
            for (size_t i = 0; i < AWAIT_NUM_TYPES; ++i)
                mAwaiters[i] = Awaiter{this, static_cast<AwaitType>(i)};
        }

        MDRHeadphones() : MDRHeadphones(nullptr, ProtocolFamily::UNKNOWN) {}


        // Move-only ctor
        MDRHeadphones(MDRHeadphones const&) = delete;
        MDRHeadphones(MDRHeadphones&&) noexcept = default;
        MDRHeadphones& operator=(MDRHeadphones const&) = delete;
        MDRHeadphones& operator=(MDRHeadphones&&) = default;

        constexpr operator bool() const noexcept { return mConn != nullptr; }

        /**
         * @breif Receive commands and process events. This is non-blocking, and should be
         *        run in - for example - your UI loop.
         * @note  This is your best friend.
         * @note  This function does not block. To not burn cycles for fun - poll on your @ref MDRConnection
         *        with @ref mdrConnectionPoll is recommended
         * @param outEvent Receives what happened, or @ref MDR_EVENT_NONE when nothing did.
         * @return MDR_RESULT_OK, or the code behind the failure. @ref GetLastError describes it.
         */
        ::MDRResult PollEvents(MDREvent& outEvent);
        void SetPacketCallback(MDRPacketCallback callback, void* userData) noexcept
        {
            mPacketCallback = callback;
            mPacketCallbackUserData = userData;
        }
        /**
         * @brief Check if @ref MDRHeadphones is ready to do more @ref Invoke.
         */
        [[nodiscard]] bool IsReady() const;
        /**
         * @brief Check if there's any @ref MDRProperty that's dirty.
         */
        [[nodiscard]] bool IsDirty() const;
        [[nodiscard]] bool IsDirtyV1() const;
        [[nodiscard]] bool IsDirtyV2() const;
        /**
         * @brief Schedules the task to be run on the next @ref MoveNext call.
         * @return @ref MDR_RESULT_OK if task has been scheduled, @ref MDR_RESULT_INPROGRESS if _another_ task
         *         is still being executed.
         * @note @ref TaskMoveNext frees the completed task.
         */
        int Invoke(MDRTask&& task);
        /**
         * @brief This does what you think it does.
         *        Schedules the calling coroutine to be executed once the next @ref AwaitType
         *        event has arrived through @ref MoveNext
         * @param type The type of event to wait for.
         * @param timeoutMS The timeout in milliseconds. If the event does not arrive within this time, the coroutine
         *                  will be resumed with @ref MDR_RESULT_ERROR_TIMEOUT.
         *                  Defaults to -1, which means @ref mDefaultTimeout is used instead.
         * @note  As always, needs @ref PollEvents
         */
        Awaiter& Await(AwaitType type, int timeoutMS = -1);
        /**
         * @brief Wake up zero or one awaited coroutine, and resume it in the current callstack.
         */
        void Awake(AwaitType type);
        /**
         * @brief This does what you think it does.
         */
        [[nodiscard]] const char* GetLastError() const { return mLastError.c_str(); }
        /**
         * @brief Record @p error against @p context and signal it on the event channel.
         */
        int SetLastError(int error, const char* context)
        {
            mLastError = mdr::Format("{} ({})", context, mdrResultString(error));
            mLastErrorCode = static_cast<::MDRResult>(error);
            return -1; // NOTE: convenience only for co_return SetLastError(...);
        }

        ProtocolFamily mProtocolFamily{ProtocolFamily::UNKNOWN};
        DetailsV1 mDetailsV1;
        DetailsV2 mDetailsV2;

#pragma region Tasks
        /**
         * @brief Selects the wire protocol and initializes its backend.
         */
        MDRTask RequestInit();
        /**
         * @brief Dispatches state synchronization to the selected backend.
         */
        MDRTask RequestSync();
        /**
         * @brief Dispatches staged changes to the selected backend.
         */
        MDRTask RequestCommit();

        /**
         * @brief Queues an arbitrary debugger payload through the normal Headphones
         * send path. This is an internal C++ developer API, not part of the C ABI.
         */
        MDRTask RequestDebugCommand(MDRBuffer payload, MDRDataType type, MDRCommandSeqNumber sequence, bool awaitAck);
        [[nodiscard]] MDRCommandSeqNumber CurrentSequenceNumber() const noexcept { return mSeqNumber; }

        MDRTask RequestInitV1();
        MDRTask RequestSyncV1();
        MDRTask RequestCommitV1();
        void SnapshotPropertiesV1();
        void RefreshSupportV1();

        /**
         * @brief Send initialization payloads to the headphones.
         * @note  To be used with @ref Invoke.
         * @return @ref MDR_EVENT_INITIALIZE_COMPLETE on completion (returned in @ref PollEvents)
         **/
        MDRTask RequestInitV2();
        /**
         * @brief Requests states that the device won't send automatically. (e.g. Battery levels)
         * @note  To be used with @ref Invoke.
         * @return @ref MDR_EVENT_SYNC_COMPLETE on completion (returned in @ref PollEvents)
         **/
        MDRTask RequestSyncV2();
        /**
         * @brief Requests all changed @ref MDRProperty up until this point to be set on the device
         * @note  To be used with @ref Invoke.
         * @return @ref MDR_EVENT_APPLY_COMPLETE on completion (returned in @ref PollEvents)
         */
        MDRTask RequestCommitV2();
        void SnapshotPropertiesV2();
#pragma endregion

        // Common lifecycle state; family device state lives in DetailsV1/DetailsV2.
        bool mInitialized{};

    private:
        /**
         * @brief @ref SetLastError for callers that report an MDR_RESULT_ rather than an event.
         */
        ::MDRResult Fail(int error, const char* context)
        {
            SetLastError(error, context);
            return mLastErrorCode;
        }

        String mLastError = "N/A";
        ::MDRResult mLastErrorCode{MDR_RESULT_ERROR_GENERAL};

        MDRPacketCallback mPacketCallback{};
        void* mPacketCallbackUserData{};
        Deque<UInt8> mRecvBuf, mSendBuf;
        MDRCommandSeqNumber mSeqNumber{0};

        MDRTask mTask;
        Array<Awaiter, AWAIT_NUM_TYPES> mAwaiters{};

        // Friend of PollEvents, so friend of yours too.
        int Receive();
        // Friend of PollEvents, so friend of yours too.
        int Send();
        /**
         * @brief Dequeues a _complete_ command payload and spawns appropriate coroutines - and advances them _here_.
         * @note  This is a no-op if buffer is incomplete and no complete command payload can be produced.
         * @note  Non-blocking. Need @ref Receive, @ref Sent to be polled periodically by @ref PollEvents
         * @return One of MDR_HEADPHONES_* event types
         */
        int MoveNext();
        /**
         * @brief Check if the coroutine frame has been completed - and if so, frees the current @ref mTask
         *        and allow subsequent @ref Invoke calls to take effect.
         * @return true if a task has been completed _here_. No tasks, or in-progress results in false.
         * @note Tasks are spawned with @ref Invoke.
         * @note Tasks are NOT resumed here - instead, they are always started by @ref Invoke, and possibly awaken by
         * @ref Awake and start execution at the respective call site.
         */
        bool TaskMoveNext(int& result);
        /**
         * @note Queues a command payload to be sent through @ref Send. You generally don't need to call this directly.
         * @note Non-blocking. Need @ref Sent to be polled periodically.
         */
        void SendCommandImpl(Span<const UInt8> command, MDRDataType type, MDRCommandSeqNumber seq);
        void SendACK(MDRCommandSeqNumber seq);
        /**
         * @note Queues a command payload of @ref MDRIsSerializable type to be sent through @ref Send.
         * @note Non-blocking. Need @ref Sent to be polled periodically.
         * @note You _usually_ need to wait for an @ref AWAIT_ACK. Use the @ref MDR_SEND_COMMAND_ACK macro to send and
         * wait for one!
         */
        template <MDRIsSerializable T>
        int SendCommandImpl(T const& command = {})
        {
            UInt8 buf[kMDRMaxPacketSize];

            MDRDataType type = MDRTraits<T>::kDataType;
            const auto serialized = T::Serialize(command, buf, kMDRMaxPacketSize);
            if (!serialized)
            {
                SetLastError(serialized.error,
                             serialized.errMessage ? serialized.errMessage : "Unable to serialize command");
                return serialized.error;
            }
            SendCommandImpl({buf, buf + serialized.value}, type, mSeqNumber);
            return MDR_RESULT_OK;
        }

        /**
         * @brief Handles current command, and generates an event associated with it.
         * @return One of MDR_HEADPHONES_* event types
         */
        int Handle(Span<const UInt8> command, MDRDataType type, MDRCommandSeqNumber seq);
        int HandleProtocolInfo(Span<const UInt8> command);
        int HandleProtocolInfoV1(Span<const UInt8> command);
        int HandleProtocolInfoV2(Span<const UInt8> command);
        int HandleCommandV1T1(Span<const UInt8> cmd, MDRCommandSeqNumber seq);
        int HandleCommandV1T2(Span<const UInt8> cmd, MDRCommandSeqNumber seq);
        int HandleCommandV2T1(Span<const UInt8> cmd, MDRCommandSeqNumber seq);
        int HandleCommandV2T2(Span<const UInt8> cmd, MDRCommandSeqNumber seq);
        void HandleAck(MDRCommandSeqNumber seq);
    };

} // namespace mdr

namespace mdr::detail
{
    inline MDRHeadphones* HeadphonesImpl(::MDRHeadphones* headphones) noexcept
    {
        return reinterpret_cast<MDRHeadphones*>(headphones);
    }

    inline const MDRHeadphones* HeadphonesImpl(const ::MDRHeadphones* headphones) noexcept
    {
        return reinterpret_cast<const MDRHeadphones*>(headphones);
    }

    inline ::MDRHeadphones* HeadphonesHandle(MDRHeadphones* headphones) noexcept
    {
        return reinterpret_cast<::MDRHeadphones*>(headphones);
    }
} // namespace mdr::detail

// NOLINTBEGIN
/**
 * @brief Sends command through @ref SendCommandImpl<T>, and re-schedule ourselves to
 *        co_await for an @ref Await(AWAIT_ACK) on the coroutine.
 * @param Type Command payload of @ref MDRIsSerializable type
 * @note  This is ONLY meaningful within a @ref MDRTask coroutine, as this schedules
 *        the current task to wait on a @ref AWAIT_ACK event.
 *
 * As to _why_ this is here instead of a templated member function - instantiated
 * templates would create their own @ref MDRTask and generate code for EACH of them,
 * while all we need is merely a `co_await`...
 *
 * TL;DR, this helps with compiler bloats. Use it well.
 *
 * @note On bumping mSeqNumber. Ignoring transport issues (which is not a thing with RFCOMM backends at least), a
 * timeout can only occur when:
 *       - The device is shutting down
 *       - Or when we actually _missed_ a packet. Which can happen as chunked packets are discared by us _currently_
 *         We should handle this (hence the FIXME). For now retrying by assuming we got another ACK works despite the
 * lack thereof.
 */
#define SendCommandACK(Type, ...)                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        int _retries;                                                                                                  \
        for (_retries = 0; _retries < mACKRetriesCount; _retries++)                                                    \
        {                                                                                                              \
            const int _sendResult = SendCommandImpl<Type>(__VA_ARGS__);                                                \
            if (_sendResult != MDR_RESULT_OK)                                                                          \
                co_return -1u;                                                                                         \
            int res = co_await Await(AWAIT_ACK, mACKRetriesTimeout);                                                   \
            if (res == MDR_RESULT_OK)                                                                                  \
                break;                                                                                                 \
            MDR_LOG("FIXME-ACK Timeout. Retry {}/{}", _retries, mACKRetriesCount);                                     \
            mSeqNumber ^= 1;                                                                                           \
        }                                                                                                              \
        if (_retries == mACKRetriesCount)                                                                              \
            co_return SetLastError(MDR_RESULT_ERROR_TIMEOUT, "Timeout exceeded waiting for device to respond");        \
    }                                                                                                                  \
    while (false)

/**
 * @brief Just a helper macro to deserialize a command payload.
 */
#define Deserialize(Type, Name, Command)                                                                               \
    auto Name##Result = Type::Deserialize((Command).data(), (Command).size());                                         \
    if (!Name##Result)                                                                                                 \
        return self->SetLastError(Name##Result.error,                                                                  \
                                  Name##Result.errMessage ? Name##Result.errMessage : "Unable to deserialize " #Type); \
    auto& Name = Name##Result.value;                                                                                   \
    MDR_LOG_DEBUG("Deserialized " #Type)

// NOLINTEND
