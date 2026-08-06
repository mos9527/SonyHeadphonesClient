#include <algorithm>
#include "Details.hpp"

namespace mdr
{
    using namespace v1;
    using namespace t1;

    namespace
    {
        template <typename T>
        bool ReadByte(Span<const UInt8> cmd, size_t offset, T& out)
        {
            if (cmd.size() <= offset)
                return false;
            out = static_cast<T>(cmd[offset]);
            return true;
        }

        v2::t1::ModelSeries ToV2(ModelSeries value)
        {
            switch (value)
            {
            case ModelSeries::NO_SERIES: return v2::t1::ModelSeries::NO_SERIES;
            case ModelSeries::EXTRA_BASS: return v2::t1::ModelSeries::EXTRA_BASS;
            case ModelSeries::HEAR: return v2::t1::ModelSeries::HEAR;
            case ModelSeries::PREMIUM: return v2::t1::ModelSeries::PREMIUM;
            case ModelSeries::SPORTS: return v2::t1::ModelSeries::SPORTS;
            case ModelSeries::CASUAL: return v2::t1::ModelSeries::CASUAL;
            default: return v2::t1::ModelSeries::OUT_OF_RANGE;
            }
        }

        v2::ModelColor ToV2(ModelColor value)
        {
            switch (value)
            {
            case ModelColor::DEFAULT: return v2::ModelColor::DEFAULT;
            case ModelColor::BLACK: return v2::ModelColor::BLACK;
            case ModelColor::WHITE: return v2::ModelColor::WHITE;
            case ModelColor::SILVER: return v2::ModelColor::SILVER;
            case ModelColor::RED: return v2::ModelColor::RED;
            case ModelColor::BLUE: return v2::ModelColor::BLUE;
            case ModelColor::PINK: return v2::ModelColor::PINK;
            case ModelColor::YELLOW: return v2::ModelColor::YELLOW;
            case ModelColor::GREEN: return v2::ModelColor::GREEN;
            case ModelColor::GRAY: return v2::ModelColor::GRAY;
            case ModelColor::GOLD: return v2::ModelColor::GOLD;
            case ModelColor::CREAM: return v2::ModelColor::CREAM;
            case ModelColor::ORANGE: return v2::ModelColor::ORANGE;
            case ModelColor::BROWN: return v2::ModelColor::BROWN;
            case ModelColor::VIOLET: return v2::ModelColor::VIOLET;
            case ModelColor::BLACK_I: return v2::ModelColor::BLACK_I;
            case ModelColor::WHITE_I: return v2::ModelColor::WHITE_I;
            case ModelColor::SILVER_I: return v2::ModelColor::SILVER_I;
            case ModelColor::RED_I: return v2::ModelColor::RED_I;
            case ModelColor::BLUE_I: return v2::ModelColor::BLUE_I;
            case ModelColor::PINK_I: return v2::ModelColor::PINK_I;
            case ModelColor::YELLOW_I: return v2::ModelColor::YELLOW_I;
            case ModelColor::GREEN_I: return v2::ModelColor::GREEN_I;
            case ModelColor::GRAY_I: return v2::ModelColor::GRAY_I;
            case ModelColor::GOLD_I: return v2::ModelColor::GOLD_I;
            case ModelColor::CREAM_I: return v2::ModelColor::CREAM_I;
            case ModelColor::ORANGE_I: return v2::ModelColor::ORANGE_I;
            case ModelColor::BROWN_I: return v2::ModelColor::BROWN_I;
            case ModelColor::VIOLET_I: return v2::ModelColor::VIOLET_I;
            default: return v2::ModelColor::DEFAULT;
            }
        }

        v2::t1::AudioCodec ToV2(AudioCodec value)
        {
            switch (value)
            {
            case AudioCodec::UNSETTLED: return v2::t1::AudioCodec::UNSETTLED;
            case AudioCodec::SBC: return v2::t1::AudioCodec::SBC;
            case AudioCodec::AAC: return v2::t1::AudioCodec::AAC;
            case AudioCodec::LDAC: return v2::t1::AudioCodec::LDAC;
            case AudioCodec::APT_X: return v2::t1::AudioCodec::APT_X;
            case AudioCodec::APT_X_HD: return v2::t1::AudioCodec::APT_X_HD;
            default: return v2::t1::AudioCodec::OTHER;
            }
        }

