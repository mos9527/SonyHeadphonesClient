#pragma once
#include "Command.hpp"
#include "ProtocolV2T1.hpp"
#include "ProtocolV2T2.hpp"
#include <mdr-c/Headphones.h>

#include <deque>
#include <coroutine>

namespace mdr
{
    // NOLINTBEGIN
    /**
     * @brief Coroutine task boilerplate from https://github.com/mos9527/coro
     * @note The coroutine MUST return one of MDR_HEADPHONES_... values indicating its completion.
     */
    struct MDRTask
    {
        struct promise_type
        {
            std::exception_ptr exception;
            std::coroutine_handle<> next;
            int result;
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

            void return_value(int v) { result = v; }
        };

        using value_type = void;
        std::coroutine_handle<promise_type> coroutine{nullptr};

        MDRTask() = default;

        explicit MDRTask(std::coroutine_handle<promise_type> handle) :
            coroutine(handle)
        {
        };
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
    template <typename T>
    struct MDRProperty
    {
        T desired{};
        T current{};

        void overwrite(T const& value) { desired = current = value; }
        [[nodiscard]] constexpr bool dirty() const noexcept { return desired != current; }
        void commit() { current = desired; }
    };

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

            static void await_resume() noexcept
            {
            }
        };

        // NOLINTEND

        MDRConnection* mConn;

        MDRHeadphones() : mConn(nullptr) {};
        explicit MDRHeadphones(MDRConnection* conn) :
            mConn(conn)
        {
        }

        // Pinned.
        MDRHeadphones(MDRHeadphones const&) = delete;
        MDRHeadphones(MDRHeadphones&&) = delete;

        constexpr operator bool() const noexcept { return mConn != nullptr; }

        /**
         * @breif Receive commands and process events. This is non-blocking, and should be
         *        run in - for example - your UI loop.
         * @note  This is your best friend.
         * @note  This function does not block. To not burn cycles for fun - poll on your @ref MDRConnection
         *        with @ref mdrConnectionPoll is recommended
         * @return One of MDR_HEADPHONES_* values
         */
        int PollEvents();
        /**
         * @brief Check if @ref MDRHeadphones is ready to do more @ref Invoke.
         */
        [[nodiscard]] bool IsReady() const;
        /**
         * @brief Check if there's any @ref MDRProperty that's dirty.
         */
        [[nodiscard]] bool IsDirty() const;
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
         * @note  As always, needs @ref PollEvents
         */
        Awaiter Await(AwaitType type);
        /**
         * @brief Wake up zero or one awaited coroutine, and resume it in the current callstack.
         */
        void Awake(AwaitType type);
        /**
         * @brief This does what you think it does.
         */
        [[nodiscard]] const char* GetLastError() const { return mLastError.c_str(); }

#pragma region States
        // @ref HandleProtocolInfoT1
        struct ProtocolStates
        {
            int version;
            int hasTable1;
            int hasTable2;
        } mProtocol{};

        // @ref HandleSupportFunctionT1
        // Q: Why not std::bitset?
        // A: They are not constexpr until C++23 - while std::array[] are since 14.
        //    Since there's no other C++23 feature usage anywhere else in the lib,
        //    we're sticking with C++20 as is.
        struct SupportStates
        {
            Array<bool, 256> table1Functions;
            Array<bool, 256> table2Functions;

            [[nodiscard]] constexpr bool contains(v2::MessageMdrV2FunctionType_Table1 v) const
            {
                return table1Functions[static_cast<UInt8>(v)];
            }

            [[nodiscard]] constexpr bool contains(v2::MessageMdrV2FunctionType_Table2 v) const
            {
                return table2Functions[static_cast<UInt8>(v)];
            }
        } mSupport{};

        String mUniqueId;
        String mFWVersion;
        String mModelName;
        v2::t1::ModelSeriesType mModelSeries{};
        v2::t1::ModelColor mModelColor{};
        v2::t1::AudioCodec mAudioCodec{};

        v2::t1::AlertMessageType mLastAlertMessage{};
        String mLastInteractionMessage;
        String mLastDeviceJSONMessage;

        struct PeripheralDevice
        {
            String macAddress;
            String name;
        };

        Vector<PeripheralDevice> mPairedDevices;
        UInt8 mPairedDevicesPlaybackDeviceIndex{};

        int mSafeListeningSoundPressure{};

        struct BatteryState
        {
            UInt8 level{};
            UInt8 threshold{};
            v2::t1::BatteryChargingStatus charging{};
        };
        BatteryState mBatteryL, mBatteryR, mBatteryCase;

        String mPlayTrackTitle;
        String mPlayTrackAlbum;
        String mPlayTrackArtist;

        struct GsCapability
        {
            v2::t1::GsSettingType type{};
            v2::t1::GsSettingInfo value{};
        };

        GsCapability mGsCapability1, mGsCapability2,
                     mGsCapability3, mGsCapability4;
#pragma endregion

#pragma region Properties
        MDRProperty<bool> mNcAsmEnabled;
        MDRProperty<bool> mNcAsmFocusOnVoice;
        MDRProperty<UInt8> mNcAsmAmbientLevel; // [0,20] - 0 is not possible on the App.
        MDRProperty<v2::t1::Function> mNcAsmButtonFunction;
        MDRProperty<v2::t1::NcAsmMode> mNcAsmMode;
        MDRProperty<bool> mNcAsmAutoAsmEnabled; // WH-1000XM6+
        MDRProperty<v2::t1::NoiseAdaptiveSensitivity> mNcAsmNoiseAdaptiveSensitivity; // WH-1000XM6+

