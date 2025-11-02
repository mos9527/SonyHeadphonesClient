#pragma clang diagnostic ignored "-Wshadow"
#include <algorithm>
#include <mdr/Headphones.hpp>
// NOLINTBEGIN
namespace mdr
{
    using namespace v2;
    MDRTask MDRHeadphones::RequestInitV2()
    {
        SendCommandACK(t1::ConnectGetProtocolInfo);
        co_await Await(AWAIT_PROTOCOL_INFO);
        MDR_CHECK_MSG(mProtocol.hasTable1, "Device doesn't support MDR V2 Table 1");
        SendCommandACK(t1::ConnectGetCapabilityInfo);

        /* Device Info */
        SendCommandACK(t1::ConnectGetDeviceInfo, {.deviceInfoType = t1::DeviceInfoType::FW_VERSION});
        SendCommandACK(t1::ConnectGetDeviceInfo, {.deviceInfoType = t1::DeviceInfoType::MODEL_NAME});
        SendCommandACK(t1::ConnectGetDeviceInfo, {.deviceInfoType = t1::DeviceInfoType::SERIES_AND_COLOR_INFO});

        // Following are cached by the offical app based on the MAC address
        {
            /* Support Functions */
            SendCommandACK(t1::ConnectGetSupportFunction);
            co_await Await(AWAIT_SUPPORT_FUNCTION);
            if (mProtocol.hasTable2)
            {
                SendCommandACK(t2::ConnectGetSupportFunction);
                co_await Await(AWAIT_SUPPORT_FUNCTION);
            }

            /* General Setting */
            t1::DisplayLanguage lang = t1::DisplayLanguage::ENGLISH;
            if (mSupport.contains(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_1))
            {
                SendCommandACK(t1::GsGetCapability, {
                               .type = t1::GsInquiredType::GENERAL_SETTING1, .displayLanguage = lang
                               });
                SendCommandACK(t1::GsGetParam, {
                               .type = t1::GsInquiredType::GENERAL_SETTING1
                               });
            }
            if (mSupport.contains(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_2))
            {
                SendCommandACK(t1::GsGetCapability, {
                               .type = t1::GsInquiredType::GENERAL_SETTING2, .displayLanguage = lang
                               });
                SendCommandACK(t1::GsGetParam, {
                               .type = t1::GsInquiredType::GENERAL_SETTING2
                               });
            }
            if (mSupport.contains(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_3))
            {
                SendCommandACK(t1::GsGetCapability, {
                               .type = t1::GsInquiredType::GENERAL_SETTING3, .displayLanguage = lang
                               });
                SendCommandACK(t1::GsGetParam, {
                               .type = t1::GsInquiredType::GENERAL_SETTING3
                               });
            }
            if (mSupport.contains(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_4))
            {
                SendCommandACK(t1::GsGetCapability, {
                               .type = t1::GsInquiredType::GENERAL_SETTING4, .displayLanguage = lang
                               });
                SendCommandACK(t1::GsGetParam, {
                               .type = t1::GsInquiredType::GENERAL_SETTING4
                               });
            }

            /* DSEE */
            if (mSupport.contains(MessageMdrV2FunctionType_Table1::UPSCALING_AUTO_OFF))
                SendCommandACK(t1::AudioGetCapability, {
                           .type = t1::AudioInquiredType::UPSCALING
                           });
        }
        /* Receive alerts for certain operations like toggling multipoint */
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::FIXED_MESSAGE))
            SendCommandACK(t1::AlertSetStatusFixedMessage, { .status = MessageMdrV2EnableDisable::ENABLE});

        /* Codec Type */
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::CODEC_INDICATOR))
            SendCommandACK(t1::CommonGetStatus, { .type = t1::CommonInquiredType::AUDIO_CODEC });

        /* Playback Metadata */
        SendCommandACK(t1::GetPlayParam,
                       { .type = t1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT });

        /* Playback Volume */
        SendCommandACK(t1::GetPlayParam, { .type = t1::PlayInquiredType::MUSIC_VOLUME });

        /* Play/Pause */
        SendCommandACK(t1::GetPlayStatus,
                       { .type = t1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT });

        /* NC/AMB */
        if (mSupport.contains(
            MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            SendCommandACK(t1::NcAsmGetParam,
                           { .type = t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS});
        }
        else if (mSupport.contains(
            MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
        {
            SendCommandACK(t1::NcAsmGetParam,
                           { .type = t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA});
        }
        else if (mSupport.contains(MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            SendCommandACK(t1::NcAsmGetParam, { .type = t1::NcAsmInquiredType::ASM_SEAMLESS});
        }

        /* Pairing Management */
        constexpr MessageMdrV2FunctionType_Table2 kPairingFunctions[] = {
            MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT,
            MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT,
            MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE
        };
        if (std::ranges::any_of(kPairingFunctions, [&](auto x) { return mSupport.contains(x); }))
        {
            /* Pairing Mode */
            SendCommandACK(t2::PeripheralGetStatus,
                           {.type = t2::PeripheralInquiredType::
                           PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE
                           });

            /* Connected Devices */
            SendCommandACK(t2::PeripheralGetParam,
                           {.type = t2::PeripheralInquiredType::
                           PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE
                           });
        }

        /* Speak To Chat */
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::SMART_TALKING_MODE_TYPE2))
        {
            SendCommandACK(t1::SystemGetParam, {.type = t1::SystemInquiredType::SMART_TALKING_MODE_TYPE2});
            SendCommandACK(t1::SystemGetExtParam, {.type = t1::SystemInquiredType::SMART_TALKING_MODE_TYPE2});
        }

        /* Listening Mode */
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::LISTENING_OPTION))
        {
            SendCommandACK(t1::AudioGetParam, {.type = t1::AudioInquiredType::BGM_MODE});
            SendCommandACK(t1::AudioGetParam, {.type = t1::AudioInquiredType::UPMIX_CINEMA});
        }

        /* Equalizer */
        SendCommandACK(t1::EqEbbGetStatus, {.type = t1::EqEbbInquiredType::PRESET_EQ});
        SendCommandACK(t1::EqEbbGetParam);

        /* Connection Quality */
        if (mSupport.contains(
            MessageMdrV2FunctionType_Table1::CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY))
            SendCommandACK(t1::AudioGetParam, {.type = t1::AudioInquiredType::CONNECTION_MODE});

        /* DSEE */
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::UPSCALING_AUTO_OFF))
        {
            SendCommandACK(t1::AudioGetStatus, {.type = t1::AudioInquiredType::UPSCALING});
            SendCommandACK(t1::AudioGetParam, {.type = t1::AudioInquiredType::UPSCALING});
        }

        /* Touch Sensor */
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::ASSIGNABLE_SETTING))
            SendCommandACK(t1::SystemGetParam, {.type = t1::SystemInquiredType::ASSIGNABLE_SETTINGS });

        /* NC/AMB Toggle */
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_CONTROL_MODE_SELECT))
            SendCommandACK(t1::NcAsmGetParam, {.type = t1::NcAsmInquiredType::NC_AMB_TOGGLE });

        /* Head Gesture */
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::HEAD_GESTURE_ON_OFF_TRAINING))
            SendCommandACK(t1::SystemGetParam, {.type = t1::SystemInquiredType::HEAD_GESTURE_ON_OFF });

        /* Auto Power Off */
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF))
        {
            SendCommandACK(t1::PowerGetParam, {.type = t1::PowerInquiredType::AUTO_POWER_OFF});
        }
        else if (mSupport.contains(MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
        {
            SendCommandACK(t1::PowerGetParam,
                           {.type = t1::PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION});
        }

        /* Pause when headphones are removed */
        SendCommandACK(t1::SystemGetParam, {.type = t1::SystemInquiredType::PLAYBACK_CONTROL_BY_WEARING });

        /* Voice Guidance */
        if (mProtocol.hasTable2)
        {
            // Enabled
            SendCommandACK(t2::VoiceGuidanceGetParam,
                           {
                           .type = t2::VoiceGuidanceInquiredType::
                           MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH
                           });
            // Volume
            SendCommandACK(t2::VoiceGuidanceGetParam, {.type = t2::VoiceGuidanceInquiredType::VOLUME});
        }

        /* LOG_SET_STATUS */
        // XXX: Figure out if there's a struct for this in the app
        constexpr UInt8 kLogSetStatusCommand[] = {
            static_cast<UInt8>(t1::Command::LOG_SET_STATUS),
            0x01, 0x00
        };
        SendCommandImpl(kLogSetStatusCommand, MDRDataType::DATA_MDR, mSeqNumber);
        co_await Await(AWAIT_ACK);
        co_return MDR_HEADPHONES_TASK_INIT_OK;
    }

    MDRTask MDRHeadphones::RequestSyncV2()
    {
        /* Single Battery */
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::BATTERY});
        }
        else if (mSupport.contains(MessageMdrV2FunctionType_Table1::BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::BATTERY_WITH_THRESHOLD});
        }

        /* L + R Battery */
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::LEFT_RIGHT_BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::LEFT_RIGHT_BATTERY});
        }
        else if (mSupport.contains(MessageMdrV2FunctionType_Table1::LR_BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::LR_BATTERY_WITH_THRESHOLD});
        }

        /* Case Battery */
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::CRADLE_BATTERY});
        }
        else if (mSupport.contains(MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::CRADLE_BATTERY_WITH_THRESHOLD});
        }

        /* Sound Pressure */
        if (mSupport.contains(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_HBS_1))
        {
            SendCommandACK(t2::SafeListeningGetExtendedParam,
                           {.type = t2::SafeListeningInquiredType::SAFE_LISTENING_HBS_1});
        }
        else if (mSupport.contains(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_HBS_2))
        {
            SendCommandACK(t2::SafeListeningGetExtendedParam,
                           {.type = t2::SafeListeningInquiredType::SAFE_LISTENING_HBS_2});
        }
        else if (mSupport.contains(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_TWS_1))
        {
            SendCommandACK(t2::SafeListeningGetExtendedParam,
                           {.type = t2::SafeListeningInquiredType::SAFE_LISTENING_TWS_1});
        }
        else if (mSupport.contains(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_TWS_2))
        {
            SendCommandACK(t2::SafeListeningGetExtendedParam,
                           {.type = t2::SafeListeningInquiredType::SAFE_LISTENING_TWS_2});
        }
        co_return MDR_HEADPHONES_TASK_SYNC_OK;
    }

    MDRTask MDRHeadphones::RequestCommitV2()
    {
        /* Shutdown */
        if (mShutdown.dirty())
        {
            using namespace t1;
            if (mSupport.contains(MessageMdrV2FunctionType_Table1::POWER_OFF) && mShutdown.desired)
            {
                SendCommandACK(PowerSetStatusPowerOff);
            }
            else
                mShutdown.overwrite(false);
        }
        /* NC/ASM */
        if (mNcAsmAmbientLevel.dirty() || mNcAsmEnabled.dirty() || mNcAsmMode.dirty() ||
            mNcAsmFocusOnVoice.dirty() || mNcAsmAutoAsmEnabled.dirty() || mNcAsmNoiseAdaptiveSensitivity.dirty())
        {
            using namespace t1;
            if (mSupport.contains(
                MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
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
            else if (mSupport.contains(MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
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
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_CONTROL_MODE_SELECT))
        {
            using namespace t1;
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
            using namespace t1;
            PlayParamPlaybackControllerVolume res;
            res.base.command = Command::PLAY_SET_PARAM;
            res.volumeValue = mPlayVolume.desired;
            SendCommandACK(PlayParamPlaybackControllerVolume, res);
            mPlayVolume.commit();
        }
        /* Play Control */
        // A bit of a special case. We reset the value to something else
        // so simply setting 'desired' repeatedly works as intended
        if (mPlayControl.dirty())
        {
            using namespace t1;
            PlayStatusSetPlaybackController res;
            res.base.command = Command::PLAY_SET_STATUS;
            res.status = MessageMdrV2EnableDisable::ENABLE;
            res.control = mPlayControl.desired;
            SendCommandACK(PlayStatusSetPlaybackController, res);
            mPlayControl.overwrite(PlaybackControl::KEY_OFF);
        }

        /* Multipoint Switch */
        if (mMultipointDeviceMac.dirty())
        {
            using namespace t2;
            PeripheralSetExtendedParamSourceSwitchControl res;
            res.base.command = Command::PERI_SET_EXTENDED_PARAM;
            if (mMultipointDeviceMac.desired.length() != 17)
                mMultipointDeviceMac.overwrite("");
            else
            {
                std::memcpy(res.targetBdAddress.data(), mMultipointDeviceMac.desired.data(), 17);
                SendCommandACK(PeripheralSetExtendedParamSourceSwitchControl, res);
                mMultipointDeviceMac.commit();
            }
        }

        /* Connection Ops */
        {
            using namespace t2;
            PeripheralInquiredType type = PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT;
            if (mSupport.contains(MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
            {
                type = PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT;
            }
            else if (
                mSupport.contains(
                    MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT)
                ||
                mSupport.contains(
                    MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE)
            )
            {
                type = PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE;
            } else
            {
                // Unsupported. Ignore the rest.
                mPairedDeviceConnectMac.overwrite("");
                mPairedDeviceDisconnectMac.overwrite("");
                mPairedDeviceUnpairMac.overwrite("");
            }
            PeripheralSetExtendedParamParingDeviceManagementCommon res;
            res.base.command = Command::PERI_SET_EXTENDED_PARAM;
            res.base.type = type;
            if (mPairedDeviceConnectMac.dirty())
            {
                res.connectivityActionType = ConnectivityActionType::CONNECT;
                std::memcpy(res.targetBdAddress.data(), mPairedDeviceConnectMac.desired.data(), 17);
                mPairedDeviceConnectMac.overwrite("");
                SendCommandACK(PeripheralSetExtendedParamParingDeviceManagementCommon, res);
            }
            if (mPairedDeviceDisconnectMac.dirty())
            {
                res.connectivityActionType = ConnectivityActionType::DISCONNECT;
                std::memcpy(res.targetBdAddress.data(), mPairedDeviceDisconnectMac.desired.data(), 17);
                mPairedDeviceDisconnectMac.overwrite("");
                SendCommandACK(PeripheralSetExtendedParamParingDeviceManagementCommon, res);
            }
            if (mPairedDeviceUnpairMac.dirty())
            {
                res.connectivityActionType = ConnectivityActionType::UNPAIR;
                std::memcpy(res.targetBdAddress.data(), mPairedDeviceUnpairMac.desired.data(), 17);
                mPairedDeviceUnpairMac.overwrite("");
                SendCommandACK(PeripheralSetExtendedParamParingDeviceManagementCommon, res);
            }
        }

        /* Pairing Mode */
        if (mPairingMode.dirty())
        {
            using namespace t2;
            PeripheralStatusPairingDeviceManagementCommon res;
            res.base.command = Command::PERI_SET_STATUS;
            res.base.type = PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE;
            res.btMode = mPairingMode.desired
                ? PeripheralBluetoothMode::INQUIRY_SCAN_MODE
                : PeripheralBluetoothMode::NORMAL_MODE;
            res.enableDisableStatus = MessageMdrV2EnableDisable::ENABLE;
            SendCommandACK(PeripheralStatusPairingDeviceManagementCommon, res);
            mPairingMode.commit();
        }

        /* STC */
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::SMART_TALKING_MODE_TYPE2))
        {
            using namespace t1;
            if (mSpeakToChatEnabled.dirty())
            {
                SystemParamSmartTalking res;
                res.base.command = Command::SYSTEM_SET_PARAM;
                res.base.type = SystemInquiredType::SMART_TALKING_MODE_TYPE2;
                res.onOffValue = mSpeakToChatEnabled.desired
                    ? MessageMdrV2EnableDisable::ENABLE
                    : MessageMdrV2EnableDisable::DISABLE;
                res.previewModeOnOffValue = MessageMdrV2EnableDisable::DISABLE;
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
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::LISTENING_OPTION))
        {
            using namespace t1;
            if (mBGMModeEnabled.dirty() || mBGMModeRoomSize.dirty())
            {
                AudioParamBGMMode res;
                res.base.command = Command::AUDIO_SET_PARAM;
                res.base.type = AudioInquiredType::BGM_MODE;
                res.onOffSettingValue = mBGMModeEnabled.desired
                    ? MessageMdrV2EnableDisable::ENABLE
                    : MessageMdrV2EnableDisable::DISABLE;
                res.targetRoomSize = mBGMModeRoomSize.desired;
                SendCommandACK(AudioParamBGMMode, res);
                mBGMModeEnabled.commit(), mBGMModeRoomSize.commit();
            }
            if (mUpmixCinemaEnabled.dirty())
            {
                AudioParamUpmixCinema res;
                res.base.command = Command::AUDIO_SET_PARAM;
                res.onOffSettingValue = mUpmixCinemaEnabled.desired
                    ? MessageMdrV2EnableDisable::ENABLE
                    : MessageMdrV2EnableDisable::DISABLE;
                SendCommandACK(AudioParamUpmixCinema, res);
                mUpmixCinemaEnabled.commit();
            }
        }

        /* EQ */
        if (mEqPresetId.dirty())
        {
            using namespace t1;
            EqEbbParamEq res;
            res.base.command = Command::EQEBB_SET_PARAM;
            res.base.type = EqEbbInquiredType::PRESET_EQ;
            res.presetId = mEqPresetId.desired;
            SendCommandACK(EqEbbParamEq, res);
            mEqPresetId.commit();
            // Ask for a equalizer param update afterwards
            SendCommandACK(EqEbbGetParam);
        }
        if (mEqConfig.dirty() || mEqClearBass.dirty())
        {
            using namespace t1;
            EqEbbParamEq res;
            res.base.command = Command::EQEBB_SET_PARAM;
            res.base.type = EqEbbInquiredType::PRESET_EQ;
            res.presetId = mEqPresetId.current;
            int eqBands = mEqConfig.desired.size(), eqOffset = 0;
            if (eqBands == 0)
            {
                mEqConfig.commit(), mEqClearBass.commit();
            }
            else
            {
                auto& bands = mEqConfig.desired;
                if (eqBands == 5)
                {
                    res.bands.value = Vector<UInt8>{{
                        static_cast<UInt8>(mEqClearBass.desired + 10),
                        static_cast<UInt8>(bands[0] + 10),
                        static_cast<UInt8>(bands[1] + 10),
                        static_cast<UInt8>(bands[2] + 10),
                        static_cast<UInt8>(bands[3] + 10),
                        static_cast<UInt8>(bands[4] + 10),
                    }};
                }
                else if (eqBands == 10)
                    res.bands.value = Vector<UInt8>{{
                        static_cast<UInt8>(bands[0] + 6),
                        static_cast<UInt8>(bands[1] + 6),
                        static_cast<UInt8>(bands[2] + 6),
                        static_cast<UInt8>(bands[3] + 6),
                        static_cast<UInt8>(bands[4] + 6),
                        static_cast<UInt8>(bands[5] + 6),
                        static_cast<UInt8>(bands[6] + 6),
                        static_cast<UInt8>(bands[7] + 6),
                        static_cast<UInt8>(bands[8] + 6),
                        static_cast<UInt8>(bands[9] + 6),
                    }};
                else
                    MDR_CHECK_MSG(false, "mEqConfig size can only be 0, 5, or 10. Got {}.", eqBands);
                mEqConfig.commit();
                mEqClearBass.commit();
                SendCommandACK(EqEbbParamEq, res);
                // Ask for a equalizer param update afterwards
                SendCommandACK(EqEbbGetParam);
            }
        }

        /* Connection Quality */
        if (mSupport.
            contains(MessageMdrV2FunctionType_Table1::CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY))
        {
            if (mAudioPriorityMode.dirty())
            {
                using namespace t1;
                AudioParamConnection res;
                res.base.command = Command::AUDIO_SET_PARAM;
                res.settingValue = mAudioPriorityMode.desired;
                SendCommandACK(AudioParamConnection, res);
                mAudioPriorityMode.commit();
            }
        }

        /* DSEE */
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::UPSCALING_AUTO_OFF))
        {
            if (mUpscalingEnabled.dirty())
            {
                using namespace t1;
                AudioParamUpscaling res;
                res.base.command = Command::AUDIO_SET_PARAM;
                res.settingValue = mUpscalingEnabled.desired ? UpscalingTypeAutoOff::AUTO : UpscalingTypeAutoOff::OFF;
                SendCommandACK(AudioParamUpscaling, res);
                mUpscalingEnabled.commit();
            }
        }

        /* Touch Functions */
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::ASSIGNABLE_SETTING))
        {
            if (mTouchFunctionLeft.dirty() || mTouchFunctionRight.dirty())
            {
                using namespace t1;
                SystemParamAssignableSettings res;
                res.base.command = Command::SYSTEM_SET_PARAM;
                res.presets.value = {mTouchFunctionLeft.desired, mTouchFunctionRight.desired};
                SendCommandACK(SystemParamAssignableSettings, res);
                mTouchFunctionLeft.commit(), mTouchFunctionRight.commit();
            }
        }

        /* Head Gesture */
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::UPSCALING_AUTO_OFF))
        {
            if (mHeadGestureEnabled.dirty())
            {
                using namespace t1;
                SystemParamCommon res;
                res.base.command = Command::SYSTEM_SET_PARAM;
                res.base.type = SystemInquiredType::HEAD_GESTURE_ON_OFF;
                res.settingValue = mHeadGestureEnabled.desired
                    ? MessageMdrV2EnableDisable::ENABLE
                    : MessageMdrV2EnableDisable::DISABLE;
                SendCommandACK(SystemParamCommon, res);
                mHeadGestureEnabled.commit();
            }
        }

        /* Auto Power Off */
        if (mSupport.contains(MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF))
        {
            using namespace t1;
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
        else if (mSupport.contains(MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
        {
            using namespace t1;
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
            MessageMdrV2FunctionType_Table1::PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF))
        {
            using namespace t1;
            if (mAutoPauseEnabled.dirty())
            {
                SystemParamCommon res;
                res.base.command = Command::SYSTEM_SET_PARAM;
                res.base.type = SystemInquiredType::PLAYBACK_CONTROL_BY_WEARING;
                res.settingValue = mAutoPauseEnabled.desired
                    ? MessageMdrV2EnableDisable::ENABLE
                    : MessageMdrV2EnableDisable::DISABLE;
                SendCommandACK(SystemParamCommon, res);
                mAutoPauseEnabled.commit();
            }
        }

        /* Voice Guidance */
        if (mVoiceGuidanceEnabled.dirty())
        {
            using namespace t2;
            VoiceGuidanceParamSettingMtk res;
            res.base.command = Command::VOICE_GUIDANCE_SET_PARAM;
            res.base.type = VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH;
            res.settingValue = mVoiceGuidanceEnabled.desired
                ? MessageMdrV2OnOffSettingValue::ON
                : MessageMdrV2OnOffSettingValue::OFF;
            SendCommandACK(VoiceGuidanceParamSettingMtk, res);
            mVoiceGuidanceVolume.commit();
        }

        /* Voice Guidance */
        if (mSupport.contains(
            MessageMdrV2FunctionType_Table2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT))
        {
            if (mVoiceGuidanceVolume.dirty())
            {
                using namespace t2;
                VoiceGuidanceSetParamVolume res;
                res.base.command = Command::VOICE_GUIDANCE_SET_PARAM;
                res.base.type = VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH;
                res.volumeValue = mVoiceGuidanceVolume.desired;
                res.feedbackSound = MessageMdrV2OnOffSettingValue::ON;
                SendCommandACK(VoiceGuidanceSetParamVolume, res);
                mVoiceGuidanceVolume.commit();
            }
        }

        /* General Settings */
        {
            using namespace t1;
            if (mSupport.contains(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_1))
            {
                if (mGsParamBool1.dirty())
                {
                    GsParamBoolean res;
                    res.base.command = Command::GENERAL_SETTING_SET_PARAM;
                    res.base.type = GsInquiredType::GENERAL_SETTING1;
                    res.settingValue = mGsParamBool1.desired ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsParamBoolean, res);
                    mGsParamBool1.commit();
                }
            }
            if (mSupport.contains(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_2))
            {
                if (mGsParamBool2.dirty())
                {
                    GsParamBoolean res;
                    res.base.command = Command::GENERAL_SETTING_SET_PARAM;
                    res.base.type = GsInquiredType::GENERAL_SETTING2;
                    res.settingValue = mGsParamBool2.desired ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsParamBoolean, res);
                    mGsParamBool2.commit();
                }
            }
            if (mSupport.contains(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_3))
            {
                if (mGsParamBool3.dirty())
                {
                    GsParamBoolean res;
                    res.base.command = Command::GENERAL_SETTING_SET_PARAM;
                    res.base.type = GsInquiredType::GENERAL_SETTING3;
                    res.settingValue = mGsParamBool3.desired ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsParamBoolean, res);
                    mGsParamBool3.commit();
                }
            }
            if (mSupport.contains(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_4))
            {
                if (mGsParamBool4.dirty())
                {
                    GsParamBoolean res;
                    res.base.command = Command::GENERAL_SETTING_SET_PARAM;
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
            using namespace t2;
            if (mSupport.contains(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_HBS_1))
            {
                SafeListeningSetParamSL res;
                res.base.type = SafeListeningInquiredType::SAFE_LISTENING_HBS_1;
                res.previewMode = MessageMdrV2EnableDisable::DISABLE;
                res.safeListeningMode = mSafeListeningPreviewMode.desired
                    ? MessageMdrV2EnableDisable::ENABLE
                    : MessageMdrV2EnableDisable::DISABLE;
                SendCommandACK(SafeListeningSetParamSL, res);
                mSafeListeningPreviewMode.commit();
            }
            else if (mSupport.contains(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_HBS_2))
            {
                SafeListeningSetParamSL res;
                res.base.type = SafeListeningInquiredType::SAFE_LISTENING_HBS_2;
                res.previewMode = MessageMdrV2EnableDisable::DISABLE;
                res.safeListeningMode = mSafeListeningPreviewMode.desired
                    ? MessageMdrV2EnableDisable::ENABLE
                    : MessageMdrV2EnableDisable::DISABLE;
                SendCommandACK(SafeListeningSetParamSL, res);
                mSafeListeningPreviewMode.commit();
            }
            else if (mSupport.contains(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_TWS_1))
            {
                SafeListeningSetParamSL res;
                res.base.type = SafeListeningInquiredType::SAFE_LISTENING_TWS_1;
                res.previewMode = MessageMdrV2EnableDisable::DISABLE;
                res.safeListeningMode = mSafeListeningPreviewMode.desired
                    ? MessageMdrV2EnableDisable::ENABLE
                    : MessageMdrV2EnableDisable::DISABLE;
                SendCommandACK(SafeListeningSetParamSL, res);
                mSafeListeningPreviewMode.commit();
            }
            else if (mSupport.contains(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_TWS_2))
            {
                SafeListeningSetParamSL res;
                res.base.type = SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
                res.previewMode = MessageMdrV2EnableDisable::DISABLE;
                res.safeListeningMode = mSafeListeningPreviewMode.desired
                    ? MessageMdrV2EnableDisable::ENABLE
                    : MessageMdrV2EnableDisable::DISABLE;
                SendCommandACK(SafeListeningSetParamSL, res);
                mSafeListeningPreviewMode.commit();
            }
        }
        co_return MDR_HEADPHONES_TASK_COMMIT_OK;
    }
#pragma endregion
}
// NOLINTEND