        v2::t1::BatteryChargingStatus ToV2(BatteryChargingStatus value)
        {
            switch (value)
            {
            case BatteryChargingStatus::NOT_CHARGING: return v2::t1::BatteryChargingStatus::NOT_CHARGING;
            case BatteryChargingStatus::CHARGING: return v2::t1::BatteryChargingStatus::CHARGING;
            default: return v2::t1::BatteryChargingStatus::UNKNOWN;
            }
        }

        v2::t1::PlaybackStatus ToV2(PlaybackStatus value)
        {
            switch (value)
            {
            case PlaybackStatus::PLAY: return v2::t1::PlaybackStatus::PLAY;
            case PlaybackStatus::PAUSE: return v2::t1::PlaybackStatus::PAUSE;
            case PlaybackStatus::STOP: return v2::t1::PlaybackStatus::STOP;
            default: return v2::t1::PlaybackStatus::UNSETTLED;
            }
        }

        v2::t1::PriorMode ToV2(ConnectionModeSettingValue value)
        {
            return value == ConnectionModeSettingValue::CONNECTION_QUALITY_PRIOR
                ? v2::t1::PriorMode::CONNECTION_QUALITY_PRIOR
                : v2::t1::PriorMode::SOUND_QUALITY_PRIOR;
        }

        v2::t1::DetectSensitivity ToV2(DetectionSensitivity value)
        {
            switch (value)
            {
            case DetectionSensitivity::HIGH: return v2::t1::DetectSensitivity::HIGH;
            case DetectionSensitivity::LOW: return v2::t1::DetectSensitivity::LOW;
            default: return v2::t1::DetectSensitivity::AUTO;
            }
        }

        v2::t1::ModeOutTime ToV2(ModeOutTime value)
        {
            switch (value)
            {
            case ModeOutTime::MID: return v2::t1::ModeOutTime::MID;
            case ModeOutTime::SLOW: return v2::t1::ModeOutTime::SLOW;
            case ModeOutTime::NONE: return v2::t1::ModeOutTime::NONE;
            default: return v2::t1::ModeOutTime::FAST;
            }
        }

        v2::t1::EqPresetId ToV2(EqPresetId value)
        {
            switch (value)
            {
            case EqPresetId::ROCK: return v2::t1::EqPresetId::ROCK;
            case EqPresetId::POP: return v2::t1::EqPresetId::POP;
            case EqPresetId::JAZZ: return v2::t1::EqPresetId::JAZZ;
            case EqPresetId::DANCE: return v2::t1::EqPresetId::DANCE;
            case EqPresetId::EDM: return v2::t1::EqPresetId::EDM;
            case EqPresetId::R_AND_B_HIP_HOP: return v2::t1::EqPresetId::R_AND_B_HIP_HOP;
            case EqPresetId::ACOUSTIC: return v2::t1::EqPresetId::ACOUSTIC;
            case EqPresetId::BRIGHT: return v2::t1::EqPresetId::BRIGHT;
            case EqPresetId::EXCITED: return v2::t1::EqPresetId::EXCITED;
            case EqPresetId::MELLOW: return v2::t1::EqPresetId::MELLOW;
            case EqPresetId::RELAXED: return v2::t1::EqPresetId::RELAXED;
            case EqPresetId::VOCAL: return v2::t1::EqPresetId::VOCAL;
            case EqPresetId::TREBLE: return v2::t1::EqPresetId::TREBLE;
            case EqPresetId::BASS: return v2::t1::EqPresetId::BASS;
            case EqPresetId::SPEECH: return v2::t1::EqPresetId::SPEECH;
            case EqPresetId::CUSTOM: return v2::t1::EqPresetId::CUSTOM;
            case EqPresetId::USER_SETTING1: return v2::t1::EqPresetId::USER_SETTING1;
            case EqPresetId::USER_SETTING2: return v2::t1::EqPresetId::USER_SETTING2;
            case EqPresetId::USER_SETTING3: return v2::t1::EqPresetId::USER_SETTING3;
            case EqPresetId::USER_SETTING4: return v2::t1::EqPresetId::USER_SETTING4;
            case EqPresetId::USER_SETTING5: return v2::t1::EqPresetId::USER_SETTING5;
            case EqPresetId::OFF: return v2::t1::EqPresetId::OFF;
            default: return v2::t1::EqPresetId::UNSPECIFIED;
            }
        }

