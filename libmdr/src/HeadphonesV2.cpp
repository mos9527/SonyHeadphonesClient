#include <algorithm>
#include "Details.hpp"
// NOLINTBEGIN
namespace mdr
{
    using namespace v2;
    MDRTask MDRHeadphones::RequestInitV2()
    {
        mProtocolFamily = ProtocolFamily::UNKNOWN;
        mProtocol = {};

        SendCommandACK(t1::ConnectGetProtocolInfo);
        const int result = co_await Await(AWAIT_PROTOCOL_INFO);
        if (result != MDR_RESULT_OK)
        {
            SetLastError(result, "Unable to initialize MDR V2");
            co_return MDR_HEADPHONES_ERROR;
        }
        if (mProtocolFamily != ProtocolFamily::V2)
        {
            SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "Device does not use MDR V2");
            co_return MDR_HEADPHONES_ERROR;
        }
        co_return co_await RequestInitV2Selected();
    }

    MDRTask MDRHeadphones::RequestInitV2Selected()
    {
        if (!mProtocol.hasTable1)
        {
            SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "Device doesn't support MDR V2 Table 1");
            co_return MDR_HEADPHONES_ERROR;
        }
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
            if (mSupport.contains(FunctionType_Table1::GENERAL_SETTING_1))
            {
                SendCommandACK(t1::GsGetCapability, {
                               .type = t1::GsInquiredType::GENERAL_SETTING1, .displayLanguage = lang
                               });
                SendCommandACK(t1::GsGetParam, {
                               .type = t1::GsInquiredType::GENERAL_SETTING1
                               });
            }
            if (mSupport.contains(FunctionType_Table1::GENERAL_SETTING_2))
            {
                SendCommandACK(t1::GsGetCapability, {
                               .type = t1::GsInquiredType::GENERAL_SETTING2, .displayLanguage = lang
                               });
                SendCommandACK(t1::GsGetParam, {
                               .type = t1::GsInquiredType::GENERAL_SETTING2
                               });
            }
            if (mSupport.contains(FunctionType_Table1::GENERAL_SETTING_3))
            {
                SendCommandACK(t1::GsGetCapability, {
                               .type = t1::GsInquiredType::GENERAL_SETTING3, .displayLanguage = lang
                               });
                SendCommandACK(t1::GsGetParam, {
                               .type = t1::GsInquiredType::GENERAL_SETTING3
                               });
            }
            if (mSupport.contains(FunctionType_Table1::GENERAL_SETTING_4))
            {
                SendCommandACK(t1::GsGetCapability, {
                               .type = t1::GsInquiredType::GENERAL_SETTING4, .displayLanguage = lang
                               });
                SendCommandACK(t1::GsGetParam, {
                               .type = t1::GsInquiredType::GENERAL_SETTING4
                               });
            }

            /* DSEE */
            if (mSupport.contains(FunctionType_Table1::UPSCALING_AUTO_OFF))
                SendCommandACK(t1::AudioGetCapability, {
                           .type = t1::AudioInquiredType::UPSCALING
                           });
        }
        /* Receive alerts for certain operations like toggling multipoint */
        if (mSupport.contains(FunctionType_Table1::FIXED_MESSAGE))
            SendCommandACK(t1::AlertSetStatusFixedMessage, { .status = EnableDisable::ENABLE});

        /* Codec Type */
        if (mSupport.contains(FunctionType_Table1::CODEC_INDICATOR))
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
            FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            SendCommandACK(t1::NcAsmGetParam,
                           { .inquiredType = t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS});
        }
        else if (mSupport.contains(
            FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
        {
            SendCommandACK(t1::NcAsmGetParam,
                           { .inquiredType = t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA});
        }
        else if (mSupport.contains(FunctionType_Table1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            SendCommandACK(t1::NcAsmGetParam, { .inquiredType = t1::NcAsmInquiredType::ASM_SEAMLESS});
        }

        /* Pairing Management */
        constexpr FunctionType_Table2 kPairingFunctions[] = {
            FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT,
            FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE
        };
        if (std::ranges::any_of(kPairingFunctions, [&](auto x) { return mSupport.contains(x); }))
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
        
        if (mSupport.contains(FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
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

        /* Speak To Chat */
        if (mSupport.contains(FunctionType_Table1::SMART_TALKING_MODE_TYPE2))
        {
            SendCommandACK(t1::SystemGetParam, {.type = t1::SystemInquiredType::SMART_TALKING_MODE_TYPE2});
            SendCommandACK(t1::SystemGetExtParam, {.type = t1::SystemInquiredType::SMART_TALKING_MODE_TYPE2});
        }

        /* Listening Mode */
        if (mSupport.contains(FunctionType_Table1::LISTENING_OPTION))
        {
            SendCommandACK(t1::AudioGetParam, {.type = t1::AudioInquiredType::BGM_MODE_AND_ERRORCODE});
            SendCommandACK(t1::AudioGetParam, {.type = t1::AudioInquiredType::UPMIX_CINEMA});
        }

        /* Equalizer */
        SendCommandACK(t1::EqEbbGetStatus, {.type = t1::EqEbbInquiredType::PRESET_EQ});
        SendCommandACK(t1::EqEbbGetParam);

        /* Connection Quality */
        if (mSupport.contains(
            FunctionType_Table1::CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY))
            SendCommandACK(t1::AudioGetParam, {.type = t1::AudioInquiredType::CONNECTION_MODE});

        /* DSEE */
        if (mSupport.contains(FunctionType_Table1::UPSCALING_AUTO_OFF))
        {
            SendCommandACK(t1::AudioGetStatus, {.type = t1::AudioInquiredType::UPSCALING});
            SendCommandACK(t1::AudioGetParam, {.type = t1::AudioInquiredType::UPSCALING});
        }

        /* Touch Sensor */
        if (mSupport.contains(FunctionType_Table1::ASSIGNABLE_SETTING))
            SendCommandACK(t1::SystemGetParam, {.type = t1::SystemInquiredType::ASSIGNABLE_SETTINGS });

        /* NC/AMB Toggle */
        if (mSupport.contains(FunctionType_Table1::AMBIENT_SOUND_CONTROL_MODE_SELECT))
            SendCommandACK(t1::NcAsmGetParam, {.inquiredType = t1::NcAsmInquiredType::NC_AMB_TOGGLE });

        /* Head Gesture */
        if (mSupport.contains(FunctionType_Table1::HEAD_GESTURE_ON_OFF_TRAINING))
            SendCommandACK(t1::SystemGetParam, {.type = t1::SystemInquiredType::HEAD_GESTURE_ON_OFF });

        /* Auto Power Off */
        if (mSupport.contains(FunctionType_Table1::AUTO_POWER_OFF))
        {
            SendCommandACK(t1::PowerGetParam, {.type = t1::PowerInquiredType::AUTO_POWER_OFF});
        }
        else if (mSupport.contains(FunctionType_Table1::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
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
        co_return MDR_HEADPHONES_TASK_INIT_OK;
    }

    MDRTask MDRHeadphones::RequestSyncV2()
    {
        /* Single Battery */
        if (mSupport.contains(FunctionType_Table1::BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::BATTERY});
        }
        else if (mSupport.contains(FunctionType_Table1::BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::BATTERY_WITH_THRESHOLD});
        }

        /* L + R Battery */
        if (mSupport.contains(FunctionType_Table1::LEFT_RIGHT_BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::LEFT_RIGHT_BATTERY});
        }
        else if (mSupport.contains(FunctionType_Table1::LR_BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::LR_BATTERY_WITH_THRESHOLD});
        }

        /* Case Battery */
        if (mSupport.contains(FunctionType_Table1::CRADLE_BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::CRADLE_BATTERY});
        }
        else if (mSupport.contains(FunctionType_Table1::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::CRADLE_BATTERY_WITH_THRESHOLD});
        }

        /* Sound Pressure */
        if (mSupport.contains(FunctionType_Table2::SAFE_LISTENING_HBS_1))
        {
            SendCommandACK(t2::SafeListeningGetExtendedParam,
                           {.inquiredType = t2::SafeListeningInquiredType::SAFE_LISTENING_HBS_1});
        }
        else if (mSupport.contains(FunctionType_Table2::SAFE_LISTENING_HBS_2))
        {
            SendCommandACK(t2::SafeListeningGetExtendedParam,
                           {.inquiredType = t2::SafeListeningInquiredType::SAFE_LISTENING_HBS_2});
        }
        else if (mSupport.contains(FunctionType_Table2::SAFE_LISTENING_TWS_1))
        {
            SendCommandACK(t2::SafeListeningGetExtendedParam,
                           {.inquiredType = t2::SafeListeningInquiredType::SAFE_LISTENING_TWS_1});
        }
        else if (mSupport.contains(FunctionType_Table2::SAFE_LISTENING_TWS_2))
        {
            SendCommandACK(t2::SafeListeningGetExtendedParam,
                           {.inquiredType = t2::SafeListeningInquiredType::SAFE_LISTENING_TWS_2});
        }
        co_return MDR_HEADPHONES_TASK_SYNC_OK;
    }

    void MDRHeadphones::SnapshotProperties()
    {
        mShutdown.submit();
        mNcAsmEnabled.submit();
        mNcAsmFocusOnVoice.submit();
        mNcAsmAmbientLevel.submit();
        mNcAsmButtonFunction.submit();
        mNcAsmMode.submit();
        mNcAsmAutoAsmEnabled.submit();
        mNcAsmNoiseAdaptiveSensitivity.submit();
        mPowerAutoOff.submit();
        mPowerAutoOffWearingDetection.submit();
        mPlayVolume.submit();
        mPlayControl.submit();
        mGsParamBool1.submit();
        mGsParamBool2.submit();
        mGsParamBool3.submit();
        mGsParamBool4.submit();
        mUpscalingEnabled.submit();
        mAudioPriorityMode.submit();
        mBGMModeEnabled.submit();
        mBGMModeRoomSize.submit();
        mUpmixCinemaEnabled.submit();
        mAutoPauseEnabled.submit();
        mTouchFunctionLeft.submit();
        mTouchFunctionRight.submit();
        mSpeakToChatEnabled.submit();
        mSpeakToChatDetectSensitivity.submit();
        mSpeakToModeOutTime.submit();
        mHeadGestureEnabled.submit();
        mEqAvailable.submit();
        mEqPresetId.submit();
        mEqClearBass.submit();
        mEqConfig.submit();
        mVoiceGuidanceEnabled.submit();
        mVoiceGuidanceVolume.submit();
        mPairingMode.submit();
        mMultipointDeviceMac.submit();
        mPairedDeviceDisconnectMac.submit();
        mPairedDeviceConnectMac.submit();
        mPairedDeviceUnpairMac.submit();
        mSafeListeningPreviewMode.submit();
    }

    MDRTask MDRHeadphones::RequestCommitV2()
    {
        SnapshotProperties();

        /* Shutdown */
        if (mShutdown.submittedDirty())
        {
            using namespace t1;
            if (mSupport.contains(FunctionType_Table1::POWER_OFF) && mShutdown.submitted)
            {
                SendCommandACK(PowerSetStatusPowerOff);
                mShutdown.commitOneShot(false);
            }
            else
                mShutdown.commitOneShot(false);
        }
        /* NC/ASM */
        if (mNcAsmAmbientLevel.submittedDirty() || mNcAsmEnabled.submittedDirty() ||
            mNcAsmMode.submittedDirty() || mNcAsmFocusOnVoice.submittedDirty() ||
            mNcAsmAutoAsmEnabled.submittedDirty() || mNcAsmNoiseAdaptiveSensitivity.submittedDirty())
        {
            using namespace t1;
            if (mSupport.contains(
                FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
            {

                NcAsmSetParamModeNcDualModeSwitchAsmSeamlessNa res;
                res.command = Command::NCASM_SET_PARAM;
                res.valueChangeStatus = ValueChangeStatus::CHANGED;
                res.ncAsmTotalEffect = mNcAsmEnabled.submitted ? NcAsmOnOffValue::ON : NcAsmOnOffValue::OFF;
                res.ncAsmMode = mNcAsmMode.submitted;
                res.ambientSoundMode = mNcAsmFocusOnVoice.submitted
                    ? AmbientSoundMode::VOICE : AmbientSoundMode::NORMAL;
                res.ambientSoundLevelValue = mNcAsmAmbientLevel.submitted;
                res.ncAsmOnOffValue = mNcAsmAutoAsmEnabled.submitted
                    ? NcAsmOnOffValue::ON : NcAsmOnOffValue::OFF;
                res.noiseAdaptiveSensitivitySettings = mNcAsmNoiseAdaptiveSensitivity.submitted;
                SendCommandACK(NcAsmSetParamModeNcDualModeSwitchAsmSeamlessNa, res);
            }
            else if (mSupport.contains(FunctionType_Table1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
            {
                NcAsmSetParamAsmSeamless res;
                res.command = Command::NCASM_SET_PARAM;
                res.valueChangeStatus = ValueChangeStatus::CHANGED;
                res.ncAsmTotalEffect = mNcAsmEnabled.submitted ? NcAsmOnOffValue::ON : NcAsmOnOffValue::OFF;
                res.ambientSoundMode = mNcAsmFocusOnVoice.submitted
                    ? AmbientSoundMode::VOICE : AmbientSoundMode::NORMAL;
                res.ambientSoundLevelValue = mNcAsmAmbientLevel.submitted;
                SendCommandACK(NcAsmSetParamAsmSeamless, res);
            }
            else
            {
                NcAsmSetParamModeNcDualModeSwitchAsmSeamless res;
                res.command = Command::NCASM_SET_PARAM;
                res.valueChangeStatus = ValueChangeStatus::CHANGED;
                res.ncAsmTotalEffect = mNcAsmEnabled.submitted ? NcAsmOnOffValue::ON : NcAsmOnOffValue::OFF;
                res.ncAsmMode = mNcAsmMode.submitted,
                    res.ambientSoundMode = mNcAsmFocusOnVoice.submitted
                    ? AmbientSoundMode::VOICE
                    : AmbientSoundMode::NORMAL;
                res.ambientSoundLevelValue = mNcAsmAmbientLevel.submitted;
                SendCommandACK(NcAsmSetParamModeNcDualModeSwitchAsmSeamless, res);
            }
            mNcAsmAmbientLevel.commit(), mNcAsmEnabled.commit(), mNcAsmMode.commit();
            mNcAsmFocusOnVoice.commit(), mNcAsmAutoAsmEnabled.commit(), mNcAsmNoiseAdaptiveSensitivity.commit();
        }

        /* NC/AMB Mode */
        if (mSupport.contains(FunctionType_Table1::AMBIENT_SOUND_CONTROL_MODE_SELECT))
        {
            using namespace t1;
            if (mNcAsmButtonFunction.submittedDirty())
            {
                NcAsmSetParamNcAmbToggle res;
                res.command = Command::NCASM_SET_PARAM;
                res.function = mNcAsmButtonFunction.submitted;
                SendCommandACK(NcAsmSetParamNcAmbToggle, res);
                mNcAsmButtonFunction.commit();
            }
        }
        /* Volume */
        if (mPlayVolume.submittedDirty())
        {
            using namespace t1;
            SetPlayParamPlaybackControllerVolume res;
            res.command = Command::PLAY_SET_PARAM;
            res.type = PlayInquiredType::MUSIC_VOLUME;
            res.volumeValue = mPlayVolume.submitted;
            SendCommandACK(SetPlayParamPlaybackControllerVolume, res);
            mPlayVolume.commit();
        }
        /* Play Control */
        // A bit of a special case. We reset the value to something else
        // so simply setting 'desired' repeatedly works as intended
        if (mPlayControl.submittedDirty())
        {
            using namespace t1;
            SetPlayStatusPlaybackController res;
            res.command = Command::PLAY_SET_STATUS;
            res.type = PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT;
            res.status = EnableDisable::ENABLE;
            res.control = mPlayControl.submitted;
            SendCommandACK(SetPlayStatusPlaybackController, res);
            mPlayControl.commitOneShot(PlaybackControl::KEY_OFF);
        }

        /* Multipoint Switch */
        if (mMultipointDeviceMac.submittedDirty())
        {
            using namespace t2;
            PeripheralSetExtendedParamSourceSwitchControl res;
            if (mMultipointDeviceMac.submitted.length() != 17)
                mMultipointDeviceMac.commitOneShot("");
            else
            {
                res.targetBdAddress.value = mMultipointDeviceMac.submitted;
                SendCommandACK(PeripheralSetExtendedParamSourceSwitchControl, res);
                mMultipointDeviceMac.commit();
            }
        }

        /* Connection Ops */
        {
            using namespace t2;
            PeripheralInquiredType type = PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT;
            if (
                mSupport.contains(
                    FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT)
                ||
                mSupport.contains(
                    FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE)
            )
            {
                type = PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE;
            }
            else if (mSupport.contains(FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
            {
                type = PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT;
            }
            else
            {
                // Unsupported. Ignore the rest.
                mPairedDeviceConnectMac.commitOneShot("");
                mPairedDeviceDisconnectMac.commitOneShot("");
                mPairedDeviceUnpairMac.commitOneShot("");
            }
            PeripheralSetExtendedParamParingDeviceManagementCommon res;
            res.inquiredType = type;
            if (mPairedDeviceConnectMac.submittedDirty())
            {
                res.connectivityActionType = ConnectivityActionType::CONNECT;
                res.btDeviceAddress.value = mPairedDeviceConnectMac.submitted;
                SendCommandACK(PeripheralSetExtendedParamParingDeviceManagementCommon, res);
                mPairedDeviceConnectMac.commitOneShot("");
            }
            if (mPairedDeviceDisconnectMac.submittedDirty())
            {
                res.connectivityActionType = ConnectivityActionType::DISCONNECT;
                res.btDeviceAddress.value = mPairedDeviceDisconnectMac.submitted;
                SendCommandACK(PeripheralSetExtendedParamParingDeviceManagementCommon, res);
                mPairedDeviceDisconnectMac.commitOneShot("");
            }
            if (mPairedDeviceUnpairMac.submittedDirty())
            {
                res.connectivityActionType = ConnectivityActionType::UNPAIR;
                res.btDeviceAddress.value = mPairedDeviceUnpairMac.submitted;
                SendCommandACK(PeripheralSetExtendedParamParingDeviceManagementCommon, res);
                mPairedDeviceUnpairMac.commitOneShot("");
            }
        

            /* Pairing Mode */
            if (mPairingMode.submittedDirty())
            {
                using namespace t2;
                PeripheralSetStatusParingDeviceManagementCommon res;
                res.inquiredType = type;
                res.btMode = mPairingMode.submitted
                    ? PeripheralBluetoothMode::INQUIRY_SCAN_MODE
                    : PeripheralBluetoothMode::NORMAL_MODE;
                res.enableDisableStatus = EnableDisable::ENABLE;
                SendCommandACK(PeripheralSetStatusParingDeviceManagementCommon, res);
                mPairingMode.commit();
            }
        }

        /* STC */
        if (mSupport.contains(FunctionType_Table1::SMART_TALKING_MODE_TYPE2))
        {
            using namespace t1;
            if (mSpeakToChatEnabled.submittedDirty())
            {
                SystemSetParamSmartTalking res;
                res.command = Command::SYSTEM_SET_PARAM;
                res.type = SystemInquiredType::SMART_TALKING_MODE_TYPE2;
                res.onOffValue = mSpeakToChatEnabled.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                res.previewModeOnOffValue = OnOffSettingValue::OFF;
                SendCommandACK(SystemSetParamSmartTalking, res);
                mSpeakToChatEnabled.commit();
            }

            if (mSpeakToChatDetectSensitivity.submittedDirty() || mSpeakToModeOutTime.submittedDirty())
            {
                SystemSetExtParamSmartTalkingModeType2 res;
                res.command = Command::SYSTEM_SET_EXT_PARAM;
                res.detectSensitivity = mSpeakToChatDetectSensitivity.submitted;
                res.modeOffTime = mSpeakToModeOutTime.submitted;
                SendCommandACK(SystemSetExtParamSmartTalkingModeType2, res);
                mSpeakToChatDetectSensitivity.commit(), mSpeakToModeOutTime.commit();
            }
        }

        /* Listening Mode */
        if (mSupport.contains(FunctionType_Table1::LISTENING_OPTION))
        {
            using namespace t1;
            if (mBGMModeEnabled.submittedDirty() || mBGMModeRoomSize.submittedDirty())
            {
                AudioSetParamBGMMode res;
                res.command = Command::AUDIO_SET_PARAM;
                res.type = AudioInquiredType::BGM_MODE_AND_ERRORCODE;
                res.onOffSettingValue = mBGMModeEnabled.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                res.targetRoomSize = mBGMModeRoomSize.submitted;
                SendCommandACK(AudioSetParamBGMMode, res);
                mBGMModeEnabled.commit(), mBGMModeRoomSize.commit();
                MDR_LOG("S/W BGM BGM {} ROOM {} UPMIX {}", mBGMModeEnabled.desired, mBGMModeRoomSize.desired, mUpmixCinemaEnabled.desired);
            }
            if (mUpmixCinemaEnabled.submittedDirty())
            {
                AudioSetParamUpmixCinema res;
                res.command = Command::AUDIO_SET_PARAM;
                res.onOffSettingValue = mUpmixCinemaEnabled.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(AudioSetParamUpmixCinema, res);
                mUpmixCinemaEnabled.commit();
                MDR_LOG("S/W CNE BGM {} ROOM {} UPMIX {}", mBGMModeEnabled.desired, mBGMModeRoomSize.desired, mUpmixCinemaEnabled.desired);
            }
        }

        /* EQ */
        if (mEqPresetId.submittedDirty())
        {
            using namespace t1;
            EqEbbSetParamEq res;
            res.command = Command::EQEBB_SET_PARAM;
            res.type = EqEbbInquiredType::PRESET_EQ;
            res.parameter.presetId = mEqPresetId.submitted;
            SendCommandACK(EqEbbSetParamEq, res);
            mEqPresetId.commit();
            // Ask for a equalizer param update afterwards
            SendCommandACK(EqEbbGetParam);
        }
        if (mEqConfig.submittedDirty() || mEqClearBass.submittedDirty())
        {
            using namespace t1;
            EqEbbSetParamEq res;
            res.command = Command::EQEBB_SET_PARAM;
            res.type = EqEbbInquiredType::PRESET_EQ;
            res.parameter.presetId = mEqPresetId.submitted;
            int eqBands = mEqConfig.submitted.size(), eqOffset = 0;
            if (eqBands == 0)
            {
                mEqConfig.commit(), mEqClearBass.commit();
            }
            else
            {
                auto& bands = mEqConfig.submitted;
                if (eqBands == 5)
                {
                    res.parameter.bandSteps.value = Vector<UInt8>{{
                        static_cast<UInt8>(mEqClearBass.submitted + 10),
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
                {
                    SetLastError(MDR_RESULT_ERROR_INVALID_ARGUMENT, "mEqConfig size must be 0, 5, or 10");
                    co_return MDR_HEADPHONES_ERROR;
                }
                SendCommandACK(EqEbbSetParamEq, res);
                mEqConfig.commit();
                mEqClearBass.commit();
                // Ask for a equalizer param update afterwards
                SendCommandACK(EqEbbGetParam);
            }
        }

        /* Connection Quality */
        if (mSupport.
            contains(FunctionType_Table1::CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY))
        {
            if (mAudioPriorityMode.submittedDirty())
            {
                using namespace t1;
                AudioSetParamConnection res;
                res.command = Command::AUDIO_SET_PARAM;
                res.settingValue = mAudioPriorityMode.submitted;
                SendCommandACK(AudioSetParamConnection, res);
                mAudioPriorityMode.commit();
            }
        }

        /* DSEE */
        if (mSupport.contains(FunctionType_Table1::UPSCALING_AUTO_OFF))
        {
            if (mUpscalingEnabled.submittedDirty())
            {
                using namespace t1;
                AudioSetParamUpscaling res;
                res.command = Command::AUDIO_SET_PARAM;
                res.type = AudioInquiredType::UPSCALING;
                res.settingValue = mUpscalingEnabled.submitted
                    ? UpscalingTypeAutoOff::AUTO : UpscalingTypeAutoOff::OFF;
                SendCommandACK(AudioSetParamUpscaling, res);
                mUpscalingEnabled.commit();
            }
        }

        /* Touch Functions */
        if (mSupport.contains(FunctionType_Table1::ASSIGNABLE_SETTING))
        {
            if (mTouchFunctionLeft.submittedDirty() || mTouchFunctionRight.submittedDirty())
            {
                using namespace t1;
                SystemSetParamAssignableSettings res;
                res.command = Command::SYSTEM_SET_PARAM;
                res.presetList.value = {mTouchFunctionLeft.submitted, mTouchFunctionRight.submitted};
                SendCommandACK(SystemSetParamAssignableSettings, res);
                mTouchFunctionLeft.commit(), mTouchFunctionRight.commit();
            }
        }

        /* Head Gesture */
        if (mSupport.contains(FunctionType_Table1::HEAD_GESTURE_ON_OFF_TRAINING))
        {
            if (mHeadGestureEnabled.submittedDirty())
            {
                using namespace t1;
                SystemSetParamCommon res;
                res.command = Command::SYSTEM_SET_PARAM;
                res.type = SystemInquiredType::HEAD_GESTURE_ON_OFF;
                res.settingValue = mHeadGestureEnabled.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SystemSetParamCommon, res);
                mHeadGestureEnabled.commit();
            }
        }

        /* Auto Power Off */
        if (mSupport.contains(FunctionType_Table1::AUTO_POWER_OFF))
        {
            using namespace t1;
            if (mPowerAutoOff.submittedDirty())
            {
                PowerSetParamAutoPowerOff res;
                res.command = Command::POWER_SET_PARAM;
                res.currentPowerOffElements = mPowerAutoOff.submitted;
                res.lastSelectPowerOffElements = AutoPowerOffElements::POWER_OFF_IN_5_MIN;
                SendCommandACK(PowerSetParamAutoPowerOff, res);
                mPowerAutoOff.commit();
            }
        }
        else if (mSupport.contains(FunctionType_Table1::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
        {
            using namespace t1;
            if (mPowerAutoOffWearingDetection.submittedDirty())
            {
                PowerSetParamAutoPowerOffWithWearingDetection res;
                res.command = Command::POWER_SET_PARAM;
                res.currentPowerOffElements = mPowerAutoOffWearingDetection.submitted;
                res.lastSelectPowerOffElements = AutoPowerOffWearingDetectionElements::POWER_OFF_IN_5_MIN;
                SendCommandACK(PowerSetParamAutoPowerOffWithWearingDetection, res);
                mPowerAutoOffWearingDetection.commit();
            }
        }

        /* Pause when device is removed */
        if (mSupport.contains(
            FunctionType_Table1::PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF))
        {
            using namespace t1;
            if (mAutoPauseEnabled.submittedDirty())
            {
                SystemSetParamCommon res;
                res.command = Command::SYSTEM_SET_PARAM;
                res.type = SystemInquiredType::PLAYBACK_CONTROL_BY_WEARING;
                res.settingValue = mAutoPauseEnabled.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SystemSetParamCommon, res);
                mAutoPauseEnabled.commit();
            }
        }

        /* Voice Guidance */
        if (mVoiceGuidanceEnabled.submittedDirty())
        {
            using namespace t2;
            VoiceGuidanceSetParamSettingMtk res;
            res.inquiredType = VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH;
            res.settingValue = mVoiceGuidanceEnabled.submitted
                ? OnOffSettingValue::ON
                : OnOffSettingValue::OFF;
            SendCommandACK(VoiceGuidanceSetParamSettingMtk, res);
            mVoiceGuidanceEnabled.commit();
        }

        /* Voice Guidance */
        if (mSupport.contains(
            FunctionType_Table2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT))
        {
            if (mVoiceGuidanceVolume.submittedDirty())
            {
                using namespace t2;
                VoiceGuidanceSetParamVolume res;
                res.inquiredType = VoiceGuidanceInquiredType::VOLUME;
                res.volumeValue = mVoiceGuidanceVolume.submitted;
                res.feedbackSound = OnOffSettingValue::ON;
                SendCommandACK(VoiceGuidanceSetParamVolume, res);
                mVoiceGuidanceVolume.commit();
            }
        }

        /* General Settings */
        {
            using namespace t1;
            if (mSupport.contains(FunctionType_Table1::GENERAL_SETTING_1))
            {
                if (mGsParamBool1.submittedDirty())
                {
                    GsSetParamBoolean res;
                    res.command = Command::GENERAL_SETTING_SET_PARAM;
                    res.type = GsInquiredType::GENERAL_SETTING1;
                    res.value = mGsParamBool1.submitted ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsSetParamBoolean, res);
                    mGsParamBool1.commit();
                }
            }
            if (mSupport.contains(FunctionType_Table1::GENERAL_SETTING_2))
            {
                if (mGsParamBool2.submittedDirty())
                {
                    GsSetParamBoolean res;
                    res.command = Command::GENERAL_SETTING_SET_PARAM;
                    res.type = GsInquiredType::GENERAL_SETTING2;
                    res.value = mGsParamBool2.submitted ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsSetParamBoolean, res);
                    mGsParamBool2.commit();
                }
            }
            if (mSupport.contains(FunctionType_Table1::GENERAL_SETTING_3))
            {
                if (mGsParamBool3.submittedDirty())
                {
                    GsSetParamBoolean res;
                    res.command = Command::GENERAL_SETTING_SET_PARAM;
                    res.type = GsInquiredType::GENERAL_SETTING3;
                    res.value = mGsParamBool3.submitted ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsSetParamBoolean, res);
                    mGsParamBool3.commit();
                }
            }
            if (mSupport.contains(FunctionType_Table1::GENERAL_SETTING_4))
            {
                if (mGsParamBool4.submittedDirty())
                {
                    GsSetParamBoolean res;
                    res.command = Command::GENERAL_SETTING_SET_PARAM;
                    res.type = GsInquiredType::GENERAL_SETTING4;
                    res.value = mGsParamBool4.submitted ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsSetParamBoolean, res);
                    mGsParamBool4.commit();
                }
            }
        }

        /* Safe Listening */
        if (mSafeListeningPreviewMode.submittedDirty())
        {
            using namespace t2;
            if (mSupport.contains(FunctionType_Table2::SAFE_LISTENING_HBS_1))
            {
                SafeListeningSetParamSL res;
                res.inquiredType = SafeListeningInquiredType::SAFE_LISTENING_HBS_1;
                res.previewMode = OnOffSettingValue::OFF;
                res.safeListeningMode = mSafeListeningPreviewMode.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SafeListeningSetParamSL, res);
                mSafeListeningPreviewMode.commit();
            }
            else if (mSupport.contains(FunctionType_Table2::SAFE_LISTENING_HBS_2))
            {
                SafeListeningSetParamSL res;
                res.inquiredType = SafeListeningInquiredType::SAFE_LISTENING_HBS_2;
                res.previewMode = OnOffSettingValue::OFF;
                res.safeListeningMode = mSafeListeningPreviewMode.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SafeListeningSetParamSL, res);
                mSafeListeningPreviewMode.commit();
            }
            else if (mSupport.contains(FunctionType_Table2::SAFE_LISTENING_TWS_1))
            {
                SafeListeningSetParamSL res;
                res.inquiredType = SafeListeningInquiredType::SAFE_LISTENING_TWS_1;
                res.previewMode = OnOffSettingValue::OFF;
                res.safeListeningMode = mSafeListeningPreviewMode.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SafeListeningSetParamSL, res);
                mSafeListeningPreviewMode.commit();
            }
            else if (mSupport.contains(FunctionType_Table2::SAFE_LISTENING_TWS_2))
            {
                SafeListeningSetParamSL res;
                res.inquiredType = SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
                res.previewMode = OnOffSettingValue::OFF;
                res.safeListeningMode = mSafeListeningPreviewMode.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SafeListeningSetParamSL, res);
                mSafeListeningPreviewMode.commit();
            }
        }
        co_return MDR_HEADPHONES_TASK_COMMIT_OK;
    }
#pragma endregion
}
// NOLINTEND
