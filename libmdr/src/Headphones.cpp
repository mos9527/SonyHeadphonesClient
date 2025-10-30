// ReSharper disable CppParameterMayBeConstPtrOrRef
#include <ranges>
#include <fmt/base.h>
#include <algorithm>
#include <mdr/Headphones.hpp>

namespace mdr
{
    MDRHeadphones::Awaiter MDRHeadphones::Await(AwaitType type)
    {
        return Awaiter{this, type};
    }

    void MDRHeadphones::Awake(AwaitType type)
    {
        if (auto await = mAwaiters[type])
        {
            mAwaiters[type] = nullptr;
            await.resume();
        }
    }

    int MDRHeadphones::PollEvents()
    {
        int r = mdrConnectionPoll(mConn, 0);
        if (r == MDR_RESULT_OK)
        {
            // Non-blocking. INPROGRESS are expected, not so much for others.
            // Failfast if that happens - the owner usually has to die.
            r = Send();
            if (r != MDR_RESULT_OK && r != MDR_RESULT_INPROGRESS)
                return MDR_HEADPHONES_ERROR;
            r = Receive();
            if (r != MDR_RESULT_OK && r != MDR_RESULT_INPROGRESS)
                return MDR_HEADPHONES_ERROR;
        } else
        {
            if (r != MDR_RESULT_ERROR_TIMEOUT)
                return MDR_HEADPHONES_ERROR;
        }
        return MoveNext();
    }

    bool MDRHeadphones::IsReady() const
    {
        return !mTask;
    }

    bool MDRHeadphones::IsDirty() const
    {
        bool dirty = mShutdown.dirty() || mNcAsmEnabled.dirty() || mNcAsmFocusOnVoice.dirty() || mNcAsmAmbientLevel.dirty();
        dirty |= mNcAsmButtonFunction.dirty() || mNcAsmMode.dirty() || mNcAsmAutoAsmEnabled.dirty();
        dirty |= mNcAsmNoiseAdaptiveSensitivity.dirty() || mPowerAutoOff.dirty() || mPlayPause.dirty();
        dirty |= mPowerAutoOffWearingDetection.dirty() || mPlayVolume.dirty() || mGsParamBool1.dirty();
        dirty |= mGsParamBool2.dirty() || mGsParamBool3.dirty() || mGsParamBool4.dirty();
        dirty |= mUpscalingType.dirty() || mUpscalingAvailable.dirty() || mUpscalingEnabled.dirty();
        dirty |= mAudioPriorityMode.dirty() || mBGMModeEnabled.dirty() || mBGMModeRoomSize.dirty();
        dirty |= mUpmixCinemaEnabled.dirty() || mAutoPauseEnabled.dirty() || mTouchFunctionLeft.dirty();
        dirty |= mTouchFunctionRight.dirty() || mSpeakToChatEnabled.dirty() || mSpeakToChatDetectSensitivity.dirty();
        dirty |= mSpeakToModeOutTime.dirty() || mHeadGestureEnabled.dirty() || mEqAvailable.dirty();
        dirty |= mEqPresetId.dirty() || mEqClearBass.dirty() || mEqConfig.dirty();
        dirty |= mVoiceGuidanceEnabled.dirty() || mVoiceGuidanceVolume.dirty() || mPairingMode.dirty();
        dirty |= mMultipointDeviceMac.dirty() || mSafeListeningPreviewMode.dirty();
        return dirty;
    }

