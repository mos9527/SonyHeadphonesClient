#include <algorithm>
#include "Details.hpp"

namespace mdr
{
    using namespace v1;
    using namespace t1;

    namespace
    {
        int HandleSupport(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            Deserialize(RetSupportFunction, result, cmd);
            std::ranges::fill(self->mDetailsV1.mSupport.functions, false);
            for (const FunctionType function : result.supportFunctions)
                self->mDetailsV1.mSupport.functions[static_cast<UInt8>(function)] = true;
            self->mDetailsV1.mSupport.provenance = DetailsV1::SupportStates::Provenance::ADVERTISED;
            self->RefreshSupportV1();
            self->Awake(MDRHeadphones::AWAIT_SUPPORT_FUNCTION);
            return MDR_EVENT_IDENTITY_CHANGED;
        }

        int HandleDeviceInfo(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            DeviceInfoInquiredType type{};
            if (!detail::ReadEnumTag(cmd, type))
                return MDR_EVENT_UNHANDLED;
            switch (type)
            {
            case DeviceInfoInquiredType::MODEL_NAME:
            {
                Deserialize(RetDeviceInfo_DeviceInfoModelName, result, cmd);
                self->mDetailsV1.mModelName = result.modelName.value;
                self->Awake(MDRHeadphones::AWAIT_MODEL_INFO);
                break;
            }
            case DeviceInfoInquiredType::FW_VERSION:
            {
                Deserialize(RetDeviceInfo_DeviceInfoFwVersion, result, cmd);
                self->mDetailsV1.mFWVersion = result.fwVersion.value;
                break;
            }
            case DeviceInfoInquiredType::SERIES_AND_COLOR_INFO:
            {
                Deserialize(RetDeviceInfo_DeviceInfoSeriesAndColor, result, cmd);
                self->mDetailsV1.mModelSeries = result.series;
                self->mDetailsV1.mModelColor = result.color;
                break;
            }
            default:
                return MDR_EVENT_UNHANDLED;
            }
            return MDR_EVENT_IDENTITY_CHANGED;
        }

        int HandleBattery(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            BatteryInquiredType type{};
            if (!detail::ReadEnumTag(cmd, type))
                return MDR_EVENT_UNHANDLED;
            const Command command = static_cast<Command>(cmd[0]);
            switch (type)
            {
            case BatteryInquiredType::BATTERY:
                if (command == Command::COMMON_NTFY_BATTERY_LEVEL)
                {
                    Deserialize(NotifyBatteryLevelBatteryParam, result, cmd);
                    self->mDetailsV1.mBatteryL = {result.level, 0xFF, result.chargingStatus};
                }
                else
                {
                    Deserialize(RetBatteryLevelBatteryParam, result, cmd);
                    self->mDetailsV1.mBatteryL = {result.level, 0xFF, result.chargingStatus};
                }
                return MDR_EVENT_BATTERY_CHANGED;
            default:
                return MDR_EVENT_UNHANDLED;
            }
        }

        int HandleAudioCodec(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            if (static_cast<Command>(cmd[0]) == Command::COMMON_NTFY_AUDIO_CODEC)
            {
                Deserialize(NotifyAudioCodec, result, cmd);
                self->mDetailsV1.mAudioCodec = result.audioCodec;
            }
            else
            {
                Deserialize(RetAudioCodec, result, cmd);
                self->mDetailsV1.mAudioCodec = result.audioCodec;
            }
            return MDR_EVENT_IDENTITY_CHANGED;
        }

