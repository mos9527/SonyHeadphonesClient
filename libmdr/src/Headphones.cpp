// ReSharper disable CppParameterMayBeConstPtrOrRef
#include <algorithm>
#include <cstring>
#include <limits>
#include <span>
#include <string_view>
#include "Details.hpp"

namespace mdr
{
    MDRHeadphones::Awaiter& MDRHeadphones::Await(AwaitType type, uint64_t wait, int timeoutMS)
    {
        auto& awaiter = mAwaiters[type];
        awaiter.wait = wait;
        awaiter.timeout = timeoutMS > 0 ? timeoutMS : mDefaultTimeoutMS;
        return awaiter;
    }

    void MDRHeadphones::Awake(AwaitType type)
    {
        MDR_CHECK(mCurrentPacket != 0 &&
                  "FIXME-No currently active packet! Awake MUST only be invoked from packet Handlers!");
        auto& awaiter = mAwaiters[type];
        awaiter.signal = mCurrentPacket;
        if (awaiter && awaiter.signal_ready())
            awaiter.resume_now(MDR_RESULT_OK);
    }

    MDRTask MDRHeadphones::RequestInit(MDRHeadphonesInitFlags flags)
    {
        switch (mProtocolFamily)
        {
        case ProtocolFamily::V1:
            co_return co_await RequestInitV1(flags);
        case ProtocolFamily::V2:
            co_return co_await RequestInitV2(flags);
        default:
            co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "MDR protocol family has not been selected");
        }
    }

    MDRTask MDRHeadphones::RequestSync(MDRHeadphonesSyncFlags flags)
    {
        switch (mProtocolFamily)
        {
        case ProtocolFamily::V1:
            co_return co_await RequestSyncV1(flags);
        case ProtocolFamily::V2:
            co_return co_await RequestSyncV2(flags);
        default:
            co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "MDR protocol has not been selected");
        }
    }

    MDRTask MDRHeadphones::RequestCommit()
    {
        switch (mProtocolFamily)
        {
        case ProtocolFamily::V1:
            co_return co_await RequestCommitV1();
        case ProtocolFamily::V2:
            co_return co_await RequestCommitV2();
        default:
            co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "MDR protocol has not been selected");
        }
    }

    MDRTask MDRHeadphones::RequestDebugCommand(MDRBuffer payload, MDRDataType type, bool awaitAck)
    {
        const uint64_t packetId = mPacketCounter;
        SendCommandImpl(payload, type, mSeqNumber);
        if (awaitAck)
        {
            const int result = co_await Await(AWAIT_ACK, packetId);
            if (result != MDR_RESULT_OK)
                co_return SetLastError(result, "Debugger command did not receive an ACK");
        }
        co_return MDR_EVENT_UNHANDLED;
    }

    ::MDRResult MDRHeadphones::PollEvents(MDREvent& outEvent)
    {
        outEvent = MDR_EVENT_NONE;
        if (mFailed)
            return mLastErrorCode;
        int r = mdrConnectionPoll(mConn, 0);
        if (r == MDR_RESULT_OK)
        {
            // Non-blocking. INPROGRESS are expected, not so much for others.
            // Failfast if that happens - the owner should kill us.
            r = Send();
            if (r != MDR_RESULT_OK && r != MDR_RESULT_INPROGRESS)
                return Fail(r, "Unable to send to the device");
            r = Receive();
            if (r != MDR_RESULT_OK && r != MDR_RESULT_INPROGRESS)
                return Fail(r, "Unable to receive from the device");
        }
        else
        {
            if (r != MDR_RESULT_ERROR_TIMEOUT && r != MDR_RESULT_INPROGRESS)
                return Fail(r, "Unable to poll the connection");
        }
        const int raw = MoveNext();
        if (raw == -1 || mFailed)
            return mLastErrorCode;
        outEvent = static_cast<MDREvent>(raw);
        return MDR_RESULT_OK;
    }

    bool MDRHeadphones::IsReady() const
    {
        return !mFailed && !mTask;
    }

    bool MDRHeadphones::IsDirty() const
    {
        if (mFailed)
            return false;
        switch (mProtocolFamily)
        {
        case ProtocolFamily::V1:
            return IsDirtyV1();
        case ProtocolFamily::V2:
            return IsDirtyV2();
        default:
            return false;
        }
    }

    int MDRHeadphones::Receive()
    {
        char buf[kMDRMaxPacketSize];
        int recvd;
        const int r = mdrConnectionRecv(mConn, buf, kMDRMaxPacketSize, &recvd);
        if (r != MDR_RESULT_OK)
            return r;
#ifdef MDR_DEBUG
        mdr::String dump = "<< ";
        for (char* p = buf; p != buf + recvd; p++)
            dump += mdr::Format("{:02X} ", static_cast<UInt8>(*p));
        MDR_LOG("{}", dump);
#endif
        mRecvBuf.insert(mRecvBuf.end(), buf, buf + recvd);
        return r;
    }

    int MDRHeadphones::Send()
    {
        if (mSendBuf.empty())
            return MDR_RESULT_OK;
        char buf[kMDRMaxPacketSize];
        int toSend = std::min(mSendBuf.size(), kMDRMaxPacketSize), sent = 0;
        std::copy_n(mSendBuf.begin(), toSend, buf);
        const int r = mdrConnectionSend(mConn, buf, toSend, &sent);
        if (r != MDR_RESULT_OK)
            return r;
#ifdef MDR_DEBUG
        mdr::String dump = ">> ";
        for (char* p = buf; p != buf + sent; p++)
            dump += mdr::Format("{:02X} ", static_cast<UInt8>(*p));
        MDR_LOG("{}", dump);
#endif
        mSendBuf.erase(mSendBuf.begin(), mSendBuf.begin() + sent);
        return r;
    }


    int MDRHeadphones::HandleProtocolInfo(Span<const UInt8> command)
    {
        switch (mProtocolFamily)
        {
        case ProtocolFamily::V1:
            return HandleProtocolInfoV1(command);
        case ProtocolFamily::V2:
            return HandleProtocolInfoV2(command);
        default:
            return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "MDR protocol family is not supported");
        }
    }

    int MDRHeadphones::Handle(Span<const UInt8> command, MDRDataType type, MDRCommandSeqNumber seq)
    {
        using enum MDRDataType;
        switch (type)
        {
        case ACK:
            HandleAck(seq);
            break;
        case DATA_MDR:
            SendACK(seq);
            if (!command.empty() && command.front() == 0x01u)
                return HandleProtocolInfo(command);
            if (mProtocolFamily == ProtocolFamily::UNKNOWN)
                return SetLastError(MDR_RESULT_ERROR_MALFORMED_PAYLOAD,
                                    "Received MDR Table 1 data before CONNECT_RET_PROTOCOL_INFO");
            switch (mProtocolFamily)
            {
            case ProtocolFamily::V1:
                return HandleCommandV1T1(command, seq);
            case ProtocolFamily::V2:
                return HandleCommandV2T1(command, seq);
            default:
                return MDR_EVENT_UNHANDLED;
            }
        case DATA_MDR_NO2:
            SendACK(seq);
            if (mProtocolFamily == ProtocolFamily::UNKNOWN)
                return SetLastError(MDR_RESULT_ERROR_MALFORMED_PAYLOAD,
                                    "Received MDR Table 2 data before CONNECT_RET_PROTOCOL_INFO");
            switch (mProtocolFamily)
            {
            case ProtocolFamily::V1:
                return HandleCommandV1T2(command, seq);
            case ProtocolFamily::V2:
                return HandleCommandV2T2(command, seq);
            default:
                return MDR_EVENT_UNHANDLED;
            }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int MDRHeadphones::MoveNext()
    {
        // Awaiter timeouts
        {
            const auto now = std::chrono::steady_clock::now();
            for (auto& awaiter : mAwaiters)
            {
                if (!awaiter)
                    continue;
                const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - awaiter.tick);
                if (duration.count() > awaiter.timeout)
                    awaiter.resume_now(MDR_RESULT_ERROR_TIMEOUT);
            }
            int taskResult;
            if (TaskMoveNext(taskResult))
                return taskResult;
        }
        const int idleCode = MDR_EVENT_NONE;
        if (mRecvBuf.empty())
            return idleCode;
        auto commandBegin = std::find(mRecvBuf.begin(), mRecvBuf.end(), kStartMarker);
        auto commandEnd = std::find(commandBegin, mRecvBuf.end(), kEndMarker);
        if (commandBegin == mRecvBuf.end() || commandEnd == mRecvBuf.end())
            return idleCode; // Incomplete
        MDRBuffer packedCommand{commandBegin, commandEnd + 1};
        MDRBuffer command;
        MDRDataType type;
        MDRCommandSeqNumber seqNum;
        const uint64_t packet = HandlePacket(MDR_PACKET_DIRECTION_RX, packedCommand);
        switch (MDRUnpackCommand(packedCommand, command, type, seqNum))
        {
        case MDRUnpackResult::OK:
        {
            mRecvBuf.erase(mRecvBuf.begin(), commandEnd + 1);
            mCurrentPacket = packet; // Within this packet's scope...
            const int event = Handle(command, type, seqNum); // Handle it
            mCurrentPacket = {}; // Clear after we're done
            return event;
        }
        case MDRUnpackResult::INCOMPLETE:
            // Incomplete. Nop.
            break;
        case MDRUnpackResult::BAD_MARKER:
        [[unlikely]]
        // FIXME Consider chunked transport.
        // This can happen and should not be handled this way. See also @ref SendCommandACK
        case MDRUnpackResult::BAD_CHECKSUM:
        [[unlikely]]
        case MDRUnpackResult::BAD_OTHER:
            [[unlikely]]
            // Unlikely. What we have now makes no sense yet markers are intact.
            MDR_LOG("FIXME-MDR packet malformed, discarding {} bytes", std::distance(mRecvBuf.begin(), commandEnd));
            mRecvBuf.erase(mRecvBuf.begin(), commandEnd);
            break;
        }
        return idleCode;
    }

    int MDRHeadphones::Invoke(MDRTask&& task)
    {
        if (mFailed)
            return mLastErrorCode;
        if (mTask)
            return MDR_RESULT_INPROGRESS;
        mTask = std::move(task);
        mTask.coroutine.resume();
        return GetFailure();
    }

    bool MDRHeadphones::TaskMoveNext(int& result)
    {
        if (!mTask || !mTask.coroutine.done())
            return false;
        result = mTask.coroutine.promise().result;
        mTask = {};
        return true;
    }

    uint64_t MDRHeadphones::HandlePacket(MDRPacketDirection direction, Span<const UInt8> frame)
    {
        const uint64_t packetId = ++mPacketCounter;
        if (mPacketCallback)
            mPacketCallback(mPacketCallbackUserData, direction, packetId, frame.data(), static_cast<int>(frame.size()));
        return packetId;
    }

    void MDRHeadphones::SendCommandImpl(Span<const UInt8> command, MDRDataType type, MDRCommandSeqNumber seq)
    {
        MDRBuffer packed = MDRPackCommand(type, seq, command);
        mSendBuf.insert(mSendBuf.end(), packed.begin(), packed.end());
        HandlePacket(MDR_PACKET_DIRECTION_TX, packed);
    }

    void MDRHeadphones::SendACK(MDRCommandSeqNumber seq)
    {
        SendCommandImpl({}, MDRDataType::ACK, 1 - seq);
    }

    void MDRHeadphones::HandleAck(MDRCommandSeqNumber seq)
    {
        const auto expected = static_cast<MDRCommandSeqNumber>(1 - mSeqNumber);
        if (seq != expected)
        {
            MDR_LOG("FIXME-Stale ACK seq {} expected {}", seq, expected);
            return;
        }
        mSeqNumber = expected;
        Awake(AWAIT_ACK);
    }
}

#pragma region C Exports
namespace
{
    using Headphones = mdr::MDRHeadphones;

    bool ValidPropertyType(MDRPropertyType type)
    {
        return type == MDR_PROPERTY_CURRENT || type == MDR_PROPERTY_DESIRED;
    }

    template <typename T>
    const T& ReadProperty(const mdr::MDRProperty<T>& property, MDRPropertyType type)
    {
        return type == MDR_PROPERTY_CURRENT ? property.mCurrent : property.mDesired;
    }

    Headphones* Impl(MDRHeadphones* headphones)
    {
        return mdr::detail::HeadphonesImpl(headphones);
    }

    const Headphones* Impl(const MDRHeadphones* headphones)
    {
        return mdr::detail::HeadphonesImpl(headphones);
    }

    template <typename F>
    decltype(auto) WithDetails(Headphones& headphones, F&& function)
    {
        if (headphones.mProtocolFamily == Headphones::ProtocolFamily::V1)
            return function(headphones.mDetailsV1);
        return function(headphones.mDetailsV2);
    }

    template <typename F>
    decltype(auto) WithDetails(const Headphones& headphones, F&& function)
    {
        if (headphones.mProtocolFamily == Headphones::ProtocolFamily::V1)
            return function(headphones.mDetailsV1);
        return function(headphones.mDetailsV2);
    }

    bool SupportsFeature(const mdr::DetailsV1& state, MDRFeature feature)
    {
        using F = mdr::v1::t1::FunctionType;
        switch (feature)
        {
        case MDR_FEATURE_IDENTITY:
            return true;
        case MDR_FEATURE_BATTERY_SINGLE:
            return state.mSupport.contains(F::BATTERY_LEVEL);
        case MDR_FEATURE_PLAYBACK_METADATA:
        case MDR_FEATURE_PLAYBACK_CONTROL:
        case MDR_FEATURE_PLAYBACK_VOLUME:
            return state.mSupport.contains(F::PLAYBACK_CONTROLLER);
        case MDR_FEATURE_NOISE_CANCELLING:
            return state.mSupport.contains(F::NOISE_CANCELLING) ||
                state.mSupport.contains(F::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE);
        case MDR_FEATURE_AMBIENT_SOUND:
            return state.mSupport.contains(F::AMBIENT_SOUND_MODE) ||
                state.mSupport.contains(F::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE);
        case MDR_FEATURE_WIND_NOISE_REDUCTION:
            // DUAL_SINGLE_OFF: SINGLE is wind noise reduction (e.g. WH-1000XM3). Assumed until the capability says otherwise.
            return state.mSupport.contains(F::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE) &&
                state.mNcSettingType == mdr::v1::t1::NcAsmSettingType::DUAL_SINGLE_OFF;
        case MDR_FEATURE_SPEAK_TO_CHAT:
            return state.mSupport.contains(F::SMART_TALKING_MODE);
        case MDR_FEATURE_EQUALIZER:
            return state.mSupport.contains(F::PRESET_EQ) || state.mSupport.contains(F::EBB) ||
                state.mSupport.contains(F::PRESET_EQ_NONCUSTOMIZABLE);
        case MDR_FEATURE_DSEE:
            return state.mSupport.contains(F::UPSCALING);
        case MDR_FEATURE_PAIRED_DEVICE_MANAGEMENT:
        case MDR_FEATURE_PAIRING_MODE:
            return state.mSupport.contains(F::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT);
        case MDR_FEATURE_GENERAL_SETTINGS:
            return state.mSupport.contains(F::GENERAL_SETTING1) || state.mSupport.contains(F::GENERAL_SETTING2) ||
                state.mSupport.contains(F::GENERAL_SETTING3);
        case MDR_FEATURE_ASSIGNABLE_CONTROLS:
            return state.mSupport.contains(F::ASSIGNABLE_SETTINGS);
        case MDR_FEATURE_AUTO_POWER_OFF:
            return state.mSupport.contains(F::AUTO_POWER_OFF);
        case MDR_FEATURE_WEARING_DETECTION:
        case MDR_FEATURE_AUTO_PAUSE:
            return state.mSupport.contains(F::CONTROL_BY_WEARING);
        case MDR_FEATURE_VOICE_GUIDANCE:
            return state.mSupport.contains(F::VOICE_GUIDANCE);
        case MDR_FEATURE_SHUTDOWN:
            return state.mSupport.contains(F::POWER_OFF);
        case MDR_FEATURE_CONNECTION_MODE:
            return state.mSupport.contains(F::CONNECTION_MODE);
        default:
            return false;
        }
    }