    int MDRHeadphones::Receive()
    {
        char buf[kMDRMaxPacketSize];
        int recvd;
        const int r = mdrConnectionRecv(mConn, buf, kMDRMaxPacketSize, &recvd);
        if (r != MDR_RESULT_OK)
            return r;
        fmt::print("<< ");
        for (char* p = buf; p != buf + recvd; p++)
            fmt::print("{:02X} ", static_cast<UInt8>(*p));
        fmt::println("");
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
        fmt::print(">> ");
        for (char* p = buf; p != buf + sent; p++)
            fmt::print("{:02X} ", static_cast<UInt8>(*p));
        fmt::println("");
        mSendBuf.erase(mSendBuf.begin(), mSendBuf.begin() + sent);
        return r;
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
            return HandleCommandV2T1(command, seq);
        case DATA_MDR_NO2:
            SendACK(seq);
            return HandleCommandV2T2(command, seq);
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int MDRHeadphones::MoveNext()
    {
        int taskResult;
        // Task done?
        if (ExceptionHandler([this, &taskResult] { return TaskMoveNext(taskResult); }))
            return taskResult;
        int idleCode = mTask ? MDR_HEADPHONES_INPROGRESS : MDR_HEADPHONES_IDLE;
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
        switch (MDRUnpackCommand(packedCommand, command, type, seqNum))
        {
        case MDRUnpackResult::OK:
            mRecvBuf.erase(mRecvBuf.begin(), commandEnd);
            return Handle(command, type, seqNum);
        case MDRUnpackResult::INCOMPLETE:
            // Incomplete. Nop.
            break;
        case MDRUnpackResult::BAD_MARKER: [[unlikely]]
        case MDRUnpackResult::BAD_CHECKSUM:
            [[unlikely]]
                // Unlikely. What we have now makes no sense yet markers are intact.
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
        auto& [exec, next, value] = mTask.coroutine.promise();
        result = value;
        if (exec)
            std::rethrow_exception(exec);
        mTask = {};
        return true;
    }

    void MDRHeadphones::SendCommandImpl(Span<const UInt8> command, MDRDataType type, MDRCommandSeqNumber seq)
    {
        MDRBuffer packed = MDRPackCommand(type, seq, command);
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

    // Sigh.
    // NOLINTBEGIN
#pragma region Tasks
    // XXX: Maybe RequestInitV2? Maybe someone with a pair of XM4s could help with V1 :D
    MDRTask MDRHeadphones::RequestInit()
    {
        SendCommandACK(v2::t1::ConnectGetProtocolInfo);
        co_await Await(AWAIT_PROTOCOL_INFO);
        MDR_CHECK_MSG(mProtocol.hasTable1, "Device doesn't support MDR V2 Table 1");
        SendCommandACK(v2::t1::ConnectGetCapabilityInfo);

        /* Device Info */
        SendCommandACK(v2::t1::ConnectGetDeviceInfo, {.deviceInfoType = v2::t1::DeviceInfoType::FW_VERSION});
        SendCommandACK(v2::t1::ConnectGetDeviceInfo, {.deviceInfoType = v2::t1::DeviceInfoType::MODEL_NAME});
        SendCommandACK(v2::t1::ConnectGetDeviceInfo, {.deviceInfoType = v2::t1::DeviceInfoType::SERIES_AND_COLOR_INFO});

        // Following are cached by the offical app based on the MAC address
        {
            /* Support Functions */
            SendCommandACK(v2::t1::ConnectGetSupportFunction);
            co_await Await(AWAIT_SUPPORT_FUNCTION);
            if (mProtocol.hasTable2)
            {
                SendCommandACK(v2::t2::ConnectGetSupportFunction);
                co_await Await(AWAIT_SUPPORT_FUNCTION);
            }

            /* General Setting */
            v2::t1::DisplayLanguage lang = v2::t1::DisplayLanguage::ENGLISH;
            if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::GENERAL_SETTING_1))
            {
                SendCommandACK(v2::t1::GsGetCapability, {
                               .type = v2::t1::GsInquiredType::GENERAL_SETTING1, .displayLanguage = lang
                               });
                SendCommandACK(v2::t1::GsGetParam, {
                               .type = v2::t1::GsInquiredType::GENERAL_SETTING1
                               });
            }
            if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::GENERAL_SETTING_2))
            {
                SendCommandACK(v2::t1::GsGetCapability, {
                               .type = v2::t1::GsInquiredType::GENERAL_SETTING2, .displayLanguage = lang
                               });
                SendCommandACK(v2::t1::GsGetParam, {
                               .type = v2::t1::GsInquiredType::GENERAL_SETTING2
                               });
            }
            if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::GENERAL_SETTING_3))
            {
                SendCommandACK(v2::t1::GsGetCapability, {
                               .type = v2::t1::GsInquiredType::GENERAL_SETTING3, .displayLanguage = lang
                               });
                SendCommandACK(v2::t1::GsGetParam, {
                               .type = v2::t1::GsInquiredType::GENERAL_SETTING3
                               });
            }
            if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::GENERAL_SETTING_4))
            {
                SendCommandACK(v2::t1::GsGetCapability, {
                               .type = v2::t1::GsInquiredType::GENERAL_SETTING4, .displayLanguage = lang
                               });
                SendCommandACK(v2::t1::GsGetParam, {
                               .type = v2::t1::GsInquiredType::GENERAL_SETTING4
                               });
            }

            /* DSEE */
            if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::UPSCALING_AUTO_OFF))
                SendCommandACK(v2::t1::AudioGetCapability, {
                           .type = v2::t1::AudioInquiredType::UPSCALING
                           });
        }
        /* [NO ACK] Receive alerts for certain operations like toggling multipoint */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::FIXED_MESSAGE))
            SendCommandImpl<v2::t1::AlertSetStatusFixedMessage>({.status = v2::MessageMdrV2EnableDisable::ENABLE});

        /* Codec Type */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::CODEC_INDICATOR))
            SendCommandACK(v2::t1::CommonGetStatus, { .type = v2::t1::CommonInquiredType::AUDIO_CODEC });

        /* Playback Metadata */
        SendCommandACK(v2::t1::GetPlayParam,
                       { .type = v2::t1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT });

        /* Playback Volume */
        SendCommandACK(v2::t1::GetPlayParam, { .type = v2::t1::PlayInquiredType::MUSIC_VOLUME });

        /* Play/Pause */
        if (mSupport.contains(
            v2::MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            SendCommandACK(v2::t1::NcAsmGetParam,
                           { .type = v2::t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS});
        }
        else if (mSupport.contains(
            v2::MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
        {
            SendCommandACK(v2::t1::NcAsmGetParam,
                           { .type = v2::t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA});
        }
        else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            SendCommandACK(v2::t1::NcAsmGetParam, { .type = v2::t1::NcAsmInquiredType::ASM_SEAMLESS});
        }

        /* Pairing Management */
        constexpr v2::MessageMdrV2FunctionType_Table2 kPairingFunctions[] = {
            v2::MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT,
            v2::MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT,
            v2::MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE
        };
        if (std::ranges::any_of(kPairingFunctions, [&](auto x) { return mSupport.contains(x); }))
        {
            /* Pairing Mode */
            SendCommandACK(v2::t2::PeripheralGetStatus,
                           {.type = v2::t2::PeripheralInquiredType::
                           PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE
                           });

            /* Connected Devices */
            SendCommandACK(v2::t2::PeripheralGetParam,
                           {.type = v2::t2::PeripheralInquiredType::
                           PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE
                           });
        }

        /* Speak To Chat */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::SMART_TALKING_MODE_TYPE2))
        {
            SendCommandACK(v2::t1::SystemGetParam, {.type = v2::t1::SystemInquiredType::SMART_TALKING_MODE_TYPE2});
            SendCommandACK(v2::t1::SystemGetExtParam, {.type = v2::t1::SystemInquiredType::SMART_TALKING_MODE_TYPE2});
        }

        /* Listening Mode */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::LISTENING_OPTION))
        {
            SendCommandACK(v2::t1::AudioGetParam, {.type = v2::t1::AudioInquiredType::BGM_MODE});
            SendCommandACK(v2::t1::AudioGetParam, {.type = v2::t1::AudioInquiredType::UPMIX_CINEMA});
        }

        /* Equalizer */
        SendCommandACK(v2::t1::EqEbbGetStatus, {.type = v2::t1::EqEbbInquiredType::PRESET_EQ});

        /* Connection Quality */
        if (mSupport.contains(
            v2::MessageMdrV2FunctionType_Table1::CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY))
            SendCommandACK(v2::t1::AudioGetParam, {.type = v2::t1::AudioInquiredType::CONNECTION_MODE});

        /* DSEE */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::UPSCALING_AUTO_OFF))
        {
            SendCommandACK(v2::t1::AudioGetStatus, {.type = v2::t1::AudioInquiredType::UPSCALING});
            SendCommandACK(v2::t1::AudioGetParam, {.type = v2::t1::AudioInquiredType::UPSCALING});
        }

        /* Touch Sensor */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::ASSIGNABLE_SETTING))
            SendCommandACK(v2::t1::SystemGetParam, {.type = v2::t1::SystemInquiredType::ASSIGNABLE_SETTINGS });

        /* NC/AMB Toggle */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_CONTROL_MODE_SELECT))
            SendCommandACK(v2::t1::NcAsmGetParam, {.type = v2::t1::NcAsmInquiredType::NC_AMB_TOGGLE });

        /* Head Gesture */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::HEAD_GESTURE_ON_OFF_TRAINING))
            SendCommandACK(v2::t1::SystemGetParam, {.type = v2::t1::SystemInquiredType::HEAD_GESTURE_ON_OFF });

        /* Auto Power Off */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF))
        {
            SendCommandACK(v2::t1::PowerGetParam, {.type = v2::t1::PowerInquiredType::AUTO_POWER_OFF});
        }
        else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
        {
            SendCommandACK(v2::t1::PowerGetParam,
                           {.type = v2::t1::PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION});
        }

        /* Pause when headphones are removed */
        SendCommandACK(v2::t1::SystemGetParam, {.type = v2::t1::SystemInquiredType::PLAYBACK_CONTROL_BY_WEARING });

        /* Voice Guidance */
        if (mProtocol.hasTable2)
        {
            // Enabled
            SendCommandACK(v2::t2::VoiceGuidanceGetParam,
                           {
                           .type = v2::t2::VoiceGuidanceInquiredType::
                           MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH
                           });
            // Volume
            SendCommandACK(v2::t2::VoiceGuidanceGetParam, {.type = v2::t2::VoiceGuidanceInquiredType::VOLUME});
        }

        /* LOG_SET_STATUS */
        // XXX: Figure out if there's a struct for this in the app
        constexpr UInt8 kLogSetStatusCommand[] = {
            static_cast<UInt8>(v2::t1::Command::LOG_SET_STATUS),
            0x01, 0x00
        };
        SendCommandImpl(kLogSetStatusCommand, MDRDataType::DATA_MDR, mSeqNumber);
        co_await Await(AWAIT_ACK);
        co_return MDR_HEADPHONES_TASK_INIT_OK;
    }

    MDRTask MDRHeadphones::RequestSync()
    {
        /* Single Battery */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(v2::t1::PowerGetStatus, {.type = v2::t1::PowerInquiredType::BATTERY});
        }
        else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(v2::t1::PowerGetStatus, {.type = v2::t1::PowerInquiredType::BATTERY_WITH_THRESHOLD});
        }

        /* L + R Battery */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::LEFT_RIGHT_BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(v2::t1::PowerGetStatus, {.type = v2::t1::PowerInquiredType::LEFT_RIGHT_BATTERY});
        }
        else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::LR_BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(v2::t1::PowerGetStatus, {.type = v2::t1::PowerInquiredType::LR_BATTERY_WITH_THRESHOLD});
        }

        /* Case Battery */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(v2::t1::PowerGetStatus, {.type = v2::t1::PowerInquiredType::CRADLE_BATTERY});
        }
        else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(v2::t1::PowerGetStatus, {.type = v2::t1::PowerInquiredType::CRADLE_BATTERY_WITH_THRESHOLD});
        }

        /* Sound Pressure */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table2::SAFE_LISTENING_HBS_1))
        {
            SendCommandACK(v2::t2::SafeListeningGetExtendedParam,
                           {.type = v2::t2::SafeListeningInquiredType::SAFE_LISTENING_HBS_1});
        }
        else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table2::SAFE_LISTENING_HBS_2))
        {
            SendCommandACK(v2::t2::SafeListeningGetExtendedParam,
                           {.type = v2::t2::SafeListeningInquiredType::SAFE_LISTENING_HBS_2});
        }
        else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table2::SAFE_LISTENING_TWS_1))
        {
            SendCommandACK(v2::t2::SafeListeningGetExtendedParam,
                           {.type = v2::t2::SafeListeningInquiredType::SAFE_LISTENING_TWS_1});
        }
        else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table2::SAFE_LISTENING_TWS_2))
        {
            SendCommandACK(v2::t2::SafeListeningGetExtendedParam,
                           {.type = v2::t2::SafeListeningInquiredType::SAFE_LISTENING_TWS_2});
        }
        co_return MDR_HEADPHONES_TASK_SYNC_OK;
    }

    MDRTask MDRHeadphones::RequestCommit()
    {
        /* Shutdown */
        if (mShutdown.dirty())
        {
            using namespace v2::t1;
            if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::POWER_OFF) && mShutdown.desired)
            {
                SendCommandACK(PowerSetStatusPowerOff);
            } else
                mShutdown.overwrite(false);
        }
        /* NC/ASM */
        if (mNcAsmAmbientLevel.dirty() || mNcAsmEnabled.dirty() || mNcAsmMode.dirty() ||
            mNcAsmFocusOnVoice.dirty() || mNcAsmAutoAsmEnabled.dirty() || mNcAsmNoiseAdaptiveSensitivity.dirty())
        {
            using namespace v2::t1;
            if (mSupport.contains(
                v2::MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
            {

                NcAsmParamModeNcDualModeSwitchAsmSeamlessNa res;
                res.base.command = Command::NCASM_SET_PARAM;
                res.base.valueChangeStatus = ValueChangeStatus::CHANGED;
                res.base.ncAsmTotalEffect = mNcAsmEnabled.desired ? NcAsmOnOffValue::ON : NcAsmOnOffValue::OFF;
                res.ncAsmMode = mNcAsmMode.desired;
                res.ambientSoundMode = mNcAsmFocusOnVoice.desired ? AmbientSoundMode::VOICE : AmbientSoundMode::NORMAL;
                res.ambientSoundLevelValue = mNcAsmAmbientLevel.desired;
                res.noiseAdaptiveOnOffValue = mNcAsmAutoAsmEnabled.desired ? NcAsmOnOffValue::ON : NcAsmOnOffValue::OFF;
                res.noiseAdaptiveSensitivitySettings = mNcAsmNoiseAdaptiveSensitivity.desired;
                SendCommandACK(NcAsmParamModeNcDualModeSwitchAsmSeamlessNa, res);
            }
            else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
            {
                NcAsmParamAsmSeamless res;
                res.base.command = Command::NCASM_SET_PARAM;
                res.base.valueChangeStatus = ValueChangeStatus::CHANGED;
                res.base.ncAsmTotalEffect = mNcAsmEnabled.desired ? NcAsmOnOffValue::ON : NcAsmOnOffValue::OFF;
                res.ambientSoundMode = mNcAsmFocusOnVoice.desired ? AmbientSoundMode::VOICE : AmbientSoundMode::NORMAL;
                res.ambientSoundLevelValue = mNcAsmAmbientLevel.desired;
                SendCommandACK(NcAsmParamAsmSeamless, res);
            }
            else
            {
                NcAsmParamModeNcDualModeSwitchAsmSeamless res;
                res.base.command = Command::NCASM_SET_PARAM;
                res.base.valueChangeStatus = ValueChangeStatus::CHANGED;
                res.base.ncAsmTotalEffect = mNcAsmEnabled.desired ? NcAsmOnOffValue::ON : NcAsmOnOffValue::OFF;
                res.ncAsmMode = mNcAsmMode.desired,
                    res.ambientSoundMode = mNcAsmFocusOnVoice.desired
                    ? AmbientSoundMode::VOICE
                    : AmbientSoundMode::NORMAL;
                res.ambientSoundLevelValue = mNcAsmAmbientLevel.desired;
                SendCommandACK(NcAsmParamModeNcDualModeSwitchAsmSeamless, res);
            }
            mNcAsmAmbientLevel.commit(), mNcAsmEnabled.commit(), mNcAsmMode.commit();
            mNcAsmFocusOnVoice.commit(), mNcAsmAutoAsmEnabled.commit(), mNcAsmNoiseAdaptiveSensitivity.commit();
        }

        /* NC/AMB Mode */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_CONTROL_MODE_SELECT))
        {
            using namespace v2::t1;
            if (mNcAsmButtonFunction.dirty())
            {
                NcAsmParamNcAmbToggle res;
                res.base.command = Command::NCASM_SET_PARAM;
                res.function = mNcAsmButtonFunction.desired;
                SendCommandACK(NcAsmParamNcAmbToggle, res);
                mNcAsmButtonFunction.commit();
            }
        }
        /* Volume */
        if (mPlayVolume.dirty())
        {
            using namespace v2::t1;
            PlayParamPlaybackControllerVolume res;
            res.base.command = Command::PLAY_SET_PARAM;
            res.volumeValue = mPlayVolume.desired;
            SendCommandACK(PlayParamPlaybackControllerVolume, res);
            mPlayVolume.commit();
        }

        /* Multipoint Switch */
        if (mMultipointDeviceMac.dirty())
        {
            using namespace v2::t2;
            PeripheralSetExtendedParamSourceSwitchControl res;
            res.base.command = Command::PERI_SET_PARAM;
            if (mMultipointDeviceMac.desired.length() != 17)
                mMultipointDeviceMac.overwrite("");
            else
            {
                std::memcpy(res.targetBdAddress.data(), mMultipointDeviceMac.desired.data(), 17);
                SendCommandACK(PeripheralSetExtendedParamSourceSwitchControl, res);
                // Do not commit - let corrosponding RET command resolve this.
            }
        }

        /* Pairing Mode */
        if (mPairingMode.dirty())
        {
            using namespace v2::t2;
            PeripheralStatusPairingDeviceManagementCommon res;
            res.base.command = Command::PERI_SET_STATUS;
            res.base.type = PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE;
            res.btMode = mPairingMode.desired
                ? PeripheralBluetoothMode::INQUIRY_SCAN_MODE
                : PeripheralBluetoothMode::NORMAL_MODE;
            res.enableDisableStatus = v2::MessageMdrV2EnableDisable::ENABLE;
            SendCommandACK(PeripheralStatusPairingDeviceManagementCommon, res);
            mPairingMode.commit();
        }

        /* STC */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::SMART_TALKING_MODE_TYPE2))
        {
            using namespace v2::t1;
            if (mSpeakToChatEnabled.dirty())
            {
                SystemParamSmartTalking res;
                res.base.command = Command::SYSTEM_SET_PARAM;
                res.base.type = SystemInquiredType::SMART_TALKING_MODE_TYPE2;
                res.onOffValue = mSpeakToChatEnabled.desired
                    ? v2::MessageMdrV2EnableDisable::ENABLE
                    : v2::MessageMdrV2EnableDisable::DISABLE;
                res.previewModeOnOffValue = v2::MessageMdrV2EnableDisable::DISABLE;
                SendCommandACK(SystemParamSmartTalking, res);
                mSpeakToChatEnabled.commit();
            }

            if (mSpeakToChatDetectSensitivity.dirty() || mSpeakToModeOutTime.dirty())
            {
                SystemExtParamSmartTalkingMode2 res;
                res.base.command = Command::SYSTEM_SET_EXT_PARAM;
                res.detectSensitivity = mSpeakToChatDetectSensitivity.desired;
                res.modeOffTime = mSpeakToModeOutTime.desired;
                SendCommandACK(SystemExtParamSmartTalkingMode2, res);
                mSpeakToChatDetectSensitivity.commit(), mSpeakToModeOutTime.commit();
            }
        }

        /* Listening Mode */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::LISTENING_OPTION))
        {
            using namespace v2::t1;
            if (mBGMModeEnabled.dirty() || mBGMModeRoomSize.dirty())
            {
                AudioParamBGMMode res;
                res.base.command = Command::AUDIO_SET_PARAM;
                res.base.type = AudioInquiredType::BGM_MODE;
                res.onOffSettingValue = mBGMModeEnabled.desired
                    ? v2::MessageMdrV2EnableDisable::ENABLE
                    : v2::MessageMdrV2EnableDisable::DISABLE;
                res.targetRoomSize = mBGMModeRoomSize.desired;
                SendCommandACK(AudioParamBGMMode, res);
                mBGMModeEnabled.commit(), mBGMModeRoomSize.commit();
            }
            if (mUpmixCinemaEnabled.dirty())
            {
                AudioParamUpmixCinema res;
                res.base.command = Command::AUDIO_SET_PARAM;
                res.onOffSettingValue = mUpmixCinemaEnabled.desired
                    ? v2::MessageMdrV2EnableDisable::ENABLE
                    : v2::MessageMdrV2EnableDisable::DISABLE;
                SendCommandACK(AudioParamUpmixCinema, res);
                mUpmixCinemaEnabled.commit();
            }
        }

        /* EQ */
        if (mEqPresetId.dirty())
        {
            using namespace v2::t1;
            EqEbbParamEq res;
            res.base.command = Command::EQEBB_SET_PARAM;
            res.base.type = EqEbbInquiredType::PRESET_EQ;
            res.presetId = mEqPresetId.desired;
            SendCommandACK(EqEbbParamEq, res);
            mEqPresetId.commit();
            // Ask for a equalizer param update afterwards
            SendCommandACK(EqEbbGetParam);
        }
        if (mEqConfig.dirty())
        {
            using namespace v2::t1;
            EqEbbParamEq res;
            res.base.command = Command::EQEBB_SET_PARAM;
            res.base.type = EqEbbInquiredType::PRESET_EQ;
            res.presetId = mEqPresetId.current;
            res.bands.value.resize(mEqConfig.desired.size());
            int eqOffset = 0;
            if (res.bands.size() == 0)
            {
                mEqConfig.commit();
            }
            else
            {
                if (res.bands.size() == 5)
                    eqOffset = 10;
                if (res.bands.size() == 10)
                    eqOffset = 6;
                MDR_CHECK_MSG(eqOffset, "mEqConfig size can only be 0, 5, or 10. Got {}.", mEqConfig.desired.size());
                for (size_t i = 0; i < mEqConfig.desired.size(); i++)
                    res.bands.value[i] = mEqConfig.desired[i] + eqOffset;
                SendCommandACK(EqEbbParamEq, res);
                mEqConfig.commit();
            }
        }

        /* Connection Quality */
        if (mSupport.
            contains(v2::MessageMdrV2FunctionType_Table1::CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY))
        {
            if (mAudioPriorityMode.dirty())
            {
                using namespace v2::t1;
                AudioParamConnection res;
                res.base.command = Command::AUDIO_SET_PARAM;
                res.settingValue = mAudioPriorityMode.desired;
                SendCommandACK(AudioParamConnection, res);
                mAudioPriorityMode.commit();
            }
        }

        /* DSEE */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::UPSCALING_AUTO_OFF))
        {
            if (mUpscalingEnabled.dirty())
            {
                using namespace v2::t1;
                AudioParamUpscaling res;
                res.base.command = Command::AUDIO_SET_PARAM;
                res.settingValue = mUpscalingEnabled.desired ? UpscalingTypeAutoOff::AUTO : UpscalingTypeAutoOff::OFF;
                SendCommandACK(AudioParamUpscaling, res);
                mUpscalingEnabled.commit();
            }
        }

        /* Touch Functions */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::ASSIGNABLE_SETTING))
        {
            if (mTouchFunctionLeft.dirty() || mTouchFunctionRight.dirty())
            {
                using namespace v2::t1;
                SystemParamAssignableSettings res;
                res.base.command = Command::SYSTEM_SET_PARAM;
                res.presets.value = {mTouchFunctionLeft.desired, mTouchFunctionRight.desired};
                SendCommandACK(SystemParamAssignableSettings, res);
                mTouchFunctionLeft.commit(), mTouchFunctionRight.commit();
            }
        }

        /* Head Gesture */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::UPSCALING_AUTO_OFF))
        {
            if (mHeadGestureEnabled.dirty())
            {
                using namespace v2::t1;
                SystemParamCommon res;
                res.base.command = Command::SYSTEM_SET_PARAM;
                res.base.type = SystemInquiredType::HEAD_GESTURE_ON_OFF;
                res.settingValue = mHeadGestureEnabled.desired
                    ? v2::MessageMdrV2EnableDisable::ENABLE
                    : v2::MessageMdrV2EnableDisable::DISABLE;
                SendCommandACK(SystemParamCommon, res);
                mHeadGestureEnabled.commit();
            }
        }

        /* Auto Power Off */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF))
        {
            using namespace v2::t1;
            if (mPowerAutoOff.dirty())
            {
                PowerParamAutoPowerOff res;
                res.base.command = Command::SYSTEM_SET_PARAM;
                res.currentPowerOffElements = mPowerAutoOff.desired;
                res.lastSelectPowerOffElements = AutoPowerOffElements::POWER_OFF_IN_5_MIN;
                SendCommandACK(PowerParamAutoPowerOff, res);
                mPowerAutoOff.commit();
            }
        }
        else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
        {
            using namespace v2::t1;
            if (mPowerAutoOffWearingDetection.dirty())
            {
                PowerParamAutoPowerOffWithWearingDetection res;
                res.base.command = Command::SYSTEM_SET_PARAM;
                res.currentPowerOffElements = mPowerAutoOffWearingDetection.desired;
                res.lastSelectPowerOffElements = AutoPowerOffWearingDetectionElements::POWER_OFF_IN_5_MIN;
                SendCommandACK(PowerParamAutoPowerOffWithWearingDetection, res);
                mPowerAutoOffWearingDetection.commit();
            }
        }

        /* Pause when device is removed */
        if (mSupport.contains(
            v2::MessageMdrV2FunctionType_Table1::PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF))
        {
            using namespace v2::t1;
            if (mAutoPauseEnabled.dirty())
            {
                SystemParamCommon res;
                res.base.command = Command::SYSTEM_SET_PARAM;
                res.base.type = SystemInquiredType::PLAYBACK_CONTROL_BY_WEARING;
                res.settingValue = mAutoPauseEnabled.desired
                    ? v2::MessageMdrV2EnableDisable::ENABLE
                    : v2::MessageMdrV2EnableDisable::DISABLE;
                SendCommandACK(SystemParamCommon, res);
                mAutoPauseEnabled.commit();
            }
        }

        if (mVoiceGuidanceEnabled.dirty())
        {
            using namespace v2::t2;
            VoiceGuidanceParamSettingMtk res;
            res.base.command = Command::VOICE_GUIDANCE_SET_PARAM;
            res.base.type = VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH;
            res.settingValue = mVoiceGuidanceEnabled.desired
                ? v2::MessageMdrV2OnOffSettingValue::ON
                : v2::MessageMdrV2OnOffSettingValue::OFF;
            SendCommandACK(VoiceGuidanceParamSettingMtk, res);
            mVoiceGuidanceVolume.commit();
        }

        /* Voice Guidance */
        if (mSupport.contains(
            v2::MessageMdrV2FunctionType_Table2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT))
        {
            if (mVoiceGuidanceVolume.dirty())
            {
                using namespace v2::t2;
                VoiceGuidanceSetParamVolume res;
                res.base.command = Command::VOICE_GUIDANCE_SET_PARAM;
                res.base.type = VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH;
                res.volumeValue = mVoiceGuidanceVolume.desired;
                res.feedbackSound = v2::MessageMdrV2OnOffSettingValue::ON;
                SendCommandACK(VoiceGuidanceSetParamVolume, res);
                mVoiceGuidanceVolume.commit();
            }
        }

        /* General Settings */
        {
            using namespace v2::t1;
            if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::GENERAL_SETTING_1))
            {
                if (mGsParamBool1.dirty())
                {
                    GsParamBoolean res;
                    res.base.command = Command::SYSTEM_SET_PARAM;
                    res.base.type = GsInquiredType::GENERAL_SETTING1;
                    res.settingValue = mGsParamBool1.desired ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsParamBoolean, res);
                    mGsParamBool1.commit();
                }
            }
            if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::GENERAL_SETTING_2))
            {
                if (mGsParamBool2.dirty())
                {
                    GsParamBoolean res;
                    res.base.command = Command::SYSTEM_SET_PARAM;
                    res.base.type = GsInquiredType::GENERAL_SETTING2;
                    res.settingValue = mGsParamBool2.desired ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsParamBoolean, res);
                    mGsParamBool2.commit();
                }
            }
            if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::GENERAL_SETTING_3))
            {
                if (mGsParamBool3.dirty())
                {
                    GsParamBoolean res;
                    res.base.command = Command::SYSTEM_SET_PARAM;
                    res.base.type = GsInquiredType::GENERAL_SETTING3;
                    res.settingValue = mGsParamBool3.desired ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsParamBoolean, res);
                    mGsParamBool3.commit();
                }
            }
            if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::GENERAL_SETTING_4))
            {
                if (mGsParamBool4.dirty())
                {
                    GsParamBoolean res;
                    res.base.command = Command::SYSTEM_SET_PARAM;
                    res.base.type = GsInquiredType::GENERAL_SETTING4;
                    res.settingValue = mGsParamBool4.desired ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsParamBoolean, res);
                    mGsParamBool4.commit();
                }
            }
        }

        /* Safe Listening */
        if (mSafeListeningPreviewMode.dirty())
        {
            using namespace v2::t2;
            if (mSupport.contains(v2::MessageMdrV2FunctionType_Table2::SAFE_LISTENING_HBS_1))
            {
                SafeListeningSetParamSL res;
                res.base.type = SafeListeningInquiredType::SAFE_LISTENING_HBS_1;
                res.previewMode = v2::MessageMdrV2EnableDisable::DISABLE;
                res.safeListeningMode = mSafeListeningPreviewMode.desired
                    ? v2::MessageMdrV2EnableDisable::ENABLE
                    : v2::MessageMdrV2EnableDisable::DISABLE;
                SendCommandACK(SafeListeningSetParamSL, res);
                mSafeListeningPreviewMode.commit();
            }
            else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table2::SAFE_LISTENING_HBS_2))
            {
                SafeListeningSetParamSL res;
                res.base.type = SafeListeningInquiredType::SAFE_LISTENING_HBS_2;
                res.previewMode = v2::MessageMdrV2EnableDisable::DISABLE;
                res.safeListeningMode = mSafeListeningPreviewMode.desired
                    ? v2::MessageMdrV2EnableDisable::ENABLE
                    : v2::MessageMdrV2EnableDisable::DISABLE;
                SendCommandACK(SafeListeningSetParamSL, res);
                mSafeListeningPreviewMode.commit();
            }
            else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table2::SAFE_LISTENING_TWS_1))
            {
                SafeListeningSetParamSL res;
                res.base.type = SafeListeningInquiredType::SAFE_LISTENING_TWS_1;
                res.previewMode = v2::MessageMdrV2EnableDisable::DISABLE;
                res.safeListeningMode = mSafeListeningPreviewMode.desired
                    ? v2::MessageMdrV2EnableDisable::ENABLE
                    : v2::MessageMdrV2EnableDisable::DISABLE;
                SendCommandACK(SafeListeningSetParamSL, res);
                mSafeListeningPreviewMode.commit();
            }
            else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table2::SAFE_LISTENING_TWS_2))
            {
                SafeListeningSetParamSL res;
                res.base.type = SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
                res.previewMode = v2::MessageMdrV2EnableDisable::DISABLE;
                res.safeListeningMode = mSafeListeningPreviewMode.desired
                    ? v2::MessageMdrV2EnableDisable::ENABLE
                    : v2::MessageMdrV2EnableDisable::DISABLE;
                SendCommandACK(SafeListeningSetParamSL, res);
                mSafeListeningPreviewMode.commit();
            }
        }
        co_return MDR_HEADPHONES_TASK_COMMIT_OK;
    }