        int HandlePlayParam(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            PlayInquiredType inquiredType{};
            if (!detail::ReadEnumTag(cmd, inquiredType, 1))
                return MDR_EVENT_UNHANDLED;
            PlaybackDetailedDataType detailedDataType{};
            if (!detail::ReadEnumTag(cmd, detailedDataType, 2))
                return MDR_EVENT_UNHANDLED;
            const Command command = static_cast<Command>(cmd[0]);
            if (detailedDataType == PlaybackDetailedDataType::VOLUME)
            {
                if (command == Command::PLAY_NTFY_PARAM)
                {
                    Deserialize(NotifyPlayParamPlaybackControllerVolumeData, result, cmd);
                    self->mDetailsV1.mPlayVolume.overwrite(result.volumeValue);
                }
                else
                {
                    Deserialize(RetPlayParamPlaybackControllerVolumeData, result, cmd);
                    self->mDetailsV1.mPlayVolume.overwrite(result.volumeValue);
                }
                return MDR_EVENT_PLAYBACK_CHANGED;
            }
            if (detailedDataType == PlaybackDetailedDataType::TRACK_NAME ||
                detailedDataType == PlaybackDetailedDataType::ALBUM_NAME ||
                detailedDataType == PlaybackDetailedDataType::ARTIST_NAME)
            {
                String value;
                if (command == Command::PLAY_NTFY_PARAM)
                {
                    Deserialize(NotifyPlayParamPlaybackControllerNameData, result, cmd);
                    value = result.playbackName.name.value;
                }
                else
                {
                    Deserialize(RetPlayParamPlaybackControllerNameData, result, cmd);
                    value = result.playbackName.name.value;
                }
                if (detailedDataType == PlaybackDetailedDataType::TRACK_NAME)
                    self->mDetailsV1.mPlayTrackTitle = std::move(value);
                else if (detailedDataType == PlaybackDetailedDataType::ALBUM_NAME)
                    self->mDetailsV1.mPlayTrackAlbum = std::move(value);
                else
                    self->mDetailsV1.mPlayTrackArtist = std::move(value);
                return MDR_EVENT_PLAYBACK_CHANGED;
            }
            return MDR_EVENT_UNHANDLED;
        }

        int HandlePlayStatus(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            if (static_cast<Command>(cmd[0]) == Command::PLAY_NTFY_STATUS)
            {
                Deserialize(NotifyPlayStatus, result, cmd);
                self->mDetailsV1.mPlayPause = result.playbackStatus;
            }
            else
            {
                Deserialize(RetPlayStatus, result, cmd);
                self->mDetailsV1.mPlayPause = result.playbackStatus;
            }
            return MDR_EVENT_PLAYBACK_CHANGED;
        }

        template <typename T>
        void ApplyNcAsm(MDRHeadphones* self, const T& result)
        {
            self->mDetailsV1.mNcAsmEnabled.overwrite(result.ncAsmEffect == NcAsmEffect::ON);
            self->mDetailsV1.mNcAsmFocusOnVoice.overwrite(result.asmId == AsmId::VOICE);
            self->mDetailsV1.mNcAsmLevel.overwrite(
                result.ncValue == static_cast<UInt8>(NcDualSingleValue::DUAL) ? -1 : result.asmValue);
        }

        int HandleNcAsm(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            NcAsmInquiredType type{};
            if (!detail::ReadEnumTag(cmd, type))
                return MDR_EVENT_UNHANDLED;
            const bool notify = static_cast<Command>(cmd[0]) == Command::NCASM_NTFY_PARAM;
            if (type == NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE)
            {
                if (notify)
                {
                    Deserialize(NotifyNcAsmParamcAsmParam, result, cmd);
                    ApplyNcAsm(self, result);
                }
                else
                {
                    Deserialize(RetNcAsmParamNcAsmParam, result, cmd);
                    ApplyNcAsm(self, result);
                }
                return MDR_EVENT_NOISE_CONTROL_CHANGED;
            }
            if (type == NcAsmInquiredType::AMBIENT_SOUND_MODE)
            {
                if (notify)
                {
                    Deserialize(NotifyNcAsmParamAsmParam, result, cmd);
                    self->mDetailsV1.mNcAsmEnabled.overwrite(result.ncAsmEffect == NcAsmEffect::ON);
                    self->mDetailsV1.mNcAsmFocusOnVoice.overwrite(result.asmId == AsmId::VOICE);
                    self->mDetailsV1.mNcAsmLevel.overwrite(result.asmValue);
                }
                else
                {
                    Deserialize(RetNcAsmParamAsmParam, result, cmd);
                    self->mDetailsV1.mNcAsmEnabled.overwrite(result.ncAsmEffect == NcAsmEffect::ON);
                    self->mDetailsV1.mNcAsmFocusOnVoice.overwrite(result.asmId == AsmId::VOICE);
                    self->mDetailsV1.mNcAsmLevel.overwrite(result.asmValue);
                }
                return MDR_EVENT_NOISE_CONTROL_CHANGED;
            }
            return MDR_EVENT_UNHANDLED;
        }