    bool SupportsFeature(const mdr::DetailsV2& state, MDRFeature feature)
    {
        using T1 = mdr::v2::t1::FunctionType;
        using T2 = mdr::v2::t2::FunctionType;
        switch (feature)
        {
        case MDR_FEATURE_IDENTITY:
            return true;
        case MDR_FEATURE_BATTERY_SINGLE:
            return state.mSupport.contains(T1::BATTERY_LEVEL_INDICATOR) ||
                state.mSupport.contains(T1::BATTERY_LEVEL_WITH_THRESHOLD);
        case MDR_FEATURE_BATTERY_LEFT_RIGHT:
            return state.mSupport.contains(T1::LEFT_RIGHT_BATTERY_LEVEL_INDICATOR) ||
                state.mSupport.contains(T1::LR_BATTERY_LEVEL_WITH_THRESHOLD);
        case MDR_FEATURE_BATTERY_CASE:
            return state.mSupport.contains(T1::CRADLE_BATTERY_LEVEL_INDICATOR) ||
                state.mSupport.contains(T1::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD);
        case MDR_FEATURE_PLAYBACK_METADATA:
        case MDR_FEATURE_PLAYBACK_CONTROL:
        case MDR_FEATURE_PLAYBACK_VOLUME:
            return state.mSupport.containsPlaybackController();
        case MDR_FEATURE_NOISE_CANCELLING:
            return state.mSupport.containsNoiseCancelling();
        case MDR_FEATURE_AMBIENT_SOUND:
            return state.mSupport.containsNoiseCancelling() || state.mSupport.contains(T1::AMBIENT_SOUND_MODE_ONOFF) ||
                state.mSupport.contains(T1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT);
        case MDR_FEATURE_ADAPTIVE_AMBIENT_SOUND:
            return state.mSupport.contains(
                T1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION);
        case MDR_FEATURE_SPEAK_TO_CHAT:
            return state.mSupport.contains(T1::SMART_TALKING_MODE_TYPE2);
        case MDR_FEATURE_LISTENING_MODE:
            return state.mSupport.contains(T1::LISTENING_OPTION);
        case MDR_FEATURE_LISTENING_BACKGROUND_MUSIC:
            return state.mSupport.contains(T1::LISTENING_OPTION) && state.mSupport.containsBGMMode();
        case MDR_FEATURE_LISTENING_CINEMA:
            return state.mSupport.contains(T1::LISTENING_OPTION) && state.mSupport.contains(T1::UPMIX_CINEMA);
        case MDR_FEATURE_LISTENING_VOICE_BOOST:
            return state.mSupport.contains(T1::LISTENING_OPTION) && state.mSupport.contains(T1::VOICE_CONTENTS);
        case MDR_FEATURE_LISTENING_SOUND_LEAKAGE_REDUCTION:
            return state.mSupport.contains(T1::LISTENING_OPTION) &&
                state.mSupport.contains(T1::SOUND_LEAKAGE_REDUCTION);
        case MDR_FEATURE_EQUALIZER:
            return state.mSupport.containsEqualizer();
        case MDR_FEATURE_DSEE:
            return state.mSupport.contains(T1::UPSCALING_AUTO_OFF);
        case MDR_FEATURE_PAIRED_DEVICE_MANAGEMENT:
        case MDR_FEATURE_PAIRING_MODE:
            return state.mSupport.containsPairingDeviceManagement();
        case MDR_FEATURE_GENERAL_SETTINGS:
            return state.mSupport.contains(T1::GENERAL_SETTING_1) || state.mSupport.contains(T1::GENERAL_SETTING_2) ||
                state.mSupport.contains(T1::GENERAL_SETTING_3) || state.mSupport.contains(T1::GENERAL_SETTING_4);
        case MDR_FEATURE_ASSIGNABLE_CONTROLS:
            return state.mSupport.contains(T1::ASSIGNABLE_SETTING) ||
                state.mSupport.contains(T1::ASSIGNABLE_SETTING_WITH_LIMITATION);
        case MDR_FEATURE_NOISE_CONTROL_BUTTON:
            return state.mSupport.contains(T1::AMBIENT_SOUND_CONTROL_MODE_SELECT);
        case MDR_FEATURE_AUTO_POWER_OFF:
            return state.mSupport.contains(T1::AUTO_POWER_OFF) ||
                state.mSupport.contains(T1::AUTO_POWER_OFF_WITH_WEARING_DETECTION);
        case MDR_FEATURE_WEARING_DETECTION:
            return state.mSupport.contains(T1::AUTO_POWER_OFF_WITH_WEARING_DETECTION) ||
                state.mSupport.contains(T1::WEARING_STATUS_DETECTOR);
        case MDR_FEATURE_AUTO_PAUSE:
            return state.mSupport.contains(T1::PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF);
        case MDR_FEATURE_HEAD_GESTURE:
            return state.mSupport.contains(T1::HEAD_GESTURE_ON_OFF_TRAINING);
        case MDR_FEATURE_VOICE_GUIDANCE:
            return state.mSupport.containsVoiceGuidance();
        case MDR_FEATURE_VOICE_GUIDANCE_VOLUME:
            return state.mSupport.containsVoiceGuidanceVolume();
        case MDR_FEATURE_SHUTDOWN:
            return state.mSupport.contains(T1::POWER_OFF);
        case MDR_FEATURE_CONNECTION_MODE:
            return state.mSupport.contains(T1::CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY);
        case MDR_FEATURE_SAFE_LISTENING:
            return state.mSupport.contains(T2::SAFE_LISTENING_HBS_1) ||
                state.mSupport.contains(T2::SAFE_LISTENING_HBS_2) ||
                state.mSupport.contains(T2::SAFE_LISTENING_TWS_1) || state.mSupport.contains(T2::SAFE_LISTENING_TWS_2);
        case MDR_FEATURE_SOURCE_SWITCH_CONTROL:
            return state.mSupport.contains(T2::SOURCE_SWITCH_CONTROL);
        case MDR_FEATURE_WEARING_STATUS:
            return state.mSupport.contains(T2::WEARING_STATUS_CHECKER) ||
                state.mWearingStatus != mdr::v2::t2::WearingStatusCode::OUT_OF_RANGE;
        case MDR_FEATURE_VOICE_ASSISTANT:
            return state.mSupport.contains(T1::VOICE_ASSISTANT_SETTINGS);
        default:
            return false;
        }
    }

    bool ValidBoolean(MDRBoolean value)
    {
        return value == MDR_FALSE || value == MDR_TRUE;
    }

    MDRResult CopyText(std::string_view text, char* buffer, uint32_t* inoutSize)
    {
        if (!inoutSize || text.size() >= std::numeric_limits<uint32_t>::max())
            return MDR_RESULT_ERROR_INVALID_ARGUMENT;
        const uint32_t required = static_cast<uint32_t>(text.size() + 1);
        if (!buffer)
        {
            if (*inoutSize != 0)
                return MDR_RESULT_ERROR_INVALID_ARGUMENT;
            *inoutSize = required;
            return MDR_RESULT_OK;
        }
        if (*inoutSize < required)
        {
            *inoutSize = required;
            return MDR_RESULT_ERROR_BUFFER_TOO_SMALL;
        }
        std::memcpy(buffer, text.data(), text.size());
        buffer[text.size()] = '\0';
        *inoutSize = required;
        return MDR_RESULT_OK;
    }

    bool SupportsPairing(const Headphones& h)
    {
        return WithDetails(h, [](const auto& state)
                           { return SupportsFeature(state, MDR_FEATURE_PAIRED_DEVICE_MANAGEMENT); });
    }

    bool SupportsSafeListening(const Headphones& h)
    {
        return WithDetails(h, [](const auto& state) { return SupportsFeature(state, MDR_FEATURE_SAFE_LISTENING); });
    }

    bool SupportsVoiceGuidance(const Headphones& h)
    {
        return WithDetails(h, [](const auto& state) { return SupportsFeature(state, MDR_FEATURE_VOICE_GUIDANCE); });
    }

    bool SupportsGeneralSetting(const Headphones& h, uint32_t index)
    {
        if (h.mProtocolFamily == Headphones::ProtocolFamily::V1)
        {
            using F = mdr::v1::t1::FunctionType;
            switch (index)
            {
            case 0:
                return h.mDetailsV1.mSupport.contains(F::GENERAL_SETTING1);
            case 1:
                return h.mDetailsV1.mSupport.contains(F::GENERAL_SETTING2);
            case 2:
                return h.mDetailsV1.mSupport.contains(F::GENERAL_SETTING3);
            default:
                return false;
            }
        }
        using F = mdr::v2::t1::FunctionType;
        switch (index)
        {
        case 0:
            return h.mDetailsV2.mSupport.contains(F::GENERAL_SETTING_1);
        case 1:
            return h.mDetailsV2.mSupport.contains(F::GENERAL_SETTING_2);
        case 2:
            return h.mDetailsV2.mSupport.contains(F::GENERAL_SETTING_3);
        case 3:
            return h.mDetailsV2.mSupport.contains(F::GENERAL_SETTING_4);
        default:
            return false;
        }
    }

    template <typename Capability>
    bool IsBooleanGeneralSetting(const Capability& capability)
    {
        return capability.type == decltype(capability.type)::BOOLEAN_TYPE;
    }

    template <typename Capability>
    bool IsListGeneralSetting(const Capability& capability)
    {
        return capability.type == decltype(capability.type)::LIST_TYPE && !capability.options.empty();
    }

    template <typename Capability>
    MDRGeneralSettingType GeneralSettingType(const Capability& capability)
    {
        if (IsBooleanGeneralSetting(capability))
            return MDR_GENERAL_SETTING_BOOLEAN;
        if (IsListGeneralSetting(capability))
            return MDR_GENERAL_SETTING_LIST;
        return MDR_GENERAL_SETTING_UNKNOWN;
    }

    // ENUM_NAME strings are identifiers like "SIDETONE_SETTING"; RAW_NAME strings are display text.
    template <typename Info>
    bool IsGeneralSettingTextKey(const Info& info)
    {
        return info.stringFormat == decltype(info.stringFormat)::ENUM_NAME;
    }

    std::string_view ModelSeriesText(const mdr::DetailsV1& state)
    {
        return mdr::v1::t1::format_as(state.mModelSeries);
    }

    std::string_view ModelSeriesText(const mdr::DetailsV2& state)
    {
        return mdr::v2::t1::format_as(state.mModelSeries);
    }

    std::string_view ModelColorText(const mdr::DetailsV1& state)
    {
        return mdr::v1::format_as(state.mModelColor);
    }

    std::string_view ModelColorText(const mdr::DetailsV2& state)
    {
        return mdr::v2::format_as(state.mModelColor);
    }

    MDRAudioCodec from_protocol(mdr::v1::t1::AudioCodec value)
    {
        using enum mdr::v1::t1::AudioCodec;
        switch (value)
        {
        case SBC:
            return MDR_AUDIO_CODEC_SBC;
        case AAC:
            return MDR_AUDIO_CODEC_AAC;
        case LDAC:
            return MDR_AUDIO_CODEC_LDAC;
        case APT_X:
            return MDR_AUDIO_CODEC_APTX;
        case APT_X_HD:
            return MDR_AUDIO_CODEC_APTX_HD;
        case OTHER:
            return MDR_AUDIO_CODEC_OTHER;
        default:
            return MDR_AUDIO_CODEC_UNKNOWN;
        }
    }

    MDRAudioCodec from_protocol(mdr::v2::t1::AudioCodec value)
    {
        using enum mdr::v2::t1::AudioCodec;
        switch (value)
        {
        case SBC:
            return MDR_AUDIO_CODEC_SBC;
        case AAC:
            return MDR_AUDIO_CODEC_AAC;
        case LDAC:
            return MDR_AUDIO_CODEC_LDAC;
        case APT_X:
            return MDR_AUDIO_CODEC_APTX;
        case APT_X_HD:
            return MDR_AUDIO_CODEC_APTX_HD;
        case LC3:
            return MDR_AUDIO_CODEC_LC3;
        case OTHER:
            return MDR_AUDIO_CODEC_OTHER;
        default:
            return MDR_AUDIO_CODEC_UNKNOWN;
        }
    }

    MDRChargingState from_protocol(mdr::v1::t1::BatteryChargingStatus value)
    {
        switch (value)
        {
        case mdr::v1::t1::BatteryChargingStatus::NOT_CHARGING:
            return MDR_CHARGING_NO;
        case mdr::v1::t1::BatteryChargingStatus::CHARGING:
            return MDR_CHARGING_YES;
        default:
            return MDR_CHARGING_UNKNOWN;
        }
    }

    MDRChargingState from_protocol(mdr::v2::t1::BatteryChargingStatus value)
    {
        using enum mdr::v2::t1::BatteryChargingStatus;
        switch (value)
        {
        case NOT_CHARGING:
            return MDR_CHARGING_NO;
        case CHARGING:
            return MDR_CHARGING_YES;
        case CHARGED:
            return MDR_CHARGING_COMPLETE;
        default:
            return MDR_CHARGING_UNKNOWN;
        }
    }

    MDRPlaybackStatus from_protocol(mdr::v1::t1::PlaybackStatus value)
    {
        switch (value)
        {
        case mdr::v1::t1::PlaybackStatus::PLAY:
            return MDR_PLAYBACK_PLAYING;
        case mdr::v1::t1::PlaybackStatus::PAUSE:
            return MDR_PLAYBACK_PAUSED;
        case mdr::v1::t1::PlaybackStatus::STOP:
            return MDR_PLAYBACK_STOPPED;
        default:
            return MDR_PLAYBACK_UNKNOWN;
        }
    }

    MDRPlaybackStatus from_protocol(mdr::v2::t1::PlaybackStatus value)
    {
        using enum mdr::v2::t1::PlaybackStatus;
        switch (value)
        {
        case PLAY:
            return MDR_PLAYBACK_PLAYING;
        case PAUSE:
            return MDR_PLAYBACK_PAUSED;
        case STOP:
            return MDR_PLAYBACK_STOPPED;
        default:
            return MDR_PLAYBACK_UNKNOWN;
        }
    }

    bool PlaybackControlFromAction(MDRPlaybackAction action, mdr::v1::t1::PlaybackControl& out)
    {
        using enum mdr::v1::t1::PlaybackControl;
        switch (action)
        {
        case MDR_PLAYBACK_PLAY:
            out = PLAY;
            return true;
        case MDR_PLAYBACK_PAUSE:
            out = PAUSE;
            return true;
        case MDR_PLAYBACK_NEXT:
            out = TRACK_UP;
            return true;
        case MDR_PLAYBACK_PREVIOUS:
            out = TRACK_DOWN;
            return true;
        default:
            return false;
        }
    }

    bool PlaybackControlFromAction(MDRPlaybackAction action, mdr::v2::t1::PlaybackControl& out)
    {
        using enum mdr::v2::t1::PlaybackControl;
        switch (action)
        {
        case MDR_PLAYBACK_PLAY:
            out = PLAY;
            return true;
        case MDR_PLAYBACK_PAUSE:
            out = PAUSE;
            return true;
        case MDR_PLAYBACK_NEXT:
            out = TRACK_UP;
            return true;
        case MDR_PLAYBACK_PREVIOUS:
            out = TRACK_DOWN;
            return true;
        default:
            return false;
        }
    }

    MDRAdaptiveSensitivity from_protocol(mdr::v2::t1::NoiseAdaptiveSensitivity value)
    {
        using enum mdr::v2::t1::NoiseAdaptiveSensitivity;
        switch (value)
        {
        case LOW:
            return MDR_ADAPTIVE_SENSITIVITY_LOW;
        case STANDARD:
            return MDR_ADAPTIVE_SENSITIVITY_STANDARD;
        case HIGH:
            return MDR_ADAPTIVE_SENSITIVITY_HIGH;
        default:
            return MDR_ADAPTIVE_SENSITIVITY_UNKNOWN;
        }
    }

    bool to_protocol(MDRAdaptiveSensitivity value, mdr::v2::t1::NoiseAdaptiveSensitivity& out)
    {
        using enum mdr::v2::t1::NoiseAdaptiveSensitivity;
        switch (value)
        {
        case MDR_ADAPTIVE_SENSITIVITY_LOW:
            out = LOW;
            return true;
        case MDR_ADAPTIVE_SENSITIVITY_STANDARD:
            out = STANDARD;
            return true;
        case MDR_ADAPTIVE_SENSITIVITY_HIGH:
            out = HIGH;
            return true;
        default:
            return false;
        }
    }

    MDRNoiseButtonMode from_protocol(mdr::v2::t1::Function value)
    {
        using enum mdr::v2::t1::Function;
        switch (value)
        {
        case NC_ASM_OFF:
            return MDR_NOISE_BUTTON_NOISE_AMBIENT_OFF;
        case NC_ASM:
            return MDR_NOISE_BUTTON_NOISE_AMBIENT;
        case NC_OFF:
            return MDR_NOISE_BUTTON_NOISE_OFF;
        case ASM_OFF:
            return MDR_NOISE_BUTTON_AMBIENT_OFF;
        default:
            return MDR_NOISE_BUTTON_NONE;
        }
    }

    bool to_protocol(MDRNoiseButtonMode value, mdr::v2::t1::Function& out)
    {
        using enum mdr::v2::t1::Function;
        switch (value)
        {
        case MDR_NOISE_BUTTON_NONE:
            out = NO_FUNCTION;
            return true;
        case MDR_NOISE_BUTTON_NOISE_AMBIENT_OFF:
            out = NC_ASM_OFF;
            return true;
        case MDR_NOISE_BUTTON_NOISE_AMBIENT:
            out = NC_ASM;
            return true;
        case MDR_NOISE_BUTTON_NOISE_OFF:
            out = NC_OFF;
            return true;
        case MDR_NOISE_BUTTON_AMBIENT_OFF:
            out = ASM_OFF;
            return true;
        default:
            return false;
        }
    }

    MDRSpeechSensitivity from_protocol(mdr::v1::t1::DetectionSensitivity value)
    {
        switch (value)
        {
        case mdr::v1::t1::DetectionSensitivity::AUTO:
            return MDR_SPEECH_SENSITIVITY_AUTO;
        case mdr::v1::t1::DetectionSensitivity::LOW:
            return MDR_SPEECH_SENSITIVITY_LOW;
        case mdr::v1::t1::DetectionSensitivity::HIGH:
            return MDR_SPEECH_SENSITIVITY_HIGH;
        default:
            return MDR_SPEECH_SENSITIVITY_UNKNOWN;
        }
    }

    bool to_protocol(MDRSpeechSensitivity value, mdr::v1::t1::DetectionSensitivity& out)
    {
        switch (value)
        {
        case MDR_SPEECH_SENSITIVITY_AUTO:
            out = mdr::v1::t1::DetectionSensitivity::AUTO;
            return true;
        case MDR_SPEECH_SENSITIVITY_LOW:
            out = mdr::v1::t1::DetectionSensitivity::LOW;
            return true;
        case MDR_SPEECH_SENSITIVITY_HIGH:
            out = mdr::v1::t1::DetectionSensitivity::HIGH;
            return true;
        default:
            return false;
        }
    }

    MDRSpeechSensitivity from_protocol(mdr::v2::t1::DetectSensitivity value)
    {
        using enum mdr::v2::t1::DetectSensitivity;
        switch (value)
        {
        case AUTO:
            return MDR_SPEECH_SENSITIVITY_AUTO;
        case LOW:
            return MDR_SPEECH_SENSITIVITY_LOW;
        case HIGH:
            return MDR_SPEECH_SENSITIVITY_HIGH;
        default:
            return MDR_SPEECH_SENSITIVITY_UNKNOWN;
        }
    }

    bool to_protocol(MDRSpeechSensitivity value, mdr::v2::t1::DetectSensitivity& out)
    {
        using enum mdr::v2::t1::DetectSensitivity;
        switch (value)
        {
        case MDR_SPEECH_SENSITIVITY_AUTO:
            out = AUTO;
            return true;
        case MDR_SPEECH_SENSITIVITY_LOW:
            out = LOW;
            return true;
        case MDR_SPEECH_SENSITIVITY_HIGH:
            out = HIGH;
            return true;
        default:
            return false;
        }
    }

    MDRSpeakTimeout from_protocol(mdr::v1::t1::ModeOutTime value)
    {
        switch (value)
        {
        case mdr::v1::t1::ModeOutTime::FAST:
            return MDR_SPEAK_TIMEOUT_SHORT;
        case mdr::v1::t1::ModeOutTime::MID:
            return MDR_SPEAK_TIMEOUT_MEDIUM;
        case mdr::v1::t1::ModeOutTime::SLOW:
            return MDR_SPEAK_TIMEOUT_LONG;
        case mdr::v1::t1::ModeOutTime::NONE:
            return MDR_SPEAK_TIMEOUT_MANUAL;
        default:
            return MDR_SPEAK_TIMEOUT_UNKNOWN;
        }
    }