#pragma endregion
    // NOLINTEND
}

#pragma region C Exports
extern "C" {
const char* mdrResultString(int err)
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
    default:
        return "Unknown";
    }
}

int mdrConnectionConnect(MDRConnection* conn, const char* macAddress, const char* serviceUUID)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->connect(conn->user, macAddress, serviceUUID);
}

void mdrConnectionDisconnect(MDRConnection* conn)
{
    if (!conn)
        return;
    return conn->disconnect(conn->user);
}

int mdrConnectionRecv(MDRConnection* conn, char* dst, int size, int* pReceived)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->recv(conn->user, dst, size, pReceived);
}

int mdrConnectionSend(MDRConnection* conn, const char* src, int size, int* pSent)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->send(conn->user, src, size, pSent);
}

int mdrConnectionPoll(MDRConnection* conn, int timeout)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->poll(conn->user, timeout);
}

int mdrConnectionGetDevicesList(MDRConnection* conn, MDRDeviceInfo** ppList, int* pCount)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->getDevicesList(conn->user, ppList, pCount);
}

int mdrConnectionFreeDevicesList(MDRConnection* conn, MDRDeviceInfo** ppList)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->freeDevicesList(conn->user, ppList);
}

const char* mdrConnectionGetLastError(MDRConnection* conn)
{
    if (!conn)
        return "FIXME: nullptr conn";
    return conn->getLastError(conn->user);
}