        int HandleEq(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            EqEbbInquiredType type{};
            if (!detail::ReadEnumTag(cmd, type) || type != EqEbbInquiredType::PRESET_EQ)
                return MDR_EVENT_UNHANDLED;
            const bool notify = static_cast<Command>(cmd[0]) == Command::EQEBB_NTFY_PARAM;
            if (notify)
            {
                Deserialize(NotifyEqEbbParamEqParam, result, cmd);
                self->mDetailsV1.mEqPresetId.overwrite(result.presetId);
                if (!result.bandSteps.value.empty())
                {
                    self->mDetailsV1.mEqClearBass.overwrite(static_cast<int>(result.bandSteps.value[0]) - 10);
                    Vector<int> bands;
                    for (size_t i = 1; i < result.bandSteps.size(); ++i)
                        bands.push_back(static_cast<int>(result.bandSteps.value[i]) - 10);
                    self->mDetailsV1.mEqConfig.overwrite(std::move(bands));
                }
            }
            else
            {
                Deserialize(RetEqEbbParamEqParam, result, cmd);
                self->mDetailsV1.mEqPresetId.overwrite(result.presetId);
                if (!result.bandSteps.value.empty())
                {
                    self->mDetailsV1.mEqClearBass.overwrite(static_cast<int>(result.bandSteps.value[0]) - 10);
                    Vector<int> bands;
                    for (size_t i = 1; i < result.bandSteps.size(); ++i)
                        bands.push_back(static_cast<int>(result.bandSteps.value[i]) - 10);
                    self->mDetailsV1.mEqConfig.overwrite(std::move(bands));
                }
            }
            self->mDetailsV1.mEqAvailable.overwrite(true);
            return MDR_EVENT_EQUALIZER_CHANGED;
        }

        int HandleAudioParam(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            AudioInquiredType type{};
            if (!detail::ReadEnumTag(cmd, type))
                return MDR_EVENT_UNHANDLED;
            const bool notify = static_cast<Command>(cmd[0]) == Command::AUDIO_NTFY_PARAM;
            if (type == AudioInquiredType::CONNECTION_MODE)
            {
                if (notify)
                {
                    Deserialize(NotifyAudioParamConnectionModeParam, result, cmd);
                    self->mDetailsV1.mAudioPriorityMode.overwrite(result.settingValue);
                }
                else
                {
                    Deserialize(RetAudioParamConnectionModeParam, result, cmd);
                    self->mDetailsV1.mAudioPriorityMode.overwrite(result.settingValue);
                }
                return MDR_EVENT_CONNECTION_MODE_CHANGED;
            }
            if (type == AudioInquiredType::UPSCALING)
            {
                if (notify)
                {
                    Deserialize(NotifyAudioParamUpscalingParam, result, cmd);
                    self->mDetailsV1.mUpscalingEnabled.overwrite(result.settingValue == UpscalingSettingValue::AUTO);
                }
                else
                {
                    Deserialize(RetAudioParamUpscalingParam, result, cmd);
                    self->mDetailsV1.mUpscalingEnabled.overwrite(result.settingValue == UpscalingSettingValue::AUTO);
                }
                return MDR_EVENT_EQUALIZER_CHANGED;
            }
            return MDR_EVENT_UNHANDLED;
        }