    bool to_protocol(MDRSpeakTimeout value, mdr::v1::t1::ModeOutTime& out)
    {
        switch (value)
        {
        case MDR_SPEAK_TIMEOUT_SHORT:
            out = mdr::v1::t1::ModeOutTime::FAST;
            return true;
        case MDR_SPEAK_TIMEOUT_MEDIUM:
            out = mdr::v1::t1::ModeOutTime::MID;
            return true;
        case MDR_SPEAK_TIMEOUT_LONG:
            out = mdr::v1::t1::ModeOutTime::SLOW;
            return true;
        case MDR_SPEAK_TIMEOUT_MANUAL:
            out = mdr::v1::t1::ModeOutTime::NONE;
            return true;
        default:
            return false;
        }
    }

    MDRSpeakTimeout from_protocol(mdr::v2::t1::ModeOutTime value)
    {
        using enum mdr::v2::t1::ModeOutTime;
        switch (value)
        {
        case FAST:
            return MDR_SPEAK_TIMEOUT_SHORT;
        case MID:
            return MDR_SPEAK_TIMEOUT_MEDIUM;
        case SLOW:
            return MDR_SPEAK_TIMEOUT_LONG;
        case NONE:
            return MDR_SPEAK_TIMEOUT_MANUAL;
        default:
            return MDR_SPEAK_TIMEOUT_UNKNOWN;
        }
    }

    bool to_protocol(MDRSpeakTimeout value, mdr::v2::t1::ModeOutTime& out)
    {
        using enum mdr::v2::t1::ModeOutTime;
        switch (value)
        {
        case MDR_SPEAK_TIMEOUT_SHORT:
            out = FAST;
            return true;
        case MDR_SPEAK_TIMEOUT_MEDIUM:
            out = MID;
            return true;
        case MDR_SPEAK_TIMEOUT_LONG:
            out = SLOW;
            return true;
        case MDR_SPEAK_TIMEOUT_MANUAL:
            out = NONE;
            return true;
        default:
            return false;
        }
    }

    MDRRoomSize from_protocol(mdr::v2::t1::RoomSize value)
    {
        using enum mdr::v2::t1::RoomSize;
        switch (value)
        {
        case SMALL:
            return MDR_ROOM_SMALL;
        case MIDDLE:
            return MDR_ROOM_MEDIUM;
        case LARGE:
            return MDR_ROOM_LARGE;
        default:
            return MDR_ROOM_UNKNOWN;
        }
    }

    bool to_protocol(MDRRoomSize value, mdr::v2::t1::RoomSize& out)
    {
        using enum mdr::v2::t1::RoomSize;
        switch (value)
        {
        case MDR_ROOM_SMALL:
            out = SMALL;
            return true;
        case MDR_ROOM_MEDIUM:
            out = MIDDLE;
            return true;
        case MDR_ROOM_LARGE:
            out = LARGE;
            return true;
        default:
            return false;
        }
    }

    MDRVoiceAssistant from_protocol(mdr::v2::t1::VoiceAssistant value)
    {
        using enum mdr::v2::t1::VoiceAssistant;
        switch (value)
        {
        case NO_FUNCTION:
            return MDR_VOICE_ASSISTANT_NONE;
        case VOICE_RECOGNITION:
            return MDR_VOICE_ASSISTANT_VOICE_RECOGNITION;
        case GOOGLE_ASSISTANT:
            return MDR_VOICE_ASSISTANT_GOOGLE_ASSISTANT;
        case AMAZON_ALEXA:
            return MDR_VOICE_ASSISTANT_AMAZON_ALEXA;
        case TENCENT_XIAOWEI:
            return MDR_VOICE_ASSISTANT_TENCENT_XIAOWEI;
        case SONY_VOICE_ASSISTANT:
            return MDR_VOICE_ASSISTANT_SONY_VOICE_CONTROL;
        case VOICE_ASSISTANT_ENABLED_IN_OTHER_DEVICE:
            return MDR_VOICE_ASSISTANT_OTHER_DEVICE;
        default:
            return MDR_VOICE_ASSISTANT_UNKNOWN;
        }
    }

    bool to_protocol(MDRVoiceAssistant value, mdr::v2::t1::VoiceAssistant& out)
    {
        using enum mdr::v2::t1::VoiceAssistant;
        switch (value)
        {
        case MDR_VOICE_ASSISTANT_NONE:
            out = NO_FUNCTION;
            return true;
        case MDR_VOICE_ASSISTANT_VOICE_RECOGNITION:
            out = VOICE_RECOGNITION;
            return true;
        case MDR_VOICE_ASSISTANT_GOOGLE_ASSISTANT:
            out = GOOGLE_ASSISTANT;
            return true;
        case MDR_VOICE_ASSISTANT_AMAZON_ALEXA:
            out = AMAZON_ALEXA;
            return true;
        case MDR_VOICE_ASSISTANT_TENCENT_XIAOWEI:
            out = TENCENT_XIAOWEI;
            return true;
        case MDR_VOICE_ASSISTANT_SONY_VOICE_CONTROL:
            out = SONY_VOICE_ASSISTANT;
            return true;
        default:
            return false;
        }
    }

    MDRVoiceAssistantKey from_protocol(mdr::v2::t1::VoiceAssistantKeyType value)
    {
        using enum mdr::v2::t1::VoiceAssistantKeyType;
        switch (value)
        {
        case FIXED_BUTTON:
            return MDR_VOICE_ASSISTANT_KEY_FIXED_BUTTON;
        case TOUCH_SENSOR_CONTROL_PANEL:
            return MDR_VOICE_ASSISTANT_KEY_TOUCH_PANEL;
        case ASSIGNABLE_BUTTON:
            return MDR_VOICE_ASSISTANT_KEY_ASSIGNABLE_BUTTON;
        case ASSIGNABLE_SENSOR:
            return MDR_VOICE_ASSISTANT_KEY_ASSIGNABLE_SENSOR;
        default:
            return MDR_VOICE_ASSISTANT_KEY_UNKNOWN;
        }
    }

    MDREqualizerPreset from_protocol(mdr::v1::t1::EqPresetId value)
    {
        using enum mdr::v1::t1::EqPresetId;
        switch (value)
        {
        case OFF:
            return MDR_EQ_OFF;
        case ROCK:
            return MDR_EQ_ROCK;
        case POP:
            return MDR_EQ_POP;
        case JAZZ:
            return MDR_EQ_JAZZ;
        case DANCE:
            return MDR_EQ_DANCE;
        case EDM:
            return MDR_EQ_EDM;
        case R_AND_B_HIP_HOP:
            return MDR_EQ_R_AND_B_HIP_HOP;
        case ACOUSTIC:
            return MDR_EQ_ACOUSTIC;
        case BRIGHT:
            return MDR_EQ_BRIGHT;
        case EXCITED:
            return MDR_EQ_EXCITED;
        case MELLOW:
            return MDR_EQ_MELLOW;
        case RELAXED:
            return MDR_EQ_RELAXED;
        case VOCAL:
            return MDR_EQ_VOCAL;
        case TREBLE:
            return MDR_EQ_TREBLE;
        case BASS:
            return MDR_EQ_BASS;
        case SPEECH:
            return MDR_EQ_SPEECH;
        case CUSTOM:
            return MDR_EQ_CUSTOM;
        case USER_SETTING1:
            return MDR_EQ_USER_1;
        case USER_SETTING2:
            return MDR_EQ_USER_2;
        case USER_SETTING3:
            return MDR_EQ_USER_3;
        case USER_SETTING4:
            return MDR_EQ_USER_4;
        case USER_SETTING5:
            return MDR_EQ_USER_5;
        default:
            return MDR_EQ_UNKNOWN;
        }
    }

    bool to_protocol(MDREqualizerPreset value, mdr::v1::t1::EqPresetId& out)
    {
        using enum mdr::v1::t1::EqPresetId;
        switch (value)
        {
        case MDR_EQ_OFF:
            out = OFF;
            return true;
        case MDR_EQ_ROCK:
            out = ROCK;
            return true;
        case MDR_EQ_POP:
            out = POP;
            return true;
        case MDR_EQ_JAZZ:
            out = JAZZ;
            return true;
        case MDR_EQ_DANCE:
            out = DANCE;
            return true;
        case MDR_EQ_EDM:
            out = EDM;
            return true;
        case MDR_EQ_R_AND_B_HIP_HOP:
            out = R_AND_B_HIP_HOP;
            return true;
        case MDR_EQ_ACOUSTIC:
            out = ACOUSTIC;
            return true;
        case MDR_EQ_BRIGHT:
            out = BRIGHT;
            return true;
        case MDR_EQ_EXCITED:
            out = EXCITED;
            return true;
        case MDR_EQ_MELLOW:
            out = MELLOW;
            return true;
        case MDR_EQ_RELAXED:
            out = RELAXED;
            return true;
        case MDR_EQ_VOCAL:
            out = VOCAL;
            return true;
        case MDR_EQ_TREBLE:
            out = TREBLE;
            return true;
        case MDR_EQ_BASS:
            out = BASS;
            return true;
        case MDR_EQ_SPEECH:
            out = SPEECH;
            return true;
        case MDR_EQ_CUSTOM:
            out = CUSTOM;
            return true;
        case MDR_EQ_USER_1:
            out = USER_SETTING1;
            return true;
        case MDR_EQ_USER_2:
            out = USER_SETTING2;
            return true;
        case MDR_EQ_USER_3:
            out = USER_SETTING3;
            return true;
        case MDR_EQ_USER_4:
            out = USER_SETTING4;
            return true;
        case MDR_EQ_USER_5:
            out = USER_SETTING5;
            return true;
        default:
            return false;
        }
    }

    MDREqualizerPreset from_protocol(mdr::v2::t1::EqPresetId value)
    {
        using enum mdr::v2::t1::EqPresetId;
        switch (value)
        {
        case OFF:
            return MDR_EQ_OFF;
        case ROCK:
            return MDR_EQ_ROCK;
        case POP:
            return MDR_EQ_POP;
        case JAZZ:
            return MDR_EQ_JAZZ;
        case DANCE:
            return MDR_EQ_DANCE;
        case EDM:
            return MDR_EQ_EDM;
        case R_AND_B_HIP_HOP:
            return MDR_EQ_R_AND_B_HIP_HOP;
        case ACOUSTIC:
            return MDR_EQ_ACOUSTIC;
        case BRIGHT:
            return MDR_EQ_BRIGHT;
        case EXCITED:
            return MDR_EQ_EXCITED;
        case MELLOW:
            return MDR_EQ_MELLOW;
        case RELAXED:
            return MDR_EQ_RELAXED;
        case VOCAL:
            return MDR_EQ_VOCAL;
        case TREBLE:
            return MDR_EQ_TREBLE;
        case BASS:
            return MDR_EQ_BASS;
        case SPEECH:
            return MDR_EQ_SPEECH;
        case HEAVY:
            return MDR_EQ_HEAVY;
        case CLEAR:
            return MDR_EQ_CLEAR;
        case HARD:
            return MDR_EQ_HARD;
        case SOFT:
            return MDR_EQ_SOFT;
        case GAMING_EQ:
            return MDR_EQ_GAMING;
        case FPS_1:
            return MDR_EQ_FPS_1;
        case FPS_2:
            return MDR_EQ_FPS_2;
        case FPS_3:
            return MDR_EQ_FPS_3;
        case CUSTOM:
            return MDR_EQ_CUSTOM;
        case USER_SETTING1:
            return MDR_EQ_USER_1;
        case USER_SETTING2:
            return MDR_EQ_USER_2;
        case USER_SETTING3:
            return MDR_EQ_USER_3;
        case USER_SETTING4:
            return MDR_EQ_USER_4;
        case USER_SETTING5:
            return MDR_EQ_USER_5;
        default:
            return MDR_EQ_UNKNOWN;
        }
    }

    bool to_protocol(MDREqualizerPreset value, mdr::v2::t1::EqPresetId& out)
    {
        using enum mdr::v2::t1::EqPresetId;
        switch (value)
        {
        case MDR_EQ_OFF:
            out = OFF;
            return true;
        case MDR_EQ_ROCK:
            out = ROCK;
            return true;
        case MDR_EQ_POP:
            out = POP;
            return true;
        case MDR_EQ_JAZZ:
            out = JAZZ;
            return true;
        case MDR_EQ_DANCE:
            out = DANCE;
            return true;
        case MDR_EQ_EDM:
            out = EDM;
            return true;
        case MDR_EQ_R_AND_B_HIP_HOP:
            out = R_AND_B_HIP_HOP;
            return true;
        case MDR_EQ_ACOUSTIC:
            out = ACOUSTIC;
            return true;
        case MDR_EQ_BRIGHT:
            out = BRIGHT;
            return true;
        case MDR_EQ_EXCITED:
            out = EXCITED;
            return true;
        case MDR_EQ_MELLOW:
            out = MELLOW;
            return true;
        case MDR_EQ_RELAXED:
            out = RELAXED;
            return true;
        case MDR_EQ_VOCAL:
            out = VOCAL;
            return true;
        case MDR_EQ_TREBLE:
            out = TREBLE;
            return true;
        case MDR_EQ_BASS:
            out = BASS;
            return true;
        case MDR_EQ_SPEECH:
            out = SPEECH;
            return true;
        case MDR_EQ_HEAVY:
            out = HEAVY;
            return true;
        case MDR_EQ_CLEAR:
            out = CLEAR;
            return true;
        case MDR_EQ_HARD:
            out = HARD;
            return true;
        case MDR_EQ_SOFT:
            out = SOFT;
            return true;
        case MDR_EQ_GAMING:
            out = GAMING_EQ;
            return true;
        case MDR_EQ_FPS_1:
            out = FPS_1;
            return true;
        case MDR_EQ_FPS_2:
            out = FPS_2;
            return true;
        case MDR_EQ_FPS_3:
            out = FPS_3;
            return true;
        case MDR_EQ_CUSTOM:
            out = CUSTOM;
            return true;
        case MDR_EQ_USER_1:
            out = USER_SETTING1;
            return true;
        case MDR_EQ_USER_2:
            out = USER_SETTING2;
            return true;
        case MDR_EQ_USER_3:
            out = USER_SETTING3;
            return true;
        case MDR_EQ_USER_4:
            out = USER_SETTING4;
            return true;
        case MDR_EQ_USER_5:
            out = USER_SETTING5;
            return true;
        default:
            return false;
        }
    }

    MDRDSEEType from_protocol(mdr::v1::t1::UpscalingType value)
    {
        switch (value)
        {
        case mdr::v1::t1::UpscalingType::DSEE:
            return MDR_DSEE_STANDARD;
        case mdr::v1::t1::UpscalingType::DSEE_HX:
            return MDR_DSEE_HX;
        case mdr::v1::t1::UpscalingType::DSEE_HX_AI:
            return MDR_DSEE_HX_AI;
        default:
            return MDR_DSEE_UNKNOWN;
        }
    }

    MDRDSEEType from_protocol(mdr::v2::t1::UpscalingType value)
    {
        using enum mdr::v2::t1::UpscalingType;
        switch (value)
        {
        case DSEE:
            return MDR_DSEE_STANDARD;
        case DSEE_HX:
            return MDR_DSEE_HX;
        case DSEE_HX_AI:
            return MDR_DSEE_HX_AI;
        case DSEE_ULTIMATE:
            return MDR_DSEE_ULTIMATE;
        default:
            return MDR_DSEE_UNKNOWN;
        }
    }

    typedef uint32_t MDRAssignableActionKeyLocation;
    MDRAssignableActionKeyLocation from_protocol(mdr::v1::t1::AssignableSettingsKey value)
    {
        using enum mdr::v1::t1::AssignableSettingsKey;
        switch (value)
        {
        case LEFT_SIDE_KEY:
            return MDR_ASSIGNABLE_ACTION_KEY_LEFT;
        case RIGHT_SIDE_KEY:
            return MDR_ASSIGNABLE_ACTION_KEY_RIGHT;
        case CUSTOM_KEY:
            return MDR_ASSIGNABLE_ACTION_KEY_CUSTOM;
        default:
            return MDR_ASSIGNABLE_ACTION_KEY_UNKNOWN;
        }
    }

    MDRAssignableActionKeyType from_protocol(mdr::v1::t1::AssignableSettingsKeyType value)
    {
        using enum mdr::v1::t1::AssignableSettingsKeyType;
        switch (value)
        {
        case TOUCH_SENSOR:
            return MDR_ASSIGNABLE_ACTION_KEY_TYPE_TOUCH_SENSOR;
        case BUTTON:
            return MDR_ASSIGNABLE_ACTION_KEY_TYPE_BUTTON;
        default:
            return MDR_ASSIGNABLE_ACTION_KEY_TYPE_UNKNOWN;
        }
    }

    MDRAssignableActionKeyLocation from_protocol(mdr::v2::t1::Key value)
    {
        using enum mdr::v2::t1::Key;
        switch (value)
        {
        case LEFT_SIDE:
            return MDR_ASSIGNABLE_ACTION_KEY_LEFT;
        case RIGHT_SIDE:
            return MDR_ASSIGNABLE_ACTION_KEY_RIGHT;
        case CUSTOM:
            return MDR_ASSIGNABLE_ACTION_KEY_CUSTOM;
        case C:
            return MDR_ASSIGNABLE_ACTION_KEY_C;
        case NC_AMB_KEY:
            return MDR_ASSIGNABLE_ACTION_KEY_NC_AMB;
        case NC_AMBIENT_KEY:
            return MDR_ASSIGNABLE_ACTION_KEY_NC_AMBIENT;
        default:
            return MDR_ASSIGNABLE_ACTION_KEY_UNKNOWN;
        }
    }

    MDRAssignableActionKeyType from_protocol(mdr::v2::t1::Type value)
    {
        using enum mdr::v2::t1::Type;
        switch (value)
        {
        case TOUCH_SENSOR:
            return MDR_ASSIGNABLE_ACTION_KEY_TYPE_TOUCH_SENSOR;
        case BUTTON:
            return MDR_ASSIGNABLE_ACTION_KEY_TYPE_BUTTON;
        case FACE_TAP:
            return MDR_ASSIGNABLE_ACTION_KEY_TYPE_FACE_TAP;
        default:
            return MDR_ASSIGNABLE_ACTION_KEY_TYPE_UNKNOWN;
        }
    }

    template <typename KeyInfo>
    MDRAssignableActionKeyType AssignableKeyType(const KeyInfo& keyInfo)
    {
        if constexpr (requires { keyInfo.keyType; })
            return from_protocol(keyInfo.keyType);
        else
            return from_protocol(keyInfo.type);
    }

    template <typename KeyInfo>
    const auto& AssignablePresets(const KeyInfo& keyInfo)
    {
        if constexpr (requires { keyInfo.presets; })
            return keyInfo.presets;
        else
            return keyInfo.assignableSettingsPreset;
    }