        v2::t1::Preset ToV2(AssignableSettingsPreset value)
        {
            switch (value)
            {
            case AssignableSettingsPreset::AMBIENT_SOUND_CONTROL:
                return v2::t1::Preset::AMBIENT_SOUND_CONTROL;
            case AssignableSettingsPreset::VOLUME_CONTROL:
                return v2::t1::Preset::VOLUME_CONTROL;
            case AssignableSettingsPreset::PLAYBACK_CONTROL:
                return v2::t1::Preset::PLAYBACK_CONTROL;
            case AssignableSettingsPreset::VOICE_RECOGNITION:
                return v2::t1::Preset::VOICE_RECOGNITION;
            case AssignableSettingsPreset::GOOGLE_ASSISTANT:
                return v2::t1::Preset::GOOGLE_ASSIST;
            case AssignableSettingsPreset::AMAZON_ALEXA:
                return v2::t1::Preset::AMAZON_ALEXA;
            case AssignableSettingsPreset::TENCENT_XIAOWEI:
                return v2::t1::Preset::TENCENT_XIAOWEI;
            default:
                return v2::t1::Preset::NO_FUNCTION;
            }
        }

        v2::t1::AutoPowerOffElements ToV2(AutoPowerOffElementId value)
        {
            switch (value)
            {
            case AutoPowerOffElementId::POWER_OFF_IN_30_MIN:
                return v2::t1::AutoPowerOffElements::POWER_OFF_IN_30_MIN;
            case AutoPowerOffElementId::POWER_OFF_IN_60_MIN:
                return v2::t1::AutoPowerOffElements::POWER_OFF_IN_60_MIN;
            case AutoPowerOffElementId::POWER_OFF_IN_180_MIN:
                return v2::t1::AutoPowerOffElements::POWER_OFF_IN_180_MIN;
            case AutoPowerOffElementId::POWER_OFF_DISABLE:
                return v2::t1::AutoPowerOffElements::POWER_OFF_DISABLE;
            default:
                return v2::t1::AutoPowerOffElements::POWER_OFF_IN_5_MIN;
            }
        }

        int HandleSupport(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            Deserialize(RetSupportFunction, result, cmd);
            std::ranges::fill(self->mSupport.v1Functions, false);
            for (const FunctionType function : result.supportFunctions)
                self->mSupport.v1Functions[static_cast<UInt8>(function)] = true;
            self->mSupport.provenance = MDRHeadphones::SupportStates::Provenance::ADVERTISED;
            self->RefreshNeutralFeaturesV1();
            self->Awake(MDRHeadphones::AWAIT_SUPPORT_FUNCTION);
            return MDR_EVENT_IDENTITY_CHANGED;
        }