        int HandleGsParam(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            GsInquiredType type{};
            if (!detail::ReadEnumTag(cmd, type))
                return MDR_EVENT_UNHANDLED;
            MDRProperty<bool>* property = nullptr;
            switch (type)
            {
            case GsInquiredType::GENERAL_SETTING1:
                property = &self->mDetailsV1.mGsParamBool1;
                break;
            case GsInquiredType::GENERAL_SETTING2:
                property = &self->mDetailsV1.mGsParamBool2;
                break;
            case GsInquiredType::GENERAL_SETTING3:
                property = &self->mDetailsV1.mGsParamBool3;
                break;
            default:
                return MDR_EVENT_UNHANDLED;
            }
            if (static_cast<Command>(cmd[0]) == Command::GENERAL_SETTING_NTNY_PARAM)
            {
                Deserialize(NotifyGsParamGsBooleanTypeValue, result, cmd);
                property->overwrite(result.settingValue == CommonOnOffSettingValue::ON);
            }
            else
            {
                Deserialize(RetGsParamGsBooleanTypeValue, result, cmd);
                property->overwrite(result.settingValue == CommonOnOffSettingValue::ON);
            }
            return MDR_EVENT_GENERAL_SETTINGS_CHANGED;
        }

        int HandleSystemParam(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            SystemInquiredType type{};
            if (!detail::ReadEnumTag(cmd, type))
                return MDR_EVENT_UNHANDLED;
            const bool notify = static_cast<Command>(cmd[0]) == Command::SYSTEM_NTFY_PARAM;
            switch (type)
            {
            case SystemInquiredType::CONTROL_BY_WEARING:
                if (notify)
                {
                    Deserialize(NotifySystemParamControlByWearingParam, result, cmd);
                    self->mDetailsV1.mAutoPauseEnabled.overwrite(
                        result.settingValue == ControlByWearingSettingValue::ON);
                }
                else
                {
                    Deserialize(RetSystemParamControlByWearingParam, result, cmd);
                    self->mDetailsV1.mAutoPauseEnabled.overwrite(
                        result.settingValue == ControlByWearingSettingValue::ON);
                }
                return MDR_EVENT_POWER_CHANGED;
            case SystemInquiredType::AUTO_POWER_OFF:
                if (notify)
                {
                    Deserialize(NotifySystemParamAutoPowerOffParam, result, cmd);
                    self->mDetailsV1.mPowerAutoOff.overwrite(result.activeElementId);
                }
                else
                {
                    Deserialize(RetSystemParamAutoPowerOffParam, result, cmd);
                    self->mDetailsV1.mPowerAutoOff.overwrite(result.activeElementId);
                }
                return MDR_EVENT_POWER_CHANGED;
            case SystemInquiredType::ASSIGNABLE_SETTINGS:
                if (notify)
                {
                    Deserialize(NotifySystemParamAssignableSettingsParam, result, cmd);
                    if (!result.presets.value.empty())
                        self->mDetailsV1.mTouchFunctionLeft.overwrite(result.presets.value[0]);
                    if (result.presets.size() > 1)
                        self->mDetailsV1.mTouchFunctionRight.overwrite(result.presets.value[1]);
                }
                else
                {
                    Deserialize(RetSystemParamAssignableSettingsParam, result, cmd);
                    if (!result.presets.value.empty())
                        self->mDetailsV1.mTouchFunctionLeft.overwrite(result.presets.value[0]);
                    if (result.presets.size() > 1)
                        self->mDetailsV1.mTouchFunctionRight.overwrite(result.presets.value[1]);
                }
                return MDR_EVENT_ASSIGNABLE_CONTROLS_CHANGED;
            case SystemInquiredType::SMART_TALKING_MODE:
                if (cmd.size() == sizeof(RetSystemParamSmartTalkingModeRetParam))
                {
                    if (notify)
                    {
                        Deserialize(NotifySystemParamSmartTalkingModeRetParam, result, cmd);
                        self->mDetailsV1.mSpeakToChatEnabled.overwrite(
                            result.settingValue == SmartTalkingModeSettingValue::ON);
                    }
                    else
                    {
                        Deserialize(RetSystemParamSmartTalkingModeRetParam, result, cmd);
                        self->mDetailsV1.mSpeakToChatEnabled.overwrite(
                            result.settingValue == SmartTalkingModeSettingValue::ON);
                    }
                }
                else
                {
                    if (notify)
                    {
                        Deserialize(NotifySystemExParamChildPayloadSmartTalkingModeExType1Param, result, cmd);
                        self->mDetailsV1.mSpeakToChatDetectSensitivity.overwrite(result.devectionSensitivity);
                        self->mDetailsV1.mSpeakToChatVoiceFocus = result.voiceFocus;
                        self->mDetailsV1.mSpeakToModeOutTime.overwrite(result.modeOutTime);
                    }
                    else
                    {
                        Deserialize(RetSystemExParamChildPayloadSmartTalkingModeExType1Param, result, cmd);
                        self->mDetailsV1.mSpeakToChatDetectSensitivity.overwrite(result.devectionSensitivity);
                        self->mDetailsV1.mSpeakToChatVoiceFocus = result.voiceFocus;
                        self->mDetailsV1.mSpeakToModeOutTime.overwrite(result.modeOutTime);
                    }
                }
                return MDR_EVENT_SPEAK_TO_CHAT_CHANGED;
            default:
                return MDR_EVENT_UNHANDLED;
            }
        }
    }

