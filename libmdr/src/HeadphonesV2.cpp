#include <algorithm>
#include "Details.hpp"
// NOLINTBEGIN
namespace mdr
{
    using namespace v2;
    MDRTask MDRHeadphones::RequestInitV2()
    {
        auto& state = mDetailsV2;
        if (mProtocolFamily != ProtocolFamily::V2)
            co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "Device does not use MDR V2");

        state.mProtocol = {};

        SendCommandACK(t1::ConnectGetProtocolInfo);
        const int result = co_await Await(AWAIT_PROTOCOL_INFO);
        if (result != MDR_RESULT_OK)
            co_return SetLastError(result, "Unable to initialize MDR V2");

        if (!state.mProtocol.hasTable1)
            co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "Device doesn't support MDR V2 Table 1");
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
            if (state.mProtocol.hasTable2)
            {
                SendCommandACK(t2::ConnectGetSupportFunction);
                co_await Await(AWAIT_SUPPORT_FUNCTION);
            }

            /* General Setting */
            t1::DisplayLanguage lang = t1::DisplayLanguage::ENGLISH;
            if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING_1))
            {
                SendCommandACK(t1::GsGetCapability, {
                               .type = t1::GsInquiredType::GENERAL_SETTING1, .displayLanguage = lang
                               });
                SendCommandACK(t1::GsGetParam, {
                               .type = t1::GsInquiredType::GENERAL_SETTING1
                               });
            }
            if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING_2))
            {
                SendCommandACK(t1::GsGetCapability, {
                               .type = t1::GsInquiredType::GENERAL_SETTING2, .displayLanguage = lang
                               });
                SendCommandACK(t1::GsGetParam, {
                               .type = t1::GsInquiredType::GENERAL_SETTING2
                               });
            }
            if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING_3))
            {
                SendCommandACK(t1::GsGetCapability, {
                               .type = t1::GsInquiredType::GENERAL_SETTING3, .displayLanguage = lang
                               });
                SendCommandACK(t1::GsGetParam, {
                               .type = t1::GsInquiredType::GENERAL_SETTING3
                               });
            }
            if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING_4))
            {
                SendCommandACK(t1::GsGetCapability, {
                               .type = t1::GsInquiredType::GENERAL_SETTING4, .displayLanguage = lang
                               });
                SendCommandACK(t1::GsGetParam, {
                               .type = t1::GsInquiredType::GENERAL_SETTING4
                               });
            }

            /* DSEE */
            if (state.mSupport.contains(t1::FunctionType::UPSCALING_AUTO_OFF))
                SendCommandACK(t1::AudioGetCapability, {
                           .type = t1::AudioInquiredType::UPSCALING
                           });
        }
        /* Receive alerts for certain operations like toggling multipoint */
        if (state.mSupport.contains(t1::FunctionType::FIXED_MESSAGE))
            SendCommandACK(t1::AlertSetStatusFixedMessage, { .status = EnableDisable::ENABLE});

        /* Codec Type */
        if (state.mSupport.contains(t1::FunctionType::CODEC_INDICATOR))
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
        if (state.mSupport.contains(
            t1::FunctionType::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            SendCommandACK(t1::NcAsmGetParam,
                           { .inquiredType = t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS});
        }
        else if (state.mSupport.contains(
            t1::FunctionType::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
        {
            SendCommandACK(t1::NcAsmGetParam,
                           { .inquiredType = t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA});
        }
        else if (state.mSupport.contains(t1::FunctionType::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            SendCommandACK(t1::NcAsmGetParam, { .inquiredType = t1::NcAsmInquiredType::ASM_SEAMLESS});
        }

        /* Pairing Management */
        constexpr t2::FunctionType kPairingFunctions[] = {
            t2::FunctionType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT,
            t2::FunctionType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE
        };
        if (std::ranges::any_of(kPairingFunctions, [&](auto x) { return state.mSupport.contains(x); }))
        {
            /* Pairing Mode */
            SendCommandACK(t2::PeripheralGetStatus,
                           {.inquiredType = t2::PeripheralInquiredType::
                           PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE
                           });

            /* Connected Devices */
            SendCommandACK(t2::PeripheralGetParam,
                           {.inquiredType = t2::PeripheralInquiredType::
                           PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE
                           });
        }
        
        if (state.mSupport.contains(t2::FunctionType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
        {
            /* Pairing Mode */
            SendCommandACK(t2::PeripheralGetStatus,
                           {.inquiredType = t2::PeripheralInquiredType::
                           PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
                           });

            /* Connected Devices */
            SendCommandACK(t2::PeripheralGetParam,
                           {.inquiredType = t2::PeripheralInquiredType::
                           PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
                           });
        }

        /* Source Switch Control */
        if (state.mSupport.contains(t2::FunctionType::SOURCE_SWITCH_CONTROL))
        {
            SendCommandACK(t2::PeripheralGetParam, {.inquiredType = t2::PeripheralInquiredType::SOURCE_SWITCH_CONTROL});
        }

        /* Speak To Chat */
        if (state.mSupport.contains(t1::FunctionType::SMART_TALKING_MODE_TYPE2))
        {
            SendCommandACK(t1::SystemGetParam, {.type = t1::SystemInquiredType::SMART_TALKING_MODE_TYPE2});
            SendCommandACK(t1::SystemGetExtParam, {.type = t1::SystemInquiredType::SMART_TALKING_MODE_TYPE2});
        }

        /* Listening Mode */
        if (state.mSupport.contains(t1::FunctionType::LISTENING_OPTION))
        {
            SendCommandACK(t1::AudioGetParam, {.type = t1::AudioInquiredType::BGM_MODE_AND_ERRORCODE});
            SendCommandACK(t1::AudioGetParam, {.type = t1::AudioInquiredType::UPMIX_CINEMA});
        }

        /* Equalizer */
        SendCommandACK(t1::EqEbbGetStatus, {.type = t1::EqEbbInquiredType::PRESET_EQ});
        SendCommandACK(t1::EqEbbGetParam);

        /* Connection Quality */
        if (state.mSupport.contains(
            t1::FunctionType::CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY))
            SendCommandACK(t1::AudioGetParam, {.type = t1::AudioInquiredType::CONNECTION_MODE});

        /* DSEE */
        if (state.mSupport.contains(t1::FunctionType::UPSCALING_AUTO_OFF))
        {
            SendCommandACK(t1::AudioGetStatus, {.type = t1::AudioInquiredType::UPSCALING});
            SendCommandACK(t1::AudioGetParam, {.type = t1::AudioInquiredType::UPSCALING});
        }

        /* Touch Sensor */
        if (state.mSupport.contains(t1::FunctionType::ASSIGNABLE_SETTING))
            SendCommandACK(t1::SystemGetParam, {.type = t1::SystemInquiredType::ASSIGNABLE_SETTINGS });

        /* NC/AMB Toggle */
        if (state.mSupport.contains(t1::FunctionType::AMBIENT_SOUND_CONTROL_MODE_SELECT))
            SendCommandACK(t1::NcAsmGetParam, {.inquiredType = t1::NcAsmInquiredType::NC_AMB_TOGGLE });

        /* Head Gesture */
        if (state.mSupport.contains(t1::FunctionType::HEAD_GESTURE_ON_OFF_TRAINING))
            SendCommandACK(t1::SystemGetParam, {.type = t1::SystemInquiredType::HEAD_GESTURE_ON_OFF });

        /* Auto Power Off */
        if (state.mSupport.contains(t1::FunctionType::AUTO_POWER_OFF))
        {
            SendCommandACK(t1::PowerGetParam, {.type = t1::PowerInquiredType::AUTO_POWER_OFF});
        }
        else if (state.mSupport.contains(t1::FunctionType::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
        {
            SendCommandACK(t1::PowerGetParam,
                           {.type = t1::PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION});
        }

        /* Pause when headphones are removed */
        SendCommandACK(t1::SystemGetParam, {.type = t1::SystemInquiredType::PLAYBACK_CONTROL_BY_WEARING });

        /* Voice Guidance */
        if (state.mProtocol.hasTable2)
        {
            // Enabled
            SendCommandACK(t2::VoiceGuidanceGetParam,
                           {
                           .inquiredType = t2::VoiceGuidanceInquiredType::
                           MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH
                           });
            // Volume
            SendCommandACK(t2::VoiceGuidanceGetParam, {.inquiredType = t2::VoiceGuidanceInquiredType::VOLUME});
        }

        /* LOG_SET_STATUS */
        // XXX: Figure out if there's a struct for this in the app
        constexpr UInt8 kLogSetStatusCommand[] = {
            static_cast<UInt8>(t1::Command::LOG_SET_STATUS),
            0x01, 0x00
        };
        SendCommandImpl(kLogSetStatusCommand, MDRDataType::DATA_MDR, mSeqNumber);
        co_await Await(AWAIT_ACK);
        mNeutralInitialized = true;
        co_return MDR_EVENT_INITIALIZE_COMPLETE;
    }

    MDRTask MDRHeadphones::RequestSyncV2()
    {
        auto& state = mDetailsV2;
        /* Single Battery */
        if (state.mSupport.contains(t1::FunctionType::BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::BATTERY});
        }
        else if (state.mSupport.contains(t1::FunctionType::BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::BATTERY_WITH_THRESHOLD});
        }

        /* L + R Battery */
        if (state.mSupport.contains(t1::FunctionType::LEFT_RIGHT_BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::LEFT_RIGHT_BATTERY});
        }
        else if (state.mSupport.contains(t1::FunctionType::LR_BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::LR_BATTERY_WITH_THRESHOLD});
        }

        /* Case Battery */
        if (state.mSupport.contains(t1::FunctionType::CRADLE_BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::CRADLE_BATTERY});
        }
        else if (state.mSupport.contains(t1::FunctionType::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::CRADLE_BATTERY_WITH_THRESHOLD});
        }

        /* Sound Pressure */
        if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_HBS_1))
        {
            SendCommandACK(t2::SafeListeningGetExtendedParam,
                           {.inquiredType = t2::SafeListeningInquiredType::SAFE_LISTENING_HBS_1});
        }
        else if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_HBS_2))
        {
            SendCommandACK(t2::SafeListeningGetExtendedParam,
                           {.inquiredType = t2::SafeListeningInquiredType::SAFE_LISTENING_HBS_2});
        }
        else if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_TWS_1))
        {
            SendCommandACK(t2::SafeListeningGetExtendedParam,
                           {.inquiredType = t2::SafeListeningInquiredType::SAFE_LISTENING_TWS_1});
        }
        else if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_TWS_2))
        {
            SendCommandACK(t2::SafeListeningGetExtendedParam,
                           {.inquiredType = t2::SafeListeningInquiredType::SAFE_LISTENING_TWS_2});
        }
        co_return MDR_EVENT_SYNC_COMPLETE;
    }

    void MDRHeadphones::SnapshotPropertiesV2()
    {
        auto& state = mDetailsV2;
        state.mShutdown.submit();
        state.mNcAsmEnabled.submit();
        state.mNcAsmFocusOnVoice.submit();
        state.mNcAsmAmbientLevel.submit();
        state.mNcAsmButtonFunction.submit();
        state.mNcAsmMode.submit();
        state.mNcAsmAutoAsmEnabled.submit();
        state.mNcAsmNoiseAdaptiveSensitivity.submit();
        state.mPowerAutoOff.submit();
        state.mPowerAutoOffWearingDetection.submit();
        state.mPlayVolume.submit();
        state.mPlayControl.submit();
        state.mGsParamBool1.submit();
        state.mGsParamBool2.submit();
        state.mGsParamBool3.submit();
        state.mGsParamBool4.submit();
        state.mUpscalingEnabled.submit();
        state.mAudioPriorityMode.submit();
        state.mBGMModeEnabled.submit();
        state.mBGMModeRoomSize.submit();
        state.mUpmixCinemaEnabled.submit();
        state.mAutoPauseEnabled.submit();
        state.mTouchFunctionLeft.submit();
        state.mTouchFunctionRight.submit();
        state.mSpeakToChatEnabled.submit();
        state.mSpeakToChatDetectSensitivity.submit();
        state.mSpeakToModeOutTime.submit();
        state.mHeadGestureEnabled.submit();
        state.mEqAvailable.submit();
        state.mEqPresetId.submit();
        state.mEqClearBass.submit();
        state.mEqConfig.submit();
        state.mVoiceGuidanceEnabled.submit();
        state.mVoiceGuidanceVolume.submit();
        state.mPairingMode.submit();
        state.mMultipointDeviceMac.submit();
        state.mSourceSwitchControlEnabled.submit();
        state.mPairedDeviceDisconnectMac.submit();
        state.mPairedDeviceConnectMac.submit();
        state.mPairedDeviceUnpairMac.submit();
        state.mSafeListeningPreviewMode.submit();
    }

    MDRTask MDRHeadphones::RequestCommitV2()
    {
        auto& state = mDetailsV2;
        SnapshotPropertiesV2();

        /* Shutdown */
        if (state.mShutdown.pending())
        {
            using namespace t1;
            if (state.mSupport.contains(t1::FunctionType::POWER_OFF) && state.mShutdown.submitted)
            {
                SendCommandACK(PowerSetStatusPowerOff);
                state.mShutdown.override(false);
            }
            else
                state.mShutdown.override(false);
        }
        /* NC/ASM */
        if (state.mNcAsmAmbientLevel.pending() || state.mNcAsmEnabled.pending() ||
            state.mNcAsmMode.pending() || state.mNcAsmFocusOnVoice.pending() ||
            state.mNcAsmAutoAsmEnabled.pending() || state.mNcAsmNoiseAdaptiveSensitivity.pending())
        {
            using namespace t1;
            if (state.mSupport.contains(
                t1::FunctionType::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
            {

                NcAsmSetParamModeNcDualModeSwitchAsmSeamlessNa res;
                res.command = Command::NCASM_SET_PARAM;
                res.valueChangeStatus = ValueChangeStatus::CHANGED;
                res.ncAsmTotalEffect = state.mNcAsmEnabled.submitted ? NcAsmOnOffValue::ON : NcAsmOnOffValue::OFF;
                res.ncAsmMode = state.mNcAsmMode.submitted;
                res.ambientSoundMode = state.mNcAsmFocusOnVoice.submitted
                    ? AmbientSoundMode::VOICE : AmbientSoundMode::NORMAL;
                res.ambientSoundLevelValue = state.mNcAsmAmbientLevel.submitted;
                res.ncAsmOnOffValue = state.mNcAsmAutoAsmEnabled.submitted
                    ? NcAsmOnOffValue::ON : NcAsmOnOffValue::OFF;
                res.noiseAdaptiveSensitivitySettings = state.mNcAsmNoiseAdaptiveSensitivity.submitted;
                SendCommandACK(NcAsmSetParamModeNcDualModeSwitchAsmSeamlessNa, res);
            }
            else if (state.mSupport.contains(t1::FunctionType::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
            {
                NcAsmSetParamAsmSeamless res;
                res.command = Command::NCASM_SET_PARAM;
                res.valueChangeStatus = ValueChangeStatus::CHANGED;
                res.ncAsmTotalEffect = state.mNcAsmEnabled.submitted ? NcAsmOnOffValue::ON : NcAsmOnOffValue::OFF;
                res.ambientSoundMode = state.mNcAsmFocusOnVoice.submitted
                    ? AmbientSoundMode::VOICE : AmbientSoundMode::NORMAL;
                res.ambientSoundLevelValue = state.mNcAsmAmbientLevel.submitted;
                SendCommandACK(NcAsmSetParamAsmSeamless, res);
            }
            else
            {
                NcAsmSetParamModeNcDualModeSwitchAsmSeamless res;
                res.command = Command::NCASM_SET_PARAM;
                res.valueChangeStatus = ValueChangeStatus::CHANGED;
                res.ncAsmTotalEffect = state.mNcAsmEnabled.submitted ? NcAsmOnOffValue::ON : NcAsmOnOffValue::OFF;
                res.ncAsmMode = state.mNcAsmMode.submitted,
                    res.ambientSoundMode = state.mNcAsmFocusOnVoice.submitted
                    ? AmbientSoundMode::VOICE
                    : AmbientSoundMode::NORMAL;
                res.ambientSoundLevelValue = state.mNcAsmAmbientLevel.submitted;
                SendCommandACK(NcAsmSetParamModeNcDualModeSwitchAsmSeamless, res);
            }
            state.mNcAsmAmbientLevel.commit(), state.mNcAsmEnabled.commit(), state.mNcAsmMode.commit();
            state.mNcAsmFocusOnVoice.commit(), state.mNcAsmAutoAsmEnabled.commit(), state.mNcAsmNoiseAdaptiveSensitivity.commit();
        }

        /* NC/AMB Mode */
        if (state.mSupport.contains(t1::FunctionType::AMBIENT_SOUND_CONTROL_MODE_SELECT))
        {
            using namespace t1;
            if (state.mNcAsmButtonFunction.pending())
            {
                NcAsmSetParamNcAmbToggle res;
                res.command = Command::NCASM_SET_PARAM;
                res.function = state.mNcAsmButtonFunction.submitted;
                SendCommandACK(NcAsmSetParamNcAmbToggle, res);
                state.mNcAsmButtonFunction.commit();
            }
        }
        /* Volume */
        if (state.mPlayVolume.pending())
        {
            using namespace t1;
            SetPlayParamPlaybackControllerVolume res;
            res.command = Command::PLAY_SET_PARAM;
            res.type = PlayInquiredType::MUSIC_VOLUME;
            res.volumeValue = state.mPlayVolume.submitted;
            SendCommandACK(SetPlayParamPlaybackControllerVolume, res);
            state.mPlayVolume.commit();
        }
        /* Play Control */
        // A bit of a special case. We reset the value to something else
        // so simply setting 'desired' repeatedly works as intended
        if (state.mPlayControl.pending())
        {
            using namespace t1;
            SetPlayStatusPlaybackController res;
            res.command = Command::PLAY_SET_STATUS;
            res.type = PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT;
            res.status = EnableDisable::ENABLE;
            res.control = state.mPlayControl.submitted;
            SendCommandACK(SetPlayStatusPlaybackController, res);
            state.mPlayControl.override(PlaybackControl::KEY_OFF);
        }

        /* Multipoint Switch */
        if (state.mMultipointDeviceMac.pending())
        {
            using namespace t2;
            PeripheralSetExtendedParamSourceSwitchControl res;
            if (state.mMultipointDeviceMac.submitted.length() != 17)
                state.mMultipointDeviceMac.override("");
            else
            {
                std::copy_n(state.mMultipointDeviceMac.submitted.begin(), 17, res.targetBdAddress.begin());
                SendCommandACK(PeripheralSetExtendedParamSourceSwitchControl, res);
                state.mMultipointDeviceMac.commit();
            }
        }

        /* Source Switch Control */
        if (state.mSourceSwitchControlEnabled.pending())
        {
            using namespace t2;
            if (state.mSupport.contains(t2::FunctionType::SOURCE_SWITCH_CONTROL))
            {
                PeripheralSetParamSourceSwitchControl res;
                res.value = state.mSourceSwitchControlEnabled.submitted ? 1 : 0;
                SendCommandACK(PeripheralSetParamSourceSwitchControl, res);
                state.mSourceSwitchControlEnabled.commit();
            }
            else
                state.mSourceSwitchControlEnabled.override(true);
        }

        /* Connection Ops */
        {
            using namespace t2;
            PeripheralInquiredType type = PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT;
            if (
                state.mSupport.contains(
                    t2::FunctionType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT)
                ||
                state.mSupport.contains(
                    t2::FunctionType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE)
            )
            {
                type = PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE;
            }
            else if (state.mSupport.contains(t2::FunctionType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
            {
                type = PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT;
            }
            else
            {
                // Unsupported. Ignore the rest.
                state.mPairedDeviceConnectMac.override("");
                state.mPairedDeviceDisconnectMac.override("");
                state.mPairedDeviceUnpairMac.override("");
            }
            PeripheralSetExtendedParamParingDeviceManagementCommon res;
            res.inquiredType = type;
            if (state.mPairedDeviceConnectMac.pending())
            {
                if (state.mPairedDeviceConnectMac.submitted.length() != 17)
                    state.mPairedDeviceConnectMac.override("");
                else
                {
                    res.connectivityActionType = ConnectivityActionType::CONNECT;
                    std::copy_n(state.mPairedDeviceConnectMac.submitted.begin(), 17, res.btDeviceAddress.begin());
                    SendCommandACK(PeripheralSetExtendedParamParingDeviceManagementCommon, res);
                    state.mPairedDeviceConnectMac.override("");
                }
            }
            if (state.mPairedDeviceDisconnectMac.pending())
            {
                if (state.mPairedDeviceDisconnectMac.submitted.length() != 17)
                    state.mPairedDeviceDisconnectMac.override("");
                else
                {
                    res.connectivityActionType = ConnectivityActionType::DISCONNECT;
                    std::copy_n(state.mPairedDeviceDisconnectMac.submitted.begin(), 17, res.btDeviceAddress.begin());
                    SendCommandACK(PeripheralSetExtendedParamParingDeviceManagementCommon, res);
                    state.mPairedDeviceDisconnectMac.override("");
                }
            }
            if (state.mPairedDeviceUnpairMac.pending())
            {
                if (state.mPairedDeviceUnpairMac.submitted.length() != 17)
                    state.mPairedDeviceUnpairMac.override("");
                else
                {
                    res.connectivityActionType = ConnectivityActionType::UNPAIR;
                    std::copy_n(state.mPairedDeviceUnpairMac.submitted.begin(), 17, res.btDeviceAddress.begin());
                    SendCommandACK(PeripheralSetExtendedParamParingDeviceManagementCommon, res);
                    state.mPairedDeviceUnpairMac.override("");
                }
            }
        

            /* Pairing Mode */
            if (state.mPairingMode.pending())
            {
                using namespace t2;
                PeripheralSetStatusParingDeviceManagementCommon res;
                res.inquiredType = type;
                res.btMode = state.mPairingMode.submitted
                    ? PeripheralBluetoothMode::INQUIRY_SCAN_MODE
                    : PeripheralBluetoothMode::NORMAL_MODE;
                res.enableDisableStatus = EnableDisable::ENABLE;
                SendCommandACK(PeripheralSetStatusParingDeviceManagementCommon, res);
                state.mPairingMode.commit();
            }
        }

        /* STC */
        if (state.mSupport.contains(t1::FunctionType::SMART_TALKING_MODE_TYPE2))
        {
            using namespace t1;
            if (state.mSpeakToChatEnabled.pending())
            {
                SystemSetParamSmartTalking res;
                res.command = Command::SYSTEM_SET_PARAM;
                res.type = SystemInquiredType::SMART_TALKING_MODE_TYPE2;
                res.onOffValue = state.mSpeakToChatEnabled.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                res.previewModeOnOffValue = OnOffSettingValue::OFF;
                SendCommandACK(SystemSetParamSmartTalking, res);
                state.mSpeakToChatEnabled.commit();
            }

            if (state.mSpeakToChatDetectSensitivity.pending() || state.mSpeakToModeOutTime.pending())
            {
                SystemSetExtParamSmartTalkingModeType2 res;
                res.command = Command::SYSTEM_SET_EXT_PARAM;
                res.detectSensitivity = state.mSpeakToChatDetectSensitivity.submitted;
                res.modeOffTime = state.mSpeakToModeOutTime.submitted;
                SendCommandACK(SystemSetExtParamSmartTalkingModeType2, res);
                state.mSpeakToChatDetectSensitivity.commit(), state.mSpeakToModeOutTime.commit();
            }
        }

        /* Listening Mode */
        if (state.mSupport.contains(t1::FunctionType::LISTENING_OPTION))
        {
            using namespace t1;
            if (state.mBGMModeEnabled.pending() || state.mBGMModeRoomSize.pending())
            {
                AudioSetParamBGMMode res;
                res.command = Command::AUDIO_SET_PARAM;
                res.type = AudioInquiredType::BGM_MODE_AND_ERRORCODE;
                res.onOffSettingValue = state.mBGMModeEnabled.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                res.targetRoomSize = state.mBGMModeRoomSize.submitted;
                SendCommandACK(AudioSetParamBGMMode, res);
                state.mBGMModeEnabled.commit(), state.mBGMModeRoomSize.commit();
                MDR_LOG("S/W BGM BGM {} ROOM {} UPMIX {}", state.mBGMModeEnabled.desired, state.mBGMModeRoomSize.desired, state.mUpmixCinemaEnabled.desired);
            }
            if (state.mUpmixCinemaEnabled.pending())
            {
                AudioSetParamUpmixCinema res;
                res.command = Command::AUDIO_SET_PARAM;
                res.onOffSettingValue = state.mUpmixCinemaEnabled.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(AudioSetParamUpmixCinema, res);
                state.mUpmixCinemaEnabled.commit();
                MDR_LOG("S/W CNE BGM {} ROOM {} UPMIX {}", state.mBGMModeEnabled.desired, state.mBGMModeRoomSize.desired, state.mUpmixCinemaEnabled.desired);
            }
        }

        /* EQ */
        if (state.mEqPresetId.pending())
        {
            using namespace t1;
            EqEbbSetParamEq res;
            res.command = Command::EQEBB_SET_PARAM;
            res.type = EqEbbInquiredType::PRESET_EQ;
            res.parameter.presetId = state.mEqPresetId.submitted;
            SendCommandACK(EqEbbSetParamEq, res);
            state.mEqPresetId.commit();
            // Ask for a equalizer param update afterwards
            SendCommandACK(EqEbbGetParam);
        }
        if (state.mEqConfig.pending() || state.mEqClearBass.pending())
        {
            using namespace t1;
            EqEbbSetParamEq res;
            res.command = Command::EQEBB_SET_PARAM;
            res.type = EqEbbInquiredType::PRESET_EQ;
            res.parameter.presetId = state.mEqPresetId.submitted;
            int eqBands = state.mEqConfig.submitted.size(), eqOffset = 0;
            if (eqBands == 0)
            {
                state.mEqConfig.commit(), state.mEqClearBass.commit();
            }
            else
            {
                auto& bands = state.mEqConfig.submitted;
                if (eqBands == 5)
                {
                    res.parameter.bandSteps.value = Vector<UInt8>{{
                        static_cast<UInt8>(state.mEqClearBass.submitted + 10),
                        static_cast<UInt8>(bands[0] + 10),
                        static_cast<UInt8>(bands[1] + 10),
                        static_cast<UInt8>(bands[2] + 10),
                        static_cast<UInt8>(bands[3] + 10),
                        static_cast<UInt8>(bands[4] + 10),
                    }};
                }
                else if (eqBands == 10)
                    res.parameter.bandSteps.value = Vector<UInt8>{{
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
                    co_return SetLastError(MDR_RESULT_ERROR_INVALID_ARGUMENT, "mEqConfig size must be 0, 5, or 10");
                SendCommandACK(EqEbbSetParamEq, res);
                state.mEqConfig.commit();
                state.mEqClearBass.commit();
                // Ask for a equalizer param update afterwards
                SendCommandACK(EqEbbGetParam);
            }
        }

        /* Connection Quality */
        if (state.mSupport.
            contains(t1::FunctionType::CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY))
        {
            if (state.mAudioPriorityMode.pending())
            {
                using namespace t1;
                AudioSetParamConnection res;
                res.command = Command::AUDIO_SET_PARAM;
                res.settingValue = state.mAudioPriorityMode.submitted;
                SendCommandACK(AudioSetParamConnection, res);
                state.mAudioPriorityMode.commit();
            }
        }

        /* DSEE */
        if (state.mSupport.contains(t1::FunctionType::UPSCALING_AUTO_OFF))
        {
            if (state.mUpscalingEnabled.pending())
            {
                using namespace t1;
                AudioSetParamUpscaling res;
                res.command = Command::AUDIO_SET_PARAM;
                res.type = AudioInquiredType::UPSCALING;
                res.settingValue = state.mUpscalingEnabled.submitted
                    ? UpscalingTypeAutoOff::AUTO : UpscalingTypeAutoOff::OFF;
                SendCommandACK(AudioSetParamUpscaling, res);
                state.mUpscalingEnabled.commit();
            }
        }

        /* Touch Functions */
        if (state.mSupport.contains(t1::FunctionType::ASSIGNABLE_SETTING))
        {
            if (state.mTouchFunctionLeft.pending() || state.mTouchFunctionRight.pending())
            {
                using namespace t1;
                SystemSetParamAssignableSettings res;
                res.command = Command::SYSTEM_SET_PARAM;
                res.presetList.value = {state.mTouchFunctionLeft.submitted, state.mTouchFunctionRight.submitted};
                SendCommandACK(SystemSetParamAssignableSettings, res);
                state.mTouchFunctionLeft.commit(), state.mTouchFunctionRight.commit();
            }
        }

        /* Head Gesture */
        if (state.mSupport.contains(t1::FunctionType::HEAD_GESTURE_ON_OFF_TRAINING))
        {
            if (state.mHeadGestureEnabled.pending())
            {
                using namespace t1;
                SystemSetParamCommon res;
                res.command = Command::SYSTEM_SET_PARAM;
                res.type = SystemInquiredType::HEAD_GESTURE_ON_OFF;
                res.settingValue = state.mHeadGestureEnabled.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SystemSetParamCommon, res);
                state.mHeadGestureEnabled.commit();
            }
        }

        /* Auto Power Off */
        if (state.mSupport.contains(t1::FunctionType::AUTO_POWER_OFF))
        {
            using namespace t1;
            if (state.mPowerAutoOff.pending())
            {
                PowerSetParamAutoPowerOff res;
                res.command = Command::POWER_SET_PARAM;
                res.currentPowerOffElements = state.mPowerAutoOff.submitted;
                res.lastSelectPowerOffElements = AutoPowerOffElements::POWER_OFF_IN_5_MIN;
                SendCommandACK(PowerSetParamAutoPowerOff, res);
                state.mPowerAutoOff.commit();
            }
        }
        else if (state.mSupport.contains(t1::FunctionType::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
        {
            using namespace t1;
            if (state.mPowerAutoOffWearingDetection.pending())
            {
                PowerSetParamAutoPowerOffWithWearingDetection res;
                res.command = Command::POWER_SET_PARAM;
                res.currentPowerOffElements = state.mPowerAutoOffWearingDetection.submitted;
                res.lastSelectPowerOffElements = AutoPowerOffWearingDetectionElements::POWER_OFF_IN_5_MIN;
                SendCommandACK(PowerSetParamAutoPowerOffWithWearingDetection, res);
                state.mPowerAutoOffWearingDetection.commit();
            }
        }

        /* Pause when device is removed */
        if (state.mSupport.contains(
            t1::FunctionType::PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF))
        {
            using namespace t1;
            if (state.mAutoPauseEnabled.pending())
            {
                SystemSetParamCommon res;
                res.command = Command::SYSTEM_SET_PARAM;
                res.type = SystemInquiredType::PLAYBACK_CONTROL_BY_WEARING;
                res.settingValue = state.mAutoPauseEnabled.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SystemSetParamCommon, res);
                state.mAutoPauseEnabled.commit();
            }
        }

        /* Voice Guidance */
        if (state.mVoiceGuidanceEnabled.pending())
        {
            using namespace t2;
            VoiceGuidanceSetParamSettingMtk res;
            res.inquiredType = VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH;
            res.settingValue = state.mVoiceGuidanceEnabled.submitted
                ? OnOffSettingValue::ON
                : OnOffSettingValue::OFF;
            SendCommandACK(VoiceGuidanceSetParamSettingMtk, res);
            state.mVoiceGuidanceEnabled.commit();
        }

        /* Voice Guidance */
        if (state.mSupport.contains(
            t2::FunctionType::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT))
        {
            if (state.mVoiceGuidanceVolume.pending())
            {
                using namespace t2;
                VoiceGuidanceSetParamVolume res;
                res.inquiredType = VoiceGuidanceInquiredType::VOLUME;
                res.volumeValue = state.mVoiceGuidanceVolume.submitted;
                res.feedbackSound = OnOffSettingValue::ON;
                SendCommandACK(VoiceGuidanceSetParamVolume, res);
                state.mVoiceGuidanceVolume.commit();
            }
        }

        /* General Settings */
        {
            using namespace t1;
            if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING_1))
            {
                if (state.mGsParamBool1.pending())
                {
                    GsSetParamBoolean res;
                    res.command = Command::GENERAL_SETTING_SET_PARAM;
                    res.type = GsInquiredType::GENERAL_SETTING1;
                    res.value = state.mGsParamBool1.submitted ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsSetParamBoolean, res);
                    state.mGsParamBool1.commit();
                }
            }
            if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING_2))
            {
                if (state.mGsParamBool2.pending())
                {
                    GsSetParamBoolean res;
                    res.command = Command::GENERAL_SETTING_SET_PARAM;
                    res.type = GsInquiredType::GENERAL_SETTING2;
                    res.value = state.mGsParamBool2.submitted ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsSetParamBoolean, res);
                    state.mGsParamBool2.commit();
                }
            }
            if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING_3))
            {
                if (state.mGsParamBool3.pending())
                {
                    GsSetParamBoolean res;
                    res.command = Command::GENERAL_SETTING_SET_PARAM;
                    res.type = GsInquiredType::GENERAL_SETTING3;
                    res.value = state.mGsParamBool3.submitted ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsSetParamBoolean, res);
                    state.mGsParamBool3.commit();
                }
            }
            if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING_4))
            {
                if (state.mGsParamBool4.pending())
                {
                    GsSetParamBoolean res;
                    res.command = Command::GENERAL_SETTING_SET_PARAM;
                    res.type = GsInquiredType::GENERAL_SETTING4;
                    res.value = state.mGsParamBool4.submitted ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsSetParamBoolean, res);
                    state.mGsParamBool4.commit();
                }
            }
        }

        /* Safe Listening */
        if (state.mSafeListeningPreviewMode.pending())
        {
            using namespace t2;
            if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_HBS_1))
            {
                SafeListeningSetParamSL res;
                res.inquiredType = SafeListeningInquiredType::SAFE_LISTENING_HBS_1;
                res.previewMode = OnOffSettingValue::OFF;
                res.safeListeningMode = state.mSafeListeningPreviewMode.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SafeListeningSetParamSL, res);
                state.mSafeListeningPreviewMode.commit();
            }
            else if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_HBS_2))
            {
                SafeListeningSetParamSL res;
                res.inquiredType = SafeListeningInquiredType::SAFE_LISTENING_HBS_2;
                res.previewMode = OnOffSettingValue::OFF;
                res.safeListeningMode = state.mSafeListeningPreviewMode.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SafeListeningSetParamSL, res);
                state.mSafeListeningPreviewMode.commit();
            }
            else if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_TWS_1))
            {
                SafeListeningSetParamSL res;
                res.inquiredType = SafeListeningInquiredType::SAFE_LISTENING_TWS_1;
                res.previewMode = OnOffSettingValue::OFF;
                res.safeListeningMode = state.mSafeListeningPreviewMode.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SafeListeningSetParamSL, res);
                state.mSafeListeningPreviewMode.commit();
            }
            else if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_TWS_2))
            {
                SafeListeningSetParamSL res;
                res.inquiredType = SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
                res.previewMode = OnOffSettingValue::OFF;
                res.safeListeningMode = state.mSafeListeningPreviewMode.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SafeListeningSetParamSL, res);
                state.mSafeListeningPreviewMode.commit();
            }
        }
        co_return MDR_EVENT_APPLY_COMPLETE;
    }

    int MDRHeadphones::HandleProtocolInfoV2(Span<const UInt8> command)
    {
        auto& state = mDetailsV2;
        if (command.size() != sizeof(v2::t1::ConnectRetProtocolInfo))
            return SetLastError(MDR_RESULT_ERROR_MALFORMED_PAYLOAD, "MDR V2 protocol info size is not valid");
        const auto result = (v2::t1::ConnectRetProtocolInfo::Deserialize)(command.data(), command.size());
        if (!result)
            return SetLastError(
                result.error,
                result.errMessage ? result.errMessage : "Unable to deserialize MDR V2 protocol info");
        state.mProtocol = {
            .version = result.value.protocolVersion,
            .hasTable1 = result.value.supportTable1Value == v2::EnableDisable::ENABLE,
            .hasTable2 = result.value.supportTable2Value == v2::EnableDisable::ENABLE
        };
        Awake(AWAIT_PROTOCOL_INFO);
        return MDR_EVENT_IDENTITY_CHANGED;
    }

    bool MDRHeadphones::IsDirtyV2() const
    {
        const auto& state = mDetailsV2;
        return state.mShutdown.dirty() || state.mNcAsmEnabled.dirty() ||
            state.mNcAsmFocusOnVoice.dirty() || state.mNcAsmAmbientLevel.dirty() ||
            state.mNcAsmButtonFunction.dirty() || state.mNcAsmMode.dirty() ||
            state.mNcAsmAutoAsmEnabled.dirty() || state.mNcAsmNoiseAdaptiveSensitivity.dirty() ||
            state.mPowerAutoOff.dirty() || state.mPowerAutoOffWearingDetection.dirty() ||
            state.mPlayVolume.dirty() || state.mPlayControl.dirty() ||
            state.mGsParamBool1.dirty() || state.mGsParamBool2.dirty() ||
            state.mGsParamBool3.dirty() || state.mGsParamBool4.dirty() ||
            state.mUpscalingEnabled.dirty() || state.mAudioPriorityMode.dirty() ||
            state.mBGMModeEnabled.dirty() || state.mBGMModeRoomSize.dirty() ||
            state.mUpmixCinemaEnabled.dirty() || state.mAutoPauseEnabled.dirty() ||
            state.mTouchFunctionLeft.dirty() || state.mTouchFunctionRight.dirty() ||
            state.mSpeakToChatEnabled.dirty() || state.mSpeakToChatDetectSensitivity.dirty() ||
            state.mSpeakToModeOutTime.dirty() || state.mHeadGestureEnabled.dirty() ||
            state.mEqAvailable.dirty() || state.mEqPresetId.dirty() ||
            state.mEqClearBass.dirty() || state.mEqConfig.dirty() ||
            state.mVoiceGuidanceEnabled.dirty() || state.mVoiceGuidanceVolume.dirty() ||
            state.mPairingMode.dirty() || state.mMultipointDeviceMac.dirty() ||
            state.mSafeListeningPreviewMode.dirty() || state.mSourceSwitchControlEnabled.dirty() ||
            state.mPairedDeviceConnectMac.dirty() || state.mPairedDeviceDisconnectMac.dirty() ||
            state.mPairedDeviceUnpairMac.dirty();
    }

#pragma endregion
}
// NOLINTEND