        int HandleDeviceInfo(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            DeviceInfoInquiredType type{};
            if (!ReadByte(cmd, 1, type))
                return MDR_EVENT_UNHANDLED;
            switch (type)
            {
            case DeviceInfoInquiredType::MODEL_NAME:
            {
                Deserialize(RetDeviceInfo_DeviceInfoModelName, result, cmd);
                self->mModelName = result.modelName.value;
                self->Awake(MDRHeadphones::AWAIT_MODEL_INFO);
                break;
            }
            case DeviceInfoInquiredType::FW_VERSION:
            {
                Deserialize(RetDeviceInfo_DeviceInfoFwVersion, result, cmd);
                self->mFWVersion = result.fwVersion.value;
                break;
            }
            case DeviceInfoInquiredType::SERIES_AND_COLOR_INFO:
            {
                Deserialize(RetDeviceInfo_DeviceInfoSeriesAndColor, result, cmd);
                self->mModelSeries = ToV2(result.series);
                self->mModelColor = ToV2(result.color);
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
            if (!ReadByte(cmd, 1, type))
                return MDR_EVENT_UNHANDLED;
            const Command command = static_cast<Command>(cmd[0]);
            switch (type)
            {
            case BatteryInquiredType::BATTERY:
                if (command == Command::COMMON_NTFY_BATTERY_LEVEL)
                {
                    Deserialize(NotifyBatteryLevelBatteryParam, result, cmd);
                    self->mBatteryL = {result.level, 0xFF, ToV2(result.chargingStatus)};
                }
                else
                {
                    Deserialize(RetBatteryLevelBatteryParam, result, cmd);
                    self->mBatteryL = {result.level, 0xFF, ToV2(result.chargingStatus)};
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
                self->mAudioCodec = ToV2(result.audioCodec);
            }
            else
            {
                Deserialize(RetAudioCodec, result, cmd);
                self->mAudioCodec = ToV2(result.audioCodec);
            }
            return MDR_EVENT_IDENTITY_CHANGED;
        }

        int HandlePlayParam(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            PlaybackDetailedDataType type{};
            if (!ReadByte(cmd, 2, type))
                return MDR_EVENT_UNHANDLED;
            const Command command = static_cast<Command>(cmd[0]);
            if (type == PlaybackDetailedDataType::VOLUME)
            {
                if (command == Command::PLAY_NTFY_PARAM)
                {
                    Deserialize(NotifyPlayParamPlaybackControllerVolumeData, result, cmd);
                    self->mPlayVolume.overwrite(result.volumeValue);
                }
                else
                {
                    Deserialize(RetPlayParamPlaybackControllerVolumeData, result, cmd);
                    self->mPlayVolume.overwrite(result.volumeValue);
                }
                return MDR_EVENT_PLAYBACK_CHANGED;
            }
            if (type == PlaybackDetailedDataType::TRACK_NAME ||
                type == PlaybackDetailedDataType::ALBUM_NAME ||
                type == PlaybackDetailedDataType::ARTIST_NAME)
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
                if (type == PlaybackDetailedDataType::TRACK_NAME)
                    self->mPlayTrackTitle = std::move(value);
                else if (type == PlaybackDetailedDataType::ALBUM_NAME)
                    self->mPlayTrackAlbum = std::move(value);
                else
                    self->mPlayTrackArtist = std::move(value);
                return MDR_EVENT_PLAYBACK_CHANGED;
            }
            return MDR_EVENT_UNHANDLED;
        }

        int HandlePlayStatus(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            if (static_cast<Command>(cmd[0]) == Command::PLAY_NTFY_STATUS)
            {
                Deserialize(NotifyPlayStatus, result, cmd);
                self->mPlayPause = ToV2(result.playbackStatus);
            }
            else
            {
                Deserialize(RetPlayStatus, result, cmd);
                self->mPlayPause = ToV2(result.playbackStatus);
            }
            return MDR_EVENT_PLAYBACK_CHANGED;
        }

        template <typename T>
        void ApplyNcAsm(MDRHeadphones* self, const T& result)
        {
            self->mNcAsmEnabled.overwrite(result.ncAsmEffect == NcAsmEffect::ON);
            self->mNcAsmMode.overwrite(
                result.asmValue > 0 ? v2::t1::NcAsmMode::ASM : v2::t1::NcAsmMode::NC);
            self->mNcAsmFocusOnVoice.overwrite(result.asmId == AsmId::VOICE);
            self->mNcAsmAmbientLevel.overwrite(result.asmValue);
        }

        int HandleNcAsm(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            NcAsmInquiredType type{};
            if (!ReadByte(cmd, 1, type))
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
                    self->mNcAsmEnabled.overwrite(result.ncAsmEffect == NcAsmEffect::ON);
                    self->mNcAsmMode.overwrite(v2::t1::NcAsmMode::ASM);
                    self->mNcAsmFocusOnVoice.overwrite(result.asmId == AsmId::VOICE);
                    self->mNcAsmAmbientLevel.overwrite(result.asmValue);
                }
                else
                {
                    Deserialize(RetNcAsmParamAsmParam, result, cmd);
                    self->mNcAsmEnabled.overwrite(result.ncAsmEffect == NcAsmEffect::ON);
                    self->mNcAsmMode.overwrite(v2::t1::NcAsmMode::ASM);
                    self->mNcAsmFocusOnVoice.overwrite(result.asmId == AsmId::VOICE);
                    self->mNcAsmAmbientLevel.overwrite(result.asmValue);
                }
                return MDR_EVENT_NOISE_CONTROL_CHANGED;
            }
            return MDR_EVENT_UNHANDLED;
        }