    int MDRHeadphones::HandleCommandV1T1(Span<const UInt8> cmd, MDRCommandSeqNumber)
    {
        if (cmd.empty())
            return MDR_EVENT_UNHANDLED;
        auto* self = this;
        switch (static_cast<Command>(cmd[0]))
        {
        case Command::CONNECT_RET_CAPABILITY_INFO:
        {
            Deserialize(RetCapabilityInfo, result, cmd);
            self->mDetailsV1.mUniqueId = result.uniqueId.value;
            return MDR_EVENT_IDENTITY_CHANGED;
        }
        case Command::CONNECT_RET_DEVICE_INFO:
            return HandleDeviceInfo(self, cmd);
        case Command::CONNECT_RET_SUPPORT_FUNCTION:
            return HandleSupport(self, cmd);
        case Command::COMMON_RET_BATTERY_LEVEL:
        case Command::COMMON_NTFY_BATTERY_LEVEL:
            return HandleBattery(self, cmd);
        case Command::COMMON_RET_AUDIO_CODEC:
        case Command::COMMON_NTFY_AUDIO_CODEC:
            return HandleAudioCodec(self, cmd);
        case Command::PLAY_RET_PARAM:
        case Command::PLAY_NTFY_PARAM:
            return HandlePlayParam(self, cmd);
        case Command::PLAY_RET_STATUS:
        case Command::PLAY_NTFY_STATUS:
            return HandlePlayStatus(self, cmd);
        case Command::NCASM_RET_PARAM:
        case Command::NCASM_NTFY_PARAM:
            return HandleNcAsm(self, cmd);
        case Command::EQEBB_RET_PARAM:
        case Command::EQEBB_NTFY_PARAM:
            return HandleEq(self, cmd);
        case Command::AUDIO_RET_PARAM:
        case Command::AUDIO_NTFY_PARAM:
            return HandleAudioParam(self, cmd);
        case Command::GENERAL_SETTING_RET_PARAM:
        case Command::GENERAL_SETTING_NTNY_PARAM:
            return HandleGsParam(self, cmd);
        case Command::SYSTEM_RET_PARAM:
        case Command::SYSTEM_NTFY_PARAM:
            return HandleSystemParam(self, cmd);
        default:
            MDR_LOG_DEBUG("** Unhandled V1 T1 {}", static_cast<Command>(cmd[0]));
            return MDR_EVENT_UNHANDLED;
        }
    }
}