    MDRAssignableAction from_protocol(mdr::v1::t1::AssignableSettingsPreset value)
    {
        using enum mdr::v1::t1::AssignableSettingsPreset;
        switch (value)
        {
        case PLAYBACK_CONTROL:
            return MDR_ASSIGNABLE_PLAYBACK;
        case AMBIENT_SOUND_CONTROL:
            return MDR_ASSIGNABLE_NOISE_CONTROL;
        case VOICE_RECOGNITION:
            return MDR_ASSIGNABLE_VOICE_RECOGNITION;
        case GOOGLE_ASSISTANT:
            return MDR_ASSIGNABLE_GOOGLE_ASSISTANT;
        case AMAZON_ALEXA:
            return MDR_ASSIGNABLE_AMAZON_ALEXA;
        case TENCENT_XIAOWEI:
            return MDR_ASSIGNABLE_TENCENT_XIAOWEI;
        default:
            return MDR_ASSIGNABLE_NONE;
        }
    }

    bool to_protocol(MDRAssignableAction value, mdr::v1::t1::AssignableSettingsPreset& out)
    {
        using enum mdr::v1::t1::AssignableSettingsPreset;
        switch (value)
        {
        case MDR_ASSIGNABLE_NONE:
            out = NO_FUNCTION;
            return true;
        case MDR_ASSIGNABLE_PLAYBACK:
            out = PLAYBACK_CONTROL;
            return true;
        case MDR_ASSIGNABLE_NOISE_CONTROL:
            out = AMBIENT_SOUND_CONTROL;
            return true;
        case MDR_ASSIGNABLE_VOICE_RECOGNITION:
            out = VOICE_RECOGNITION;
            return true;
        case MDR_ASSIGNABLE_GOOGLE_ASSISTANT:
            out = GOOGLE_ASSISTANT;
            return true;
        case MDR_ASSIGNABLE_AMAZON_ALEXA:
            out = AMAZON_ALEXA;
            return true;
        case MDR_ASSIGNABLE_TENCENT_XIAOWEI:
            out = TENCENT_XIAOWEI;
            return true;
        default:
            return false;
        }
    }

    MDRAssignableAction from_protocol(mdr::v2::t1::Preset value)
    {
        using enum mdr::v2::t1::Preset;
        switch (value)
        {
        case NO_FUNCTION:
            return MDR_ASSIGNABLE_NONE;
        case PLAYBACK_CONTROL:
            return MDR_ASSIGNABLE_PLAYBACK;
        case AMBIENT_SOUND_CONTROL:
            return MDR_ASSIGNABLE_NOISE_CONTROL;
        case AMBIENT_SOUND_CONTROL_QUICK_ACCESS:
            return MDR_ASSIGNABLE_NOISE_CONTROL_QUICK_ACCESS;
        case TRACK_CONTROL:
            return MDR_ASSIGNABLE_TRACK_CONTROL;
        case VOICE_RECOGNITION:
            return MDR_ASSIGNABLE_VOICE_RECOGNITION;
        case GOOGLE_ASSIST:
            return MDR_ASSIGNABLE_GOOGLE_ASSISTANT;
        case AMAZON_ALEXA:
            return MDR_ASSIGNABLE_AMAZON_ALEXA;
        case TENCENT_XIAOWEI:
            return MDR_ASSIGNABLE_TENCENT_XIAOWEI;
        case MS:
            return MDR_ASSIGNABLE_MICROSOFT_CORTANA;
        case QUICK_ACCESS:
            return MDR_ASSIGNABLE_QUICK_ACCESS;
        case VOLUME_CONTROL:
            return MDR_ASSIGNABLE_VOLUME;
        case PLAYBACK_CONTROL_VOICE_ASSISTANT_LIMITATION:
            return MDR_ASSIGNABLE_PLAYBACK_VOICE_ASSISTANT_LIMITATION;
        case TENCENT_XIAOWEI_Q_MSC:
            return MDR_ASSIGNABLE_TENCENT_XIAOWEI_Q_MSC;
        case TEAMS:
            return MDR_ASSIGNABLE_TEAMS;
        case GOOGLE_ASSISTANT_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION:
            return MDR_ASSIGNABLE_GOOGLE_ASSISTANT_BT_CLASSIC_CAUTION;
        case AMAZON_ALEXA_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION:
            return MDR_ASSIGNABLE_AMAZON_ALEXA_BT_CLASSIC_CAUTION;
        case TENCENT_XIAOWEI_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION:
            return MDR_ASSIGNABLE_TENCENT_XIAOWEI_BT_CLASSIC_CAUTION;
        case QUICK_ACCESS_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION:
            return MDR_ASSIGNABLE_QUICK_ACCESS_BT_CLASSIC_CAUTION;
        case AMBIENT_SOUND_CONTROL_QUICK_ACCESS_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION:
            return MDR_ASSIGNABLE_NOISE_CONTROL_QUICK_ACCESS_BT_CLASSIC_CAUTION;
        case TENCENT_XIAOWEI_Q_MSC_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION:
            return MDR_ASSIGNABLE_TENCENT_XIAOWEI_Q_MSC_BT_CLASSIC_CAUTION;
        case AMBIENT_SOUND_CONTROL_MIC:
            return MDR_ASSIGNABLE_NOISE_CONTROL_MIC;
        case LISTENING_MODE_QUICK_ACCESS:
            return MDR_ASSIGNABLE_LISTENING_MODE_QUICK_ACCESS;
        case AMBIENT_SOUND_CONTROL_LISTENING_MODE:
            return MDR_ASSIGNABLE_NOISE_CONTROL_LISTENING_MODE;
        case CHAT_MIX:
            return MDR_ASSIGNABLE_CHAT_MIX;
        case CUSTOM1:
            return MDR_ASSIGNABLE_CUSTOM1;
        case CUSTOM2:
            return MDR_ASSIGNABLE_CUSTOM2;
        default:
            return MDR_ASSIGNABLE_UNKNOWN;
        }
    }

    bool to_protocol(MDRAssignableAction value, mdr::v2::t1::Preset& out)
    {
        using enum mdr::v2::t1::Preset;
        switch (value)
        {
        case MDR_ASSIGNABLE_NONE:
            out = NO_FUNCTION;
            return true;
        case MDR_ASSIGNABLE_PLAYBACK:
            out = PLAYBACK_CONTROL;
            return true;
        case MDR_ASSIGNABLE_NOISE_CONTROL:
            out = AMBIENT_SOUND_CONTROL;
            return true;
        case MDR_ASSIGNABLE_NOISE_CONTROL_QUICK_ACCESS:
            out = AMBIENT_SOUND_CONTROL_QUICK_ACCESS;
            return true;
        case MDR_ASSIGNABLE_TRACK_CONTROL:
            out = TRACK_CONTROL;
            return true;
        case MDR_ASSIGNABLE_VOICE_RECOGNITION:
            out = VOICE_RECOGNITION;
            return true;
        case MDR_ASSIGNABLE_GOOGLE_ASSISTANT:
            out = GOOGLE_ASSIST;
            return true;
        case MDR_ASSIGNABLE_AMAZON_ALEXA:
            out = AMAZON_ALEXA;
            return true;
        case MDR_ASSIGNABLE_TENCENT_XIAOWEI:
            out = TENCENT_XIAOWEI;
            return true;
        case MDR_ASSIGNABLE_MICROSOFT_CORTANA:
            out = MS;
            return true;
        case MDR_ASSIGNABLE_QUICK_ACCESS:
            out = QUICK_ACCESS;
            return true;
        case MDR_ASSIGNABLE_VOLUME:
            out = VOLUME_CONTROL;
            return true;
        case MDR_ASSIGNABLE_PLAYBACK_VOICE_ASSISTANT_LIMITATION:
            out = PLAYBACK_CONTROL_VOICE_ASSISTANT_LIMITATION;
            return true;
        case MDR_ASSIGNABLE_TENCENT_XIAOWEI_Q_MSC:
            out = TENCENT_XIAOWEI_Q_MSC;
            return true;
        case MDR_ASSIGNABLE_TEAMS:
            out = TEAMS;
            return true;
        case MDR_ASSIGNABLE_GOOGLE_ASSISTANT_BT_CLASSIC_CAUTION:
            out = GOOGLE_ASSISTANT_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION;
            return true;
        case MDR_ASSIGNABLE_AMAZON_ALEXA_BT_CLASSIC_CAUTION:
            out = AMAZON_ALEXA_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION;
            return true;
        case MDR_ASSIGNABLE_TENCENT_XIAOWEI_BT_CLASSIC_CAUTION:
            out = TENCENT_XIAOWEI_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION;
            return true;
        case MDR_ASSIGNABLE_QUICK_ACCESS_BT_CLASSIC_CAUTION:
            out = QUICK_ACCESS_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION;
            return true;
        case MDR_ASSIGNABLE_NOISE_CONTROL_QUICK_ACCESS_BT_CLASSIC_CAUTION:
            out = AMBIENT_SOUND_CONTROL_QUICK_ACCESS_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION;
            return true;
        case MDR_ASSIGNABLE_TENCENT_XIAOWEI_Q_MSC_BT_CLASSIC_CAUTION:
            out = TENCENT_XIAOWEI_Q_MSC_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION;
            return true;
        case MDR_ASSIGNABLE_NOISE_CONTROL_MIC:
            out = AMBIENT_SOUND_CONTROL_MIC;
            return true;
        case MDR_ASSIGNABLE_LISTENING_MODE_QUICK_ACCESS:
            out = LISTENING_MODE_QUICK_ACCESS;
            return true;
        case MDR_ASSIGNABLE_NOISE_CONTROL_LISTENING_MODE:
            out = AMBIENT_SOUND_CONTROL_LISTENING_MODE;
            return true;
        case MDR_ASSIGNABLE_CHAT_MIX:
            out = CHAT_MIX;
            return true;
        case MDR_ASSIGNABLE_CUSTOM1:
            out = CUSTOM1;
            return true;
        case MDR_ASSIGNABLE_CUSTOM2:
            out = CUSTOM2;
            return true;
        default:
            return false;
        }
    }

    uint32_t AutoPowerMinutes(mdr::v1::t1::AutoPowerOffElementId value)
    {
        using enum mdr::v1::t1::AutoPowerOffElementId;
        switch (value)
        {
        case POWER_OFF_IN_5_MIN:
            return 5;
        case POWER_OFF_IN_30_MIN:
            return 30;
        case POWER_OFF_IN_60_MIN:
            return 60;
        case POWER_OFF_IN_180_MIN:
            return 180;
        default:
            return 0;
        }
    }

    bool AutoPowerFromMinutes(uint32_t minutes, mdr::v1::t1::AutoPowerOffElementId& out)
    {
        using enum mdr::v1::t1::AutoPowerOffElementId;
        switch (minutes)
        {
        case 0:
            out = POWER_OFF_DISABLE;
            return true;
        case 5:
            out = POWER_OFF_IN_5_MIN;
            return true;
        case 30:
            out = POWER_OFF_IN_30_MIN;
            return true;
        case 60:
            out = POWER_OFF_IN_60_MIN;
            return true;
        case 180:
            out = POWER_OFF_IN_180_MIN;
            return true;
        default:
            return false;
        }
    }

    uint32_t AutoPowerMinutes(mdr::v2::t1::AutoPowerOffElements value)
    {
        using enum mdr::v2::t1::AutoPowerOffElements;
        switch (value)
        {
        case POWER_OFF_IN_5_MIN:
            return 5;
        case POWER_OFF_IN_15_MIN:
            return 15;
        case POWER_OFF_IN_30_MIN:
            return 30;
        case POWER_OFF_IN_60_MIN:
            return 60;
        case POWER_OFF_IN_180_MIN:
            return 180;
        default:
            return 0;
        }
    }

    uint32_t AutoPowerMinutes(mdr::v2::t1::AutoPowerOffWearingDetectionElements value)
    {
        using enum mdr::v2::t1::AutoPowerOffWearingDetectionElements;
        switch (value)
        {
        case POWER_OFF_IN_5_MIN:
            return 5;
        case POWER_OFF_IN_15_MIN:
            return 15;
        case POWER_OFF_IN_30_MIN:
            return 30;
        case POWER_OFF_IN_60_MIN:
            return 60;
        case POWER_OFF_IN_180_MIN:
            return 180;
        default:
            return 0;
        }
    }

    bool AutoPowerFromMinutes(uint32_t minutes, mdr::v2::t1::AutoPowerOffElements& out)
    {
        using enum mdr::v2::t1::AutoPowerOffElements;
        switch (minutes)
        {
        case 0:
            out = POWER_OFF_DISABLE;
            return true;
        case 5:
            out = POWER_OFF_IN_5_MIN;
            return true;
        case 15:
            out = POWER_OFF_IN_15_MIN;
            return true;
        case 30:
            out = POWER_OFF_IN_30_MIN;
            return true;
        case 60:
            out = POWER_OFF_IN_60_MIN;
            return true;
        case 180:
            out = POWER_OFF_IN_180_MIN;
            return true;
        default:
            return false;
        }
    }

    bool AutoPowerFromMinutes(uint32_t minutes, mdr::v2::t1::AutoPowerOffWearingDetectionElements& out)
    {
        using enum mdr::v2::t1::AutoPowerOffWearingDetectionElements;
        switch (minutes)
        {
        case 0:
            out = POWER_OFF_DISABLE;
            return true;
        case 5:
            out = POWER_OFF_IN_5_MIN;
            return true;
        case 15:
            out = POWER_OFF_IN_15_MIN;
            return true;
        case 30:
            out = POWER_OFF_IN_30_MIN;
            return true;
        case 60:
            out = POWER_OFF_IN_60_MIN;
            return true;
        case 180:
            out = POWER_OFF_IN_180_MIN;
            return true;
        default:
            return false;
        }
    }

    struct ValueRange
    {
        int32_t min, max, step;
    };

    constexpr ValueRange kPlaybackVolumeRange{0, 30, 1};
    constexpr ValueRange kAmbientLevelRange{1, 20, 1};
    constexpr ValueRange kEqualizer5BandRange{-10, 10, 1};
    constexpr ValueRange kEqualizer10BandRange{-6, 6, 1};
    constexpr ValueRange kClearBassRange{-10, 10, 1};
    constexpr ValueRange kVoiceGuidanceVolumeRange{-2, 2, 1}; // CODEGEN Range -2 2.

    ValueRange PlaybackVolumeRange(const mdr::DetailsV1& state)
    {
        // volumeStep counts positions: WH-1000XM3 sends 31 for 0..30.
        // TODO @mos9527: verify if this quirk is present in all of V1 devices
        if (state.mPlayVolumeStep >= 2)
            return {0, state.mPlayVolumeStep - 1, 1};
        return kPlaybackVolumeRange;
    }

    ValueRange PlaybackVolumeRange(const mdr::DetailsV2&)
    {
        return kPlaybackVolumeRange;
    }

    ValueRange AmbientLevelRange(const mdr::DetailsV1& state)
    {
        // E.g. WH-1000XM3 advertises 20 steps for levels 1..20.
        // TODO @mos9527: verify on other V1 devices
        if (state.mAsmStep >= 1)
            return {1, state.mAsmStep, 1};
        return kAmbientLevelRange;
    }

    ValueRange AmbientLevelRange(const mdr::DetailsV2&)
    {
        return kAmbientLevelRange;
    }

    template <typename State>
    bool EqualizerBandFrequency(const State& state, uint32_t index, int32_t& out)
    {
        // EQEBB_RET_EXTENDED_INFO, both protocols: Clear Bass (if any) as SPECIFIC_INFORMATION, then the bands in Hz.
        // e.g. WH-1000XM3 (V1): {CB, 400 .. 16000}; WH-1000XM6 (V2): {31 .. 16000}.
        const size_t count = state.mEqConfig.mDesired.size();
        mdr::Vector<int32_t> frequencies;
        for (const auto& band : state.mEqBandInfo)
            if (band.infoType == decltype(band.infoType)::HZ)
                frequencies.push_back(static_cast<int32_t>(band.valueAsFrequency));
        if (frequencies.size() != count)
        {
            // No (matching) band info yet: fall back to the protocol's well-known 5/10-band layouts.
            static constexpr int32_t kDefault5[] = {400, 1000, 2500, 6300, 16000};
            static constexpr int32_t kDefault10[] = {31, 63, 125, 250, 500, 1000, 2000, 4000, 8000, 16000};
            frequencies.clear();
            if (count == 5)
                frequencies.assign(std::begin(kDefault5), std::end(kDefault5));
            else if (count == 10)
                frequencies.assign(std::begin(kDefault10), std::end(kDefault10));
        }
        if (index >= frequencies.size())
            return false;
        out = frequencies[index];
        return true;
    }

    bool DefaultEqualizerBandRange(uint32_t count, ValueRange& out)
    {
        if (count == 5)
            out = kEqualizer5BandRange;
        else if (count == 10)
            out = kEqualizer10BandRange;
        else
            return false;
        return true;
    }

    bool EqualizerBandRange(const mdr::DetailsV1& state, uint32_t count, ValueRange& out)
    {
        // The advertised band count includes Clear Bass (raw band 0).
        if (state.mEqCapabilityStep == 0 || state.mEqCapabilityBands != count + 1)
            return DefaultEqualizerBandRange(count, out);
        const int offset = state.EqStepOffset();
        out = {-offset, offset, 1};
        return true;
    }

    bool EqualizerBandRange(const mdr::DetailsV2& state, uint32_t count, ValueRange& out)
    {
        // The advertised band count is the raw count: WH-1000XM6 sends 10 bands / 13 steps for -6..6.
        const size_t rawBands = count + (state.EqClearBassInRaw(count) ? 1 : 0);
        if (state.mEqCapabilityStep == 0 || state.mEqCapabilityBands != rawBands)
            return DefaultEqualizerBandRange(count, out);
        const int offset = state.EqStepOffset(rawBands);
        out = {-offset, offset, 1};
        return true;
    }

    ValueRange ClearBassRange(const mdr::DetailsV1& state)
    {
        // Clear Bass shares the raw EQ encoding as band 0.
        const int offset = state.EqStepOffset();
        return {-offset, offset, 1};
    }

    ValueRange ClearBassRange(const mdr::DetailsV2& state)
    {
        // Clear Bass shares the raw EQ encoding as band 0.
        const size_t bands = state.mEqConfig.mDesired.size();
        if (!state.EqClearBassInRaw(bands))
            return kClearBassRange;
        const int offset = state.EqStepOffset(bands + 1);
        return {-offset, offset, 1};
    }

    bool ClearBassAvailable(const mdr::DetailsV1& state)
    {
        // Clear Bass only exists in the 6-band (Clear Bass + 5) layout; the 10-band layout has none.
        return SupportsFeature(state, MDR_FEATURE_EQUALIZER) && state.mEqConfig.mDesired.size() == 5;
    }