        int HandleEq(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            EqEbbInquiredType type{};
            if (!ReadByte(cmd, 1, type) || type != EqEbbInquiredType::PRESET_EQ)
                return MDR_EVENT_UNHANDLED;
            const bool notify = static_cast<Command>(cmd[0]) == Command::EQEBB_NTFY_PARAM;
            if (notify)
            {
                Deserialize(NotifyEqEbbParamEqParam, result, cmd);
                self->mEqPresetId.overwrite(ToV2(result.presetId));
                if (!result.bandSteps.value.empty())
                {
                    self->mEqClearBass.overwrite(static_cast<int>(result.bandSteps.value[0]) - 10);
                    Vector<int> bands;
                    for (size_t i = 1; i < result.bandSteps.size(); ++i)
                        bands.push_back(static_cast<int>(result.bandSteps.value[i]) - 10);
                    self->mEqConfig.overwrite(std::move(bands));
                }
            }
            else
            {
                Deserialize(RetEqEbbParamEqParam, result, cmd);
                self->mEqPresetId.overwrite(ToV2(result.presetId));
                if (!result.bandSteps.value.empty())
                {
                    self->mEqClearBass.overwrite(static_cast<int>(result.bandSteps.value[0]) - 10);
                    Vector<int> bands;
                    for (size_t i = 1; i < result.bandSteps.size(); ++i)
                        bands.push_back(static_cast<int>(result.bandSteps.value[i]) - 10);
                    self->mEqConfig.overwrite(std::move(bands));
                }
            }
            self->mEqAvailable.overwrite(true);
            return MDR_EVENT_EQUALIZER_CHANGED;
        }

        int HandleAudioParam(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            AudioInquiredType type{};
            if (!ReadByte(cmd, 1, type))
                return MDR_EVENT_UNHANDLED;
            const bool notify = static_cast<Command>(cmd[0]) == Command::AUDIO_NTFY_PARAM;
            if (type == AudioInquiredType::CONNECTION_MODE)
            {
                if (notify)
                {
                    Deserialize(NotifyAudioParamConnectionModeParam, result, cmd);
                    self->mAudioPriorityMode.overwrite(ToV2(result.settingValue));
                }
                else
                {
                    Deserialize(RetAudioParamConnectionModeParam, result, cmd);
                    self->mAudioPriorityMode.overwrite(ToV2(result.settingValue));
                }
                return MDR_EVENT_CONNECTION_MODE_CHANGED;
            }
            if (type == AudioInquiredType::UPSCALING)
            {
                if (notify)
                {
                    Deserialize(NotifyAudioParamUpscalingParam, result, cmd);
                    self->mUpscalingEnabled.overwrite(result.settingValue == UpscalingSettingValue::AUTO);
                }
                else
                {
                    Deserialize(RetAudioParamUpscalingParam, result, cmd);
                    self->mUpscalingEnabled.overwrite(result.settingValue == UpscalingSettingValue::AUTO);
                }
                return MDR_EVENT_EQUALIZER_CHANGED;
            }
            return MDR_EVENT_UNHANDLED;
        }

