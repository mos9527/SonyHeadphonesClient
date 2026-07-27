#include <mdr/Headphones.hpp>
#include <algorithm>
#include "Macros.hpp"
namespace mdr
{
    using namespace v2;
    using namespace t1;

    int HandleProtocolInfoT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(ConnectRetProtocolInfo, res, cmd);
        self->mProtocol = {
            .version = res.protocolVersion,
            .hasTable1 = res.supportTable1Value == MessageMdrV2EnableDisable::ENABLE,
            .hasTable2 = res.supportTable1Value == MessageMdrV2EnableDisable::ENABLE
        };
        self->Awake(MDRHeadphones::AWAIT_PROTOCOL_INFO);
        return MDR_HEADPHONES_EVT_OK;
    }

    int HandleSupportFunctionT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(ConnectRetSupportFunction, res, cmd);
        std::ranges::fill(self->mSupport.table1Functions, false);
        for (auto fun : res.supportFunctions)
            self->mSupport.table1Functions[static_cast<UInt8>(fun.table1)] = true;
        self->Awake(MDRHeadphones::AWAIT_SUPPORT_FUNCTION);
        return MDR_HEADPHONES_EVT_SUPPORT_FUNCTIONS;
    }

    int HandleCapabilityInfoT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(ConnectRetCapabilityInfo, res, cmd);
        self->mUniqueId = res.uniqueID.value;
        return MDR_HEADPHONES_EVT_OK;
    }

    int HandleDeviceInfoT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(ConnectRetDeviceInfoBase, base, cmd);
        using enum DeviceInfoType;
        switch (base.type)
        {
        case MODEL_NAME:
        {
            Deserialize(ConnectRetDeviceInfoModelName, res, cmd);
            self->mModelName = res.value.value;
            return MDR_HEADPHONES_EVT_DEVICE_INFO;
        }
        case FW_VERSION:
        {
            Deserialize(ConnectRetDeviceInfoFwVersion, res, cmd);
            self->mFWVersion = res.value.value;
            return MDR_HEADPHONES_EVT_DEVICE_INFO;
        }
        case SERIES_AND_COLOR_INFO:
        {
            Deserialize(ConnectRetDeviceInfoSeriesAndColor, res, cmd);
            self->mModelSeries = res.series;
            self->mModelColor = res.color;
            return MDR_HEADPHONES_EVT_DEVICE_INFO;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandleCommonStatusT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(CommonBase, base, cmd);
        using enum CommonInquiredType;
        switch (base.type)
        {
        case AUDIO_CODEC:
        {
            Deserialize(CommonStatusAudioCodec, res, cmd);
            self->mAudioCodec = res.audioCodec;
            return MDR_HEADPHONES_EVT_CODEC;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandleNcAsmParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(NcAsmBase, base, cmd);
        using enum NcAsmInquiredType;
        switch (base.type)
        {
        case MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS:
        {
            if (self->mSupport.contains(
                MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
            {
                Deserialize(NcAsmParamModeNcDualModeSwitchAsmSeamless, res, cmd);
                self->mNcAsmEnabled.overwrite(res.base.ncAsmTotalEffect == NcAsmOnOffValue::ON);
                self->mNcAsmMode.overwrite(res.ncAsmMode);
                self->mNcAsmFocusOnVoice.overwrite(res.ambientSoundMode == AmbientSoundMode::VOICE);
                self->mNcAsmAmbientLevel.overwrite(res.ambientSoundLevelValue);
                return MDR_HEADPHONES_EVT_NCASM_PARAM;
            }
            break;
        }
        case MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA:
        {
            if (self->mSupport.contains(
                MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
            {
                Deserialize(NcAsmParamModeNcDualModeSwitchAsmSeamlessNa, res, cmd);
                self->mNcAsmEnabled.overwrite(res.base.ncAsmTotalEffect == NcAsmOnOffValue::ON);
                self->mNcAsmMode.overwrite(res.ncAsmMode);
                self->mNcAsmFocusOnVoice.overwrite(res.ambientSoundMode == AmbientSoundMode::VOICE);
                self->mNcAsmAmbientLevel.overwrite(res.ambientSoundLevelValue);
                self->mNcAsmAutoAsmEnabled.overwrite(res.noiseAdaptiveOnOffValue == NcAsmOnOffValue::ON);
                self->mNcAsmNoiseAdaptiveSensitivity.overwrite(res.noiseAdaptiveSensitivitySettings);
                return MDR_HEADPHONES_EVT_NCASM_PARAM;
            }
            break;
        }
        case ASM_SEAMLESS:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
            {
                Deserialize(NcAsmParamAsmSeamless, res, cmd);
                self->mNcAsmEnabled.overwrite(res.base.ncAsmTotalEffect == NcAsmOnOffValue::ON);
                self->mNcAsmFocusOnVoice.overwrite(res.ambientSoundMode == AmbientSoundMode::VOICE);
                self->mNcAsmAmbientLevel.overwrite(res.ambientSoundLevelValue);
                return MDR_HEADPHONES_EVT_NCASM_PARAM;
            }
            break;
        }
        case NC_AMB_TOGGLE:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_CONTROL_MODE_SELECT))
            {
                Deserialize(NcAsmParamNcAmbToggle, res, cmd);
                self->mNcAsmButtonFunction.overwrite(res.function);
                return MDR_HEADPHONES_EVT_NCASM_BUTTON_MODE;
            }
            break;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }
    int HandlePowerStatusT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(PowerBase, base, cmd);
        using enum PowerInquiredType;
        switch (base.type)
        {
        case BATTERY:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::BATTERY_LEVEL_INDICATOR))
            {
                Deserialize(PowerRetStatusBattery, res, cmd);
                self->mBatteryL = {
                    res.batteryStatus.batteryLevel,
                    0xFF,
                    res.batteryStatus.chargingStatus
                };
                return MDR_HEADPHONES_EVT_BATTERY;
            }
            break;
        }
        case LEFT_RIGHT_BATTERY:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::LEFT_RIGHT_BATTERY_LEVEL_INDICATOR))
            {
                Deserialize(PowerRetStatusLeftRightBattery, res, cmd);
                self->mBatteryL = {
                    res.batteryStatus.leftBatteryLevel,
                    0xFF,
                    res.batteryStatus.leftChargingStatus
                };
                self->mBatteryR = {
                    res.batteryStatus.rightBatteryLevel,
                    0xFF,
                    res.batteryStatus.rightChargingStatus
                };
                return MDR_HEADPHONES_EVT_BATTERY;
            }
            break;
        }
        case CRADLE_BATTERY:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_INDICATOR))
            {
                Deserialize(PowerRetStatusCradleBattery, res, cmd);
                self->mBatteryCase = {
                    res.batteryStatus.batteryLevel,
                    0xFF,
                    res.batteryStatus.chargingStatus
                };
                return MDR_HEADPHONES_EVT_BATTERY;
            }
            break;
        }
        case BATTERY_WITH_THRESHOLD:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::BATTERY_LEVEL_WITH_THRESHOLD))
            {
                Deserialize(PowerRetStatusBatteryThreshold, res, cmd);
                self->mBatteryL = {
                    res.batteryStatus.batteryStatus.batteryLevel,
                    res.batteryStatus.batteryThreshold,
                    res.batteryStatus.batteryStatus.chargingStatus
                };
                return MDR_HEADPHONES_EVT_BATTERY;
            }
            break;
        }
        case LR_BATTERY_WITH_THRESHOLD:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::LR_BATTERY_LEVEL_WITH_THRESHOLD))
            {
                Deserialize(PowerRetStatusLeftRightBatteryThreshold, res, cmd);
                self->mBatteryL = {
                    res.batteryStatus.leftBatteryLevel,
                    res.leftBatteryThreshold,
                    res.batteryStatus.leftChargingStatus
                };
                self->mBatteryR = {
                    res.batteryStatus.rightBatteryLevel,
                    res.rightBatteryThreshold,
                    res.batteryStatus.rightChargingStatus
                };
                return MDR_HEADPHONES_EVT_BATTERY;
            }
            break;
        }
        case CRADLE_BATTERY_WITH_THRESHOLD:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD))
            {
                Deserialize(PowerRetStatusCradleBatteryThreshold, res, cmd);
                self->mBatteryCase = {
                    res.batteryStatus.batteryStatus.batteryLevel,
                    res.batteryStatus.batteryThreshold,
                    res.batteryStatus.batteryStatus.chargingStatus
                };
                return MDR_HEADPHONES_EVT_BATTERY;
            }
            break;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }
    int HandlePlayParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(PlayParamBase, base, cmd);
        using enum PlayInquiredType;
        switch (base.type)
        {
        case PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT:
        {
            Deserialize(PlayParamPlaybackControllerName, res, cmd);
            self->mPlayTrackTitle = res.playbackNames.value[0].playbackName.value;
            self->mPlayTrackAlbum = res.playbackNames.value[1].playbackName.value;
            self->mPlayTrackArtist = res.playbackNames.value[2].playbackName.value;
            return MDR_HEADPHONES_EVT_PLAYBACK_METADATA;
        }
        case MUSIC_VOLUME:
        {
            Deserialize(PlayParamPlaybackControllerVolume, res, cmd);
            self->mPlayVolume.overwrite(res.volumeValue);
            return MDR_HEADPHONES_EVT_PLAYBACK_VOLUME;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }
    int HandlePowerParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(PowerBase, base, cmd);
        using enum PowerInquiredType;
        switch (base.type)
        {
        case AUTO_POWER_OFF:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF))
            {
                Deserialize(PowerParamAutoPowerOff, res, cmd);
                self->mPowerAutoOff.overwrite(res.currentPowerOffElements);
                return MDR_HEADPHONES_EVT_AUTO_POWER_OFF_PARAM;
            }
            break;
        }
        case AUTO_POWER_OFF_WEARING_DETECTION:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
            {
                Deserialize(PowerParamAutoPowerOffWithWearingDetection, res, cmd);
                self->mPowerAutoOffWearingDetection.overwrite(res.currentPowerOffElements);
                return MDR_HEADPHONES_EVT_AUTO_POWER_OFF_PARAM;
            }
            break;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandlePlaybackStatusT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(PlayBase, base, cmd);
        using enum PlayInquiredType;
        switch (base.type)
        {
        case PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT:
        {
            Deserialize(PlayStatusPlaybackController, res, cmd);
            self->mPlayPause = res.playbackStatus;
            return MDR_HEADPHONES_EVT_PLAYBACK_METADATA;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandleGsCapabilityT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(GsRetCapability, res, cmd);
        using enum GsInquiredType;
        switch (res.type)
        {
        case GENERAL_SETTING1:
        {
            self->mGsCapability1 = {res.settingType, res.settingInfo};
            return MDR_HEADPHONES_EVT_GENERAL_SETTING_1;
        }
        case GENERAL_SETTING2:
        {
            self->mGsCapability2 = {res.settingType, res.settingInfo};
            return MDR_HEADPHONES_EVT_GENERAL_SETTING_2;
        }
        case GENERAL_SETTING3:
        {
            self->mGsCapability3 = {res.settingType, res.settingInfo};
            return MDR_HEADPHONES_EVT_GENERAL_SETTING_3;
        }
        case GENERAL_SETTING4:
        {
            self->mGsCapability4 = {res.settingType, res.settingInfo};
            return MDR_HEADPHONES_EVT_GENERAL_SETTING_4;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandleGsParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(GsParamBase, base, cmd);
        using enum GsSettingType;
        using enum GsInquiredType;
        auto Write = [&](MDRProperty<bool>& dstBool) -> int
        {
            switch (base.settingType)
            {
            case BOOLEAN_TYPE:
            {
                Deserialize(GsParamBoolean, res, cmd);
                dstBool.overwrite(res.settingValue == GsSettingValue::ON);
                return MDR_HEADPHONES_EVT_OK;
            }
            default:
                break;
            }
            return MDR_HEADPHONES_EVT_UNHANDLED;
        };
        switch (base.type)
        {
        case GENERAL_SETTING1:
        {
            return Write(self->mGsParamBool1);
        }
        case GENERAL_SETTING2:
        {
            return Write(self->mGsParamBool2);
        }
        case GENERAL_SETTING3:
        {
            return Write(self->mGsParamBool3);
        }
        case GENERAL_SETTING4:
        {
            return Write(self->mGsParamBool4);
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandleAudioCapabilityT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(AudioBase, base, cmd);
        using enum AudioInquiredType;
        switch (base.type)
        {
        case UPSCALING:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::UPSCALING_AUTO_OFF))
            {
                Deserialize(AudioRetCapabilityUpscaling, res, cmd);
                self->mUpscalingType = res.upscalingType;
                return MDR_HEADPHONES_EVT_UPSCALING_MODE;
            }
            return MDR_HEADPHONES_EVT_UNHANDLED;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandleAudioStatusT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(AudioBase, base, cmd);
        using enum AudioInquiredType;
        switch (base.type)
        {
        case UPSCALING:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::UPSCALING_AUTO_OFF))
            {
                Deserialize(AudioStatusCommon, res, cmd);
                self->mUpscalingAvailable = res.status == MessageMdrV2EnableDisable::ENABLE;
                return MDR_HEADPHONES_EVT_UPSCALING_MODE;
            }
            return MDR_HEADPHONES_EVT_UNHANDLED;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandleAudioParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(AudioBase, base, cmd);
        using enum AudioInquiredType;
        switch (base.type)
        {
        case CONNECTION_MODE:
        {
            if (self->mSupport.contains(
                MessageMdrV2FunctionType_Table1::CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY))
            {
                Deserialize(AudioParamConnection, res, cmd);
                self->mAudioPriorityMode.overwrite(res.settingValue);
                return MDR_HEADPHONES_EVT_CONNECTION_MODE;
            }
            return MDR_HEADPHONES_EVT_UNHANDLED;
        }
        case UPSCALING:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::UPSCALING_AUTO_OFF))
            {
                Deserialize(AudioParamUpscaling, res, cmd);
                self->mUpscalingEnabled.overwrite(res.settingValue == UpscalingTypeAutoOff::AUTO);
                return MDR_HEADPHONES_EVT_UPSCALING_MODE;
            }
            return MDR_HEADPHONES_EVT_UNHANDLED;
        }
        case BGM_MODE:
        case BGM_MODE_AND_ERRORCODE:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::LISTENING_OPTION))
            {
                Deserialize(AudioParamBGMMode, res, cmd);
                self->mBGMModeEnabled.overwrite(res.onOffSettingValue == MessageMdrV2EnableDisable::ENABLE);
                self->mBGMModeRoomSize.overwrite(res.targetRoomSize);
                return MDR_HEADPHONES_EVT_OK;
            }
            return MDR_HEADPHONES_EVT_UNHANDLED;
        }
        case UPMIX_CINEMA:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::LISTENING_OPTION))
            {
                Deserialize(AudioParamUpmixCinema, res, cmd);
                self->mUpmixCinemaEnabled.overwrite(res.onOffSettingValue == MessageMdrV2EnableDisable::ENABLE);
                return MDR_HEADPHONES_EVT_OK;
            }
            return MDR_HEADPHONES_EVT_UNHANDLED;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandleSystemParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(SystemBase, base, cmd);
        using enum SystemInquiredType;
        switch (base.type)
        {
        case PLAYBACK_CONTROL_BY_WEARING:
        {
            if (self->mSupport.contains(
                MessageMdrV2FunctionType_Table1::PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF))
            {
                Deserialize(SystemParamCommon, res, cmd);
                self->mAutoPauseEnabled.overwrite(res.settingValue == MessageMdrV2EnableDisable::ENABLE);
                return MDR_HEADPHONES_EVT_PLAYBACK_PLAY_PAUSE;
            }
            return MDR_HEADPHONES_EVT_UNHANDLED;
        }
        case ASSIGNABLE_SETTINGS:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::ASSIGNABLE_SETTING))
            {
                Deserialize(SystemParamAssignableSettings, res, cmd);
                if (res.presets.size() == 2)
                {
                    self->mTouchFunctionLeft.overwrite(res.presets.value[0]);
                    self->mTouchFunctionRight.overwrite(res.presets.value[1]);
                }
                return MDR_HEADPHONES_EVT_NCASM_BUTTON_MODE;
            }
            return MDR_HEADPHONES_EVT_UNHANDLED;
        }
        case SMART_TALKING_MODE_TYPE2:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::SMART_TALKING_MODE_TYPE2))
            {
                Deserialize(SystemParamSmartTalking, res, cmd);
                self->mSpeakToChatEnabled.overwrite(res.onOffValue == MessageMdrV2EnableDisable::ENABLE);
                return MDR_HEADPHONES_EVT_SPEAK_TO_CHAT_ENABLED;
            }
            return MDR_HEADPHONES_EVT_UNHANDLED;
        }
        case HEAD_GESTURE_ON_OFF:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::HEAD_GESTURE_ON_OFF_TRAINING))
            {
                Deserialize(SystemParamCommon, res, cmd);
                self->mHeadGestureEnabled.overwrite(res.settingValue == MessageMdrV2EnableDisable::ENABLE);
                return MDR_HEADPHONES_EVT_HEAD_GESTURE;
            }
            return MDR_HEADPHONES_EVT_UNHANDLED;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandleSystemExtParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(SystemExtBase, base, cmd);
        using enum SystemInquiredType;
        switch (base.type)
        {
        case SMART_TALKING_MODE_TYPE2:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::SMART_TALKING_MODE_TYPE2))
            {
                Deserialize(SystemExtParamSmartTalkingMode2, res, cmd);
                self->mSpeakToChatDetectSensitivity.overwrite(res.detectSensitivity);
                self->mSpeakToModeOutTime.overwrite(res.modeOffTime);
                return MDR_HEADPHONES_EVT_SPEAK_TO_CHAT_PARAM;
            }
            return MDR_HEADPHONES_EVT_UNHANDLED;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandleEqEbbStatusT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(EqEbbBase, base, cmd);
        using enum EqEbbInquiredType;
        switch (base.type)
        {
        case PRESET_EQ:
        {
            Deserialize(EqEbbStatusOnOff, res, cmd);
            self->mEqAvailable.overwrite(res.status == MessageMdrV2OnOffSettingValue::ON);
            return MDR_HEADPHONES_EVT_EQUALIZER_AVAILABLE;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandleEqEbbParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(EqEbbBase, base, cmd);
        using enum EqEbbInquiredType;
        switch (base.type)
        {
        case PRESET_EQ:
        {
            Deserialize(EqEbbParamEq, res, cmd);
            self->mEqPresetId.overwrite(res.presetId);
            switch (res.bands.size())
            {
            case 0:
                return MDR_HEADPHONES_EVT_EQUALIZER_PARAM;
            case 6:
                self->mEqClearBass.overwrite(res.bands.value[0] - 10);
                self->mEqConfig.overwrite({
                    res.bands.value[1] - 10, // 400
                    res.bands.value[2] - 10, // 1k
                    res.bands.value[3] - 10, // 2.5k
                    res.bands.value[4] - 10, // 6.3k
                    res.bands.value[5] - 10, // 16k
                });
                return MDR_HEADPHONES_EVT_EQUALIZER_PARAM;
            case 10:
                self->mEqClearBass.overwrite(0); // Unavailable
                self->mEqConfig.overwrite({
                    res.bands.value[0] - 6, // 31
                    res.bands.value[1] - 6, // 63
                    res.bands.value[2] - 6, // 125
                    res.bands.value[3] - 6, // 250
                    res.bands.value[4] - 6, // 500
                    res.bands.value[5] - 6, // 1k
                    res.bands.value[6] - 6, // 2k
                    res.bands.value[7] - 6, // 4k
                    res.bands.value[8] - 6, // 8k
                    res.bands.value[9] - 6, // 16k
                });
                return MDR_HEADPHONES_EVT_EQUALIZER_PARAM;
            default:
                break;
            }
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandleAlertParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(AlertBase, base, cmd);
        using enum AlertInquiredType;
        switch (base.type)
        {
        case FIXED_MESSAGE:
        {
            if (self->mSupport.contains(MessageMdrV2FunctionType_Table1::FIXED_MESSAGE))
            {
                Deserialize(AlertNotifyParamFixedMessage, res, cmd);
                using enum AlertActionType;
                switch (res.actionType)
                {
                case POSITIVE_NEGATIVE:
                {
                    self->mLastAlertMessage = res.messageType;
                    return MDR_HEADPHONES_EVT_ALERT;
                }
                default:
                    break;
                }
            }
            return MDR_HEADPHONES_EVT_UNHANDLED;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandleLogParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        // XXX: Don't have the corresponding struct in the official app yet, and
        //      these don't get serialized in a way that's consistent with the rest.
        //      So excuse the rawdogged parsing - FIXME.
        if (cmd.size() < 2)
            return self->SetLastError(MDR_RESULT_ERROR_MALFORMED_PAYLOAD, "Malformed log parameter");

        const UInt8* begin = nullptr;
        size_t remaining = 0;
        switch (cmd[1])
        {
        case 0x00:
        {
            MDRPrefixedString res;
            if (cmd.size() > 2 && cmd[2])
            {
                begin = &cmd[2]; // key...
                remaining = cmd.size() - 2;
            }
            else if (cmd.size() > 3)
            {
                begin = &cmd[3]; // op...
                remaining = cmd.size() - 3;
            }
            else
            {
                return self->SetLastError(MDR_RESULT_ERROR_MALFORMED_PAYLOAD, "Malformed log parameter");
            }
            const auto readResult = MDRPrefixedString::Read(&begin, res, remaining);
            if (!readResult)
                return self->SetLastError(readResult.error, "Unable to deserialize log parameter");
            self->mLastDeviceJSONMessage = res.value;
            return MDR_HEADPHONES_EVT_OK;
        }
        case 0x01:
        {
            if (cmd.size() < 4)
                return self->SetLastError(MDR_RESULT_ERROR_MALFORMED_PAYLOAD, "Malformed interaction parameter");
            self->mLastInteractionMessage = mdr::String(cmd.begin() + 4, cmd.end());
            return MDR_HEADPHONES_EVT_OK;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int MDRHeadphones::HandleCommandV2T1(Span<const UInt8> cmd, MDRCommandSeqNumber seq)
    {
        auto* self = this;
        Deserialize(CommandBase, base, cmd);
        using enum Command;
        MDR_LOG_DEBUG("<< {}", base.command);
        switch (base.command)
        {
        case CONNECT_RET_PROTOCOL_INFO:
            return HandleProtocolInfoT1(this, cmd);
        case CONNECT_RET_SUPPORT_FUNCTION:
            return HandleSupportFunctionT1(this, cmd);
        case CONNECT_RET_CAPABILITY_INFO:
            return HandleCapabilityInfoT1(this, cmd);
        case CONNECT_RET_DEVICE_INFO:
            return HandleDeviceInfoT1(this, cmd);
        case COMMON_RET_STATUS:
        case COMMON_NTFY_STATUS:
            return HandleCommonStatusT1(this, cmd);
        case NCASM_RET_PARAM:
        case NCASM_NTFY_PARAM:
            return HandleNcAsmParamT1(this, cmd);
        case POWER_RET_STATUS:
        case POWER_NTFY_STATUS:
            return HandlePowerStatusT1(this, cmd);
        case PLAY_RET_PARAM:
        case PLAY_NTFY_PARAM:
            return HandlePlayParamT1(this, cmd);
        case POWER_RET_PARAM:
        case POWER_NTFY_PARAM:
            return HandlePowerParamT1(this, cmd);
        case PLAY_RET_STATUS:
        case PLAY_NTFY_STATUS:
            return HandlePlaybackStatusT1(this, cmd);
        case GENERAL_SETTING_RET_CAPABILITY:
            return HandleGsCapabilityT1(this, cmd);
        case GENERAL_SETTING_RET_PARAM:
        case GENERAL_SETTING_NTFY_PARAM:
            return HandleGsParamT1(this, cmd);
        case AUDIO_RET_CAPABILITY:
            return HandleAudioCapabilityT1(this, cmd);
        case AUDIO_RET_STATUS:
        case AUDIO_NTFY_STATUS:
            return HandleAudioStatusT1(this, cmd);
        case AUDIO_RET_PARAM:
        case AUDIO_NTFY_PARAM:
            return HandleAudioParamT1(this, cmd);
        case SYSTEM_RET_PARAM:
        case SYSTEM_NTFY_PARAM:
            return HandleSystemParamT1(this, cmd);
        case SYSTEM_RET_EXT_PARAM:
        case SYSTEM_NTFY_EXT_PARAM:
            return HandleSystemExtParamT1(this, cmd);
        case EQEBB_RET_STATUS:
        case EQEBB_NTFY_STATUS:
            return HandleEqEbbStatusT1(this, cmd);
        case EQEBB_RET_PARAM:
        case EQEBB_NTFY_PARAM:
            return HandleEqEbbParamT1(this, cmd);
        case ALERT_NTFY_PARAM:
            return HandleAlertParamT1(this, cmd);
        case LOG_NTFY_PARAM:
            return HandleLogParamT1(this, cmd);
        default:
            MDR_LOG_DEBUG("^^ Unhandled {}", base.command);
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }
}