    bool ClearBassAvailable(const mdr::DetailsV2& state)
    {
        // Follows the device's own param layout once it has sent one.
        return SupportsFeature(state, MDR_FEATURE_EQUALIZER) &&
            state.EqClearBassInRaw(state.mEqConfig.mDesired.size()) && !state.mEqConfig.mDesired.empty();
    }

    template <typename State>
    ValueRange VoiceGuidanceVolumeRange(const State&)
    {
        return kVoiceGuidanceVolumeRange;
    }

    template <typename Element>
    bool AutoPowerRoundTrips(Element element, uint32_t& minutes)
    {
        // AutoPowerMinutes maps unknown elements to 0, so only accept elements that map back to themselves.
        minutes = AutoPowerMinutes(element);
        Element back{};
        return AutoPowerFromMinutes(minutes, back) && back == element;
    }

    mdr::Vector<uint32_t> AutoPowerOffOptions(const mdr::DetailsV1& state)
    {
        using enum mdr::v1::t1::AutoPowerOffElementId;
        static constexpr mdr::v1::t1::AutoPowerOffElementId kDefault[] = {
            POWER_OFF_IN_5_MIN, POWER_OFF_IN_30_MIN, POWER_OFF_IN_60_MIN, POWER_OFF_IN_180_MIN, POWER_OFF_DISABLE};
        mdr::Vector<uint32_t> out;
        const auto append = [&](auto element)
        {
            uint32_t minutes = 0;
            if (AutoPowerRoundTrips(element, minutes) && std::ranges::find(out, minutes) == out.end())
                out.push_back(minutes);
        };
        if (!state.mAutoPowerOffCandidates.empty())
            std::ranges::for_each(state.mAutoPowerOffCandidates, append);
        if (out.empty())
            std::ranges::for_each(kDefault, append);
        return out;
    }

    mdr::Vector<uint32_t> AutoPowerOffOptions(const mdr::DetailsV2&)
    {
        // NOTE: Simply per protocol. This shouldn't be device-dependent (V2)
        return {5, 15, 30, 60, 180, 0};
    }

    bool InRange(const ValueRange& range, int64_t value)
    {
        return value >= range.min && value <= range.max;
    }

    MDRResult ReadRange(const ValueRange& range, MDRValue key, MDRValue minKey, int32_t& out)
    {
        out = key == minKey ? range.min : key == minKey + 1 ? range.max : range.step;
        return MDR_RESULT_OK;
    }

    template <typename State>
    MDRResult ReadValue(const State& state, MDRValue key, uint32_t index, int32_t& out)
    {
        switch (key)
        {
        case MDR_VALUE_PLAYBACK_VOLUME_MIN:
        case MDR_VALUE_PLAYBACK_VOLUME_MAX:
        case MDR_VALUE_PLAYBACK_VOLUME_STEP:
            if (!SupportsFeature(state, MDR_FEATURE_PLAYBACK_VOLUME))
                return MDR_RESULT_ERROR_NOT_SUPPORTED;
            return ReadRange(PlaybackVolumeRange(state), key, MDR_VALUE_PLAYBACK_VOLUME_MIN, out);
        case MDR_VALUE_AMBIENT_LEVEL_MIN:
        case MDR_VALUE_AMBIENT_LEVEL_MAX:
        case MDR_VALUE_AMBIENT_LEVEL_STEP:
            if (!SupportsFeature(state, MDR_FEATURE_AMBIENT_SOUND))
                return MDR_RESULT_ERROR_NOT_SUPPORTED;
            return ReadRange(AmbientLevelRange(state), key, MDR_VALUE_AMBIENT_LEVEL_MIN, out);
        case MDR_VALUE_EQUALIZER_BAND_MIN:
        case MDR_VALUE_EQUALIZER_BAND_MAX:
        case MDR_VALUE_EQUALIZER_BAND_STEP:
        {
            if (!SupportsFeature(state, MDR_FEATURE_EQUALIZER))
                return MDR_RESULT_ERROR_NOT_SUPPORTED;
            ValueRange range{};
            if (!EqualizerBandRange(state, index, range))
                return MDR_RESULT_ERROR_NOT_FOUND;
            return ReadRange(range, key, MDR_VALUE_EQUALIZER_BAND_MIN, out);
        }
        case MDR_VALUE_EQUALIZER_CLEAR_BASS_MIN:
        case MDR_VALUE_EQUALIZER_CLEAR_BASS_MAX:
        case MDR_VALUE_EQUALIZER_CLEAR_BASS_STEP:
            if (!ClearBassAvailable(state))
                return MDR_RESULT_ERROR_NOT_SUPPORTED;
            return ReadRange(ClearBassRange(state), key, MDR_VALUE_EQUALIZER_CLEAR_BASS_MIN, out);
        case MDR_VALUE_VOICE_GUIDANCE_VOLUME_MIN:
        case MDR_VALUE_VOICE_GUIDANCE_VOLUME_MAX:
        case MDR_VALUE_VOICE_GUIDANCE_VOLUME_STEP:
            if (!SupportsFeature(state, MDR_FEATURE_VOICE_GUIDANCE_VOLUME))
                return MDR_RESULT_ERROR_NOT_SUPPORTED;
            return ReadRange(VoiceGuidanceVolumeRange(state), key, MDR_VALUE_VOICE_GUIDANCE_VOLUME_MIN, out);
        case MDR_VALUE_AUTO_POWER_OFF_OPTION_COUNT:
        case MDR_VALUE_AUTO_POWER_OFF_OPTION:
        {
            if (!SupportsFeature(state, MDR_FEATURE_AUTO_POWER_OFF))
                return MDR_RESULT_ERROR_NOT_SUPPORTED;
            const auto options = AutoPowerOffOptions(state);
            if (key == MDR_VALUE_AUTO_POWER_OFF_OPTION_COUNT)
                out = static_cast<int32_t>(options.size());
            else if (index < options.size())
                out = static_cast<int32_t>(options[index]);
            else
                return MDR_RESULT_ERROR_NOT_FOUND;
            return MDR_RESULT_OK;
        }
        case MDR_VALUE_EQUALIZER_BAND_FREQUENCY:
            if (!SupportsFeature(state, MDR_FEATURE_EQUALIZER))
                return MDR_RESULT_ERROR_NOT_SUPPORTED;
            return EqualizerBandFrequency(state, index, out) ? MDR_RESULT_OK : MDR_RESULT_ERROR_NOT_FOUND;
        default:
            return MDR_RESULT_ERROR_INVALID_ARGUMENT;
        }
    }

    MDRResult ValidateDeviceId(const MDRPairedDeviceAction& action, std::string_view& out)
    {
        if (!action.device_id || action.device_id_size == 0)
            return MDR_RESULT_ERROR_INVALID_ARGUMENT;
        uint32_t length = action.device_id_size;
        if (action.device_id[length - 1] == '\0')
            --length;
        if (length != 17 || std::memchr(action.device_id, '\0', length))
            return MDR_RESULT_ERROR_INVALID_ARGUMENT;
        out = {action.device_id, length};
        return MDR_RESULT_OK;
    }
}

extern "C" {
const char* mdrResultString(MDRResult err)
{
    switch (err)
    {
    case MDR_RESULT_OK:
        return "OK";
    case MDR_RESULT_INPROGRESS:
        return "Task in progress";
    case MDR_RESULT_ERROR_GENERAL:
        return "General error";
    case MDR_RESULT_ERROR_NOT_FOUND:
        return "Resource not found";
    case MDR_RESULT_ERROR_TIMEOUT:
        return "Timed out";
    case MDR_RESULT_ERROR_NET:
        return "Networking error";
    case MDR_RESULT_ERROR_NO_CONNECTION:
        return "No connection has been established";
    case MDR_RESULT_ERROR_BAD_ADDRESS:
        return "Invalid address information";
    case MDR_RESULT_ERROR_NOT_SUPPORTED:
        return "Not supported";
    case MDR_RESULT_ERROR_BUFFER_TOO_SMALL:
        return "Buffer too small";
    case MDR_RESULT_ERROR_MALFORMED_PAYLOAD:
        return "Malformed payload";
    case MDR_RESULT_ERROR_INVALID_ARGUMENT:
        return "Invalid argument";
    case MDR_RESULT_ERROR_ABI_MISMATCH:
        return "Library does not implement the caller's MDR ABI version";
    default:
        return "Unknown";
    }
}

MDRResult mdrConnectionConnect(MDRConnection* conn, const char* macAddress, const char* serviceUUID)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->connect(conn->user, macAddress, serviceUUID);
}

void mdrConnectionDisconnect(MDRConnection* conn)
{
    if (conn)
        conn->disconnect(conn->user);
}

MDRResult mdrConnectionRecv(MDRConnection* conn, char* dst, int size, int* pReceived)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->recv(conn->user, dst, size, pReceived);
}

MDRResult mdrConnectionSend(MDRConnection* conn, const char* src, int size, int* pSent)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->send(conn->user, src, size, pSent);
}

MDRResult mdrConnectionPoll(MDRConnection* conn, int timeout)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->poll(conn->user, timeout);
}

MDRResult mdrConnectionGetDevicesList(MDRConnection* conn, MDRDeviceInfo** ppList, int* pCount)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->getDevicesList(conn->user, ppList, pCount);
}

MDRResult mdrConnectionFreeDevicesList(MDRConnection* conn, MDRDeviceInfo** ppList)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->freeDevicesList(conn->user, ppList);
}

const char* mdrConnectionGetLastError(MDRConnection* conn)
{
    if (!conn)
        return "No connection";
    return conn->getLastError(conn->user);
}

MDRResult mdrHeadphonesCreate(uint32_t abiVersion, MDRConnection* connection, MDRProtocolVersion protocolVersion,
                              MDRHeadphones** outHeadphones)
{
    // Clear the handle before anything else, so a caller that ignores the result is not left
    // holding whatever was in the variable to begin with.
    if (outHeadphones)
        *outHeadphones = nullptr;
    // Ahead of the argument checks: reading MDRConnection at all assumes we agree on its layout,
    // which is exactly what this version stands for.
    if (abiVersion != MDR_ABI_VERSION)
        return MDR_RESULT_ERROR_ABI_MISMATCH;
    if (!connection || !outHeadphones || !connection->recv || !connection->send || !connection->poll)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;

    Headphones::ProtocolFamily family;
    switch (protocolVersion)
    {
    case MDR_PROTOCOL_V1:
        family = Headphones::ProtocolFamily::V1;
        break;
    case MDR_PROTOCOL_V2:
        family = Headphones::ProtocolFamily::V2;
        break;
    default:
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    }

    auto* headphones = mdr::Construct<Headphones>(connection, family);
    if (!headphones)
        return MDR_RESULT_ERROR_GENERAL;
    *outHeadphones = mdr::detail::HeadphonesHandle(headphones);
    return MDR_RESULT_OK;
}

void mdrHeadphonesDestroy(MDRHeadphones* headphones)
{
    if (headphones)
        mdr::Destruct(Impl(headphones));
}

MDRBoolean mdrHeadphonesIsInitialized(const MDRHeadphones* headphones)
{
    return static_cast<MDRBoolean>(headphones && Impl(headphones)->mInitialized);
}

MDRBoolean mdrHeadphonesIsReady(const MDRHeadphones* headphones)
{
    return static_cast<MDRBoolean>(headphones && Impl(headphones)->IsReady());
}

MDRBoolean mdrHeadphonesIsDirty(const MDRHeadphones* headphones)
{
    return static_cast<MDRBoolean>(headphones && Impl(headphones)->IsDirty());
}

MDRResult mdrHeadphonesRequestInit(MDRHeadphones* headphones, MDRHeadphonesInitFlags flags)
{
    if (!headphones || flags == MDR_HEADPHONES_INIT_NONE ||
        (flags & ~MDR_HEADPHONES_INIT_DEFAULT) != MDR_HEADPHONES_INIT_NONE)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (const auto failure = h->GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h->IsReady())
        return MDR_RESULT_INPROGRESS;

    const MDRHeadphonesInitFlags completedOrRequested = h->mInitFlags | flags;
    if ((completedOrRequested & MDR_HEADPHONES_INIT_SUPPORT_FUNCTIONS) != 0u &&
        (completedOrRequested & MDR_HEADPHONES_INIT_IDENTITY) == 0u)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const MDRHeadphonesInitFlags optionalFlags = MDR_HEADPHONES_INIT_DEFAULT & ~MDR_HEADPHONES_INIT_DISCOVERY;
    if ((completedOrRequested & optionalFlags) != 0u &&
        (completedOrRequested & MDR_HEADPHONES_INIT_SUPPORT_FUNCTIONS) == 0u)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;

    const MDRHeadphonesInitFlags pendingFlags = flags & ~h->mInitFlags;
    return h->Invoke(h->RequestInit(pendingFlags));
}

MDRResult mdrHeadphonesRequestSync(MDRHeadphones* headphones, MDRHeadphonesSyncFlags flags)
{
    if (!headphones || flags == MDR_HEADPHONES_SYNC_NONE ||
        (flags & ~MDR_HEADPHONES_SYNC_DEFAULT) != MDR_HEADPHONES_SYNC_NONE)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (const auto failure = h->GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h->IsReady())
        return MDR_RESULT_INPROGRESS;
    return h->Invoke(h->RequestSync(flags));
}