        int HandleGsParam(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            GsInquiredType type{};
            if (!ReadByte(cmd, 1, type))
                return MDR_EVENT_UNHANDLED;
            MDRProperty<bool>* property = nullptr;
            switch (type)
            {
            case GsInquiredType::GENERAL_SETTING1:
                property = &self->mGsParamBool1;
                break;
            case GsInquiredType::GENERAL_SETTING2:
                property = &self->mGsParamBool2;
                break;
            case GsInquiredType::GENERAL_SETTING3:
                property = &self->mGsParamBool3;
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
            if (!ReadByte(cmd, 1, type))
                return MDR_EVENT_UNHANDLED;
            const bool notify = static_cast<Command>(cmd[0]) == Command::SYSTEM_NTFY_PARAM;
            switch (type)
            {
            case SystemInquiredType::CONTROL_BY_WEARING:
                if (notify)
                {
                    Deserialize(NotifySystemParamControlByWearingParam, result, cmd);
                    self->mAutoPauseEnabled.overwrite(
                        result.settingValue == ControlByWearingSettingValue::ON);
                }
                else
                {
                    Deserialize(RetSystemParamControlByWearingParam, result, cmd);
                    self->mAutoPauseEnabled.overwrite(
                        result.settingValue == ControlByWearingSettingValue::ON);
                }
                return MDR_EVENT_POWER_CHANGED;
            case SystemInquiredType::AUTO_POWER_OFF:
                if (notify)
                {
                    Deserialize(NotifySystemParamAutoPowerOffParam, result, cmd);
                    self->mPowerAutoOff.overwrite(ToV2(result.activeElementId));
                }
                else
                {
                    Deserialize(RetSystemParamAutoPowerOffParam, result, cmd);
                    self->mPowerAutoOff.overwrite(ToV2(result.activeElementId));
                }
                return MDR_EVENT_POWER_CHANGED;
            case SystemInquiredType::ASSIGNABLE_SETTINGS:
                if (notify)
                {
                    Deserialize(NotifySystemParamAssignableSettingsParam, result, cmd);
                    if (!result.presets.value.empty())
                        self->mTouchFunctionLeft.overwrite(ToV2(result.presets.value[0]));
                    if (result.presets.size() > 1)
                        self->mTouchFunctionRight.overwrite(ToV2(result.presets.value[1]));
                }
                else
                {
                    Deserialize(RetSystemParamAssignableSettingsParam, result, cmd);
                    if (!result.presets.value.empty())
                        self->mTouchFunctionLeft.overwrite(ToV2(result.presets.value[0]));
                    if (result.presets.size() > 1)
                        self->mTouchFunctionRight.overwrite(ToV2(result.presets.value[1]));
                }
                return MDR_EVENT_ASSIGNABLE_CONTROLS_CHANGED;
            case SystemInquiredType::SMART_TALKING_MODE:
                if (cmd.size() == sizeof(RetSystemParamSmartTalkingModeRetParam))
                {
                    if (notify)
                    {
                        Deserialize(NotifySystemParamSmartTalkingModeRetParam, result, cmd);
                        self->mSpeakToChatEnabled.overwrite(
                            result.settingValue == SmartTalkingModeSettingValue::ON);
                    }
                    else
                    {
                        Deserialize(RetSystemParamSmartTalkingModeRetParam, result, cmd);
                        self->mSpeakToChatEnabled.overwrite(
                            result.settingValue == SmartTalkingModeSettingValue::ON);
                    }
                }
                else
                {
                    if (notify)
                    {
                        Deserialize(NotifySystemExParamChildPayloadSmartTalkingModeExType1Param, result, cmd);
                        self->mSpeakToChatDetectSensitivity.overwrite(ToV2(result.devectionSensitivity));
                        self->mV1SpeakToChatVoiceFocus = result.voiceFocus;
                        self->mSpeakToModeOutTime.overwrite(ToV2(result.modeOutTime));
                    }
                    else
                    {
                        Deserialize(RetSystemExParamChildPayloadSmartTalkingModeExType1Param, result, cmd);
                        self->mSpeakToChatDetectSensitivity.overwrite(ToV2(result.devectionSensitivity));
                        self->mV1SpeakToChatVoiceFocus = result.voiceFocus;
                        self->mSpeakToModeOutTime.overwrite(ToV2(result.modeOutTime));
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
        switch (static_cast<Command>(cmd[0]))
        {
        case Command::CONNECT_RET_CAPABILITY_INFO:
        {
            auto* self = this;
            Deserialize(RetCapabilityInfo, result, cmd);
            mUniqueId = result.uniqueId.value;
            return MDR_EVENT_IDENTITY_CHANGED;
        }
        case Command::CONNECT_RET_DEVICE_INFO:
            return HandleDeviceInfo(this, cmd);
        case Command::CONNECT_RET_SUPPORT_FUNCTION:
            return HandleSupport(this, cmd);
        case Command::COMMON_RET_BATTERY_LEVEL:
        case Command::COMMON_NTFY_BATTERY_LEVEL:
            return HandleBattery(this, cmd);
        case Command::COMMON_RET_AUDIO_CODEC:
        case Command::COMMON_NTFY_AUDIO_CODEC:
            return HandleAudioCodec(this, cmd);
        case Command::PLAY_RET_PARAM:
        case Command::PLAY_NTFY_PARAM:
            return HandlePlayParam(this, cmd);
        case Command::PLAY_RET_STATUS:
        case Command::PLAY_NTFY_STATUS:
            return HandlePlayStatus(this, cmd);
        case Command::NCASM_RET_PARAM:
        case Command::NCASM_NTFY_PARAM:
            return HandleNcAsm(this, cmd);
        case Command::EQEBB_RET_PARAM:
        case Command::EQEBB_NTFY_PARAM:
            return HandleEq(this, cmd);
        case Command::AUDIO_RET_PARAM:
        case Command::AUDIO_NTFY_PARAM:
            return HandleAudioParam(this, cmd);
        case Command::GENERAL_SETTING_RET_PARAM:
        case Command::GENERAL_SETTING_NTNY_PARAM:
            return HandleGsParam(this, cmd);
        case Command::SYSTEM_RET_PARAM:
        case Command::SYSTEM_NTFY_PARAM:
            return HandleSystemParam(this, cmd);
        default:
            MDR_LOG_DEBUG("** Unhandled V1 T1 {}", static_cast<Command>(cmd[0]));
            return MDR_EVENT_UNHANDLED;
        }
    }
}