MDRHeadphones* mdrHeadphonesCreate(MDRConnection* conn)
{
    return reinterpret_cast<MDRHeadphones*>(new mdr::MDRHeadphones(conn));
}

void mdrHeadphonesDestroy(MDRHeadphones* h)
{
    delete reinterpret_cast<mdr::MDRHeadphones*>(h);
}

int mdrHeadphonesPollEvents(MDRHeadphones* p)
{
    auto h = reinterpret_cast<mdr::MDRHeadphones*>(p);
    return h->PollEvents();
}

int mdrHeadphonesRequestIsReady(MDRHeadphones* p)
{
    auto h = reinterpret_cast<mdr::MDRHeadphones*>(p);
    if (!h->IsReady())
        return MDR_RESULT_INPROGRESS;
    return MDR_RESULT_OK;
}

int mdrHeadphonesRequestInit(MDRHeadphones* p)
{
    auto h = reinterpret_cast<mdr::MDRHeadphones*>(p);
    return h->Invoke(h->RequestInit());
}

int mdrHeadphonesRequestSync(MDRHeadphones* p)
{
    auto h = reinterpret_cast<mdr::MDRHeadphones*>(p);
    return h->Invoke(h->RequestSync());
}

int mdrHeadphonesIsDirty(MDRHeadphones* p)
{
    auto h = reinterpret_cast<mdr::MDRHeadphones*>(p);
    if (h->IsDirty())
    {
        return MDR_RESULT_INPROGRESS;
    }
    return MDR_RESULT_OK;
}

int mdrHeadphonesRequestCommit(MDRHeadphones* p)
{
    auto h = reinterpret_cast<mdr::MDRHeadphones*>(p);
    return h->Invoke(h->RequestCommit());
}

const char* mdrHeadphonesGetLastError(MDRHeadphones* p)
{
    auto h = reinterpret_cast<mdr::MDRHeadphones*>(p);
    return h->GetLastError();
}
}
#pragma endregion