MDRResult mdrHeadphonesGetSync(MDRHeadphones* headphones, MDRHeadphonesSyncFlags* outFlags)
{
    if (!headphones || !outFlags)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    *outFlags = h.mSyncFlags;
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesRequestCommit(MDRHeadphones* headphones)
{
    if (!headphones)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (const auto failure = h->GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h->IsReady())
        return MDR_RESULT_INPROGRESS;
    return h->Invoke(h->RequestCommit());
}

MDRResult mdrDebugRequestCommand(MDRHeadphones* headphones, const unsigned char* payload, uint32_t payloadSize,
                                 uint32_t dataType, MDRBoolean awaitAck)
{
    if (!headphones || (!payload && payloadSize != 0u) || payloadSize > mdr::kMDRMaxPacketSize ||
        dataType > std::numeric_limits<mdr::UInt8>::max() || (awaitAck != MDR_FALSE && awaitAck != MDR_TRUE))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (const auto failure = h->GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h->IsReady())
        return MDR_RESULT_INPROGRESS;

    mdr::MDRBuffer ownedPayload;
    if (payloadSize != 0u)
        ownedPayload.assign(payload, payload + payloadSize);
    return h->Invoke(
        h->RequestDebugCommand(std::move(ownedPayload), static_cast<mdr::MDRDataType>(dataType), awaitAck == MDR_TRUE));
}

MDRResult mdrHeadphonesSetAlertResponse(MDRHeadphones* headphones, MDRAlertAction action)
{
    if (!headphones || (action != MDR_ALERT_ACTION_POSITIVE && action != MDR_ALERT_ACTION_NEGATIVE))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h.mInitialized && !h.IsReady())
        return MDR_RESULT_INPROGRESS;
    return WithDetails(h,
                       [&](auto& state) -> MDRResult
                       {
                           if (!state.mAlertAwaitingResponse)
                               return MDR_RESULT_ERROR_NOT_FOUND;
                           using Action = decltype(state.mAlertResponse.mDesired.action);
                           state.mAlertResponse.stage(
                               {state.mLastAlertMessage,
                                action == MDR_ALERT_ACTION_POSITIVE ? Action::POSITIVE : Action::NEGATIVE,
                                state.mAlertSerial});
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesPoll(MDRHeadphones* headphones, MDREvent* outEvent)
{
    if (!headphones || !outEvent)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    return Impl(headphones)->PollEvents(*outEvent);
}

MDRResult mdrHeadphonesSetTimeout(MDRHeadphones* headphones, MDRTimeoutType type, int value)
{
    if (!headphones)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    return Impl(headphones)->SetTimeout(type, value);
}

void mdrDebugSetPacketCallback(MDRHeadphones* headphones, MDRPacketCallback callback, void* userData)
{
    if (headphones)
        Impl(headphones)->SetPacketCallback(callback, userData);
}

MDRResult mdrHeadphonesGetFeature(MDRHeadphones* headphones, MDRFeature feature,
                                  MDRFeatureAvailability* outAvailability)
{
    if (!headphones || !outAvailability)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    // Keep the upper bound on the last MDR_FEATURE_* id, or newly added features read as invalid.
    if (feature < MDR_FEATURE_IDENTITY || feature > MDR_FEATURE_MAX_VALUE)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h.mInitialized)
    {
        *outAvailability = MDR_AVAILABILITY_UNKNOWN;
        return MDR_RESULT_OK;
    }
    *outAvailability = WithDetails(h, [feature](const auto& state) { return SupportsFeature(state, feature); })
        ? MDR_AVAILABILITY_AVAILABLE
        : MDR_AVAILABILITY_UNAVAILABLE;
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetText(MDRHeadphones* headphones, MDRText text, uint32_t index, char* buffer,
                               uint32_t* inoutSize)
{
    if (!headphones)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (text == MDR_TEXT_LAST_ERROR)
        return CopyText(h.GetLastError(), buffer, inoutSize);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    return WithDetails(
        h,
        [&](const auto& state) -> MDRResult
        {
            switch (text)
            {
            case MDR_TEXT_MODEL_NAME:
                return CopyText({state.mModelName.data(), state.mModelName.size()}, buffer, inoutSize);
            case MDR_TEXT_UNIQUE_ID:
                return CopyText({state.mUniqueId.data(), state.mUniqueId.size()}, buffer, inoutSize);
            case MDR_TEXT_FIRMWARE_VERSION:
                return CopyText({state.mFWVersion.data(), state.mFWVersion.size()}, buffer, inoutSize);
            case MDR_TEXT_MODEL_SERIES:
                return CopyText(ModelSeriesText(state), buffer, inoutSize);
            case MDR_TEXT_MODEL_COLOR:
                return CopyText(ModelColorText(state), buffer, inoutSize);
            case MDR_TEXT_TRACK_TITLE:
                return CopyText({state.mPlayTrackTitle.data(), state.mPlayTrackTitle.size()}, buffer, inoutSize);
            case MDR_TEXT_TRACK_ALBUM:
                return CopyText({state.mPlayTrackAlbum.data(), state.mPlayTrackAlbum.size()}, buffer, inoutSize);
            case MDR_TEXT_TRACK_ARTIST:
                return CopyText({state.mPlayTrackArtist.data(), state.mPlayTrackArtist.size()}, buffer, inoutSize);
            case MDR_TEXT_PAIRED_DEVICE_ID:
            case MDR_TEXT_PAIRED_DEVICE_NAME:
                if (index >= state.mPairedDevices.size())
                    return MDR_RESULT_ERROR_NOT_FOUND;
                if (text == MDR_TEXT_PAIRED_DEVICE_ID)
                    return CopyText(
                        {state.mPairedDevices[index].macAddress.data(), state.mPairedDevices[index].macAddress.size()},
                        buffer, inoutSize);
                return CopyText({state.mPairedDevices[index].name.data(), state.mPairedDevices[index].name.size()},
                                buffer, inoutSize);
            case MDR_TEXT_EQUALIZER_PRESET_NAME:
                if (index >= state.mEqPresets.size())
                    return MDR_RESULT_ERROR_NOT_FOUND;
                return CopyText({state.mEqPresets[index].name.data(), state.mEqPresets[index].name.size()}, buffer,
                                inoutSize);
            case MDR_TEXT_GENERAL_SETTING_SUBJECT:
            case MDR_TEXT_GENERAL_SETTING_SUMMARY:
            {
                if (index >= std::size(state.mGsCapability))
                    return MDR_RESULT_ERROR_NOT_FOUND;
                const auto& value = text == MDR_TEXT_GENERAL_SETTING_SUBJECT
                    ? state.mGsCapability[index].value.subject.value
                    : state.mGsCapability[index].value.summary.value;
                return CopyText({value.data(), value.size()}, buffer, inoutSize);
            }
            case MDR_TEXT_GENERAL_SETTING_OPTION_SUBJECT:
            case MDR_TEXT_GENERAL_SETTING_OPTION_SUMMARY:
            {
                const uint32_t setting = index & 0xffu, option = index >> 8u;
                if (setting >= std::size(state.mGsCapability) || option >= state.mGsCapability[setting].options.size())
                    return MDR_RESULT_ERROR_NOT_FOUND;
                const auto& info = state.mGsCapability[setting].options[option];
                const auto& value =
                    text == MDR_TEXT_GENERAL_SETTING_OPTION_SUBJECT ? info.subject.value : info.summary.value;
                return CopyText({value.data(), value.size()}, buffer, inoutSize);
            }
            case MDR_TEXT_LAST_ALERT:
                return CopyText(format_as(state.mLastAlertMessage), buffer, inoutSize);
            case MDR_TEXT_LAST_INTERACTION:
                return CopyText({state.mLastInteractionMessage.data(), state.mLastInteractionMessage.size()}, buffer,
                                inoutSize);
            case MDR_TEXT_LAST_DEVICE_MESSAGE:
                return CopyText({state.mLastDeviceJSONMessage.data(), state.mLastDeviceJSONMessage.size()}, buffer,
                                inoutSize);
            default:
                return MDR_RESULT_ERROR_INVALID_ARGUMENT;
            }
        });
}

MDRResult mdrHeadphonesGetValue(MDRHeadphones* headphones, MDRValue value, uint32_t index, int32_t* outValue)
{
    if (!headphones || !outValue)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    if (value < MDR_VALUE_PLAYBACK_VOLUME_MIN || value > MDR_VALUE_EQUALIZER_BAND_FREQUENCY)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h.mInitialized)
        return MDR_RESULT_INPROGRESS;
    return WithDetails(h, [&](const auto& state) { return ReadValue(state, value, index, *outValue); });
}

MDRResult mdrHeadphonesGetModel(MDRHeadphones* headphones, MDRModel* outIdentity)
{
    if (!headphones || !outIdentity)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    return WithDetails(h,
                       [&](const auto& state) -> MDRResult
                       {
                           *outIdentity = {.protocol_version = static_cast<uint32_t>(state.mProtocol.version),
                                           .audio_codec = from_protocol(state.mAudioCodec),
                                           .model_color = static_cast<uint32_t>(state.mModelColor)};
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesGetBatteries(MDRHeadphones* headphones, MDRBattery* batteries, uint32_t* inoutCount)
{
    if (!headphones || !inoutCount)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    return WithDetails(h,
                       [&](const auto& state) -> MDRResult
                       {
                           const bool hasLR = SupportsFeature(state, MDR_FEATURE_BATTERY_LEFT_RIGHT);
                           const bool hasSingle = !hasLR && SupportsFeature(state, MDR_FEATURE_BATTERY_SINGLE);
                           const bool hasCase = SupportsFeature(state, MDR_FEATURE_BATTERY_CASE);
                           const uint32_t required = (hasLR ? 2u : hasSingle ? 1u : 0u) + (hasCase ? 1u : 0u);
                           if (!batteries)
                           {
                               if (*inoutCount != 0)
                                   return MDR_RESULT_ERROR_INVALID_ARGUMENT;
                               *inoutCount = required;
                               return MDR_RESULT_OK;
                           }
                           if (*inoutCount < required)
                           {
                               *inoutCount = required;
                               return MDR_RESULT_ERROR_BUFFER_TOO_SMALL;
                           }
                           uint32_t out = 0;
                           const auto write = [&](MDRBatteryPart part, const auto& battery)
                           {
                               batteries[out++] = {.part = part,
                                                   .level_percent = battery.level,
                                                   .update_threshold_percent = battery.threshold,
                                                   .charging = from_protocol(battery.charging)};
                           };
                           if (hasLR)
                           {
                               write(MDR_BATTERY_LEFT, state.mBatteryL);
                               write(MDR_BATTERY_RIGHT, state.mBatteryR);
                           }
                           else if (hasSingle)
                               write(MDR_BATTERY_MAIN, state.mBatteryL);
                           if (hasCase)
                               write(MDR_BATTERY_CASE, state.mBatteryCase);
                           *inoutCount = required;
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesGetPlayback(MDRHeadphones* headphones, MDRPropertyType type, MDRPlayback* outPlayback)
{
    if (!headphones || !ValidPropertyType(type) || !outPlayback)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    return WithDetails(h,
                       [&](const auto& state) -> MDRResult
                       {
                           *outPlayback = {.status = from_protocol(state.mPlayPause),
                                           .volume = static_cast<uint32_t>(ReadProperty(state.mPlayVolume, type))};
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesSetPlaybackVolume(MDRHeadphones* headphones, uint32_t volume)
{
    if (!headphones)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto& h = *Impl(headphones);
    if (!InRange(WithDetails(h, [](const auto& state) { return PlaybackVolumeRange(state); }), volume))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h.mInitialized && !h.IsReady())
        return MDR_RESULT_INPROGRESS;
    return WithDetails(h,
                       [&](auto& state) -> MDRResult
                       {
                           state.mPlayVolume.stage(static_cast<int>(volume));
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesSendPlaybackCommand(MDRHeadphones* headphones, const MDRPlaybackCommand* command)
{
    if (!headphones || !command)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h.mInitialized && !h.IsReady())
        return MDR_RESULT_INPROGRESS;
    return WithDetails(h,
                       [&](auto& state) -> MDRResult
                       {
                           auto value = state.mPlayControl.mDesired;
                           if (!PlaybackControlFromAction(command->action, value))
                               return MDR_RESULT_ERROR_INVALID_ARGUMENT;
                           state.mPlayControl.stage(value);
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesGetNoiseControl(MDRHeadphones* headphones, MDRPropertyType type,
                                       MDRNoiseControl* outNoiseControl)
{
    if (!headphones || !ValidPropertyType(type) || !outNoiseControl)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (h.mProtocolFamily == Headphones::ProtocolFamily::V1)
    {
        using mdr::v1::t1::NcDualSingleValue;
        const auto& state = h.mDetailsV1;
        // V1 has an on/off switch plus an NC selector: DUAL is NC, SINGLE is wind noise reduction, OFF is ambient.
        const auto nc = ReadProperty(state.mNcAsmNcValue, type);
        const MDRNoiseMode mode = !ReadProperty(state.mNcAsmEnabled, type) ? MDR_NOISE_MODE_OFF
            : nc == NcDualSingleValue::DUAL                                ? MDR_NOISE_MODE_CANCELLING
            : nc == NcDualSingleValue::SINGLE                              ? MDR_NOISE_MODE_WIND_NOISE_REDUCTION
                                                                           : MDR_NOISE_MODE_AMBIENT;
        *outNoiseControl = {
            .mode = mode,
            .ambient_level = static_cast<uint32_t>(std::max(ReadProperty(state.mNcAsmAmbientLevel, type), 0)),
            .changing_asm_level = static_cast<MDRBoolean>(ReadProperty(state.mNcAsmChangingLevel, type)),
            .focus_on_voice = static_cast<MDRBoolean>(ReadProperty(state.mNcAsmFocusOnVoice, type)),
            .button_mode = MDR_NOISE_BUTTON_NONE,
            .adaptive_ambient = MDR_FALSE,
            .adaptive_sensitivity = MDR_ADAPTIVE_SENSITIVITY_UNKNOWN};
        return MDR_RESULT_OK;
    }
    const auto& state = h.mDetailsV2;
    *outNoiseControl = {
        .mode = !ReadProperty(state.mNcAsmEnabled, type)                         ? MDR_NOISE_MODE_OFF
            : ReadProperty(state.mNcAsmMode, type) == mdr::v2::t1::NcAsmMode::NC ? MDR_NOISE_MODE_CANCELLING
                                                                                 : MDR_NOISE_MODE_AMBIENT,
        .ambient_level = static_cast<uint32_t>(ReadProperty(state.mNcAsmAmbientLevel, type)),
        .changing_asm_level = static_cast<MDRBoolean>(ReadProperty(state.mNcAsmChangingAsmLevel, type)),
        .focus_on_voice = static_cast<MDRBoolean>(ReadProperty(state.mNcAsmFocusOnVoice, type)),
        .button_mode = from_protocol(ReadProperty(state.mNcAsmButtonFunction, type)),
        .adaptive_ambient = static_cast<MDRBoolean>(ReadProperty(state.mNcAsmAutoAsmEnabled, type)),
        .adaptive_sensitivity = from_protocol(ReadProperty(state.mNcAsmNoiseAdaptiveSensitivity, type))};
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetNoiseControl(MDRHeadphones* headphones, const MDRNoiseControl* noiseControl)
{
    if (!headphones || !noiseControl || noiseControl->mode > MDR_NOISE_MODE_WIND_NOISE_REDUCTION ||
        !ValidBoolean(noiseControl->changing_asm_level) || !ValidBoolean(noiseControl->focus_on_voice) ||
        !ValidBoolean(noiseControl->adaptive_ambient))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (const auto failure = h->GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h->mInitialized && !h->IsReady())
        return MDR_RESULT_INPROGRESS;
    // Only the upper bound is enforced: outside ambient mode the device reports, and accepts, level 0.
    const int64_t ambientMax = WithDetails(*h, [](const auto& state) { return AmbientLevelRange(state).max; });
    if (noiseControl->ambient_level > ambientMax)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    if (noiseControl->mode == MDR_NOISE_MODE_WIND_NOISE_REDUCTION &&
        !WithDetails(*h, [](const auto& state) { return SupportsFeature(state, MDR_FEATURE_WIND_NOISE_REDUCTION); }))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    if (h->mProtocolFamily == Headphones::ProtocolFamily::V1)
    {
        using mdr::v1::t1::NcDualSingleValue;
        if (noiseControl->button_mode != MDR_NOISE_BUTTON_NONE || noiseControl->adaptive_ambient != MDR_FALSE ||
            noiseControl->adaptive_sensitivity != MDR_ADAPTIVE_SENSITIVITY_UNKNOWN)
            return MDR_RESULT_ERROR_NOT_SUPPORTED;
        auto& state = h->mDetailsV1;
        const bool enabled = noiseControl->mode != MDR_NOISE_MODE_OFF;
        const bool changingLevel = noiseControl->changing_asm_level != MDR_FALSE;
        const auto effect = !enabled       ? mdr::v1::t1::NcAsmEffect::OFF
            : changingLevel                ? mdr::v1::t1::NcAsmEffect::ADJUSTMENT_IN_PROGRESS
            : state.mNcAsmEnabled.mDesired ? mdr::v1::t1::NcAsmEffect::ADJUSTMENT_COMPLETION
                                           : mdr::v1::t1::NcAsmEffect::ON;
        // OFF keeps the previous NC selection so turning it back on restores it.
        const auto nc = noiseControl->mode == MDR_NOISE_MODE_CANCELLING ? NcDualSingleValue::DUAL
            : noiseControl->mode == MDR_NOISE_MODE_WIND_NOISE_REDUCTION ? NcDualSingleValue::SINGLE
            : noiseControl->mode == MDR_NOISE_MODE_AMBIENT              ? NcDualSingleValue::OFF
                                                                        : state.mNcAsmNcValue.mDesired;
        state.mNcAsmEffect.stage(effect);
        state.mNcAsmEnabled.stage(enabled);
        state.mNcAsmNcValue.stage(nc);
        state.mNcAsmAmbientLevel.stage(static_cast<int>(noiseControl->ambient_level));
        state.mNcAsmChangingLevel.stage(noiseControl->changing_asm_level != MDR_FALSE);
        state.mNcAsmFocusOnVoice.stage(noiseControl->focus_on_voice != MDR_FALSE);
        return MDR_RESULT_OK;
    }
    auto& state = h->mDetailsV2;
    mdr::v2::t1::Function button{};
    auto sensitivity = state.mNcAsmNoiseAdaptiveSensitivity.mDesired;
    if (!to_protocol(noiseControl->button_mode, button) ||
        (noiseControl->adaptive_sensitivity != MDR_ADAPTIVE_SENSITIVITY_UNKNOWN &&
         !to_protocol(noiseControl->adaptive_sensitivity, sensitivity)))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    state.mNcAsmEnabled.stage(noiseControl->mode != MDR_NOISE_MODE_OFF);
    state.mNcAsmMode.stage(noiseControl->mode == MDR_NOISE_MODE_AMBIENT ? mdr::v2::t1::NcAsmMode::ASM
                                                                        : mdr::v2::t1::NcAsmMode::NC);
    state.mNcAsmAmbientLevel.stage(static_cast<int>(noiseControl->ambient_level));
    state.mNcAsmChangingAsmLevel.stage(noiseControl->changing_asm_level != MDR_FALSE);
    state.mNcAsmFocusOnVoice.stage(noiseControl->focus_on_voice != MDR_FALSE);
    state.mNcAsmButtonFunction.stage(button);
    state.mNcAsmAutoAsmEnabled.stage(noiseControl->adaptive_ambient != MDR_FALSE);
    state.mNcAsmNoiseAdaptiveSensitivity.stage(sensitivity);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetSpeakToChat(MDRHeadphones* headphones, MDRPropertyType type, MDRSpeakToChat* outSpeakToChat)
{
    if (!headphones || !ValidPropertyType(type) || !outSpeakToChat)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    return WithDetails(h,
                       [&](const auto& state) -> MDRResult
                       {
                           *outSpeakToChat = {
                               .enabled = static_cast<MDRBoolean>(ReadProperty(state.mSpeakToChatEnabled, type)),
                               .sensitivity = from_protocol(ReadProperty(state.mSpeakToChatDetectSensitivity, type)),
                               .timeout = from_protocol(ReadProperty(state.mSpeakToModeOutTime, type))};
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesSetSpeakToChat(MDRHeadphones* headphones, const MDRSpeakToChat* speakToChat)
{
    if (!headphones || !speakToChat || !ValidBoolean(speakToChat->enabled))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h.mInitialized && !h.IsReady())
        return MDR_RESULT_INPROGRESS;
    if (!WithDetails(h, [](const auto& state) { return SupportsFeature(state, MDR_FEATURE_SPEAK_TO_CHAT); }))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    return WithDetails(
        h,
        [&](auto& state) -> MDRResult
        {
            auto sensitivity = state.mSpeakToChatDetectSensitivity.mDesired;
            auto timeout = state.mSpeakToModeOutTime.mDesired;
            if ((speakToChat->sensitivity != MDR_SPEECH_SENSITIVITY_UNKNOWN &&
                 !to_protocol(speakToChat->sensitivity, sensitivity)) ||
                (speakToChat->timeout != MDR_SPEAK_TIMEOUT_UNKNOWN && !to_protocol(speakToChat->timeout, timeout)))
                return MDR_RESULT_ERROR_INVALID_ARGUMENT;
            state.mSpeakToChatEnabled.stage(speakToChat->enabled != MDR_FALSE);
            state.mSpeakToChatDetectSensitivity.stage(sensitivity);
            state.mSpeakToModeOutTime.stage(timeout);
            return MDR_RESULT_OK;
        });
}

MDRResult mdrHeadphonesGetListening(MDRHeadphones* headphones, MDRPropertyType type, MDRListening* outListening)
{
    if (!headphones || !ValidPropertyType(type) || !outListening)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (h.mProtocolFamily == Headphones::ProtocolFamily::V1)
    {
        *outListening = {.mode = MDR_LISTENING_STANDARD, .background_room = MDR_ROOM_UNKNOWN};
        return MDR_RESULT_OK;
    }
    const auto& state = h.mDetailsV2;
    const MDRListeningMode mode = ReadProperty(state.mBGMModeEnabled, type) ? MDR_LISTENING_BACKGROUND_MUSIC
        : ReadProperty(state.mUpmixCinemaEnabled, type)                     ? MDR_LISTENING_CINEMA
        : ReadProperty(state.mVoiceContentsEnabled, type)                   ? MDR_LISTENING_VOICE_BOOST
        : ReadProperty(state.mSoundLeakageReductionEnabled, type)           ? MDR_LISTENING_SOUND_LEAKAGE_REDUCTION
                                                                            : MDR_LISTENING_STANDARD;
    *outListening = {.mode = mode, .background_room = from_protocol(ReadProperty(state.mBGMModeRoomSize, type))};
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetListening(MDRHeadphones* headphones, const MDRListening* listening)
{
    if (!headphones || !listening || listening->mode > MDR_LISTENING_SOUND_LEAKAGE_REDUCTION)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (const auto failure = h->GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h->mInitialized && !h->IsReady())
        return MDR_RESULT_INPROGRESS;
    if (h->mProtocolFamily != Headphones::ProtocolFamily::V2)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    auto& state = h->mDetailsV2;
    static constexpr MDRFeature kModeFeatures[] = {0, /* MDR_LISTENING_STANDARD */
                                                   MDR_FEATURE_LISTENING_BACKGROUND_MUSIC, MDR_FEATURE_LISTENING_CINEMA,
                                                   MDR_FEATURE_LISTENING_VOICE_BOOST,
                                                   MDR_FEATURE_LISTENING_SOUND_LEAKAGE_REDUCTION};
    if (listening->mode != MDR_LISTENING_STANDARD && !SupportsFeature(state, kModeFeatures[listening->mode]))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    // A mode the device disabled can reboot it once applied (#76).
    if (state.listeningStatusOf(listening->mode).available == MDR_FALSE)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    auto room = state.mBGMModeRoomSize.mDesired;
    if (listening->background_room != MDR_ROOM_UNKNOWN && !to_protocol(listening->background_room, room))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    if (listening->mode == MDR_LISTENING_BACKGROUND_MUSIC && listening->background_room == MDR_ROOM_UNKNOWN)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    state.mBGMModeEnabled.stage(listening->mode == MDR_LISTENING_BACKGROUND_MUSIC);
    state.mUpmixCinemaEnabled.stage(listening->mode == MDR_LISTENING_CINEMA);
    state.mVoiceContentsEnabled.stage(listening->mode == MDR_LISTENING_VOICE_BOOST);
    state.mSoundLeakageReductionEnabled.stage(listening->mode == MDR_LISTENING_SOUND_LEAKAGE_REDUCTION);
    state.mBGMModeRoomSize.stage(room);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetListeningStatus(MDRHeadphones* headphones, MDRListeningMode mode,
                                          MDRListeningStatus* outStatus)
{
    if (!headphones || !outStatus || mode > MDR_LISTENING_SOUND_LEAKAGE_REDUCTION)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (h.mProtocolFamily != Headphones::ProtocolFamily::V2)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    static constexpr MDRFeature kModeFeatures[] = {MDR_FEATURE_LISTENING_MODE, MDR_FEATURE_LISTENING_BACKGROUND_MUSIC,
                                                   MDR_FEATURE_LISTENING_CINEMA, MDR_FEATURE_LISTENING_VOICE_BOOST,
                                                   MDR_FEATURE_LISTENING_SOUND_LEAKAGE_REDUCTION};
    if (!SupportsFeature(h.mDetailsV2, kModeFeatures[mode]))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    *outStatus = h.mDetailsV2.listeningStatusOf(mode);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetEqualizer(MDRHeadphones* headphones, MDRPropertyType type, MDREqualizer* outEqualizer)
{
    if (!headphones || !ValidPropertyType(type) || !outEqualizer)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    return WithDetails(h,
                       [&](const auto& state) -> MDRResult
                       {
                           *outEqualizer = {
                               .preset = from_protocol(ReadProperty(state.mEqPresetId, type)),
                               .clear_bass = static_cast<int32_t>(ReadProperty(state.mEqClearBass, type)),
                               .band_count = static_cast<uint32_t>(ReadProperty(state.mEqConfig, type).size()),
                               .dsee_enabled = static_cast<MDRBoolean>(ReadProperty(state.mUpscalingEnabled, type)),
                               .dsee_type = from_protocol(state.mUpscalingType),
                               .available = static_cast<MDRBoolean>(state.mEqAvailable.mCurrent),
                               .dsee_available = static_cast<MDRBoolean>(state.mUpscalingAvailable)};
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesSetEqualizerPreset(MDRHeadphones* headphones, MDREqualizerPreset presetId)
{
    if (!headphones)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h.mInitialized && !h.IsReady())
        return MDR_RESULT_INPROGRESS;
    return WithDetails(h,
                       [&](auto& state) -> MDRResult
                       {
                           decltype(state.mEqPresetId.mDesired) preset{};
                           if (!to_protocol(presetId, preset))
                               return MDR_RESULT_ERROR_INVALID_ARGUMENT;
                           // Devices silently ignore presets they did not advertise.
                           if (!state.mEqPresets.empty() &&
                               std::none_of(state.mEqPresets.begin(), state.mEqPresets.end(),
                                            [&](const auto& entry) { return entry.presetId == preset; }))
                               return MDR_RESULT_ERROR_NOT_SUPPORTED;
                           state.mEqPresetId.stage(preset);
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesSetEqualizerClearBass(MDRHeadphones* headphones, int32_t clearBass)
{
    if (!headphones)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto& h = *Impl(headphones);
    if (!InRange(WithDetails(h, [](const auto& state) { return ClearBassRange(state); }), clearBass))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h.mInitialized && !h.IsReady())
        return MDR_RESULT_INPROGRESS;
    return WithDetails(h,
                       [&](auto& state) -> MDRResult
                       {
                           state.mEqClearBass.stage(static_cast<int>(clearBass));
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesSetDSEE(MDRHeadphones* headphones, MDRBoolean enabled)
{
    if (!headphones || !ValidBoolean(enabled))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h.mInitialized && !h.IsReady())
        return MDR_RESULT_INPROGRESS;
    return WithDetails(h,
                       [&](auto& state) -> MDRResult
                       {
                           state.mUpscalingEnabled.stage(enabled != MDR_FALSE);
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesGetEqualizerBands(MDRHeadphones* headphones, MDRPropertyType type, int32_t* bands,
                                         uint32_t* inoutCount)
{
    if (!headphones || !ValidPropertyType(type) || !inoutCount)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    return WithDetails(h,
                       [&](const auto& state) -> MDRResult
                       {
                           const auto& values = ReadProperty(state.mEqConfig, type);
                           const uint32_t required = static_cast<uint32_t>(values.size());
                           if (!bands)
                           {
                               if (*inoutCount != 0)
                                   return MDR_RESULT_ERROR_INVALID_ARGUMENT;
                               *inoutCount = required;
                               return MDR_RESULT_OK;
                           }
                           if (*inoutCount < required)
                           {
                               *inoutCount = required;
                               return MDR_RESULT_ERROR_BUFFER_TOO_SMALL;
                           }
                           for (uint32_t i = 0; i < required; ++i)
                               bands[i] = static_cast<int32_t>(values[i]);
                           *inoutCount = required;
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesSetEqualizerBands(MDRHeadphones* headphones, const int32_t* bands, uint32_t count)
{
    if (!headphones || !bands)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto& h = *Impl(headphones);
    ValueRange range{};
    if (!WithDetails(h, [&](const auto& state) { return EqualizerBandRange(state, count, range); }))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    mdr::Vector<int> values;
    values.reserve(count);
    for (uint32_t i = 0; i < count; ++i)
    {
        if (!InRange(range, bands[i]))
            return MDR_RESULT_ERROR_INVALID_ARGUMENT;
        values.push_back(static_cast<int>(bands[i]));
    }
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h.mInitialized && !h.IsReady())
        return MDR_RESULT_INPROGRESS;
    return WithDetails(h,
                       [&](auto& state) -> MDRResult
                       {
                           state.mEqConfig.stage(values);
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesGetEqualizerPresets(MDRHeadphones* headphones, MDREqualizerPreset* presets, uint32_t* inoutCount)
{
    if (!headphones || !inoutCount)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    return WithDetails(h,
                       [&](const auto& state) -> MDRResult
                       {
                           const uint32_t required = static_cast<uint32_t>(state.mEqPresets.size());
                           if (!presets)
                           {
                               if (*inoutCount != 0)
                                   return MDR_RESULT_ERROR_INVALID_ARGUMENT;
                               *inoutCount = required;
                               return MDR_RESULT_OK;
                           }
                           if (*inoutCount < required)
                           {
                               *inoutCount = required;
                               return MDR_RESULT_ERROR_BUFFER_TOO_SMALL;
                           }
                           for (uint32_t i = 0; i < required; ++i)
                               presets[i] = from_protocol(state.mEqPresets[i].presetId);
                           *inoutCount = required;
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesGetPairedDevices(MDRHeadphones* headphones, MDRPairedDevice* devices, uint32_t* inoutCount)
{
    if (!headphones || !inoutCount)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    return WithDetails(h,
                       [&](const auto& state) -> MDRResult
                       {
                           const uint32_t required = static_cast<uint32_t>(state.mPairedDevices.size());
                           if (!devices)
                           {
                               if (*inoutCount != 0)
                                   return MDR_RESULT_ERROR_INVALID_ARGUMENT;
                               *inoutCount = required;
                               return MDR_RESULT_OK;
                           }
                           if (*inoutCount < required)
                           {
                               *inoutCount = required;
                               return MDR_RESULT_ERROR_BUFFER_TOO_SMALL;
                           }
                           for (uint32_t i = 0; i < required; ++i)
                           {
                               const auto& dev = state.mPairedDevices[i];
                               devices[i] = {
                                   .connected = static_cast<MDRBoolean>(dev.connected),
                                   .playback_device = static_cast<MDRBoolean>(dev.playbackDevice),
                               };
                               const auto& mac = dev.macAddress;
                               const size_t macLen = mac.size() < 17 ? mac.size() : 17;
                               std::memcpy(devices[i].macAddress, mac.data(), macLen);
                               devices[i].macAddress[macLen] = '\0';
                               const auto& nm = dev.name;
                               const size_t nameLen = nm.size() < (sizeof(devices[i].name) - 1)
                                   ? nm.size()
                                   : (sizeof(devices[i].name) - 1);
                               std::memcpy(devices[i].name, nm.data(), nameLen);
                               devices[i].name[nameLen] = '\0';
                           }
                           *inoutCount = required;
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesSetPairedDevice(MDRHeadphones* headphones, const MDRPairedDeviceAction* action)
{
    if (!headphones || !action)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    std::string_view id;
    const MDRResult validation = ValidateDeviceId(*action, id);
    if (validation != MDR_RESULT_OK)
        return validation;
    auto* h = Impl(headphones);
    if (const auto failure = h->GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h->mInitialized && !h->IsReady())
        return MDR_RESULT_INPROGRESS;
    if (!SupportsPairing(*h))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    if (h->mProtocolFamily == Headphones::ProtocolFamily::V1)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    const mdr::String value{id.begin(), id.end()};
    switch (action->command)
    {
    case MDR_PAIRED_DEVICE_CONNECT:
        h->mDetailsV2.mPairedDeviceConnectMac.stage(value);
        break;
    case MDR_PAIRED_DEVICE_DISCONNECT:
        h->mDetailsV2.mPairedDeviceDisconnectMac.stage(value);
        break;
    case MDR_PAIRED_DEVICE_SELECT_PLAYBACK:
        h->mDetailsV2.mMultipointDeviceMac.stage(value);
        break;
    case MDR_PAIRED_DEVICE_UNPAIR:
        h->mDetailsV2.mPairedDeviceUnpairMac.stage(value);
        break;
    default:
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    }
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetPairing(MDRHeadphones* headphones, MDRPropertyType type, MDRPairing* outPairing)
{
    if (!headphones || !ValidPropertyType(type) || !outPairing)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    return WithDetails(h,
                       [&](const auto& state) -> MDRResult
                       {
                           *outPairing = {.enabled = static_cast<MDRBoolean>(ReadProperty(state.mPairingMode, type))};
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesSetPairing(MDRHeadphones* headphones, const MDRPairing* pairing)
{
    if (!headphones || !pairing || !ValidBoolean(pairing->enabled))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (const auto failure = h->GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h->mInitialized && !h->IsReady())
        return MDR_RESULT_INPROGRESS;
    if (!SupportsPairing(*h))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    return WithDetails(*h,
                       [&](auto& state) -> MDRResult
                       {
                           state.mPairingMode.stage(pairing->enabled != MDR_FALSE);
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesGetSourceSwitchControl(MDRHeadphones* headphones, MDRPropertyType type, MDRBoolean* outEnabled)
{
    if (!headphones || !ValidPropertyType(type) || !outEnabled)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto* h = Impl(headphones);
    if (const auto failure = h->GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (h->mProtocolFamily != Headphones::ProtocolFamily::V2)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    *outEnabled = static_cast<MDRBoolean>(ReadProperty(h->mDetailsV2.mSourceSwitchControlEnabled, type));
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetSourceSwitchControl(MDRHeadphones* headphones, MDRBoolean enabled)
{
    if (!headphones || !ValidBoolean(enabled))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (const auto failure = h->GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h->mInitialized && !h->IsReady())
        return MDR_RESULT_INPROGRESS;
    if (h->mProtocolFamily != Headphones::ProtocolFamily::V2 ||
        !SupportsFeature(h->mDetailsV2, MDR_FEATURE_SOURCE_SWITCH_CONTROL))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    h->mDetailsV2.mSourceSwitchControlResult = mdr::v2::t2::SourceSwitchControlResult::SUCCESS;
    h->mDetailsV2.mSourceSwitchControlEnabled.stage(enabled != MDR_FALSE);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetSourceSwitchControlResult(MDRHeadphones* headphones, MDRSourceSwitchControlResult* outResult)
{
    if (!headphones || !outResult)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto* h = Impl(headphones);
    if (const auto failure = h->GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (h->mProtocolFamily != Headphones::ProtocolFamily::V2)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    using enum mdr::v2::t2::SourceSwitchControlResult;
    switch (h->mDetailsV2.mSourceSwitchControlResult)
    {
    case SUCCESS:
        *outResult = MDR_SOURCE_SWITCH_CONTROL_SUCCESS;
        break;
    case FAIL_CALLING:
        *outResult = MDR_SOURCE_SWITCH_CONTROL_FAILED_ON_CALL;
        break;
    case FAIL_A2DP_NOT_CONNECT:
        *outResult = MDR_SOURCE_SWITCH_CONTROL_FAILED_NOT_CONNECTED;
        break;
    case FAIL_GIVE_PRIORITY_TO_VOICE_ASSISTANT:
        *outResult = MDR_SOURCE_SWITCH_CONTROL_FAILED_VOICE_ASSISTANT;
        break;
    default:
        *outResult = MDR_SOURCE_SWITCH_CONTROL_FAILED;
        break;
    }
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetGeneralSettingInfo(MDRHeadphones* headphones, MDRGeneralSettingInfo* settings,
                                             uint32_t* inoutCount)
{
    if (!headphones || !inoutCount)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    return WithDetails(h,
                       [&](const auto& state) -> MDRResult
                       {
                           uint32_t required = 0;
                           for (uint32_t i = 0; i < 4; ++i)
                               required += SupportsGeneralSetting(h, i) ? 1u : 0u;
                           if (!settings)
                           {
                               if (*inoutCount != 0)
                                   return MDR_RESULT_ERROR_INVALID_ARGUMENT;
                               *inoutCount = required;
                               return MDR_RESULT_OK;
                           }
                           if (*inoutCount < required)
                           {
                               *inoutCount = required;
                               return MDR_RESULT_ERROR_BUFFER_TOO_SMALL;
                           }
                           uint32_t out = 0;
                           for (uint32_t i = 0; i < std::size(state.mGsCapability); ++i)
                           {
                               if (!SupportsGeneralSetting(h, i))
                                   continue;
                               const auto& capability = state.mGsCapability[i];
                               const auto settingType = GeneralSettingType(capability);
                               settings[out++] = {
                                   .index = i,
                                   .type = settingType,
                                   .writable = static_cast<MDRBoolean>(settingType != MDR_GENERAL_SETTING_UNKNOWN),
                                   .option_count = settingType == MDR_GENERAL_SETTING_LIST
                                       ? static_cast<uint32_t>(capability.options.size())
                                       : 0u,
                                   .text_is_key = static_cast<MDRBoolean>(IsGeneralSettingTextKey(capability.value))};
                           }
                           *inoutCount = required;
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesGetGeneralSetting(MDRHeadphones* headphones, MDRPropertyType type, uint32_t index,
                                         MDRGeneralSetting* outSetting)
{
    if (!headphones || !ValidPropertyType(type) || !outSetting)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!SupportsGeneralSetting(h, index)) // This also checks index
        return MDR_RESULT_ERROR_NOT_FOUND;
    return WithDetails(
        h,
        [&](const auto& state) -> MDRResult
        {
            switch (GeneralSettingType(state.mGsCapability[index]))
            {
            case MDR_GENERAL_SETTING_BOOLEAN:
                *outSetting = {.index = index,
                               .boolean_value = static_cast<MDRBoolean>(ReadProperty(state.mGsParamBool[index], type)),
                               .list_value = 0};
                return MDR_RESULT_OK;
            case MDR_GENERAL_SETTING_LIST:
                *outSetting = {.index = index,
                               .boolean_value = MDR_FALSE,
                               .list_value =
                                   static_cast<uint32_t>(std::max(ReadProperty(state.mGsParamList[index], type), 0))};
                return MDR_RESULT_OK;
            default:
                return MDR_RESULT_ERROR_NOT_SUPPORTED;
            }
        });
}

MDRResult mdrHeadphonesSetGeneralSetting(MDRHeadphones* headphones, const MDRGeneralSetting* setting)
{
    if (!headphones || !setting || !ValidBoolean(setting->boolean_value))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h.mInitialized && !h.IsReady())
        return MDR_RESULT_INPROGRESS;
    if (!SupportsGeneralSetting(h, setting->index)) // This also checks index
        return MDR_RESULT_ERROR_NOT_FOUND;
    return WithDetails(h,
                       [&](auto& state) -> MDRResult
                       {
                           const auto& capability = state.mGsCapability[setting->index];
                           switch (GeneralSettingType(capability))
                           {
                           case MDR_GENERAL_SETTING_BOOLEAN:
                               state.mGsParamBool[setting->index].stage(setting->boolean_value != MDR_FALSE);
                               return MDR_RESULT_OK;
                           case MDR_GENERAL_SETTING_LIST:
                               if (setting->list_value >= capability.options.size())
                                   return MDR_RESULT_ERROR_INVALID_ARGUMENT;
                               state.mGsParamList[setting->index].stage(static_cast<int>(setting->list_value));
                               return MDR_RESULT_OK;
                           default:
                               return MDR_RESULT_ERROR_NOT_SUPPORTED;
                           }
                       });
}

MDRResult mdrHeadphonesGetAssignableControls(MDRHeadphones* headphones, MDRPropertyType type,
                                             MDRAssignableControl* outControls, uint32_t* inoutCount)
{
    if (!headphones || !ValidPropertyType(type) || !inoutCount)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!WithDetails(h, [](const auto& state) { return SupportsFeature(state, MDR_FEATURE_ASSIGNABLE_CONTROLS); }))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    return WithDetails(h,
                       [&](const auto& state) -> MDRResult
                       {
                           const auto& presets = ReadProperty(state.mAssignableSettingsPresets, type);
                           // Consistency check
                           if (state.mAssignableSettingsKeys.size() != presets.size())
                               return MDR_RESULT_ERROR_NOT_SUPPORTED;
                           const uint32_t required = static_cast<uint32_t>(presets.size());
                           if (!outControls)
                           {
                               if (*inoutCount != 0)
                                   return MDR_RESULT_ERROR_INVALID_ARGUMENT;
                               *inoutCount = required;
                               return MDR_RESULT_OK;
                           }
                           if (*inoutCount < required)
                           {
                               *inoutCount = required;
                               return MDR_RESULT_ERROR_BUFFER_TOO_SMALL;
                           }
                           for (uint32_t i = 0; i < required; ++i)
                           {
                               outControls[i] = {.location = from_protocol(state.mAssignableSettingsKeys[i].key),
                                                 .type = AssignableKeyType(state.mAssignableSettingsKeys[i]),
                                                 .action = from_protocol(presets[i])};
                           }
                           *inoutCount = required;
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesGetAssignableControlActions(MDRHeadphones* headphones, MDRAssignableActionKeyLocation key,
                                                   MDRAssignableAction* outOptions, uint32_t* inoutCount)
{
    if (!headphones || !inoutCount)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!WithDetails(h, [](const auto& state) { return SupportsFeature(state, MDR_FEATURE_ASSIGNABLE_CONTROLS); }))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    return WithDetails(h,
                       [&](const auto& state) -> MDRResult
                       {
                           auto it =
                               std::find_if(state.mAssignableSettingsKeys.begin(), state.mAssignableSettingsKeys.end(),
                                            [&](const auto& keyInfo) { return from_protocol(keyInfo.key) == key; });
                           if (it == state.mAssignableSettingsKeys.end())
                               return MDR_RESULT_ERROR_NOT_FOUND;
                           const auto& presets = AssignablePresets(*it);
                           const uint32_t required = static_cast<uint32_t>(presets.size());
                           if (!outOptions)
                           {
                               if (*inoutCount != 0)
                                   return MDR_RESULT_ERROR_INVALID_ARGUMENT;
                               *inoutCount = required;
                               return MDR_RESULT_OK;
                           }
                           if (*inoutCount < required)
                           {
                               *inoutCount = required;
                               return MDR_RESULT_ERROR_BUFFER_TOO_SMALL;
                           }
                           for (uint32_t i = 0; i < required; ++i)
                           {
                               outOptions[i] = from_protocol(presets.value[i].preset);
                           }
                           *inoutCount = required;
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesSetAssignableControls(MDRHeadphones* headphones, const MDRAssignableControl* controls,
                                             uint32_t count)
{
    if (!headphones || !controls || count == 0)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h.mInitialized && !h.IsReady())
        return MDR_RESULT_INPROGRESS;
    if (!WithDetails(h, [](const auto& state) { return SupportsFeature(state, MDR_FEATURE_ASSIGNABLE_CONTROLS); }))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    return WithDetails(
        h,
        [&](auto& state) -> MDRResult
        {
            // Sanity check
            if (state.mAssignableSettingsKeys.size() != state.mAssignableSettingsPresets.mCurrent.size())
                return MDR_RESULT_ERROR_NOT_SUPPORTED;
            // Check input length
            if (count != state.mAssignableSettingsKeys.size())
                return MDR_RESULT_ERROR_INVALID_ARGUMENT;
            decltype(state.mAssignableSettingsPresets.mCurrent) newPresets(count);
            for (uint32_t i = 0; i < count; ++i)
            {
                const auto& control = controls[i];
                if (control.location == MDR_ASSIGNABLE_ACTION_KEY_UNKNOWN ||
                    control.type == MDR_ASSIGNABLE_ACTION_KEY_TYPE_UNKNOWN)
                    return MDR_RESULT_ERROR_INVALID_ARGUMENT;
                if (std::find_if(controls, controls + i, [&](const auto& previous)
                                 { return previous.location == control.location; }) != controls + i)
                    return MDR_RESULT_ERROR_INVALID_ARGUMENT;
                const auto key =
                    std::find_if(state.mAssignableSettingsKeys.begin(), state.mAssignableSettingsKeys.end(),
                                 [&](const auto& keyInfo) { return from_protocol(keyInfo.key) == control.location; });
                if (key == state.mAssignableSettingsKeys.end() || AssignableKeyType(*key) != control.type)
                    return MDR_RESULT_ERROR_INVALID_ARGUMENT;
                auto& preset = newPresets[static_cast<size_t>(key - state.mAssignableSettingsKeys.begin())];
                if (!to_protocol(control.action, preset))
                    return MDR_RESULT_ERROR_INVALID_ARGUMENT;
                const auto& advertised = AssignablePresets(*key);
                if (std::find_if(advertised.begin(), advertised.end(),
                                 [&](const auto& option) { return option.preset == preset; }) == advertised.end())
                    return MDR_RESULT_ERROR_INVALID_ARGUMENT;
            }
            state.mAssignableSettingsPresets.stage(std::move(newPresets));
            return MDR_RESULT_OK;
        });
}

MDRResult mdrHeadphonesGetPower(MDRHeadphones* headphones, MDRPropertyType type, MDRPower* outPower)
{
    if (!headphones || !ValidPropertyType(type) || !outPower)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (h.mProtocolFamily == Headphones::ProtocolFamily::V1)
    {
        const auto& state = h.mDetailsV1;
        *outPower = {.auto_power_off_minutes = AutoPowerMinutes(ReadProperty(state.mPowerAutoOff, type)),
                     .wearing_power = MDR_WEARING_POWER_UNAVAILABLE,
                     .auto_pause = static_cast<MDRBoolean>(ReadProperty(state.mAutoPauseEnabled, type)),
                     .head_gesture = static_cast<MDRBoolean>(ReadProperty(state.mHeadGestureEnabled, type)),
                     .shutdown_requested = static_cast<MDRBoolean>(ReadProperty(state.mShutdown, type))};
        return MDR_RESULT_OK;
    }
    const auto& state = h.mDetailsV2;
    using T1 = mdr::v2::t1::FunctionType;
    const bool wearing = state.mSupport.contains(T1::AUTO_POWER_OFF_WITH_WEARING_DETECTION);
    const auto wearingValue = ReadProperty(state.mPowerAutoOffWearingDetection, type);
    *outPower = {.auto_power_off_minutes = wearing ? AutoPowerMinutes(wearingValue)
                                                   : AutoPowerMinutes(ReadProperty(state.mPowerAutoOff, type)),
                 .wearing_power = !wearing ? MDR_WEARING_POWER_UNAVAILABLE
                     : wearingValue ==
                         mdr::v2::t1::AutoPowerOffWearingDetectionElements::POWER_OFF_WHEN_REMOVED_FROM_EARS
                     ? MDR_WEARING_POWER_WHEN_REMOVED
                     : MDR_WEARING_POWER_DISABLED,
                 .auto_pause = static_cast<MDRBoolean>(ReadProperty(state.mAutoPauseEnabled, type)),
                 .head_gesture = static_cast<MDRBoolean>(ReadProperty(state.mHeadGestureEnabled, type)),
                 .shutdown_requested = static_cast<MDRBoolean>(ReadProperty(state.mShutdown, type))};
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetPower(MDRHeadphones* headphones, const MDRPower* power)
{
    if (!headphones || !power || power->wearing_power > MDR_WEARING_POWER_WHEN_REMOVED ||
        !ValidBoolean(power->auto_pause) || !ValidBoolean(power->head_gesture) ||
        !ValidBoolean(power->shutdown_requested))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (const auto failure = h->GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h->mInitialized && !h->IsReady())
        return MDR_RESULT_INPROGRESS;
    const auto autoPowerOptions = WithDetails(*h, [](const auto& state) { return AutoPowerOffOptions(state); });
    if (MDRPower current{};
        std::ranges::find(autoPowerOptions, power->auto_power_off_minutes) == autoPowerOptions.end() &&
        (mdrHeadphonesGetPower(headphones, MDR_PROPERTY_DESIRED, &current) != MDR_RESULT_OK ||
         current.auto_power_off_minutes != power->auto_power_off_minutes))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    if (h->mProtocolFamily == Headphones::ProtocolFamily::V1)
    {
        auto value = h->mDetailsV1.mPowerAutoOff.mDesired;
        if (!AutoPowerFromMinutes(power->auto_power_off_minutes, value) ||
            power->wearing_power == MDR_WEARING_POWER_WHEN_REMOVED)
            return MDR_RESULT_ERROR_INVALID_ARGUMENT;
        h->mDetailsV1.mPowerAutoOff.stage(value);
        h->mDetailsV1.mAutoPauseEnabled.stage(power->auto_pause != MDR_FALSE);
        h->mDetailsV1.mHeadGestureEnabled.stage(power->head_gesture != MDR_FALSE);
        h->mDetailsV1.mShutdown.stage(power->shutdown_requested != MDR_FALSE);
        return MDR_RESULT_OK;
    }
    auto& state = h->mDetailsV2;
    mdr::v2::t1::AutoPowerOffElements autoPower{};
    mdr::v2::t1::AutoPowerOffWearingDetectionElements wearingPower{};
    if (!AutoPowerFromMinutes(power->auto_power_off_minutes, autoPower) ||
        !AutoPowerFromMinutes(power->auto_power_off_minutes, wearingPower))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    using T1 = mdr::v2::t1::FunctionType;
    if (state.mSupport.contains(T1::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
    {
        state.mPowerAutoOffWearingDetection.stage(
            power->wearing_power == MDR_WEARING_POWER_WHEN_REMOVED
                ? mdr::v2::t1::AutoPowerOffWearingDetectionElements::POWER_OFF_WHEN_REMOVED_FROM_EARS
                : wearingPower);
    }
    else if (state.mSupport.contains(T1::AUTO_POWER_OFF) || SupportsFeature(state, MDR_FEATURE_AUTO_POWER_OFF))
        state.mPowerAutoOff.stage(autoPower);
    state.mAutoPauseEnabled.stage(power->auto_pause != MDR_FALSE);
    state.mHeadGestureEnabled.stage(power->head_gesture != MDR_FALSE);
    state.mShutdown.stage(power->shutdown_requested != MDR_FALSE);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetVoiceGuidance(MDRHeadphones* headphones, MDRPropertyType type,
                                        MDRVoiceGuidance* outVoiceGuidance)
{
    if (!headphones || !ValidPropertyType(type) || !outVoiceGuidance)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    return WithDetails(h,
                       [&](const auto& state) -> MDRResult
                       {
                           *outVoiceGuidance = {
                               .enabled = static_cast<MDRBoolean>(ReadProperty(state.mVoiceGuidanceEnabled, type)),
                               .volume = static_cast<int32_t>(ReadProperty(state.mVoiceGuidanceVolume, type))};
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesSetVoiceGuidance(MDRHeadphones* headphones, const MDRVoiceGuidance* voiceGuidance)
{
    if (!headphones || !voiceGuidance || !ValidBoolean(voiceGuidance->enabled))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (!InRange(WithDetails(*h, [](const auto& state) { return VoiceGuidanceVolumeRange(state); }),
                 voiceGuidance->volume))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    if (const auto failure = h->GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h->mInitialized && !h->IsReady())
        return MDR_RESULT_INPROGRESS;
    if (!SupportsVoiceGuidance(*h))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    return WithDetails(*h,
                       [&](auto& state) -> MDRResult
                       {
                           state.mVoiceGuidanceEnabled.stage(voiceGuidance->enabled != MDR_FALSE);
                           if (SupportsFeature(state, MDR_FEATURE_VOICE_GUIDANCE_VOLUME))
                               state.mVoiceGuidanceVolume.stage(static_cast<int>(voiceGuidance->volume));
                           return MDR_RESULT_OK;
                       });
}

MDRResult mdrHeadphonesGetVoiceAssistant(MDRHeadphones* headphones, MDRPropertyType type,
                                         MDRVoiceAssistantSettings* outSettings)
{
    if (!headphones || !ValidPropertyType(type) || !outSettings)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (h.mProtocolFamily != Headphones::ProtocolFamily::V2)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    const auto& state = h.mDetailsV2;
    *outSettings = {.assistant = from_protocol(ReadProperty(state.mVoiceAssistant, type)),
                    .available = state.mVoiceAssistantAvailable ? MDR_TRUE : MDR_FALSE,
                    .key = from_protocol(state.mVoiceAssistantKeyType)};
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetVoiceAssistantOptions(MDRHeadphones* headphones, MDRVoiceAssistantOption* options,
                                                uint32_t* inoutCount)
{
    if (!headphones || !inoutCount)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (h.mProtocolFamily != Headphones::ProtocolFamily::V2 ||
        !SupportsFeature(h.mDetailsV2, MDR_FEATURE_VOICE_ASSISTANT))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    const auto& state = h.mDetailsV2;
    uint32_t required = 0;
    state.forEachVoiceAssistantOption([&](auto) { ++required; });
    if (!options)
    {
        if (*inoutCount != 0)
            return MDR_RESULT_ERROR_INVALID_ARGUMENT;
        *inoutCount = required;
        return MDR_RESULT_OK;
    }
    if (*inoutCount < required)
    {
        *inoutCount = required;
        return MDR_RESULT_ERROR_BUFFER_TOO_SMALL;
    }
    uint32_t out = 0;
    state.forEachVoiceAssistantOption(
        [&](auto assistant)
        {
            options[out++] = {.assistant = from_protocol(assistant),
                              .available = state.voiceAssistantExcluded(assistant) ? MDR_FALSE : MDR_TRUE};
        });
    *inoutCount = required;
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetVoiceAssistant(MDRHeadphones* headphones, MDRVoiceAssistant assistant)
{
    mdr::v2::t1::VoiceAssistant value{};
    if (!headphones || !to_protocol(assistant, value))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (const auto failure = h->GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h->mInitialized && !h->IsReady())
        return MDR_RESULT_INPROGRESS;
    if (h->mProtocolFamily != Headphones::ProtocolFamily::V2 ||
        !SupportsFeature(h->mDetailsV2, MDR_FEATURE_VOICE_ASSISTANT))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    auto& state = h->mDetailsV2;
    bool offered = false;
    state.forEachVoiceAssistantOption([&](auto option) { offered |= option == value; });
    if (!offered || !state.mVoiceAssistantAvailable || state.voiceAssistantExcluded(value))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    state.mVoiceAssistant.stage(value);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetConnectionMode(MDRHeadphones* headphones, MDRPropertyType type, MDRConnectionMode* outMode)
{
    if (!headphones || !ValidPropertyType(type) || !outMode)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (h.mProtocolFamily == Headphones::ProtocolFamily::V1)
    {
        const auto value = ReadProperty(h.mDetailsV1.mAudioPriorityMode, type);
        outMode->audio_priority = value == mdr::v1::t1::ConnectionModeSettingValue::SOUND_QUALITY_PRIOR
            ? MDR_AUDIO_PRIORITY_QUALITY
            : MDR_AUDIO_PRIORITY_STABILITY;
    }
    else
    {
        const auto value = ReadProperty(h.mDetailsV2.mAudioPriorityMode, type);
        outMode->audio_priority = value == mdr::v2::t1::PriorMode::SOUND_QUALITY_PRIOR ? MDR_AUDIO_PRIORITY_QUALITY
            : value == mdr::v2::t1::PriorMode::CONNECTION_QUALITY_PRIOR                ? MDR_AUDIO_PRIORITY_STABILITY
                                                                                       : MDR_AUDIO_PRIORITY_UNKNOWN;
    }
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetConnectionMode(MDRHeadphones* headphones, const MDRConnectionMode* mode)
{
    if (!headphones || !mode)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (const auto failure = h->GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h->mInitialized && !h->IsReady())
        return MDR_RESULT_INPROGRESS;
    if (!WithDetails(*h, [](const auto& state) { return SupportsFeature(state, MDR_FEATURE_CONNECTION_MODE); }))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    if (h->mProtocolFamily == Headphones::ProtocolFamily::V1)
    {
        switch (mode->audio_priority)
        {
        case MDR_AUDIO_PRIORITY_QUALITY:
            h->mDetailsV1.mAudioPriorityMode.stage(mdr::v1::t1::ConnectionModeSettingValue::SOUND_QUALITY_PRIOR);
            return MDR_RESULT_OK;
        case MDR_AUDIO_PRIORITY_STABILITY:
            h->mDetailsV1.mAudioPriorityMode.stage(mdr::v1::t1::ConnectionModeSettingValue::CONNECTION_QUALITY_PRIOR);
            return MDR_RESULT_OK;
        default:
            return MDR_RESULT_ERROR_INVALID_ARGUMENT;
        }
    }
    switch (mode->audio_priority)
    {
    case MDR_AUDIO_PRIORITY_QUALITY:
        h->mDetailsV2.mAudioPriorityMode.stage(mdr::v2::t1::PriorMode::SOUND_QUALITY_PRIOR);
        break;
    case MDR_AUDIO_PRIORITY_STABILITY:
        h->mDetailsV2.mAudioPriorityMode.stage(mdr::v2::t1::PriorMode::CONNECTION_QUALITY_PRIOR);
        break;
    default:
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    }
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetSafeListening(MDRHeadphones* headphones, MDRPropertyType type,
                                        MDRSafeListening* outSafeListening)
{
    if (!headphones || !ValidPropertyType(type) || !outSafeListening)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (h.mProtocolFamily != Headphones::ProtocolFamily::V2)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    const auto& state = h.mDetailsV2;
    *outSafeListening = {.sound_pressure = static_cast<uint32_t>(std::clamp(state.mSafeListeningSoundPressure, 0, 255)),
                         .preview = static_cast<MDRBoolean>(ReadProperty(state.mSafeListeningPreviewMode, type))};
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetSafeListening(MDRHeadphones* headphones, const MDRSafeListening* safeListening)
{
    if (!headphones || !safeListening || !ValidBoolean(safeListening->preview) || safeListening->sound_pressure > 255u)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (const auto failure = h->GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (!h->mInitialized && !h->IsReady())
        return MDR_RESULT_INPROGRESS;
    if (h->mProtocolFamily != Headphones::ProtocolFamily::V2 || !SupportsSafeListening(*h))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    auto& state = h->mDetailsV2;
    if (safeListening->sound_pressure != static_cast<uint32_t>(std::clamp(state.mSafeListeningSoundPressure, 0, 255)))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    state.mSafeListeningPreviewMode.stage(safeListening->preview != MDR_FALSE);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetWearingStatus(MDRHeadphones* headphones, MDRWearingStatus* outStatus)
{
    if (!headphones || !outStatus)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (const auto failure = h.GetFailure(); failure != MDR_RESULT_OK)
        return failure;
    if (h.mProtocolFamily != Headphones::ProtocolFamily::V2)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    using Code = mdr::v2::t2::WearingStatusCode;
    switch (h.mDetailsV2.mWearingStatus)
    {
    case Code::NORMAL:
        *outStatus = MDR_WEARING_STATUS_WORN;
        break;
    case Code::LEFT_SIDE_NOT_WEAR:
        *outStatus = MDR_WEARING_STATUS_LEFT_REMOVED;
        break;
    case Code::RIGHT_SIDE_NOT_WEAR:
        *outStatus = MDR_WEARING_STATUS_RIGHT_REMOVED;
        break;
    case Code::BOTH_NOT_WEAR:
        *outStatus = MDR_WEARING_STATUS_REMOVED;
        break;
    default:
        *outStatus = MDR_WEARING_STATUS_UNKNOWN;
        break;
    }
    return MDR_RESULT_OK;
}
}
#pragma endregion
