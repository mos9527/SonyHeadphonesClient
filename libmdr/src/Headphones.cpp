// ReSharper disable CppParameterMayBeConstPtrOrRef
#include <ranges>
#include <fmt/base.h>
#include <algorithm>
#include <cstring>
#include <limits>
#include <string_view>
#include <mdr/ProtocolV1T1.hpp>
#include "Details.hpp"

namespace mdr
{
    MDRHeadphones::Awaiter& MDRHeadphones::Await(AwaitType type)
    {
        return mAwaiters[type];
    }

    void MDRHeadphones::Awake(AwaitType type)
    {
        if (auto& await = mAwaiters[type])
            await.resume_now(MDR_RESULT_OK);
    }

    void MDRHeadphones::RefreshNeutralFeaturesV2()
    {
        using T1 = v2::t1::FunctionType;
        using T2 = v2::t2::FunctionType;
        auto& features = mSupport.neutralFeatures;
        std::ranges::fill(features, false);
        features[MDR_FEATURE_IDENTITY] = true;
        features[MDR_FEATURE_BATTERY_SINGLE] =
            mSupport.contains(T1::BATTERY_LEVEL_INDICATOR) ||
            mSupport.contains(T1::BATTERY_LEVEL_WITH_THRESHOLD);
        features[MDR_FEATURE_BATTERY_LEFT_RIGHT] =
            mSupport.contains(T1::LEFT_RIGHT_BATTERY_LEVEL_INDICATOR) ||
            mSupport.contains(T1::LR_BATTERY_LEVEL_WITH_THRESHOLD);
        features[MDR_FEATURE_BATTERY_CASE] =
            mSupport.contains(T1::CRADLE_BATTERY_LEVEL_INDICATOR) ||
            mSupport.contains(T1::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD);
        const bool playback =
            mSupport.contains(T1::PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT) ||
            mSupport.contains(T1::PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT_AND_MUTE) ||
            mSupport.contains(T1::PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE) ||
            mSupport.contains(T1::PLAYBACK_CONTROLLER_WITH_FUNCTION_CHANGE);
        features[MDR_FEATURE_PLAYBACK_METADATA] = playback;
        features[MDR_FEATURE_PLAYBACK_CONTROL] = playback;
        features[MDR_FEATURE_PLAYBACK_VOLUME] = playback;
        const bool noise =
            mSupport.contains(T1::NOISE_CANCELLING_ONOFF) ||
            mSupport.contains(T1::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_ONOFF) ||
            mSupport.contains(T1::NOISE_CANCELLING_DUAL_SINGLE_OFF_AND_AMBIENT_SOUND_MODE_ONOFF) ||
            mSupport.contains(T1::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT) ||
            mSupport.contains(T1::NOISE_CANCELLING_DUAL_SINGLE_OFF_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT) ||
            mSupport.contains(T1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AUTO_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT) ||
            mSupport.contains(T1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_SINGLE_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT) ||
            mSupport.contains(T1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT) ||
            mSupport.contains(
                T1::MODE_NC_NCSS_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_WITH_TEST_MODE) ||
            mSupport.contains(
                T1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION);
        features[MDR_FEATURE_NOISE_CANCELLING] = noise;
        features[MDR_FEATURE_AMBIENT_SOUND] = noise ||
            mSupport.contains(T1::AMBIENT_SOUND_MODE_ONOFF) ||
            mSupport.contains(T1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT);
        features[MDR_FEATURE_ADAPTIVE_AMBIENT_SOUND] = mSupport.contains(
            T1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION);
        features[MDR_FEATURE_SPEAK_TO_CHAT] = mSupport.contains(T1::SMART_TALKING_MODE_TYPE2);
        features[MDR_FEATURE_LISTENING_MODE] = mSupport.contains(T1::LISTENING_OPTION);
        features[MDR_FEATURE_EQUALIZER] =
            mSupport.contains(T1::PRESET_EQ) || mSupport.contains(T1::CUSTOM_EQ) ||
            mSupport.contains(T1::PRESET_EQ_NON_CUSTOMIZABLE) ||
            mSupport.contains(T1::PRESET_EQ_AND_ULT_MODE) ||
            mSupport.contains(T1::SOUND_EFFECT) || mSupport.contains(T1::TURN_KEY_EQ) ||
            mSupport.contains(T1::PRESET_EQ_AND_ERRORCODE) ||
            mSupport.contains(T1::CUSTOMIZABLE_SOUND_EFFECT);
        features[MDR_FEATURE_DSEE] = mSupport.contains(T1::UPSCALING_AUTO_OFF);
        const bool pairing =
            mSupport.contains(T2::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT) ||
            mSupport.contains(T2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT) ||
            mSupport.contains(T2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE);
        features[MDR_FEATURE_PAIRED_DEVICE_MANAGEMENT] = pairing;
        features[MDR_FEATURE_PAIRING_MODE] = pairing;
        features[MDR_FEATURE_SOURCE_SWITCH_CONTROL] = mSupport.contains(T2::SOURCE_SWITCH_CONTROL);
        features[MDR_FEATURE_GENERAL_SETTINGS] =
            mSupport.contains(T1::GENERAL_SETTING_1) || mSupport.contains(T1::GENERAL_SETTING_2) ||
            mSupport.contains(T1::GENERAL_SETTING_3) || mSupport.contains(T1::GENERAL_SETTING_4);
        features[MDR_FEATURE_ASSIGNABLE_CONTROLS] = mSupport.contains(T1::ASSIGNABLE_SETTING);
        features[MDR_FEATURE_NOISE_CONTROL_BUTTON] =
            mSupport.contains(T1::AMBIENT_SOUND_CONTROL_MODE_SELECT);
        features[MDR_FEATURE_AUTO_POWER_OFF] =
            mSupport.contains(T1::AUTO_POWER_OFF) ||
            mSupport.contains(T1::AUTO_POWER_OFF_WITH_WEARING_DETECTION);
        features[MDR_FEATURE_WEARING_DETECTION] =
            mSupport.contains(T1::AUTO_POWER_OFF_WITH_WEARING_DETECTION) ||
            mSupport.contains(T1::WEARING_STATUS_DETECTOR);
        features[MDR_FEATURE_AUTO_PAUSE] =
            mSupport.contains(T1::PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF);
        features[MDR_FEATURE_HEAD_GESTURE] =
            mSupport.contains(T1::HEAD_GESTURE_ON_OFF_TRAINING);
        const bool voiceGuidance =
            mSupport.contains(T2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH) ||
            mSupport.contains(T2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH) ||
            mSupport.contains(
                T2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT) ||
            mSupport.contains(T2::VOICE_GUIDANCE_VOLUME_SETTING_MTK_FIXED_TO_5_STEPS) ||
            mSupport.contains(T2::VOICE_GUIDANCE_SETTING_SUPPORT_LANGUAGE_SWITCH) ||
            mSupport.contains(T2::VOICE_GUIDANCE_SETTING_ONLY_ON_OFF_SWITCH);
        features[MDR_FEATURE_VOICE_GUIDANCE] = voiceGuidance;
        features[MDR_FEATURE_VOICE_GUIDANCE_VOLUME] =
            mSupport.contains(
                T2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT) ||
            mSupport.contains(T2::VOICE_GUIDANCE_VOLUME_SETTING_MTK_FIXED_TO_5_STEPS);
        features[MDR_FEATURE_SHUTDOWN] = mSupport.contains(T1::POWER_OFF);
        features[MDR_FEATURE_CONNECTION_MODE] =
            mSupport.contains(T1::CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY);
        features[MDR_FEATURE_SAFE_LISTENING] =
            mSupport.contains(T2::SAFE_LISTENING_HBS_1) ||
            mSupport.contains(T2::SAFE_LISTENING_HBS_2) ||
            mSupport.contains(T2::SAFE_LISTENING_TWS_1) ||
            mSupport.contains(T2::SAFE_LISTENING_TWS_2);
        mSupport.provenance = SupportStates::Provenance::ADVERTISED;
    }

