#include <algorithm>
#include "Details.hpp"
namespace mdr
{
    using namespace v2;
    using namespace t1;

    int HandleSupportFunctionT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(ConnectRetSupportFunction, res, cmd);
        std::ranges::fill(self->mDetailsV2.mSupport.table1Functions, false);
        for (auto fun : res.supportFunctions)
            self->mDetailsV2.mSupport.table1Functions[static_cast<UInt8>(fun.functionType)] = true;
        self->mDetailsV2.mSupport.provenance = DetailsV2::SupportStates::Provenance::ADVERTISED;
        self->Awake(MDRHeadphones::AWAIT_SUPPORT_FUNCTION);
        return MDR_EVENT_IDENTITY_CHANGED;
    }

    int HandleCapabilityInfoT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(ConnectRetCapabilityInfo, res, cmd);
        self->mDetailsV2.mUniqueId = res.uniqueId.value;
        return MDR_EVENT_IDENTITY_CHANGED;
    }

    int HandleDeviceInfoT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        DeviceInfoType type{};
        if (!detail::ReadEnumTag(cmd, type))
            return MDR_EVENT_UNHANDLED;
        using enum DeviceInfoType;
        switch (type)
        {
        case MODEL_NAME:
        {
            Deserialize(ConnectRetDeviceInfoModelName, res, cmd);
            self->mDetailsV2.mModelName = res.modelName.value;
            return MDR_EVENT_IDENTITY_CHANGED;
        }
        case FW_VERSION:
        {
            Deserialize(ConnectRetDeviceInfoFwVersion, res, cmd);
            self->mDetailsV2.mFWVersion = res.fwVersion.value;
            return MDR_EVENT_IDENTITY_CHANGED;
        }
        case SERIES_AND_COLOR_INFO:
        {
            Deserialize(ConnectRetDeviceInfoSeriesAndColor, res, cmd);
            self->mDetailsV2.mModelSeries = res.modelSeries;
            self->mDetailsV2.mModelColor = res.modelColor;
            return MDR_EVENT_IDENTITY_CHANGED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandleCommonStatusT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        CommonInquiredType type{};
        if (!detail::ReadEnumTag(cmd, type))
            return MDR_EVENT_UNHANDLED;
        using enum CommonInquiredType;
        switch (type)
        {
        case AUDIO_CODEC:
        {
            const auto command = static_cast<Command>(cmd[0]);
            if (command == Command::COMMON_NTFY_STATUS)
            {
                Deserialize(CommonNotifyStatusAudioCodec, res, cmd);
                self->mDetailsV2.mAudioCodec = res.audioCodec;
            }
            else
            {
                Deserialize(CommonRetStatusAudioCodec, res, cmd);
                self->mDetailsV2.mAudioCodec = res.audioCodec;
            }
            return MDR_EVENT_IDENTITY_CHANGED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandleNcAsmParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        NcAsmInquiredType type{};
        if (!detail::ReadEnumTag(cmd, type))
            return MDR_EVENT_UNHANDLED;
        const auto command = static_cast<Command>(cmd[0]);
        using enum NcAsmInquiredType;
        switch (type)
        {
        case MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS:
        {
            if (self->mDetailsV2.mSupport.contains(
                t1::FunctionType::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
            {
                if (command == Command::NCASM_NTFY_PARAM)
                {
                    Deserialize(NcAsmNtfyParamModeNcDualModeSwitchAsmSeamless, res, cmd);
                    self->mDetailsV2.mNcAsmChangingAsmLevel.overwrite(
                        res.valueChangeStatus == ValueChangeStatus::UNDER_CHANGING);
                    self->mDetailsV2.mNcAsmEnabled.overwrite(res.ncAsmTotalEffect == NcAsmOnOffValue::ON);
                    self->mDetailsV2.mNcAsmMode.overwrite(res.ncAsmMode);
                    self->mDetailsV2.mNcAsmFocusOnVoice.overwrite(res.ambientSoundMode == AmbientSoundMode::VOICE);
                    self->mDetailsV2.mNcAsmAmbientLevel.overwrite(res.ambientSoundLevelValue);
                }
                else
                {
                    Deserialize(NcAsmRetParamModeNcDualModeSwitchAsmSeamless, res, cmd);
                    self->mDetailsV2.mNcAsmChangingAsmLevel.overwrite(
                        res.valueChangeStatus == ValueChangeStatus::UNDER_CHANGING);
                    self->mDetailsV2.mNcAsmEnabled.overwrite(res.ncAsmTotalEffect == NcAsmOnOffValue::ON);
                    self->mDetailsV2.mNcAsmMode.overwrite(res.ncAsmMode);
                    self->mDetailsV2.mNcAsmFocusOnVoice.overwrite(res.ambientSoundMode == AmbientSoundMode::VOICE);
                    self->mDetailsV2.mNcAsmAmbientLevel.overwrite(res.ambientSoundLevelValue);
                }
                return MDR_EVENT_NOISE_CONTROL_CHANGED;
            }
            break;
        }
        case MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA:
        {
            if (self->mDetailsV2.mSupport.contains(
                t1::FunctionType::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
            {
                if (command == Command::NCASM_NTFY_PARAM)
                {
                    Deserialize(NcAsmNtfyParamModeNcDualModeSwitchAsmSeamlessNa, res, cmd);
                    self->mDetailsV2.mNcAsmChangingAsmLevel.overwrite(
                        res.valueChangeStatus == ValueChangeStatus::UNDER_CHANGING);
                    self->mDetailsV2.mNcAsmEnabled.overwrite(res.ncAsmTotalEffect == NcAsmOnOffValue::ON);
                    self->mDetailsV2.mNcAsmMode.overwrite(res.ncAsmMode);
                    self->mDetailsV2.mNcAsmFocusOnVoice.overwrite(res.ambientSoundMode == AmbientSoundMode::VOICE);
                    self->mDetailsV2.mNcAsmAmbientLevel.overwrite(res.ambientSoundLevelValue);
                    self->mDetailsV2.mNcAsmAutoAsmEnabled.overwrite(res.ncAsmOnOffValue == NcAsmOnOffValue::ON);
                    self->mDetailsV2.mNcAsmNoiseAdaptiveSensitivity.overwrite(res.noiseAdaptiveSensitivitySettings);
                }
                else
                {
                    Deserialize(NcAsmRetParamModeNcDualModeSwitchAsmSeamlessNa, res, cmd);
                    self->mDetailsV2.mNcAsmChangingAsmLevel.overwrite(
                        res.valueChangeStatus == ValueChangeStatus::UNDER_CHANGING);
                    self->mDetailsV2.mNcAsmEnabled.overwrite(res.ncAsmTotalEffect == NcAsmOnOffValue::ON);
                    self->mDetailsV2.mNcAsmMode.overwrite(res.ncAsmMode);
                    self->mDetailsV2.mNcAsmFocusOnVoice.overwrite(res.ambientSoundMode == AmbientSoundMode::VOICE);
                    self->mDetailsV2.mNcAsmAmbientLevel.overwrite(res.ambientSoundLevelValue);
                    self->mDetailsV2.mNcAsmAutoAsmEnabled.overwrite(res.ncAsmOnOffValue == NcAsmOnOffValue::ON);
                    self->mDetailsV2.mNcAsmNoiseAdaptiveSensitivity.overwrite(res.noiseAdaptiveSensitivitySettings);
                }
                return MDR_EVENT_NOISE_CONTROL_CHANGED;
            }
            break;
        }
        case ASM_SEAMLESS:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
            {
                if (command == Command::NCASM_NTFY_PARAM)
                {
                    Deserialize(NcAsmNtfyParamAsmSeamless, res, cmd);
                    self->mDetailsV2.mNcAsmChangingAsmLevel.overwrite(
                        res.valueChangeStatus == ValueChangeStatus::UNDER_CHANGING);
                    self->mDetailsV2.mNcAsmEnabled.overwrite(res.ncAsmTotalEffect == NcAsmOnOffValue::ON);
                    self->mDetailsV2.mNcAsmFocusOnVoice.overwrite(res.ambientSoundMode == AmbientSoundMode::VOICE);
                    self->mDetailsV2.mNcAsmAmbientLevel.overwrite(res.ambientSoundLevelValue);
                }
                else
                {
                    Deserialize(NcAsmRetParamAsmSeamless, res, cmd);
                    self->mDetailsV2.mNcAsmChangingAsmLevel.overwrite(
                        res.valueChangeStatus == ValueChangeStatus::UNDER_CHANGING);
                    self->mDetailsV2.mNcAsmEnabled.overwrite(res.ncAsmTotalEffect == NcAsmOnOffValue::ON);
                    self->mDetailsV2.mNcAsmFocusOnVoice.overwrite(res.ambientSoundMode == AmbientSoundMode::VOICE);
                    self->mDetailsV2.mNcAsmAmbientLevel.overwrite(res.ambientSoundLevelValue);
                }
                return MDR_EVENT_NOISE_CONTROL_CHANGED;
            }
            break;
        }
        case NC_AMB_TOGGLE:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::AMBIENT_SOUND_CONTROL_MODE_SELECT))
            {
                if (command == Command::NCASM_NTFY_PARAM)
                {
                    Deserialize(NcAsmNtfyParamNcAmbToggle, res, cmd);
                    self->mDetailsV2.mNcAsmButtonFunction.overwrite(res.function);
                }
                else
                {
                    Deserialize(NcAsmRetParamNcAmbToggle, res, cmd);
                    self->mDetailsV2.mNcAsmButtonFunction.overwrite(res.function);
                }
                return MDR_EVENT_NOISE_CONTROL_CHANGED;
            }
            break;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }
    int HandlePowerStatusT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        PowerInquiredType type{};
        if (!detail::ReadEnumTag(cmd, type))
            return MDR_EVENT_UNHANDLED;
        const auto command = static_cast<Command>(cmd[0]);
        using enum PowerInquiredType;
        switch (type)
        {
        case BATTERY:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::BATTERY_LEVEL_INDICATOR))
            {
                if (command == Command::POWER_NTFY_STATUS)
                {
                    Deserialize(PowerNotifyStatusBattery, res, cmd);
                    self->mDetailsV2.mBatteryL = {res.batteryLevel, 0xFF, res.chargingStatus};
                }
                else
                {
                    Deserialize(PowerRetStatusBattery, res, cmd);
                    self->mDetailsV2.mBatteryL = {res.batteryLevel, 0xFF, res.chargingStatus};
                }
                return MDR_EVENT_BATTERY_CHANGED;
            }
            break;
        }
        case LEFT_RIGHT_BATTERY:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::LEFT_RIGHT_BATTERY_LEVEL_INDICATOR))
            {
                if (command == Command::POWER_NTFY_STATUS)
                {
                    Deserialize(PowerNotifyStatusLeftRightBattery, res, cmd);
                    self->mDetailsV2.mBatteryL = {res.leftBatteryLevel, 0xFF, res.leftChargingStatus};
                    self->mDetailsV2.mBatteryR = {res.rightBatteryLevel, 0xFF, res.rightChargingStatus};
                }
                else
                {
                    Deserialize(PowerRetStatusLeftRightBattery, res, cmd);
                    self->mDetailsV2.mBatteryL = {res.leftBatteryLevel, 0xFF, res.leftChargingStatus};
                    self->mDetailsV2.mBatteryR = {res.rightBatteryLevel, 0xFF, res.rightChargingStatus};
                }
                return MDR_EVENT_BATTERY_CHANGED;
            }
            break;
        }
        case CRADLE_BATTERY:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::CRADLE_BATTERY_LEVEL_INDICATOR))
            {
                if (command == Command::POWER_NTFY_STATUS)
                {
                    Deserialize(PowerNotifyStatusCradleBattery, res, cmd);
                    self->mDetailsV2.mBatteryCase = {res.batteryLevel, 0xFF, res.chargingStatus};
                }
                else
                {
                    Deserialize(PowerRetStatusCradleBattery, res, cmd);
                    self->mDetailsV2.mBatteryCase = {res.batteryLevel, 0xFF, res.chargingStatus};
                }
                return MDR_EVENT_BATTERY_CHANGED;
            }
            break;
        }
        case BATTERY_WITH_THRESHOLD:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::BATTERY_LEVEL_WITH_THRESHOLD))
            {
                if (command == Command::POWER_NTFY_STATUS)
                {
                    Deserialize(PowerNotifyStatusBatteryThreshold, res, cmd);
                    self->mDetailsV2.mBatteryL = {res.value1, res.batteryThreshold, res.batteryChargingStatus};
                }
                else
                {
                    Deserialize(PowerRetStatusBatteryThreshold, res, cmd);
                    self->mDetailsV2.mBatteryL = {res.value1, res.batteryThreshold, res.batteryChargingStatus};
                }
                return MDR_EVENT_BATTERY_CHANGED;
            }
            break;
        }
        case LR_BATTERY_WITH_THRESHOLD:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::LR_BATTERY_LEVEL_WITH_THRESHOLD))
            {
                if (command == Command::POWER_NTFY_STATUS)
                {
                    Deserialize(PowerNotifyStatusLeftRightBatteryThreshold, res, cmd);
                    self->mDetailsV2.mBatteryL = {res.leftBatteryLevel, res.leftBatteryThreshold, res.leftChargingStatus};
                    self->mDetailsV2.mBatteryR = {res.rightBatteryLevel, res.rightBatteryThreshold, res.rightChargingStatus};
                }
                else
                {
                    Deserialize(PowerRetStatusLeftRightBatteryThreshold, res, cmd);
                    self->mDetailsV2.mBatteryL = {res.leftBatteryLevel, res.leftBatteryThreshold, res.leftChargingStatus};
                    self->mDetailsV2.mBatteryR = {res.rightBatteryLevel, res.rightBatteryThreshold, res.rightChargingStatus};
                }
                return MDR_EVENT_BATTERY_CHANGED;
            }
            break;
        }
        case CRADLE_BATTERY_WITH_THRESHOLD:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD))
            {
                if (command == Command::POWER_NTFY_STATUS)
                {
                    Deserialize(PowerNotifyStatusCradleBatteryThreshold, res, cmd);
                    self->mDetailsV2.mBatteryCase = {res.value1, res.batteryThreshold, res.batteryChargingStatus};
                }
                else
                {
                    Deserialize(PowerRetStatusCradleBatteryThreshold, res, cmd);
                    self->mDetailsV2.mBatteryCase = {res.value1, res.batteryThreshold, res.batteryChargingStatus};
                }
                return MDR_EVENT_BATTERY_CHANGED;
            }
            break;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }
    int HandlePlayParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        PlayInquiredType type{};
        if (!detail::ReadEnumTag(cmd, type))
            return MDR_EVENT_UNHANDLED;
        const auto command = static_cast<Command>(cmd[0]);
        using enum PlayInquiredType;
        switch (type)
        {
        case PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT:
        {
            if (command == Command::PLAY_NTFY_PARAM)
            {
                Deserialize(NotifyPlayParamPlaybackControllerName, res, cmd);
                self->mDetailsV2.mPlayTrackTitle = res.playbackNames.value[0].name.value;
                self->mDetailsV2.mPlayTrackAlbum = res.playbackNames.value[1].name.value;
                self->mDetailsV2.mPlayTrackArtist = res.playbackNames.value[2].name.value;
            }
            else
            {
                Deserialize(RetPlayParamPlaybackControllerName, res, cmd);
                self->mDetailsV2.mPlayTrackTitle = res.playbackNames.value[0].name.value;
                self->mDetailsV2.mPlayTrackAlbum = res.playbackNames.value[1].name.value;
                self->mDetailsV2.mPlayTrackArtist = res.playbackNames.value[2].name.value;
            }
            return MDR_EVENT_PLAYBACK_CHANGED;
        }
        case MUSIC_VOLUME:
        {
            if (command == Command::PLAY_NTFY_PARAM)
            {
                Deserialize(NotifyPlayParamPlaybackControllerVolume, res, cmd);
                self->mDetailsV2.mPlayVolume.overwrite(res.volumeValue);
            }
            else
            {
                Deserialize(RetPlayParamPlaybackControllerVolume, res, cmd);
                self->mDetailsV2.mPlayVolume.overwrite(res.volumeValue);
            }
            return MDR_EVENT_PLAYBACK_CHANGED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }
    int HandlePowerParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        PowerInquiredType type{};
        if (!detail::ReadEnumTag(cmd, type))
            return MDR_EVENT_UNHANDLED;
        const auto command = static_cast<Command>(cmd[0]);
        using enum PowerInquiredType;
        switch (type)
        {
        case AUTO_POWER_OFF:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::AUTO_POWER_OFF))
            {
                if (command == Command::POWER_NTFY_PARAM)
                {
                    Deserialize(PowerNotifyParamAutoPowerOff, res, cmd);
                    self->mDetailsV2.mPowerAutoOff.overwrite(res.currentPowerOffElements);
                }
                else
                {
                    Deserialize(PowerRetParamAutoPowerOff, res, cmd);
                    self->mDetailsV2.mPowerAutoOff.overwrite(res.currentPowerOffElements);
                }
                return MDR_EVENT_POWER_CHANGED;
            }
            break;
        }
        case AUTO_POWER_OFF_WEARING_DETECTION:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
            {
                if (command == Command::POWER_NTFY_PARAM)
                {
                    Deserialize(PowerNotifyParamAutoPowerOffWithWearingDetection, res, cmd);
                    self->mDetailsV2.mPowerAutoOffWearingDetection.overwrite(res.currentPowerOffElements);
                }
                else
                {
                    Deserialize(PowerRetParamAutoPowerOffWithWearingDetection, res, cmd);
                    self->mDetailsV2.mPowerAutoOffWearingDetection.overwrite(res.currentPowerOffElements);
                }
                return MDR_EVENT_POWER_CHANGED;
            }
            break;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandlePlaybackStatusT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        PlayInquiredType type{};
        if (!detail::ReadEnumTag(cmd, type))
            return MDR_EVENT_UNHANDLED;
        const auto command = static_cast<Command>(cmd[0]);
        using enum PlayInquiredType;
        switch (type)
        {
        case PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT:
        {
            if (command == Command::PLAY_NTFY_STATUS)
            {
                Deserialize(NotifyPlayStatusPlaybackController, res, cmd);
                self->mDetailsV2.mPlayPause = res.playbackStatus;
            }
            else
            {
                Deserialize(RetPlayStatusPlaybackController, res, cmd);
                self->mDetailsV2.mPlayPause = res.playbackStatus;
            }
            return MDR_EVENT_PLAYBACK_CHANGED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandleGsCapabilityT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(GsRetCapability, res, cmd);
        using enum GsInquiredType;
        switch (res.type)
        {
        case GENERAL_SETTING1:
        {
            self->mDetailsV2.mGsCapability1 = {res.settingType, {res.gsStringFormat, res.value3, res.value4}};
            return MDR_EVENT_GENERAL_SETTINGS_CHANGED;
        }
        case GENERAL_SETTING2:
        {
            self->mDetailsV2.mGsCapability2 = {res.settingType, {res.gsStringFormat, res.value3, res.value4}};
            return MDR_EVENT_GENERAL_SETTINGS_CHANGED;
        }
        case GENERAL_SETTING3:
        {
            self->mDetailsV2.mGsCapability3 = {res.settingType, {res.gsStringFormat, res.value3, res.value4}};
            return MDR_EVENT_GENERAL_SETTINGS_CHANGED;
        }
        case GENERAL_SETTING4:
        {
            self->mDetailsV2.mGsCapability4 = {res.settingType, {res.gsStringFormat, res.value3, res.value4}};
            return MDR_EVENT_GENERAL_SETTINGS_CHANGED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandleGsParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        GsInquiredType type{};
        if (!detail::ReadEnumTag(cmd, type))
            return MDR_EVENT_UNHANDLED;
        const auto command = static_cast<Command>(cmd[0]);
        using enum GsInquiredType;
        auto Write = [&](MDRProperty<bool>& dstBool) -> int
        {
            if (command == Command::GENERAL_SETTING_NTNY_PARAM)
            {
                Deserialize(GsNotifyParamBoolean, res, cmd);
                dstBool.overwrite(res.value == GsSettingValue::ON);
                return MDR_EVENT_GENERAL_SETTINGS_CHANGED;
            }
            Deserialize(GsRetParamBoolean, res, cmd);
            dstBool.overwrite(res.value == GsSettingValue::ON);
            return MDR_EVENT_GENERAL_SETTINGS_CHANGED;
        };
        switch (type)
        {
        case GENERAL_SETTING1:
        {
            return Write(self->mDetailsV2.mGsParamBool1);
        }
        case GENERAL_SETTING2:
        {
            return Write(self->mDetailsV2.mGsParamBool2);
        }
        case GENERAL_SETTING3:
        {
            return Write(self->mDetailsV2.mGsParamBool3);
        }
        case GENERAL_SETTING4:
        {
            return Write(self->mDetailsV2.mGsParamBool4);
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandleAudioCapabilityT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        AudioInquiredType type{};
        if (!detail::ReadEnumTag(cmd, type))
            return MDR_EVENT_UNHANDLED;
        using enum AudioInquiredType;
        switch (type)
        {
        case UPSCALING:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::UPSCALING_AUTO_OFF))
            {
                Deserialize(AudioRetCapabilityUpscaling, res, cmd);
                self->mDetailsV2.mUpscalingType = res.upscalingType;
                return MDR_EVENT_EQUALIZER_CHANGED;
            }
            return MDR_EVENT_UNHANDLED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandleAudioStatusT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        AudioInquiredType type{};
        if (!detail::ReadEnumTag(cmd, type))
            return MDR_EVENT_UNHANDLED;
        using enum AudioInquiredType;
        switch (type)
        {
        case UPSCALING:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::UPSCALING_AUTO_OFF))
            {
                const auto command = static_cast<Command>(cmd[0]);
                if (command == Command::AUDIO_NTFY_STATUS)
                {
                    Deserialize(AudioNotifyStatusCommon, res, cmd);
                    self->mDetailsV2.mUpscalingAvailable = res.status == EnableDisable::ENABLE;
                }
                else
                {
                    Deserialize(AudioRetStatusCommon, res, cmd);
                    self->mDetailsV2.mUpscalingAvailable = res.status == EnableDisable::ENABLE;
                }
                return MDR_EVENT_EQUALIZER_CHANGED;
            }
            return MDR_EVENT_UNHANDLED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandleAudioParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        AudioInquiredType type{};
        if (!detail::ReadEnumTag(cmd, type))
            return MDR_EVENT_UNHANDLED;
        const auto command = static_cast<Command>(cmd[0]);
        using enum AudioInquiredType;
        switch (type)
        {
        case CONNECTION_MODE:
        {
            if (self->mDetailsV2.mSupport.contains(
                t1::FunctionType::CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY))
            {
                if (command == Command::AUDIO_NTFY_PARAM)
                {
                    Deserialize(AudioNtfyParamConnection, res, cmd);
                    self->mDetailsV2.mAudioPriorityMode.overwrite(res.settingValue);
                }
                else
                {
                    Deserialize(AudioRetParamConnection, res, cmd);
                    self->mDetailsV2.mAudioPriorityMode.overwrite(res.settingValue);
                }
                return MDR_EVENT_CONNECTION_MODE_CHANGED;
            }
            return MDR_EVENT_UNHANDLED;
        }
        case UPSCALING:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::UPSCALING_AUTO_OFF))
            {
                if (command == Command::AUDIO_NTFY_PARAM)
                {
                    Deserialize(AudioNtfyParamUpscaling, res, cmd);
                    self->mDetailsV2.mUpscalingEnabled.overwrite(res.settingValue == UpscalingTypeAutoOff::AUTO);
                }
                else
                {
                    Deserialize(AudioRetParamUpscaling, res, cmd);
                    self->mDetailsV2.mUpscalingEnabled.overwrite(res.settingValue == UpscalingTypeAutoOff::AUTO);
                }
                return MDR_EVENT_EQUALIZER_CHANGED;
            }
            return MDR_EVENT_UNHANDLED;
        }
        case BGM_MODE:
        case BGM_MODE_AND_ERRORCODE:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::LISTENING_OPTION))
            {
                if (command == Command::AUDIO_NTFY_PARAM)
                {
                    Deserialize(AudioNotifyParamBGMMode, res, cmd);
                    self->mDetailsV2.mBGMModeEnabled.overwrite(res.onOffSettingValue == OnOffSettingValue::ON);
                    self->mDetailsV2.mBGMModeRoomSize.overwrite(res.targetRoomSize);
                }
                else
                {
                    Deserialize(AudioRetParamBGMMode, res, cmd);
                    self->mDetailsV2.mBGMModeEnabled.overwrite(res.onOffSettingValue == OnOffSettingValue::ON);
                    self->mDetailsV2.mBGMModeRoomSize.overwrite(res.targetRoomSize);
                }
                return MDR_EVENT_LISTENING_MODE_CHANGED;
            }
            return MDR_EVENT_UNHANDLED;
        }
        case UPMIX_CINEMA:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::LISTENING_OPTION))
            {
                if (command == Command::AUDIO_NTFY_PARAM)
                {
                    Deserialize(AudioNotifyParamUpmixCinema, res, cmd);
                    self->mDetailsV2.mUpmixCinemaEnabled.overwrite(res.onOffSettingValue == OnOffSettingValue::ON);
                }
                else
                {
                    Deserialize(AudioRetParamUpmixCinema, res, cmd);
                    self->mDetailsV2.mUpmixCinemaEnabled.overwrite(res.onOffSettingValue == OnOffSettingValue::ON);
                }
                return MDR_EVENT_LISTENING_MODE_CHANGED;
            }
            return MDR_EVENT_UNHANDLED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandleSystemParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        SystemInquiredType type{};
        if (!detail::ReadEnumTag(cmd, type))
            return MDR_EVENT_UNHANDLED;
        const auto command = static_cast<Command>(cmd[0]);
        using enum SystemInquiredType;
        switch (type)
        {
        case PLAYBACK_CONTROL_BY_WEARING:
        {
            if (self->mDetailsV2.mSupport.contains(
                t1::FunctionType::PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF))
            {
                if (command == Command::SYSTEM_NTFY_PARAM)
                {
                    Deserialize(SystemNotifyParamCommon, res, cmd);
                    self->mDetailsV2.mAutoPauseEnabled.overwrite(res.settingValue == OnOffSettingValue::ON);
                }
                else
                {
                    Deserialize(SystemRetParamCommon, res, cmd);
                    self->mDetailsV2.mAutoPauseEnabled.overwrite(res.settingType == OnOffSettingValue::ON);
                }
                return MDR_EVENT_PLAYBACK_CHANGED;
            }
            return MDR_EVENT_UNHANDLED;
        }
        case ASSIGNABLE_SETTINGS:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::ASSIGNABLE_SETTING))
            {
                if (command == Command::SYSTEM_NTFY_PARAM)
                {
                    Deserialize(SystemNotifyParamAssignableSettings, res, cmd);
                    if (res.presetList.size() == 2)
                    {
                        self->mDetailsV2.mTouchFunctionLeft.overwrite(res.presetList.value[0]);
                        self->mDetailsV2.mTouchFunctionRight.overwrite(res.presetList.value[1]);
                    }
                }
                else
                {
                    Deserialize(SystemRetParamAssignableSettings, res, cmd);
                    if (res.presetList.size() == 2)
                    {
                        self->mDetailsV2.mTouchFunctionLeft.overwrite(res.presetList.value[0]);
                        self->mDetailsV2.mTouchFunctionRight.overwrite(res.presetList.value[1]);
                    }
                }
                return MDR_EVENT_NOISE_CONTROL_CHANGED;
            }
            return MDR_EVENT_UNHANDLED;
        }
        case SMART_TALKING_MODE_TYPE2:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::SMART_TALKING_MODE_TYPE2))
            {
                if (command == Command::SYSTEM_NTFY_PARAM)
                {
                    Deserialize(SystemNotifyParamSmartTalking, res, cmd);
                    self->mDetailsV2.mSpeakToChatEnabled.overwrite(res.onOffValue == OnOffSettingValue::ON);
                }
                else
                {
                    Deserialize(SystemRetParamSmartTalking, res, cmd);
                    self->mDetailsV2.mSpeakToChatEnabled.overwrite(res.onOffValue == OnOffSettingValue::ON);
                }
                return MDR_EVENT_SPEAK_TO_CHAT_CHANGED;
            }
            return MDR_EVENT_UNHANDLED;
        }
        case HEAD_GESTURE_ON_OFF:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::HEAD_GESTURE_ON_OFF_TRAINING))
            {
                if (command == Command::SYSTEM_NTFY_PARAM)
                {
                    Deserialize(SystemNotifyParamCommon, res, cmd);
                    self->mDetailsV2.mHeadGestureEnabled.overwrite(res.settingValue == OnOffSettingValue::ON);
                }
                else
                {
                    Deserialize(SystemRetParamCommon, res, cmd);
                    self->mDetailsV2.mHeadGestureEnabled.overwrite(res.settingType == OnOffSettingValue::ON);
                }
                return MDR_EVENT_POWER_CHANGED;
            }
            return MDR_EVENT_UNHANDLED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandleSystemExtParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        if (!self->mDetailsV2.mSupport.contains(t1::FunctionType::SMART_TALKING_MODE_TYPE2))
            return MDR_EVENT_UNHANDLED;

        const auto command = static_cast<Command>(cmd[0]);
        if (command == Command::SYSTEM_NTFY_EXT_PARAM)
        {
            Deserialize(SystemNotifyExtParamSmartTalkingModeType2, res, cmd);
            self->mDetailsV2.mSpeakToChatDetectSensitivity.overwrite(res.detectSensitivity);
            self->mDetailsV2.mSpeakToModeOutTime.overwrite(res.modeOffTime);
        }
        else
        {
            Deserialize(SystemRetExtParamSmartTalkingModeType2, res, cmd);
            self->mDetailsV2.mSpeakToChatDetectSensitivity.overwrite(res.detectSensitivity);
            self->mDetailsV2.mSpeakToModeOutTime.overwrite(res.modeOffTime);
        }
        return MDR_EVENT_SPEAK_TO_CHAT_CHANGED;
    }

    int HandleEqEbbStatusT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        EqEbbInquiredType type{};
        if (!detail::ReadEnumTag(cmd, type))
            return MDR_EVENT_UNHANDLED;
        using enum EqEbbInquiredType;
        switch (type)
        {
        case PRESET_EQ:
        {
            const auto command = static_cast<Command>(cmd[0]);
            if (command == Command::EQEBB_NTFY_STATUS)
            {
                Deserialize(EqEbbNtfyStatus, res, cmd);
                self->mDetailsV2.mEqAvailable.overwrite(res.enableDisable == EnableDisable::ENABLE);
            }
            else
            {
                Deserialize(EqEbbRetStatus, res, cmd);
                self->mDetailsV2.mEqAvailable.overwrite(res.enableDisable == EnableDisable::ENABLE);
            }
            return MDR_EVENT_EQUALIZER_CHANGED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandleEqEbbParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        EqEbbInquiredType type{};
        if (!detail::ReadEnumTag(cmd, type))
            return MDR_EVENT_UNHANDLED;
        using enum EqEbbInquiredType;
        switch (type)
        {
        case PRESET_EQ:
        {
            const auto command = static_cast<Command>(cmd[0]);
            if (command == Command::EQEBB_NTFY_PARAM)
            {
                Deserialize(EqEbbNtfyParamEq, res, cmd);
                self->mDetailsV2.mEqPresetId.overwrite(res.parameter.presetId);
                switch (res.parameter.bandSteps.size())
                {
                case 0:
                    return MDR_EVENT_EQUALIZER_CHANGED;
                case 6:
                    self->mDetailsV2.mEqClearBass.overwrite(res.parameter.bandSteps.value[0] - 10);
                    self->mDetailsV2.mEqConfig.overwrite({
                        res.parameter.bandSteps.value[1] - 10, // 400
                        res.parameter.bandSteps.value[2] - 10, // 1k
                        res.parameter.bandSteps.value[3] - 10, // 2.5k
                        res.parameter.bandSteps.value[4] - 10, // 6.3k
                        res.parameter.bandSteps.value[5] - 10, // 16k
                    });
                    return MDR_EVENT_EQUALIZER_CHANGED;
                case 10:
                    self->mDetailsV2.mEqClearBass.overwrite(0); // Unavailable
                    self->mDetailsV2.mEqConfig.overwrite({
                        res.parameter.bandSteps.value[0] - 6, // 31
                        res.parameter.bandSteps.value[1] - 6, // 63
                        res.parameter.bandSteps.value[2] - 6, // 125
                        res.parameter.bandSteps.value[3] - 6, // 250
                        res.parameter.bandSteps.value[4] - 6, // 500
                        res.parameter.bandSteps.value[5] - 6, // 1k
                        res.parameter.bandSteps.value[6] - 6, // 2k
                        res.parameter.bandSteps.value[7] - 6, // 4k
                        res.parameter.bandSteps.value[8] - 6, // 8k
                        res.parameter.bandSteps.value[9] - 6, // 16k
                    });
                    return MDR_EVENT_EQUALIZER_CHANGED;
                default:
                    break;
                }
            }
            else
            {
                Deserialize(EqEbbRetParamEq, res, cmd);
                self->mDetailsV2.mEqPresetId.overwrite(res.parameter.presetId);
                switch (res.parameter.bandSteps.size())
                {
                case 0:
                    return MDR_EVENT_EQUALIZER_CHANGED;
                case 6:
                    self->mDetailsV2.mEqClearBass.overwrite(res.parameter.bandSteps.value[0] - 10);
                    self->mDetailsV2.mEqConfig.overwrite({
                        res.parameter.bandSteps.value[1] - 10, // 400
                        res.parameter.bandSteps.value[2] - 10, // 1k
                        res.parameter.bandSteps.value[3] - 10, // 2.5k
                        res.parameter.bandSteps.value[4] - 10, // 6.3k
                        res.parameter.bandSteps.value[5] - 10, // 16k
                    });
                    return MDR_EVENT_EQUALIZER_CHANGED;
                case 10:
                    self->mDetailsV2.mEqClearBass.overwrite(0); // Unavailable
                    self->mDetailsV2.mEqConfig.overwrite({
                        res.parameter.bandSteps.value[0] - 6, // 31
                        res.parameter.bandSteps.value[1] - 6, // 63
                        res.parameter.bandSteps.value[2] - 6, // 125
                        res.parameter.bandSteps.value[3] - 6, // 250
                        res.parameter.bandSteps.value[4] - 6, // 500
                        res.parameter.bandSteps.value[5] - 6, // 1k
                        res.parameter.bandSteps.value[6] - 6, // 2k
                        res.parameter.bandSteps.value[7] - 6, // 4k
                        res.parameter.bandSteps.value[8] - 6, // 8k
                        res.parameter.bandSteps.value[9] - 6, // 16k
                    });
                    return MDR_EVENT_EQUALIZER_CHANGED;
                default:
                    break;
                }
            }
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandleAlertParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        AlertInquiredType type{};
        if (!detail::ReadEnumTag(cmd, type))
            return MDR_EVENT_UNHANDLED;
        using enum AlertInquiredType;
        switch (type)
        {
        case FIXED_MESSAGE:
        {
            if (self->mDetailsV2.mSupport.contains(t1::FunctionType::FIXED_MESSAGE))
            {
                Deserialize(AlertNotifyParamFixedMessage, res, cmd);
                using enum AlertActionType;
                switch (res.actionType)
                {
                case POSITIVE_NEGATIVE:
                {
                    self->mDetailsV2.mLastAlertMessage = res.messageType;
                    return MDR_EVENT_ALERT;
                }
                default:
                    break;
                }
            }
            return MDR_EVENT_UNHANDLED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
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
            self->mDetailsV2.mLastDeviceJSONMessage = res.value;
            return MDR_EVENT_DEVICE_MESSAGE;
        }
        case 0x01:
        {
            if (cmd.size() < 4)
                return self->SetLastError(MDR_RESULT_ERROR_MALFORMED_PAYLOAD, "Malformed interaction parameter");
            self->mDetailsV2.mLastInteractionMessage = mdr::String(cmd.begin() + 4, cmd.end());
            return MDR_EVENT_INTERACTION;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int MDRHeadphones::HandleCommandV2T1(Span<const UInt8> cmd, MDRCommandSeqNumber seq)
    {
        auto* self = this;
        using enum Command;
        if (cmd.empty())
            return MDR_EVENT_UNHANDLED;
        const auto command = static_cast<Command>(cmd[0]);
        MDR_LOG_DEBUG(">> {}", command);
        switch (command)
        {
        case CONNECT_RET_SUPPORT_FUNCTION:
            return HandleSupportFunctionT1(self, cmd);
        case CONNECT_RET_CAPABILITY_INFO:
            return HandleCapabilityInfoT1(self, cmd);
        case CONNECT_RET_DEVICE_INFO:
            return HandleDeviceInfoT1(self, cmd);
        case COMMON_RET_STATUS:
        case COMMON_NTFY_STATUS:
            return HandleCommonStatusT1(self, cmd);
        case NCASM_RET_PARAM:
        case NCASM_NTFY_PARAM:
            return HandleNcAsmParamT1(self, cmd);
        case POWER_RET_STATUS:
        case POWER_NTFY_STATUS:
            return HandlePowerStatusT1(self, cmd);
        case PLAY_RET_PARAM:
        case PLAY_NTFY_PARAM:
            return HandlePlayParamT1(self, cmd);
        case POWER_RET_PARAM:
        case POWER_NTFY_PARAM:
            return HandlePowerParamT1(self, cmd);
        case PLAY_RET_STATUS:
        case PLAY_NTFY_STATUS:
            return HandlePlaybackStatusT1(self, cmd);
        case GENERAL_SETTING_RET_CAPABILITY:
            return HandleGsCapabilityT1(self, cmd);
        case GENERAL_SETTING_RET_PARAM:
        case GENERAL_SETTING_NTNY_PARAM:
            return HandleGsParamT1(self, cmd);
        case AUDIO_RET_CAPABILITY:
            return HandleAudioCapabilityT1(self, cmd);
        case AUDIO_RET_STATUS:
        case AUDIO_NTFY_STATUS:
            return HandleAudioStatusT1(self, cmd);
        case AUDIO_RET_PARAM:
        case AUDIO_NTFY_PARAM:
            return HandleAudioParamT1(self, cmd);
        case SYSTEM_RET_PARAM:
        case SYSTEM_NTFY_PARAM:
            return HandleSystemParamT1(self, cmd);
        case SYSTEM_RET_EXT_PARAM:
        case SYSTEM_NTFY_EXT_PARAM:
            return HandleSystemExtParamT1(self, cmd);
        case EQEBB_RET_STATUS:
        case EQEBB_NTFY_STATUS:
            return HandleEqEbbStatusT1(self, cmd);
        case EQEBB_RET_PARAM:
        case EQEBB_NTFY_PARAM:
            return HandleEqEbbParamT1(self, cmd);
        case ALERT_NTFY_PARAM:
            return HandleAlertParamT1(self, cmd);
        case LOG_NTFY_PARAM:
            return HandleLogParamT1(self, cmd);
        default:
            MDR_LOG_DEBUG("^^ Unhandled {}", command);
        }
        return MDR_EVENT_UNHANDLED;
    }
}