        MDRProperty<v2::t1::AutoPowerOffElements> mPowerAutoOff;
        MDRProperty<v2::t1::AutoPowerOffWearingDetectionElements> mPowerAutoOffWearingDetection;

        MDRProperty<v2::t1::PlaybackStatus> mPlayPause;
        MDRProperty<UInt8> mPlayVolume; // [0,30]


        MDRProperty<bool> mGsParamBool1, mGsParamBool2,
                          mGsParamBool3, mGsParamBool4;

        MDRProperty<v2::t1::UpscalingType> mUpscalingType;
        MDRProperty<bool> mUpscalingAvailable;
        MDRProperty<bool> mUpscalingEnabled;

        MDRProperty<v2::t1::PriorMode> mAudioPriorityMode;

        MDRProperty<bool> mBGMModeEnabled;
        MDRProperty<v2::t1::RoomSize> mBGMModeRoomSize;
        MDRProperty<bool> mUpmixCinemaEnabled;

        MDRProperty<bool> mAutoPauseEnabled;

        MDRProperty<v2::t1::Preset> mTouchFunctionLeft, mTouchFunctionRight;

        MDRProperty<bool> mSpeakToChatEnabled;
        MDRProperty<v2::t1::DetectSensitivity> mSpeakToChatDetectSensitivity;
        MDRProperty<v2::t1::ModeOutTime> mSpeakToModeOutTime;

        MDRProperty<bool> mHeadGestureEnabled;


        MDRProperty<bool> mEqAvailable;
        MDRProperty<v2::t1::EqPresetId> mEqPresetId;
        MDRProperty<int> mEqClearBass;
        // Non-zero band count of either 5: [400,1k,2.5k,6.3k,16k] or 10: [31,63,125,250,500,1k,2k,4k,8k,16k]
        MDRProperty<Vector<int>> mEqConfig;

        MDRProperty<bool> mVoiceGuidanceEnabled;
        // Volume range [-2,2]
        MDRProperty<int> mVoiceGuidanceVolume;

        MDRProperty<bool> mPairingMode;

        MDRProperty<String> mMultipointDeviceMac;

        MDRProperty<bool> mSafeListeningPreviewMode;
#pragma endregion

#pragma region Tasks
        /**
         * @brief Send initialization payloads to the headphones.
         * @note  To be used with @ref Invoke.
         * @return @ref MDR_HEADPHONES_TASK_INIT_OK on completion (returned in @ref PollEvents)
         **/
        MDRTask RequestInit();
        /**
         * @brief Requests states that the device won't send automatically. (e.g. Battery levels)
         * @note  To be used with @ref Invoke.
         * @return @ref MDR_HEADPHONES_TASK_SYNC_OK on completion (returned in @ref PollEvents)
         **/
        MDRTask RequestSync();
        /**
         * @brief Requests all changed @ref MDRProperty up until this point to be set on the device
         * @note  To be used with @ref Invoke.
        * @return @ref MDR_HEADPHONES_TASK_COMMIT_OK on completion (returned in @ref PollEvents)
         */
        MDRTask RequestCommit();
#pragma endregion

    private:
        String mLastError{};

        std::deque<UInt8> mRecvBuf, mSendBuf;
        MDRCommandSeqNumber mSeqNumber{0};

        MDRTask mTask;
        Array<std::coroutine_handle<>, AWAIT_NUM_TYPES> mAwaiters{};

        // XXX: So, very naive. We just die if anything bad happens.
        bool ExceptionHandler(auto&& func)
        {
            try
            {
                return func();
            }
            catch (const std::runtime_error& exc)
            {
                fmt::println("!!! Exception: {}", exc.what());
                mLastError = exc.what();
                mConn->disconnect(mConn->user);
            }
            return false;
        }

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
         * @note Tasks are NOT resumed here - instead, they are always started by @ref Invoke, and possibly awaken by @ref Awake
         *       and start execution at the respective call site.
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
         * @note You _usually_ need to wait for an @ref AWAIT_ACK. Use the @ref MDR_SEND_COMMAND_ACK macro to send and wait for one!
         */
        template <MDRIsSerializable T>
        void SendCommandImpl(T const& command = {})
        {
            UInt8 buf[kMDRMaxPacketSize];

            MDRDataType type = MDRTraits<T>::kDataType;
            T::Validate(command); // <- Throws if something's bad
            size_t size = T::Serialize(command, buf, kMDRMaxPacketSize);
            SendCommandImpl({buf, buf + size}, type, mSeqNumber);
        }
        /**
         * @brief Handles current command, and generates an event associated with it.
         * @return One of MDR_HEADPHONES_* event types
         */
        int Handle(Span<const UInt8> command, MDRDataType type, MDRCommandSeqNumber seq);
        int HandleCommandV2T1(Span<const UInt8> cmd, MDRCommandSeqNumber seq);
        int HandleCommandV2T2(Span<const UInt8> cmd, MDRCommandSeqNumber seq);
        void HandleAck(MDRCommandSeqNumber seq);
    };
}

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
 */
#define SendCommandACK(Type, ...) \
    { \
    SendCommandImpl<Type>( __VA_ARGS__ ); \
    co_await Await(AWAIT_ACK); \
    }
// NOLINTEND