    MDRTask MDRHeadphones::RequestInit()
    {
        mProtocol = {};

        SendCommandACK(v2::t1::ConnectGetProtocolInfo);
        const int result = co_await Await(AWAIT_PROTOCOL_INFO);
        if (result != MDR_RESULT_OK)
            co_return SetLastError(result, "Unable to initialize MDR");

        switch (mProtocolFamily)
        {
        case ProtocolFamily::V1:
            co_return co_await RequestInitV1();
        case ProtocolFamily::V2:
            co_return co_await RequestInitV2Selected();
        default:
            co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "MDR protocol family has not been selected");
        }
    }

    MDRTask MDRHeadphones::RequestSync()
    {
        switch (mProtocolFamily)
        {
        case ProtocolFamily::V1:
            co_return co_await RequestSyncV1();
        case ProtocolFamily::V2:
            co_return co_await RequestSyncV2();
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

    MDRTask MDRHeadphones::RequestDebugCommand(
        MDRBuffer payload,
        MDRDataType type,
        MDRCommandSeqNumber sequence,
        bool awaitAck
    )
    {
        SendCommandImpl(payload, type, sequence);
        if (awaitAck)
        {
            const int result = co_await Await(AWAIT_ACK);
            if (result != MDR_RESULT_OK)
                co_return SetLastError(result, "Debugger command did not receive an ACK");
        }
        co_return MDR_EVENT_UNHANDLED;
    }

    ::MDRResult MDRHeadphones::PollEvents(MDREvent& outEvent)
    {
        outEvent = MDR_EVENT_NONE;
        int r = mdrConnectionPoll(mConn, 0);
        if (r == MDR_RESULT_OK)
        {
            // Non-blocking. INPROGRESS are expected, not so much for others.
            // Failfast if that happens - the owner usually has to die.
            r = Send();
            if (r != MDR_RESULT_OK && r != MDR_RESULT_INPROGRESS)
                return Fail(r, "Unable to send to the device");
            r = Receive();
            if (r != MDR_RESULT_OK && r != MDR_RESULT_INPROGRESS)
                return Fail(r, "Unable to receive from the device");
        }
        else
        {
            if (r != MDR_RESULT_ERROR_TIMEOUT)
                return Fail(r, "Unable to poll the connection");
        }
        // Anything that failed deeper in - a handler, a running task - only reaches us as the
        // channel's marker, so the code it recorded on the way out is all we have to go on.
        const int raw = MoveNext();
        if (raw == -1)
            return mLastErrorCode;
        outEvent = static_cast<MDREvent>(raw);
        return MDR_RESULT_OK;
    }

    bool MDRHeadphones::IsReady() const
    {
        return !mTask;
    }

    bool MDRHeadphones::IsDirty() const
    {
        bool dirty = mShutdown.dirty() || mNcAsmEnabled.dirty() || mNcAsmFocusOnVoice.dirty() || mNcAsmAmbientLevel.
            dirty();
        dirty |= mNcAsmButtonFunction.dirty() || mNcAsmMode.dirty() || mNcAsmAutoAsmEnabled.dirty();
        dirty |= mNcAsmNoiseAdaptiveSensitivity.dirty() || mPowerAutoOff.dirty() || mPlayControl.dirty();
        dirty |= mPowerAutoOffWearingDetection.dirty() || mPlayVolume.dirty() || mGsParamBool1.dirty();
        dirty |= mGsParamBool2.dirty() || mGsParamBool3.dirty() || mGsParamBool4.dirty();
        dirty |= mUpscalingEnabled.dirty();
        dirty |= mAudioPriorityMode.dirty() || mBGMModeEnabled.dirty() || mBGMModeRoomSize.dirty();
        dirty |= mUpmixCinemaEnabled.dirty() || mAutoPauseEnabled.dirty() || mTouchFunctionLeft.dirty();
        dirty |= mTouchFunctionRight.dirty() || mSpeakToChatEnabled.dirty() || mSpeakToChatDetectSensitivity.dirty();
        dirty |= mSpeakToModeOutTime.dirty() || mHeadGestureEnabled.dirty() || mEqAvailable.dirty();
        dirty |= mEqPresetId.dirty() || mEqClearBass.dirty() || mEqConfig.dirty();
        dirty |= mVoiceGuidanceEnabled.dirty() || mVoiceGuidanceVolume.dirty() || mPairingMode.dirty();
        dirty |= mMultipointDeviceMac.dirty() || mSafeListeningPreviewMode.dirty();
        dirty |= mSourceSwitchControlEnabled.dirty();
        dirty |= mPairedDeviceConnectMac.dirty() || mPairedDeviceDisconnectMac.dirty() || mPairedDeviceUnpairMac.
            dirty();
        return dirty;
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
            dump += mdr::Format("{:X} ", static_cast<UInt8>(*p));
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
        mdr::String dump = "<< ";
        for (char* p = buf; p != buf + sent; p++)
            dump += mdr::Format("{:X} ", static_cast<UInt8>(*p));
        MDR_LOG("{}", dump);
#endif
        mSendBuf.erase(mSendBuf.begin(), mSendBuf.begin() + sent);
        return r;
    }


    int MDRHeadphones::HandleProtocolInfo(Span<const UInt8> command)
    {
        // The protocol family is fixed at construction time; here we only parse the
        // advertised version and table support, validating the reply matches it.
        switch (mProtocolFamily)
        {
        case ProtocolFamily::V1:
            if (command.size() != sizeof(v1::t1::RetProtocolInfo))
                return SetLastError(
                    MDR_RESULT_ERROR_MALFORMED_PAYLOAD,
                    "MDR V1 protocol info size is not valid");
            {
                const auto result = (v1::t1::RetProtocolInfo::Deserialize)(command.data(), command.size());
                if (!result)
                    return SetLastError(
                        result.error,
                        result.errMessage ? result.errMessage : "Unable to deserialize MDR V1 protocol info");
                mProtocol = {
                    .version = result.value.protocolVersion,
                    .hasTable1 = true,
                    .hasTable2 = false
                };
            }
            break;
        case ProtocolFamily::V2:
            if (command.size() != sizeof(v2::t1::ConnectRetProtocolInfo))
                return SetLastError(
                    MDR_RESULT_ERROR_MALFORMED_PAYLOAD,
                    "MDR V2 protocol info size is not valid");
            {
                const auto result = (v2::t1::ConnectRetProtocolInfo::Deserialize)(command.data(), command.size());
                if (!result)
                    return SetLastError(
                        result.error,
                        result.errMessage ? result.errMessage : "Unable to deserialize MDR V2 protocol info");
                mProtocol = {
                    .version = result.value.protocolVersion,
                    .hasTable1 = result.value.supportTable1Value == v2::EnableDisable::ENABLE,
                    .hasTable2 = result.value.supportTable2Value == v2::EnableDisable::ENABLE
                };
            }
            break;
        default:
            return SetLastError(
                MDR_RESULT_ERROR_NOT_SUPPORTED,
                "MDR protocol family is not supported");
        }

        Awake(AWAIT_PROTOCOL_INFO);
        return MDR_EVENT_IDENTITY_CHANGED;
    }

    int MDRHeadphones::Handle(Span<const UInt8> command, MDRDataType type, MDRCommandSeqNumber seq)
    {
        using enum MDRDataType;
        mSeqNumber = seq;
        switch (type)
        {
        case ACK:
            HandleAck(seq);
            break;
        case DATA_MDR:
            SendACK(seq);
            if (!command.empty() &&
                command.front() == static_cast<UInt8>(v2::t1::Command::CONNECT_RET_PROTOCOL_INFO))
                return HandleProtocolInfo(command);
            if (mProtocolFamily == ProtocolFamily::UNKNOWN)
                return SetLastError(
                    MDR_RESULT_ERROR_MALFORMED_PAYLOAD,
                    "Received MDR Table 1 data before CONNECT_RET_PROTOCOL_INFO");
            switch (mProtocolFamily)
            {
            case ProtocolFamily::V1: return HandleCommandV1T1(command, seq);
            case ProtocolFamily::V2: return HandleCommandV2T1(command, seq);
            default: return MDR_EVENT_UNHANDLED;
            }
        case DATA_MDR_NO2:
            SendACK(seq);
            if (mProtocolFamily == ProtocolFamily::UNKNOWN)
                return SetLastError(
                    MDR_RESULT_ERROR_MALFORMED_PAYLOAD,
                    "Received MDR Table 2 data before CONNECT_RET_PROTOCOL_INFO");
            switch (mProtocolFamily)
            {
            case ProtocolFamily::V1: return HandleCommandV1T2(command, seq);
            case ProtocolFamily::V2: return HandleCommandV2T2(command, seq);
            default: return MDR_EVENT_UNHANDLED;
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
            using namespace std::literals;
            time_t now = time(nullptr);
            for (auto& awaiter : mAwaiters)
            {
                if (!awaiter) continue;
                auto duration = now - awaiter.tick;
                if (duration > kAwaitTimeout)
                    awaiter.resume_now(MDR_RESULT_ERROR_TIMEOUT);
            }
            int taskResult;
            if (TaskMoveNext(taskResult))
                return taskResult;
        }
        const int idleCode = MDR_EVENT_NONE;
        if (mRecvBuf.empty())
            return idleCode;
        auto commandBegin = std::ranges::find(mRecvBuf, kStartMarker);
        auto commandEnd = std::ranges::find(commandBegin, mRecvBuf.end(), kEndMarker);
        if (commandBegin == mRecvBuf.end() || commandEnd == mRecvBuf.end())
            return idleCode; // Incomplete
        MDRBuffer packedCommand{commandBegin, commandEnd + 1};
        MDRBuffer command;
        MDRDataType type;
        MDRCommandSeqNumber seqNum;
        if (mPacketCallback)
        {
            mPacketCallback(
                mPacketCallbackUserData,
                MDR_PACKET_DIRECTION_RX,
                packedCommand.data(),
                static_cast<int>(packedCommand.size())
            );
        }
        switch (MDRUnpackCommand(packedCommand, command, type, seqNum))
        {
        case MDRUnpackResult::OK:
            mRecvBuf.erase(mRecvBuf.begin(), commandEnd + 1);
            return Handle(command, type, seqNum);
        case MDRUnpackResult::INCOMPLETE:
            // Incomplete. Nop.
            break;
        case MDRUnpackResult::BAD_MARKER: [[unlikely]]
            // FIXME Consider chunked transport.
            // This can happen and should not be handled this way. See also @ref SendCommandACK
        case MDRUnpackResult::BAD_CHECKSUM: [[unlikely]]
        case MDRUnpackResult::BAD_OTHER: [[unlikely]]
            // Unlikely. What we have now makes no sense yet markers are intact.
            MDR_LOG("FIXME-MDR packet malformed, discarding {} bytes", std::distance(mRecvBuf.begin(), commandEnd));
            mRecvBuf.erase(mRecvBuf.begin(), commandEnd);
            break;
        }
        return idleCode;
    }

    int MDRHeadphones::Invoke(MDRTask&& task)
    {
        if (mTask)
            return MDR_RESULT_INPROGRESS;
        mTask = std::move(task);
        mTask.coroutine.resume();
        return MDR_RESULT_OK;
    }

    bool MDRHeadphones::TaskMoveNext(int& result)
    {
        if (!mTask || !mTask.coroutine.done())
            return false;
        result = mTask.coroutine.promise().result;
        mTask = {};
        return true;
    }

    void MDRHeadphones::SendCommandImpl(Span<const UInt8> command, MDRDataType type, MDRCommandSeqNumber seq)
    {
        MDRBuffer packed = MDRPackCommand(type, seq, command);
        if (mPacketCallback)
        {
            mPacketCallback(
                mPacketCallbackUserData,
                MDR_PACKET_DIRECTION_TX,
                packed.data(),
                static_cast<int>(packed.size())
            );
        }
        mSendBuf.insert(mSendBuf.end(), packed.begin(), packed.end());
    }

    void MDRHeadphones::SendACK(MDRCommandSeqNumber seq)
    {
        SendCommandImpl({}, MDRDataType::ACK, 1 - seq);
    }

    void MDRHeadphones::HandleAck(MDRCommandSeqNumber)
    {
        Awake(AWAIT_ACK);
    }
}

#pragma region C Exports
namespace
{
    using Headphones = mdr::MDRHeadphones;

    Headphones* Impl(MDRHeadphones* headphones)
    {
        return mdr::detail::HeadphonesImpl(headphones);
    }

    const Headphones* Impl(const MDRHeadphones* headphones)
    {
        return mdr::detail::HeadphonesImpl(headphones);
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
        return h.mSupport.contains(MDR_FEATURE_PAIRED_DEVICE_MANAGEMENT);
    }

    bool SupportsSafeListening(const Headphones& h)
    {
        return h.mSupport.contains(MDR_FEATURE_SAFE_LISTENING);
    }

    bool SupportsVoiceGuidance(const Headphones& h)
    {
        return h.mSupport.contains(MDR_FEATURE_VOICE_GUIDANCE);
    }

    bool SupportsGeneralSetting(const Headphones& h, uint32_t index)
    {
        switch (index)
        {
        case 0:
            return h.mSupport.contains(mdr::v1::t1::FunctionType::GENERAL_SETTING1) ||
                h.mSupport.contains(mdr::v2::t1::FunctionType::GENERAL_SETTING_1);
        case 1:
            return h.mSupport.contains(mdr::v1::t1::FunctionType::GENERAL_SETTING2) ||
                h.mSupport.contains(mdr::v2::t1::FunctionType::GENERAL_SETTING_2);
        case 2:
            return h.mSupport.contains(mdr::v1::t1::FunctionType::GENERAL_SETTING3) ||
                h.mSupport.contains(mdr::v2::t1::FunctionType::GENERAL_SETTING_3);
        case 3:
            return h.mSupport.contains(mdr::v2::t1::FunctionType::GENERAL_SETTING_4);
        default: return false;
        }
    }

    MDRAudioCodec ToNeutral(mdr::v2::t1::AudioCodec value)
    {
        using enum mdr::v2::t1::AudioCodec;
        switch (value)
        {
        case SBC: return MDR_AUDIO_CODEC_SBC;
        case AAC: return MDR_AUDIO_CODEC_AAC;
        case LDAC: return MDR_AUDIO_CODEC_LDAC;
        case APT_X: return MDR_AUDIO_CODEC_APTX;
        case APT_X_HD: return MDR_AUDIO_CODEC_APTX_HD;
        case LC3: return MDR_AUDIO_CODEC_LC3;
        case OTHER: return MDR_AUDIO_CODEC_OTHER;
        default: return MDR_AUDIO_CODEC_UNKNOWN;
        }
    }

    MDRChargingState ToNeutral(mdr::v2::t1::BatteryChargingStatus value)
    {
        using enum mdr::v2::t1::BatteryChargingStatus;
        switch (value)
        {
        case NOT_CHARGING: return MDR_CHARGING_NO;
        case CHARGING: return MDR_CHARGING_YES;
        case CHARGED: return MDR_CHARGING_COMPLETE;
        default: return MDR_CHARGING_UNKNOWN;
        }
    }

    MDRPlaybackStatus ToNeutral(mdr::v2::t1::PlaybackStatus value)
    {
        using enum mdr::v2::t1::PlaybackStatus;
        switch (value)
        {
        case PLAY: return MDR_PLAYBACK_PLAYING;
        case PAUSE: return MDR_PLAYBACK_PAUSED;
        case STOP: return MDR_PLAYBACK_STOPPED;
        default: return MDR_PLAYBACK_UNKNOWN;
        }
    }

    MDRAdaptiveSensitivity ToNeutral(mdr::v2::t1::NoiseAdaptiveSensitivity value)
    {
        using enum mdr::v2::t1::NoiseAdaptiveSensitivity;
        switch (value)
        {
        case LOW: return MDR_ADAPTIVE_SENSITIVITY_LOW;
        case STANDARD: return MDR_ADAPTIVE_SENSITIVITY_STANDARD;
        case HIGH: return MDR_ADAPTIVE_SENSITIVITY_HIGH;
        default: return MDR_ADAPTIVE_SENSITIVITY_UNKNOWN;
        }
    }

    bool FromNeutral(MDRAdaptiveSensitivity value, mdr::v2::t1::NoiseAdaptiveSensitivity& out)
    {
        using enum mdr::v2::t1::NoiseAdaptiveSensitivity;
        switch (value)
        {
        case MDR_ADAPTIVE_SENSITIVITY_LOW: out = LOW; return true;
        case MDR_ADAPTIVE_SENSITIVITY_STANDARD: out = STANDARD; return true;
        case MDR_ADAPTIVE_SENSITIVITY_HIGH: out = HIGH; return true;
        default: return false;
        }
    }

    MDRNoiseButtonMode ToNeutral(mdr::v2::t1::Function value)
    {
        using enum mdr::v2::t1::Function;
        switch (value)
        {
        case NC_ASM_OFF: return MDR_NOISE_BUTTON_NOISE_AMBIENT_OFF;
        case NC_ASM: return MDR_NOISE_BUTTON_NOISE_AMBIENT;
        case NC_OFF: return MDR_NOISE_BUTTON_NOISE_OFF;
        case ASM_OFF: return MDR_NOISE_BUTTON_AMBIENT_OFF;
        default: return MDR_NOISE_BUTTON_NONE;
        }
    }

    bool FromNeutral(MDRNoiseButtonMode value, mdr::v2::t1::Function& out)
    {
        using enum mdr::v2::t1::Function;
        switch (value)
        {
        case MDR_NOISE_BUTTON_NONE: out = NO_FUNCTION; return true;
        case MDR_NOISE_BUTTON_NOISE_AMBIENT_OFF: out = NC_ASM_OFF; return true;
        case MDR_NOISE_BUTTON_NOISE_AMBIENT: out = NC_ASM; return true;
        case MDR_NOISE_BUTTON_NOISE_OFF: out = NC_OFF; return true;
        case MDR_NOISE_BUTTON_AMBIENT_OFF: out = ASM_OFF; return true;
        default: return false;
        }
    }

    MDRSpeechSensitivity ToNeutral(mdr::v2::t1::DetectSensitivity value)
    {
        using enum mdr::v2::t1::DetectSensitivity;
        switch (value)
        {
        case AUTO: return MDR_SPEECH_SENSITIVITY_AUTO;
        case LOW: return MDR_SPEECH_SENSITIVITY_LOW;
        case HIGH: return MDR_SPEECH_SENSITIVITY_HIGH;
        default: return MDR_SPEECH_SENSITIVITY_UNKNOWN;
        }
    }

    bool FromNeutral(MDRSpeechSensitivity value, mdr::v2::t1::DetectSensitivity& out)
    {
        using enum mdr::v2::t1::DetectSensitivity;
        switch (value)
        {
        case MDR_SPEECH_SENSITIVITY_AUTO: out = AUTO; return true;
        case MDR_SPEECH_SENSITIVITY_LOW: out = LOW; return true;
        case MDR_SPEECH_SENSITIVITY_HIGH: out = HIGH; return true;
        default: return false;
        }
    }

    MDRSpeakTimeout ToNeutral(mdr::v2::t1::ModeOutTime value)
    {
        using enum mdr::v2::t1::ModeOutTime;
        switch (value)
        {
        case FAST: return MDR_SPEAK_TIMEOUT_SHORT;
        case MID: return MDR_SPEAK_TIMEOUT_MEDIUM;
        case SLOW: return MDR_SPEAK_TIMEOUT_LONG;
        case NONE: return MDR_SPEAK_TIMEOUT_MANUAL;
        default: return MDR_SPEAK_TIMEOUT_UNKNOWN;
        }
    }

    bool FromNeutral(MDRSpeakTimeout value, mdr::v2::t1::ModeOutTime& out)
    {
        using enum mdr::v2::t1::ModeOutTime;
        switch (value)
        {
        case MDR_SPEAK_TIMEOUT_SHORT: out = FAST; return true;
        case MDR_SPEAK_TIMEOUT_MEDIUM: out = MID; return true;
        case MDR_SPEAK_TIMEOUT_LONG: out = SLOW; return true;
        case MDR_SPEAK_TIMEOUT_MANUAL: out = NONE; return true;
        default: return false;
        }
    }

    MDRRoomSize ToNeutral(mdr::v2::t1::RoomSize value)
    {
        using enum mdr::v2::t1::RoomSize;
        switch (value)
        {
        case SMALL: return MDR_ROOM_SMALL;
        case MIDDLE: return MDR_ROOM_MEDIUM;
        case LARGE: return MDR_ROOM_LARGE;
        default: return MDR_ROOM_UNKNOWN;
        }
    }

    bool FromNeutral(MDRRoomSize value, mdr::v2::t1::RoomSize& out)
    {
        using enum mdr::v2::t1::RoomSize;
        switch (value)
        {
        case MDR_ROOM_SMALL: out = SMALL; return true;
        case MDR_ROOM_MEDIUM: out = MIDDLE; return true;
        case MDR_ROOM_LARGE: out = LARGE; return true;
        default: return false;
        }
    }

    MDREqualizerPreset ToNeutral(mdr::v2::t1::EqPresetId value)
    {
        using enum mdr::v2::t1::EqPresetId;
        switch (value)
        {
        case OFF: return MDR_EQ_OFF;
        case ROCK: return MDR_EQ_ROCK;
        case POP: return MDR_EQ_POP;
        case JAZZ: return MDR_EQ_JAZZ;
        case DANCE: return MDR_EQ_DANCE;
        case EDM: return MDR_EQ_EDM;
        case R_AND_B_HIP_HOP: return MDR_EQ_R_AND_B_HIP_HOP;
        case ACOUSTIC: return MDR_EQ_ACOUSTIC;
        case BRIGHT: return MDR_EQ_BRIGHT;
        case EXCITED: return MDR_EQ_EXCITED;
        case MELLOW: return MDR_EQ_MELLOW;
        case RELAXED: return MDR_EQ_RELAXED;
        case VOCAL: return MDR_EQ_VOCAL;
        case TREBLE: return MDR_EQ_TREBLE;
        case BASS: return MDR_EQ_BASS;
        case SPEECH: return MDR_EQ_SPEECH;
        case HEAVY: return MDR_EQ_HEAVY;
        case CLEAR: return MDR_EQ_CLEAR;
        case HARD: return MDR_EQ_HARD;
        case SOFT: return MDR_EQ_SOFT;
        case GAMING_EQ: return MDR_EQ_GAMING;
        case FPS_1: return MDR_EQ_FPS_1;
        case FPS_2: return MDR_EQ_FPS_2;
        case FPS_3: return MDR_EQ_FPS_3;
        case CUSTOM: return MDR_EQ_CUSTOM;
        case USER_SETTING1: return MDR_EQ_USER_1;
        case USER_SETTING2: return MDR_EQ_USER_2;
        case USER_SETTING3: return MDR_EQ_USER_3;
        case USER_SETTING4: return MDR_EQ_USER_4;
        case USER_SETTING5: return MDR_EQ_USER_5;
        default: return MDR_EQ_UNKNOWN;
        }
    }

    bool FromNeutral(MDREqualizerPreset value, mdr::v2::t1::EqPresetId& out)
    {
        using enum mdr::v2::t1::EqPresetId;
        switch (value)
        {
        case MDR_EQ_OFF: out = OFF; return true;
        case MDR_EQ_ROCK: out = ROCK; return true;
        case MDR_EQ_POP: out = POP; return true;
        case MDR_EQ_JAZZ: out = JAZZ; return true;
        case MDR_EQ_DANCE: out = DANCE; return true;
        case MDR_EQ_EDM: out = EDM; return true;
        case MDR_EQ_R_AND_B_HIP_HOP: out = R_AND_B_HIP_HOP; return true;
        case MDR_EQ_ACOUSTIC: out = ACOUSTIC; return true;
        case MDR_EQ_BRIGHT: out = BRIGHT; return true;
        case MDR_EQ_EXCITED: out = EXCITED; return true;
        case MDR_EQ_MELLOW: out = MELLOW; return true;
        case MDR_EQ_RELAXED: out = RELAXED; return true;
        case MDR_EQ_VOCAL: out = VOCAL; return true;
        case MDR_EQ_TREBLE: out = TREBLE; return true;
        case MDR_EQ_BASS: out = BASS; return true;
        case MDR_EQ_SPEECH: out = SPEECH; return true;
        case MDR_EQ_HEAVY: out = HEAVY; return true;
        case MDR_EQ_CLEAR: out = CLEAR; return true;
        case MDR_EQ_HARD: out = HARD; return true;
        case MDR_EQ_SOFT: out = SOFT; return true;
        case MDR_EQ_GAMING: out = GAMING_EQ; return true;
        case MDR_EQ_FPS_1: out = FPS_1; return true;
        case MDR_EQ_FPS_2: out = FPS_2; return true;
        case MDR_EQ_FPS_3: out = FPS_3; return true;
        case MDR_EQ_CUSTOM: out = CUSTOM; return true;
        case MDR_EQ_USER_1: out = USER_SETTING1; return true;
        case MDR_EQ_USER_2: out = USER_SETTING2; return true;
        case MDR_EQ_USER_3: out = USER_SETTING3; return true;
        case MDR_EQ_USER_4: out = USER_SETTING4; return true;
        case MDR_EQ_USER_5: out = USER_SETTING5; return true;
        default: return false;
        }
    }

    MDRDSEEType ToNeutral(mdr::v2::t1::UpscalingType value)
    {
        using enum mdr::v2::t1::UpscalingType;
        switch (value)
        {
        case DSEE: return MDR_DSEE_STANDARD;
        case DSEE_HX: return MDR_DSEE_HX;
        case DSEE_HX_AI: return MDR_DSEE_HX_AI;
        case DSEE_ULTIMATE: return MDR_DSEE_ULTIMATE;
        default: return MDR_DSEE_UNKNOWN;
        }
    }

    MDRAssignableAction ToNeutral(mdr::v2::t1::Preset value)
    {
        using enum mdr::v2::t1::Preset;
        switch (value)
        {
        case PLAYBACK_CONTROL:
        case PLAYBACK_CONTROL_VOICE_ASSISTANT_LIMITATION:
            return MDR_ASSIGNABLE_PLAYBACK;
        case AMBIENT_SOUND_CONTROL:
            return MDR_ASSIGNABLE_NOISE_CONTROL;
        case AMBIENT_SOUND_CONTROL_QUICK_ACCESS:
            return MDR_ASSIGNABLE_NOISE_CONTROL_QUICK_ACCESS;
        case TRACK_CONTROL:
            return MDR_ASSIGNABLE_TRACK_CONTROL;
        case VOICE_RECOGNITION:
        case GOOGLE_ASSIST:
        case AMAZON_ALEXA:
        case TENCENT_XIAOWEI:
        case MS:
            return MDR_ASSIGNABLE_VOICE_ASSISTANT;
        case QUICK_ACCESS:
            return MDR_ASSIGNABLE_QUICK_ACCESS;
        default:
            return MDR_ASSIGNABLE_NONE;
        }
    }

    bool FromNeutral(MDRAssignableAction value, mdr::v2::t1::Preset& out)
    {
        using enum mdr::v2::t1::Preset;
        switch (value)
        {
        case MDR_ASSIGNABLE_NONE: out = NO_FUNCTION; return true;
        case MDR_ASSIGNABLE_PLAYBACK: out = PLAYBACK_CONTROL; return true;
        case MDR_ASSIGNABLE_NOISE_CONTROL: out = AMBIENT_SOUND_CONTROL; return true;
        case MDR_ASSIGNABLE_NOISE_CONTROL_QUICK_ACCESS: out = AMBIENT_SOUND_CONTROL_QUICK_ACCESS; return true;
        case MDR_ASSIGNABLE_TRACK_CONTROL: out = TRACK_CONTROL; return true;
        case MDR_ASSIGNABLE_VOICE_ASSISTANT: out = VOICE_RECOGNITION; return true;
        case MDR_ASSIGNABLE_QUICK_ACCESS: out = QUICK_ACCESS; return true;
        default: return false;
        }
    }

    uint32_t AutoPowerMinutes(mdr::v2::t1::AutoPowerOffElements value)
    {
        using enum mdr::v2::t1::AutoPowerOffElements;
        switch (value)
        {
        case POWER_OFF_IN_5_MIN: return 5;
        case POWER_OFF_IN_15_MIN: return 15;
        case POWER_OFF_IN_30_MIN: return 30;
        case POWER_OFF_IN_60_MIN: return 60;
        case POWER_OFF_IN_180_MIN: return 180;
        default: return 0;
        }
    }

    uint32_t AutoPowerMinutes(mdr::v2::t1::AutoPowerOffWearingDetectionElements value)
    {
        using enum mdr::v2::t1::AutoPowerOffWearingDetectionElements;
        switch (value)
        {
        case POWER_OFF_IN_5_MIN: return 5;
        case POWER_OFF_IN_15_MIN: return 15;
        case POWER_OFF_IN_30_MIN: return 30;
        case POWER_OFF_IN_60_MIN: return 60;
        case POWER_OFF_IN_180_MIN: return 180;
        default: return 0;
        }
    }

    bool AutoPowerFromMinutes(uint32_t minutes, mdr::v2::t1::AutoPowerOffElements& out)
    {
        using enum mdr::v2::t1::AutoPowerOffElements;
        switch (minutes)
        {
        case 0: out = POWER_OFF_DISABLE; return true;
        case 5: out = POWER_OFF_IN_5_MIN; return true;
        case 15: out = POWER_OFF_IN_15_MIN; return true;
        case 30: out = POWER_OFF_IN_30_MIN; return true;
        case 60: out = POWER_OFF_IN_60_MIN; return true;
        case 180: out = POWER_OFF_IN_180_MIN; return true;
        default: return false;
        }
    }

    bool AutoPowerFromMinutes(uint32_t minutes, mdr::v2::t1::AutoPowerOffWearingDetectionElements& out)
    {
        using enum mdr::v2::t1::AutoPowerOffWearingDetectionElements;
        switch (minutes)
        {
        case 0: out = POWER_OFF_DISABLE; return true;
        case 5: out = POWER_OFF_IN_5_MIN; return true;
        case 15: out = POWER_OFF_IN_15_MIN; return true;
        case 30: out = POWER_OFF_IN_30_MIN; return true;
        case 60: out = POWER_OFF_IN_60_MIN; return true;
        case 180: out = POWER_OFF_IN_180_MIN; return true;
        default: return false;
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
    case MDR_RESULT_OK: return "OK";
    case MDR_RESULT_INPROGRESS: return "Task in progress";
    case MDR_RESULT_ERROR_GENERAL: return "General error";
    case MDR_RESULT_ERROR_NOT_FOUND: return "Resource not found";
    case MDR_RESULT_ERROR_TIMEOUT: return "Timed out";
    case MDR_RESULT_ERROR_NET: return "Networking error";
    case MDR_RESULT_ERROR_NO_CONNECTION: return "No connection has been established";
    case MDR_RESULT_ERROR_BAD_ADDRESS: return "Invalid address information";
    case MDR_RESULT_ERROR_NOT_SUPPORTED: return "Not supported";
    case MDR_RESULT_ERROR_BUFFER_TOO_SMALL: return "Buffer too small";
    case MDR_RESULT_ERROR_MALFORMED_PAYLOAD: return "Malformed payload";
    case MDR_RESULT_ERROR_INVALID_ARGUMENT: return "Invalid argument";
    case MDR_RESULT_ERROR_ABI_MISMATCH: return "Library does not implement the caller's MDR ABI version";
    default: return "Unknown";
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

MDRResult mdrHeadphonesCreate(
    uint32_t abiVersion, MDRConnection* connection, MDRProtocolVersion protocolVersion, MDRHeadphones** outHeadphones)
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
    case MDR_PROTOCOL_V1: family = Headphones::ProtocolFamily::V1; break;
    case MDR_PROTOCOL_V2: family = Headphones::ProtocolFamily::V2; break;
    default: return MDR_RESULT_ERROR_INVALID_ARGUMENT;
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
    return static_cast<MDRBoolean>(headphones && Impl(headphones)->mNeutralInitialized);
}

MDRBoolean mdrHeadphonesIsReady(const MDRHeadphones* headphones)
{
    return static_cast<MDRBoolean>(headphones && Impl(headphones)->IsReady());
}

MDRBoolean mdrHeadphonesIsDirty(const MDRHeadphones* headphones)
{
    return static_cast<MDRBoolean>(headphones && Impl(headphones)->IsDirty());
}

MDRResult mdrHeadphonesRequestInit(MDRHeadphones* headphones)
{
    if (!headphones)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (!h->IsReady())
        return MDR_RESULT_INPROGRESS;
    return h->Invoke(h->RequestInit());
}

MDRResult mdrHeadphonesRequestFetch(MDRHeadphones* headphones)
{
    if (!headphones)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (!h->IsReady())
        return MDR_RESULT_INPROGRESS;
    return h->Invoke(h->RequestSync());
}

MDRResult mdrHeadphonesRequestCommit(MDRHeadphones* headphones)
{
    if (!headphones)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (!h->IsReady())
        return MDR_RESULT_INPROGRESS;
    return h->Invoke(h->RequestCommit());
}

MDRResult mdrHeadphonesPoll(MDRHeadphones* headphones, MDREvent* outEvent)
{
    if (!headphones || !outEvent)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    return Impl(headphones)->PollEvents(*outEvent);
}

void mdrHeadphonesSetPacketCallback(MDRHeadphones* headphones, MDRPacketCallback callback, void* userData)
{
    if (headphones)
        Impl(headphones)->SetPacketCallback(callback, userData);
}

MDRResult mdrHeadphonesGetFeature(
    MDRHeadphones* headphones, MDRFeature feature, MDRFeatureAvailability* outAvailability)
{
    if (!headphones || !outAvailability)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    // Keep the upper bound on the last MDR_FEATURE_* id, or newly added features read as invalid.
    if (feature < MDR_FEATURE_IDENTITY || feature > MDR_FEATURE_SOURCE_SWITCH_CONTROL)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (!h.mNeutralInitialized)
    {
        *outAvailability = MDR_AVAILABILITY_UNKNOWN;
        return MDR_RESULT_OK;
    }
    *outAvailability = h.mSupport.contains(feature)
        ? MDR_AVAILABILITY_AVAILABLE
        : MDR_AVAILABILITY_UNAVAILABLE;
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetText(
    MDRHeadphones* headphones, MDRText text, uint32_t index, char* buffer, uint32_t* inoutSize)
{
    if (!headphones)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    switch (text)
    {
    case MDR_TEXT_MODEL_NAME:
        return CopyText({h.mModelName.data(), h.mModelName.size()}, buffer, inoutSize);
    case MDR_TEXT_UNIQUE_ID:
        return CopyText({h.mUniqueId.data(), h.mUniqueId.size()}, buffer, inoutSize);
    case MDR_TEXT_FIRMWARE_VERSION:
        return CopyText({h.mFWVersion.data(), h.mFWVersion.size()}, buffer, inoutSize);
    case MDR_TEXT_MODEL_SERIES:
        {
            return CopyText(mdr::v2::t1::format_as(h.mModelSeries), buffer, inoutSize);
        }
    case MDR_TEXT_MODEL_COLOR:
        {
            return CopyText(mdr::v2::format_as(h.mModelColor), buffer, inoutSize);
        }
    case MDR_TEXT_TRACK_TITLE:
        return CopyText({h.mPlayTrackTitle.data(), h.mPlayTrackTitle.size()}, buffer, inoutSize);
    case MDR_TEXT_TRACK_ALBUM:
        return CopyText({h.mPlayTrackAlbum.data(), h.mPlayTrackAlbum.size()}, buffer, inoutSize);
    case MDR_TEXT_TRACK_ARTIST:
        return CopyText({h.mPlayTrackArtist.data(), h.mPlayTrackArtist.size()}, buffer, inoutSize);
    case MDR_TEXT_PAIRED_DEVICE_ID:
    case MDR_TEXT_PAIRED_DEVICE_NAME:
        if (index >= h.mPairedDevices.size())
            return MDR_RESULT_ERROR_NOT_FOUND;
        if (text == MDR_TEXT_PAIRED_DEVICE_ID)
            return CopyText(
                {h.mPairedDevices[index].macAddress.data(), h.mPairedDevices[index].macAddress.size()},
                buffer, inoutSize);
        return CopyText(
            {h.mPairedDevices[index].name.data(), h.mPairedDevices[index].name.size()}, buffer, inoutSize);
    case MDR_TEXT_GENERAL_SETTING_SUBJECT:
    case MDR_TEXT_GENERAL_SETTING_SUMMARY:
        {
            const Headphones::GsCapability* capabilities[] = {
                &h.mGsCapability1, &h.mGsCapability2, &h.mGsCapability3, &h.mGsCapability4
            };
            if (index >= std::size(capabilities))
                return MDR_RESULT_ERROR_NOT_FOUND;
            const auto& value = text == MDR_TEXT_GENERAL_SETTING_SUBJECT
                ? capabilities[index]->value.subject.value
                : capabilities[index]->value.summary.value;
            return CopyText({value.data(), value.size()}, buffer, inoutSize);
        }
    case MDR_TEXT_LAST_ERROR:
        return CopyText(h.GetLastError(), buffer, inoutSize);
    case MDR_TEXT_LAST_ALERT:
        {
            const auto value = mdr::Format("{}", static_cast<unsigned>(h.mLastAlertMessage));
            return CopyText({value.data(), value.size()}, buffer, inoutSize);
        }
    case MDR_TEXT_LAST_INTERACTION:
        return CopyText({h.mLastInteractionMessage.data(), h.mLastInteractionMessage.size()}, buffer, inoutSize);
    case MDR_TEXT_LAST_DEVICE_MESSAGE:
        return CopyText({h.mLastDeviceJSONMessage.data(), h.mLastDeviceJSONMessage.size()}, buffer, inoutSize);
    default:
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    }
}

MDRResult mdrHeadphonesGetModel(MDRHeadphones* headphones, MDRModel* outIdentity)
{
    if (!headphones || !outIdentity)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    *outIdentity = {
        .protocol_version = static_cast<uint32_t>(h.mProtocol.version),
        .audio_codec = ToNeutral(h.mAudioCodec),
        .model_color = static_cast<uint8_t>(h.mModelColor)
    };
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetBatteries(
    MDRHeadphones* headphones, MDRBattery* batteries, uint32_t* inoutCount)
{
    if (!headphones || !inoutCount)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    const bool hasLR = h.mSupport.contains(MDR_FEATURE_BATTERY_LEFT_RIGHT);
    const bool hasSingle = !hasLR && h.mSupport.contains(MDR_FEATURE_BATTERY_SINGLE);
    const bool hasCase = h.mSupport.contains(MDR_FEATURE_BATTERY_CASE);
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
    const auto write = [&](MDRBatteryPart part, const Headphones::BatteryState& state)
    {
        batteries[out++] = {
            .part = part,
            .present = MDR_TRUE,
            .level_percent = state.level,
            .update_threshold_percent = state.threshold,
            .charging = ToNeutral(state.charging)
        };
    };
    if (hasLR)
    {
        write(MDR_BATTERY_LEFT, h.mBatteryL);
        write(MDR_BATTERY_RIGHT, h.mBatteryR);
    }
    else if (hasSingle)
        write(MDR_BATTERY_MAIN, h.mBatteryL);
    if (hasCase)
        write(MDR_BATTERY_CASE, h.mBatteryCase);
    *inoutCount = required;
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetPlayback(MDRHeadphones* headphones, MDRPlayback* outPlayback)
{
    if (!headphones || !outPlayback)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    *outPlayback = {
        .status = ToNeutral(h.mPlayPause),
        .volume = static_cast<uint8_t>(h.mPlayVolume.current)
    };
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetPlayback(MDRHeadphones* headphones, const MDRPlayback* playback)
{
    if (!headphones || !playback || playback->volume > 30 ||
        playback->status > MDR_PLAYBACK_PAUSED)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    const auto currentStatus = ToNeutral(h->mPlayPause);
    if (playback->status != MDR_PLAYBACK_UNKNOWN && playback->status != currentStatus)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    h->mPlayVolume.stage(playback->volume);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesPlayback(MDRHeadphones* headphones, const MDRPlaybackCommand* command)
{
    if (!headphones || !command)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    using enum mdr::v2::t1::PlaybackControl;
    auto* h = Impl(headphones);
    switch (command->action)
    {
    case MDR_PLAYBACK_PLAY: h->mPlayControl.stage(PLAY); break;
    case MDR_PLAYBACK_PAUSE: h->mPlayControl.stage(PAUSE); break;
    case MDR_PLAYBACK_NEXT: h->mPlayControl.stage(TRACK_UP); break;
    case MDR_PLAYBACK_PREVIOUS: h->mPlayControl.stage(TRACK_DOWN); break;
    default: return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    }
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetNoiseControl(
    MDRHeadphones* headphones, MDRNoiseControl* outNoiseControl)
{
    if (!headphones || !outNoiseControl)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    const bool enabled = h.mNcAsmEnabled.current;
    const auto mode = h.mNcAsmMode.current;
    *outNoiseControl = {
        .mode = !enabled ? MDR_NOISE_MODE_OFF :
            mode == mdr::v2::t1::NcAsmMode::NC ? MDR_NOISE_MODE_CANCELLING : MDR_NOISE_MODE_AMBIENT,
        .ambient_level = static_cast<uint8_t>(h.mNcAsmAmbientLevel.current),
        .focus_on_voice = static_cast<MDRBoolean>(h.mNcAsmFocusOnVoice.current),
        .button_mode = ToNeutral(h.mNcAsmButtonFunction.current),
        .adaptive_ambient = static_cast<MDRBoolean>(h.mNcAsmAutoAsmEnabled.current),
        .adaptive_sensitivity = ToNeutral(h.mNcAsmNoiseAdaptiveSensitivity.current)
    };
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetNoiseControl(
    MDRHeadphones* headphones, const MDRNoiseControl* noiseControl)
{
    if (!headphones || !noiseControl ||
        noiseControl->mode > MDR_NOISE_MODE_AMBIENT || noiseControl->ambient_level > 20 ||
        !ValidBoolean(noiseControl->focus_on_voice) || !ValidBoolean(noiseControl->adaptive_ambient))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    mdr::v2::t1::Function button{};
    auto sensitivity = h->mNcAsmNoiseAdaptiveSensitivity.desired;
    if (!FromNeutral(noiseControl->button_mode, button) ||
        (noiseControl->adaptive_sensitivity != MDR_ADAPTIVE_SENSITIVITY_UNKNOWN &&
         !FromNeutral(noiseControl->adaptive_sensitivity, sensitivity)))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    h->mNcAsmEnabled.stage(noiseControl->mode != MDR_NOISE_MODE_OFF);
    h->mNcAsmMode.stage(noiseControl->mode == MDR_NOISE_MODE_AMBIENT
        ? mdr::v2::t1::NcAsmMode::ASM : mdr::v2::t1::NcAsmMode::NC);
    h->mNcAsmAmbientLevel.stage(noiseControl->ambient_level);
    h->mNcAsmFocusOnVoice.stage(noiseControl->focus_on_voice != MDR_FALSE);
    h->mNcAsmButtonFunction.stage(button);
    h->mNcAsmAutoAsmEnabled.stage(noiseControl->adaptive_ambient != MDR_FALSE);
    h->mNcAsmNoiseAdaptiveSensitivity.stage(sensitivity);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetSpeakToChat(
    MDRHeadphones* headphones, MDRSpeakToChat* outSpeakToChat)
{
    if (!headphones || !outSpeakToChat)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    *outSpeakToChat = {
        .enabled = static_cast<MDRBoolean>(h.mSpeakToChatEnabled.current),
        .sensitivity = ToNeutral(h.mSpeakToChatDetectSensitivity.current),
        .timeout = ToNeutral(h.mSpeakToModeOutTime.current)
    };
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetSpeakToChat(
    MDRHeadphones* headphones, const MDRSpeakToChat* speakToChat)
{
    if (!headphones || !speakToChat || !ValidBoolean(speakToChat->enabled))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (!h->mSupport.contains(MDR_FEATURE_SPEAK_TO_CHAT))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    auto sensitivity = h->mSpeakToChatDetectSensitivity.desired;
    auto timeout = h->mSpeakToModeOutTime.desired;
    if ((speakToChat->sensitivity != MDR_SPEECH_SENSITIVITY_UNKNOWN &&
         !FromNeutral(speakToChat->sensitivity, sensitivity)) ||
        (speakToChat->timeout != MDR_SPEAK_TIMEOUT_UNKNOWN &&
         !FromNeutral(speakToChat->timeout, timeout)))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    h->mSpeakToChatEnabled.stage(speakToChat->enabled != MDR_FALSE);
    h->mSpeakToChatDetectSensitivity.stage(sensitivity);
    h->mSpeakToModeOutTime.stage(timeout);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetListening(
    MDRHeadphones* headphones, MDRListening* outListening)
{
    if (!headphones || !outListening)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    const bool cinema = h.mUpmixCinemaEnabled.current;
    const bool background = h.mBGMModeEnabled.current;
    *outListening = {
        .mode = cinema ? MDR_LISTENING_CINEMA :
            background ? MDR_LISTENING_BACKGROUND_MUSIC : MDR_LISTENING_STANDARD,
        .background_room = ToNeutral(h.mBGMModeRoomSize.current)
    };
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetListening(MDRHeadphones* headphones, const MDRListening* listening)
{
    if (!headphones || !listening || listening->mode > MDR_LISTENING_CINEMA)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (!h->mSupport.contains(mdr::v2::t1::FunctionType::LISTENING_OPTION))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    auto room = h->mBGMModeRoomSize.desired;
    if (listening->background_room != MDR_ROOM_UNKNOWN && !FromNeutral(listening->background_room, room))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    if (listening->mode == MDR_LISTENING_BACKGROUND_MUSIC && listening->background_room == MDR_ROOM_UNKNOWN)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    h->mBGMModeEnabled.stage(listening->mode == MDR_LISTENING_BACKGROUND_MUSIC);
    h->mUpmixCinemaEnabled.stage(listening->mode == MDR_LISTENING_CINEMA);
    h->mBGMModeRoomSize.stage(room);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetEqualizer(
    MDRHeadphones* headphones, MDREqualizer* outEqualizer)
{
    if (!headphones || !outEqualizer)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    *outEqualizer = {
        .preset = ToNeutral(h.mEqPresetId.current),
        .clear_bass = static_cast<int8_t>(h.mEqClearBass.current),
        .band_count = static_cast<uint32_t>(h.mEqConfig.current.size()),
        .dsee_enabled = static_cast<MDRBoolean>(h.mUpscalingEnabled.current),
        .dsee_type = ToNeutral(h.mUpscalingType)
    };
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetEqualizer(MDRHeadphones* headphones, const MDREqualizer* equalizer)
{
    if (!headphones || !equalizer || equalizer->clear_bass < -10 ||
        equalizer->clear_bass > 10 || !ValidBoolean(equalizer->dsee_enabled) ||
        (equalizer->band_count != 0 && equalizer->band_count != 5 && equalizer->band_count != 10))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    auto preset = h->mEqPresetId.desired;
    if (equalizer->preset != MDR_EQ_UNKNOWN && !FromNeutral(equalizer->preset, preset))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    if (equalizer->dsee_type != MDR_DSEE_UNKNOWN && equalizer->dsee_type != ToNeutral(h->mUpscalingType))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    const auto existingCount = h->mEqConfig.desired.size();
    if (equalizer->band_count != 0 && equalizer->band_count != existingCount)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    h->mEqPresetId.stage(preset);
    h->mEqClearBass.stage(equalizer->clear_bass);
    h->mUpscalingEnabled.stage(equalizer->dsee_enabled != MDR_FALSE);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetEqualizerBands(
    MDRHeadphones* headphones, int8_t* bands, uint32_t* inoutCount)
{
    if (!headphones || !inoutCount)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& values = Impl(headphones)->mEqConfig.current;
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
        bands[i] = static_cast<int8_t>(values[i]);
    *inoutCount = required;
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetEqualizerBands(MDRHeadphones* headphones, const int8_t* bands, uint32_t count)
{
    if (!headphones || !bands || (count != 5 && count != 10))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const int minimum = count == 5 ? -10 : -6;
    const int maximum = count == 5 ? 10 : 6;
    mdr::Vector<int> values;
    values.reserve(count);
    for (uint32_t i = 0; i < count; ++i)
    {
        if (bands[i] < minimum || bands[i] > maximum)
            return MDR_RESULT_ERROR_INVALID_ARGUMENT;
        values.push_back(bands[i]);
    }
    auto* h = Impl(headphones);
    h->mEqConfig.stage(std::move(values));
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetPairedDevices(
    MDRHeadphones* headphones, MDRPairedDevice* devices, uint32_t* inoutCount)
{
    if (!headphones || !inoutCount)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    const uint32_t required = static_cast<uint32_t>(h.mPairedDevices.size());
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
        const auto& dev = h.mPairedDevices[i];
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
            ? nm.size() : (sizeof(devices[i].name) - 1);
        std::memcpy(devices[i].name, nm.data(), nameLen);
        devices[i].name[nameLen] = '\0';
    }
    *inoutCount = required;
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetPairedDevice(
    MDRHeadphones* headphones, const MDRPairedDeviceAction* action)
{
    if (!headphones || !action)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    std::string_view id;
    const MDRResult validation = ValidateDeviceId(*action, id);
    if (validation != MDR_RESULT_OK)
        return validation;
    auto* h = Impl(headphones);
    if (!SupportsPairing(*h))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    if (h->mProtocolFamily == Headphones::ProtocolFamily::V1)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    const mdr::String value{id.begin(), id.end()};
    switch (action->command)
    {
    case MDR_PAIRED_DEVICE_CONNECT: h->mPairedDeviceConnectMac.stage(value); break;
    case MDR_PAIRED_DEVICE_DISCONNECT: h->mPairedDeviceDisconnectMac.stage(value); break;
    case MDR_PAIRED_DEVICE_SELECT_PLAYBACK: h->mMultipointDeviceMac.stage(value); break;
    case MDR_PAIRED_DEVICE_UNPAIR: h->mPairedDeviceUnpairMac.stage(value); break;
    default: return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    }
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetPairing(
    MDRHeadphones* headphones, MDRPairing* outPairing)
{
    if (!headphones || !outPairing)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    *outPairing = {
        .enabled = static_cast<MDRBoolean>(Impl(headphones)->mPairingMode.current)
    };
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetPairing(MDRHeadphones* headphones, const MDRPairing* pairing)
{
    if (!headphones || !pairing || !ValidBoolean(pairing->enabled))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (!SupportsPairing(*h))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    h->mPairingMode.stage(pairing->enabled != MDR_FALSE);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetSourceSwitchControl(MDRHeadphones* headphones, MDRBoolean* outEnabled)
{
    if (!headphones || !outEnabled)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    *outEnabled = static_cast<MDRBoolean>(Impl(headphones)->mSourceSwitchControlEnabled.current);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetSourceSwitchControl(MDRHeadphones* headphones, MDRBoolean enabled)
{
    if (!headphones || !ValidBoolean(enabled))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (!h->mSupport.contains(MDR_FEATURE_SOURCE_SWITCH_CONTROL))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    h->mSourceSwitchControlResult = mdr::v2::t2::SourceSwitchControlResult::SUCCESS;
    h->mSourceSwitchControlEnabled.stage(enabled != MDR_FALSE);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetSourceSwitchControlResult(MDRHeadphones* headphones, MDRSourceSwitchControlResult* outResult)
{
    if (!headphones || !outResult)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    using enum mdr::v2::t2::SourceSwitchControlResult;
    switch (Impl(headphones)->mSourceSwitchControlResult)
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

MDRResult mdrHeadphonesGetGeneralSettingInfo(
    MDRHeadphones* headphones, MDRGeneralSettingInfo* settings, uint32_t* inoutCount)
{
    if (!headphones || !inoutCount)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
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
    const Headphones::GsCapability* capabilities[] = {
        &h.mGsCapability1, &h.mGsCapability2, &h.mGsCapability3, &h.mGsCapability4
    };
    uint32_t out = 0;
    for (uint32_t i = 0; i < 4; ++i)
    {
        if (!SupportsGeneralSetting(h, i))
            continue;
        settings[out++] = {
            .index = i,
            .type = capabilities[i]->type == mdr::v2::t1::GsSettingType::BOOLEAN_TYPE
                ? MDR_GENERAL_SETTING_BOOLEAN : MDR_GENERAL_SETTING_UNKNOWN,
            .writable = static_cast<MDRBoolean>(
                capabilities[i]->type == mdr::v2::t1::GsSettingType::BOOLEAN_TYPE)
        };
    }
    *inoutCount = required;
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetGeneralSetting(
    MDRHeadphones* headphones, uint32_t index, MDRGeneralSetting* outSetting)
{
    if (!headphones || index >= 4 || !outSetting)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    if (!SupportsGeneralSetting(h, index))
        return MDR_RESULT_ERROR_NOT_FOUND;
    const Headphones::GsCapability* capabilities[] = {
        &h.mGsCapability1, &h.mGsCapability2, &h.mGsCapability3, &h.mGsCapability4
    };
    if (capabilities[index]->type != mdr::v2::t1::GsSettingType::BOOLEAN_TYPE)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    const mdr::MDRProperty<bool>* values[] = {
        &h.mGsParamBool1, &h.mGsParamBool2, &h.mGsParamBool3, &h.mGsParamBool4
    };
    *outSetting = {
        .index = index,
        .boolean_value = static_cast<MDRBoolean>(values[index]->current)
    };
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetGeneralSetting(
    MDRHeadphones* headphones, const MDRGeneralSetting* setting)
{
    if (!headphones || !setting || setting->index >= 4 ||
        !ValidBoolean(setting->boolean_value))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (!SupportsGeneralSetting(*h, setting->index))
        return MDR_RESULT_ERROR_NOT_FOUND;
    const Headphones::GsCapability* capabilities[] = {
        &h->mGsCapability1, &h->mGsCapability2, &h->mGsCapability3, &h->mGsCapability4
    };
    if (capabilities[setting->index]->type != mdr::v2::t1::GsSettingType::BOOLEAN_TYPE)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    mdr::MDRProperty<bool>* values[] = {
        &h->mGsParamBool1, &h->mGsParamBool2, &h->mGsParamBool3, &h->mGsParamBool4
    };
    values[setting->index]->stage(setting->boolean_value != MDR_FALSE);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetAssignableControls(
    MDRHeadphones* headphones, MDRAssignableControls* outControls)
{
    if (!headphones || !outControls)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    *outControls = {
        .left = ToNeutral(h.mTouchFunctionLeft.current),
        .right = ToNeutral(h.mTouchFunctionRight.current)
    };
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetAssignableControls(
    MDRHeadphones* headphones, const MDRAssignableControls* controls)
{
    if (!headphones || !controls)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    mdr::v2::t1::Preset left{}, right{};
    if (!FromNeutral(controls->left, left) || !FromNeutral(controls->right, right))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (!h->mSupport.contains(MDR_FEATURE_ASSIGNABLE_CONTROLS))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    h->mTouchFunctionLeft.stage(left);
    h->mTouchFunctionRight.stage(right);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetPower(MDRHeadphones* headphones, MDRPower* outPower)
{
    if (!headphones || !outPower)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    using T1 = mdr::v2::t1::FunctionType;
    const bool wearing = h.mSupport.contains(T1::AUTO_POWER_OFF_WITH_WEARING_DETECTION);
    const auto wearingValue = h.mPowerAutoOffWearingDetection.current;
    *outPower = {
        .auto_power_off_minutes = wearing ? AutoPowerMinutes(wearingValue) :
            AutoPowerMinutes(h.mPowerAutoOff.current),
        .wearing_power = !wearing ? MDR_WEARING_POWER_UNAVAILABLE :
            wearingValue == mdr::v2::t1::AutoPowerOffWearingDetectionElements::POWER_OFF_WHEN_REMOVED_FROM_EARS
                ? MDR_WEARING_POWER_WHEN_REMOVED : MDR_WEARING_POWER_DISABLED,
        .auto_pause = static_cast<MDRBoolean>(h.mAutoPauseEnabled.current),
        .head_gesture = static_cast<MDRBoolean>(h.mHeadGestureEnabled.current),
        .shutdown_requested = static_cast<MDRBoolean>(h.mShutdown.current)
    };
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetPower(MDRHeadphones* headphones, const MDRPower* power)
{
    if (!headphones || !power ||
        power->wearing_power > MDR_WEARING_POWER_WHEN_REMOVED || !ValidBoolean(power->auto_pause) ||
        !ValidBoolean(power->head_gesture) || !ValidBoolean(power->shutdown_requested))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    mdr::v2::t1::AutoPowerOffElements autoPower{};
    mdr::v2::t1::AutoPowerOffWearingDetectionElements wearingPower{};
    if (!AutoPowerFromMinutes(power->auto_power_off_minutes, autoPower) ||
        !AutoPowerFromMinutes(power->auto_power_off_minutes, wearingPower))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    using T1 = mdr::v2::t1::FunctionType;
    if (h->mSupport.contains(T1::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
    {
        h->mPowerAutoOffWearingDetection.stage(
            power->wearing_power == MDR_WEARING_POWER_WHEN_REMOVED
                ? mdr::v2::t1::AutoPowerOffWearingDetectionElements::POWER_OFF_WHEN_REMOVED_FROM_EARS
                : wearingPower);
    }
    else if (h->mSupport.contains(T1::AUTO_POWER_OFF))
        h->mPowerAutoOff.stage(autoPower);
    else if (h->mSupport.contains(MDR_FEATURE_AUTO_POWER_OFF))
        h->mPowerAutoOff.stage(autoPower);
    h->mAutoPauseEnabled.stage(power->auto_pause != MDR_FALSE);
    h->mHeadGestureEnabled.stage(power->head_gesture != MDR_FALSE);
    h->mShutdown.stage(power->shutdown_requested != MDR_FALSE);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetVoiceGuidance(
    MDRHeadphones* headphones, MDRVoiceGuidance* outVoiceGuidance)
{
    if (!headphones || !outVoiceGuidance)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    *outVoiceGuidance = {
        .enabled = static_cast<MDRBoolean>(h.mVoiceGuidanceEnabled.current),
        .volume = static_cast<int8_t>(h.mVoiceGuidanceVolume.current)
    };
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetVoiceGuidance(
    MDRHeadphones* headphones, const MDRVoiceGuidance* voiceGuidance)
{
    if (!headphones || !voiceGuidance ||
        !ValidBoolean(voiceGuidance->enabled) || voiceGuidance->volume < -2 || voiceGuidance->volume > 2)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (!SupportsVoiceGuidance(*h))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    h->mVoiceGuidanceEnabled.stage(voiceGuidance->enabled != MDR_FALSE);
    if (h->mSupport.contains(MDR_FEATURE_VOICE_GUIDANCE_VOLUME))
        h->mVoiceGuidanceVolume.stage(voiceGuidance->volume);
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetConnectionMode(
    MDRHeadphones* headphones, MDRConnectionMode* outMode)
{
    if (!headphones || !outMode)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto value = Impl(headphones)->mAudioPriorityMode.current;
    *outMode = {
        .audio_priority = value == mdr::v2::t1::PriorMode::SOUND_QUALITY_PRIOR
            ? MDR_AUDIO_PRIORITY_QUALITY :
            value == mdr::v2::t1::PriorMode::CONNECTION_QUALITY_PRIOR
                ? MDR_AUDIO_PRIORITY_STABILITY : MDR_AUDIO_PRIORITY_UNKNOWN
    };
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetConnectionMode(
    MDRHeadphones* headphones, const MDRConnectionMode* mode)
{
    if (!headphones || !mode)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (!h->mSupport.contains(MDR_FEATURE_CONNECTION_MODE))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    switch (mode->audio_priority)
    {
    case MDR_AUDIO_PRIORITY_QUALITY:
        h->mAudioPriorityMode.stage(mdr::v2::t1::PriorMode::SOUND_QUALITY_PRIOR);
        break;
    case MDR_AUDIO_PRIORITY_STABILITY:
        h->mAudioPriorityMode.stage(mdr::v2::t1::PriorMode::CONNECTION_QUALITY_PRIOR);
        break;
    default:
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    }
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesGetSafeListening(
    MDRHeadphones* headphones, MDRSafeListening* outSafeListening)
{
    if (!headphones || !outSafeListening)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const auto& h = *Impl(headphones);
    *outSafeListening = {
        .sound_pressure = static_cast<uint8_t>(std::clamp(h.mSafeListeningSoundPressure, 0, 255)),
        .preview = static_cast<MDRBoolean>(h.mSafeListeningPreviewMode.current)
    };
    return MDR_RESULT_OK;
}

MDRResult mdrHeadphonesSetSafeListening(
    MDRHeadphones* headphones, const MDRSafeListening* safeListening)
{
    if (!headphones || !safeListening || !ValidBoolean(safeListening->preview))
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    auto* h = Impl(headphones);
    if (!SupportsSafeListening(*h))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    if (safeListening->sound_pressure != static_cast<uint8_t>(std::clamp(h->mSafeListeningSoundPressure, 0, 255)))
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    h->mSafeListeningPreviewMode.stage(safeListening->preview != MDR_FALSE);
    return MDR_RESULT_OK;
}
}
#pragma endregion
