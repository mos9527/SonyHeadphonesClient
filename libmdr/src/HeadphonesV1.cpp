#include <algorithm>
#include "Details.hpp"

namespace mdr
{
    using namespace v1;

    void MDRHeadphones::RefreshNeutralFeaturesV1()
    {
        using F = t1::FunctionType;
        auto& features = mSupport.neutralFeatures;
        std::ranges::fill(features, false);
        features[MDR_FEATURE_IDENTITY] = true;
        features[MDR_FEATURE_BATTERY_SINGLE] = mSupport.contains(F::BATTERY_LEVEL);
        const bool playback = mSupport.contains(F::PLAYBACK_CONTROLLER);
        features[MDR_FEATURE_PLAYBACK_METADATA] = playback;
        features[MDR_FEATURE_PLAYBACK_CONTROL] = playback;
        features[MDR_FEATURE_PLAYBACK_VOLUME] = playback;
        features[MDR_FEATURE_NOISE_CANCELLING] =
            mSupport.contains(F::NOISE_CANCELLING) ||
            mSupport.contains(F::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE);
        features[MDR_FEATURE_AMBIENT_SOUND] =
            mSupport.contains(F::AMBIENT_SOUND_MODE) ||
            mSupport.contains(F::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE);
        features[MDR_FEATURE_SPEAK_TO_CHAT] = mSupport.contains(F::SMART_TALKING_MODE);
        features[MDR_FEATURE_EQUALIZER] =
            mSupport.contains(F::PRESET_EQ) ||
            mSupport.contains(F::EBB) ||
            mSupport.contains(F::PRESET_EQ_NONCUSTOMIZABLE);
        features[MDR_FEATURE_DSEE] = mSupport.contains(F::UPSCALING);
        const bool pairing = mSupport.contains(F::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT);
        features[MDR_FEATURE_PAIRED_DEVICE_MANAGEMENT] = pairing;
        features[MDR_FEATURE_PAIRING_MODE] = pairing;
        features[MDR_FEATURE_GENERAL_SETTINGS] =
            mSupport.contains(F::GENERAL_SETTING1) ||
            mSupport.contains(F::GENERAL_SETTING2) ||
            mSupport.contains(F::GENERAL_SETTING3);
        features[MDR_FEATURE_ASSIGNABLE_CONTROLS] = mSupport.contains(F::ASSIGNABLE_SETTINGS);
        features[MDR_FEATURE_AUTO_POWER_OFF] = mSupport.contains(F::AUTO_POWER_OFF);
        features[MDR_FEATURE_WEARING_DETECTION] = mSupport.contains(F::CONTROL_BY_WEARING);
        features[MDR_FEATURE_AUTO_PAUSE] = mSupport.contains(F::CONTROL_BY_WEARING);
        features[MDR_FEATURE_VOICE_GUIDANCE] = mSupport.contains(F::VOICE_GUIDANCE);
        features[MDR_FEATURE_SHUTDOWN] = mSupport.contains(F::POWER_OFF);
        features[MDR_FEATURE_CONNECTION_MODE] = mSupport.contains(F::CONNECTION_MODE);
        mProtocol.hasTable2 = pairing || mSupport.contains(F::VOICE_GUIDANCE);
    }

    namespace
    {
        t1::PlaybackControl ToV1(v2::t1::PlaybackControl value)
        {
            switch (value)
            {
            case v2::t1::PlaybackControl::PAUSE: return t1::PlaybackControl::PAUSE;
            case v2::t1::PlaybackControl::TRACK_UP: return t1::PlaybackControl::TRACK_UP;
            case v2::t1::PlaybackControl::TRACK_DOWN: return t1::PlaybackControl::TRACK_DOWN;
            case v2::t1::PlaybackControl::STOP: return t1::PlaybackControl::STOP;
            case v2::t1::PlaybackControl::PLAY: return t1::PlaybackControl::PLAY;
            case v2::t1::PlaybackControl::FAST_FORWARD: return t1::PlaybackControl::FAST_FORWARD;
            case v2::t1::PlaybackControl::FAST_REWIND: return t1::PlaybackControl::FAST_REWIND;
            default: return t1::PlaybackControl::KEY_OFF;
            }
        }

        t1::EqPresetId ToV1(v2::t1::EqPresetId value)
        {
            switch (value)
            {
            case v2::t1::EqPresetId::ROCK: return t1::EqPresetId::ROCK;
            case v2::t1::EqPresetId::POP: return t1::EqPresetId::POP;
            case v2::t1::EqPresetId::JAZZ: return t1::EqPresetId::JAZZ;
            case v2::t1::EqPresetId::DANCE: return t1::EqPresetId::DANCE;
            case v2::t1::EqPresetId::EDM: return t1::EqPresetId::EDM;
            case v2::t1::EqPresetId::R_AND_B_HIP_HOP: return t1::EqPresetId::R_AND_B_HIP_HOP;
            case v2::t1::EqPresetId::ACOUSTIC: return t1::EqPresetId::ACOUSTIC;
            case v2::t1::EqPresetId::BRIGHT: return t1::EqPresetId::BRIGHT;
            case v2::t1::EqPresetId::EXCITED: return t1::EqPresetId::EXCITED;
            case v2::t1::EqPresetId::MELLOW: return t1::EqPresetId::MELLOW;
            case v2::t1::EqPresetId::RELAXED: return t1::EqPresetId::RELAXED;
            case v2::t1::EqPresetId::VOCAL: return t1::EqPresetId::VOCAL;
            case v2::t1::EqPresetId::TREBLE: return t1::EqPresetId::TREBLE;
            case v2::t1::EqPresetId::BASS: return t1::EqPresetId::BASS;
            case v2::t1::EqPresetId::SPEECH: return t1::EqPresetId::SPEECH;
            case v2::t1::EqPresetId::CUSTOM: return t1::EqPresetId::CUSTOM;
            case v2::t1::EqPresetId::USER_SETTING1: return t1::EqPresetId::USER_SETTING1;
            case v2::t1::EqPresetId::USER_SETTING2: return t1::EqPresetId::USER_SETTING2;
            case v2::t1::EqPresetId::USER_SETTING3: return t1::EqPresetId::USER_SETTING3;
            case v2::t1::EqPresetId::USER_SETTING4: return t1::EqPresetId::USER_SETTING4;
            case v2::t1::EqPresetId::USER_SETTING5: return t1::EqPresetId::USER_SETTING5;
            default: return t1::EqPresetId::OFF;
            }
        }

        t1::DetectionSensitivity ToV1(v2::t1::DetectSensitivity value)
        {
            switch (value)
            {
            case v2::t1::DetectSensitivity::HIGH: return t1::DetectionSensitivity::HIGH;
            case v2::t1::DetectSensitivity::LOW: return t1::DetectionSensitivity::LOW;
            default: return t1::DetectionSensitivity::AUTO;
            }
        }

        t1::ModeOutTime ToV1(v2::t1::ModeOutTime value)
        {
            switch (value)
            {
            case v2::t1::ModeOutTime::MID: return t1::ModeOutTime::MID;
            case v2::t1::ModeOutTime::SLOW: return t1::ModeOutTime::SLOW;
            case v2::t1::ModeOutTime::NONE: return t1::ModeOutTime::NONE;
            default: return t1::ModeOutTime::FAST;
            }
        }

        t1::AutoPowerOffElementId ToV1(v2::t1::AutoPowerOffElements value)
        {
            switch (value)
            {
            case v2::t1::AutoPowerOffElements::POWER_OFF_IN_30_MIN:
                return t1::AutoPowerOffElementId::POWER_OFF_IN_30_MIN;
            case v2::t1::AutoPowerOffElements::POWER_OFF_IN_60_MIN:
                return t1::AutoPowerOffElementId::POWER_OFF_IN_60_MIN;
            case v2::t1::AutoPowerOffElements::POWER_OFF_IN_180_MIN:
                return t1::AutoPowerOffElementId::POWER_OFF_IN_180_MIN;
            case v2::t1::AutoPowerOffElements::POWER_OFF_DISABLE:
                return t1::AutoPowerOffElementId::POWER_OFF_DISABLE;
            default:
                return t1::AutoPowerOffElementId::POWER_OFF_IN_5_MIN;
            }
        }

        t1::AssignableSettingsPreset ToV1(v2::t1::Preset value)
        {
            switch (value)
            {
            case v2::t1::Preset::AMBIENT_SOUND_CONTROL:
                return t1::AssignableSettingsPreset::AMBIENT_SOUND_CONTROL;
            case v2::t1::Preset::VOLUME_CONTROL:
                return t1::AssignableSettingsPreset::VOLUME_CONTROL;
            case v2::t1::Preset::PLAYBACK_CONTROL:
                return t1::AssignableSettingsPreset::PLAYBACK_CONTROL;
            case v2::t1::Preset::VOICE_RECOGNITION:
                return t1::AssignableSettingsPreset::VOICE_RECOGNITION;
            case v2::t1::Preset::GOOGLE_ASSIST:
                return t1::AssignableSettingsPreset::GOOGLE_ASSISTANT;
            case v2::t1::Preset::AMAZON_ALEXA:
                return t1::AssignableSettingsPreset::AMAZON_ALEXA;
            case v2::t1::Preset::TENCENT_XIAOWEI:
                return t1::AssignableSettingsPreset::TENCENT_XIAOWEI;
            default:
                return t1::AssignableSettingsPreset::NO_FUNCTION;
            }
        }

        UInt8 V1NcValueForAmbientLevel(int level)
        {
            if (level <= 0)
                return static_cast<UInt8>(t1::NcDualSingleValue::DUAL);
            if (level == 1)
                return static_cast<UInt8>(t1::NcDualSingleValue::SINGLE);
            return static_cast<UInt8>(t1::NcDualSingleValue::OFF);
        }

        void AddV1Profile(MDRHeadphones& headphones, bool xm4)
        {
            using F = t1::FunctionType;
            constexpr F common[] = {
                F::BATTERY_LEVEL,
                F::CODEC_INDICATOR,
                F::POWER_OFF,
                F::PRESET_EQ,
                F::EBB,
                F::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE,
                F::PLAYBACK_CONTROLLER,
                F::CONNECTION_MODE,
                F::UPSCALING,
                F::CONTROL_BY_WEARING,
                F::AUTO_POWER_OFF,
                F::ASSIGNABLE_SETTINGS,
            };
            for (const F function : common)
                headphones.mSupport.v1Functions[static_cast<UInt8>(function)] = true;
            if (xm4)
            {
                constexpr F xm4Only[] = {
                    F::SMART_TALKING_MODE,
                    F::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT,
                    F::VOICE_GUIDANCE,
                };
                for (const F function : xm4Only)
                    headphones.mSupport.v1Functions[static_cast<UInt8>(function)] = true;
            }
            headphones.mSupport.provenance = MDRHeadphones::SupportStates::Provenance::LEGACY_PROFILE;
            headphones.RefreshNeutralFeaturesV1();
        }
    }

    MDRTask MDRHeadphones::RequestInitV1()
    {
        if (!mProtocol.hasTable1)
            co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "Device doesn't support MDR V1 Table 1");

        SendCommandACK(t1::GetCapabilityInfo);
        SendCommandACK(t1::GetDeviceInfo, {.inquiredType = t1::DeviceInfoInquiredType::MODEL_NAME});
        SendCommandACK(t1::GetDeviceInfo, {.inquiredType = t1::DeviceInfoInquiredType::FW_VERSION});
        SendCommandACK(t1::GetDeviceInfo, {.inquiredType = t1::DeviceInfoInquiredType::SERIES_AND_COLOR_INFO});

        SendCommandACK(t1::GetSupportFunction);
        const int supportResult = co_await Await(AWAIT_SUPPORT_FUNCTION);
        if (supportResult != MDR_RESULT_OK)
        {
            std::ranges::fill(mSupport.v1Functions, false);
            if (mModelName.empty())
                co_await Await(AWAIT_MODEL_INFO);
            if (mModelName == "WH-1000XM4")
                AddV1Profile(*this, true);
            else if (mModelName == "WH-1000XM3")
                AddV1Profile(*this, false);
            else
            {
                mSupport.provenance = SupportStates::Provenance::UNKNOWN;
                RefreshNeutralFeaturesV1();
                mNeutralInitialized = true;
                co_return MDR_EVENT_INITIALIZE_COMPLETE;
            }
        }

        using F = t1::FunctionType;
        if (mSupport.contains(F::CODEC_INDICATOR))
            SendCommandACK(t1::GetAudioCodec);
        if (mSupport.contains(F::PLAYBACK_CONTROLLER))
        {
            SendCommandACK(t1::GetPlayParam, {
                .type = t1::PlayInquiredType::PLAYBACK_CONTROLLER,
                .dataType = t1::PlaybackDetailedDataType::TRACK_NAME
            });
            SendCommandACK(t1::GetPlayParam, {
                .type = t1::PlayInquiredType::PLAYBACK_CONTROLLER,
                .dataType = t1::PlaybackDetailedDataType::ALBUM_NAME
            });
            SendCommandACK(t1::GetPlayParam, {
                .type = t1::PlayInquiredType::PLAYBACK_CONTROLLER,
                .dataType = t1::PlaybackDetailedDataType::ARTIST_NAME
            });
            SendCommandACK(t1::GetPlayParam, {
                .type = t1::PlayInquiredType::PLAYBACK_CONTROLLER,
                .dataType = t1::PlaybackDetailedDataType::VOLUME
            });
            SendCommandACK(t1::GetPlayStatus, {.type = t1::PlayInquiredType::PLAYBACK_CONTROLLER});
        }
        if (mSupport.contains(F::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE))
        {
            SendCommandACK(t1::GetNcAsmCapability, {
                .type = t1::NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE
            });
            SendCommandACK(t1::GetNcAsmParam, {
                .type = t1::NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE
            });
        }
        else
        {
            if (mSupport.contains(F::NOISE_CANCELLING))
                SendCommandACK(t1::GetNcAsmParam, {.type = t1::NcAsmInquiredType::NOISE_CANCELLING});
            if (mSupport.contains(F::AMBIENT_SOUND_MODE))
                SendCommandACK(t1::GetNcAsmParam, {.type = t1::NcAsmInquiredType::AMBIENT_SOUND_MODE});
        }
        if (mSupport.contains(F::PRESET_EQ))
        {
            SendCommandACK(t1::GetEqEbbCapability, {
                .type = t1::EqEbbInquiredType::PRESET_EQ,
                .language = t1::DisplayLanguage::ENGLISH
            });
            SendCommandACK(t1::GetEqEbbParam, {.type = t1::EqEbbInquiredType::PRESET_EQ});
        }
        if (mSupport.contains(F::UPSCALING))
        {
            SendCommandACK(t1::GetAudioCapability, {.inquiredType = t1::AudioInquiredType::UPSCALING});
            SendCommandACK(t1::GetAudioParam, {.audioInquiredType = t1::AudioInquiredType::UPSCALING});
        }
        if (mSupport.contains(F::CONNECTION_MODE))
            SendCommandACK(t1::GetAudioParam, {.audioInquiredType = t1::AudioInquiredType::CONNECTION_MODE});
        if (mSupport.contains(F::GENERAL_SETTING1))
        {
            SendCommandACK(t1::GetGsCapability, {
                .type = t1::GsInquiredType::GENERAL_SETTING1,
                .displayLanguage = t1::DisplayLanguage::ENGLISH
            });
            SendCommandACK(t1::GetGsParam, {.type = t1::GsInquiredType::GENERAL_SETTING1});
        }
        if (mSupport.contains(F::GENERAL_SETTING2))
        {
            SendCommandACK(t1::GetGsCapability, {
                .type = t1::GsInquiredType::GENERAL_SETTING2,
                .displayLanguage = t1::DisplayLanguage::ENGLISH
            });
            SendCommandACK(t1::GetGsParam, {.type = t1::GsInquiredType::GENERAL_SETTING2});
        }
        if (mSupport.contains(F::GENERAL_SETTING3))
        {
            SendCommandACK(t1::GetGsCapability, {
                .type = t1::GsInquiredType::GENERAL_SETTING3,
                .displayLanguage = t1::DisplayLanguage::ENGLISH
            });
            SendCommandACK(t1::GetGsParam, {.type = t1::GsInquiredType::GENERAL_SETTING3});
        }
        if (mSupport.contains(F::ASSIGNABLE_SETTINGS))
            SendCommandACK(t1::GetSystemParam, {
                .systemInquiredType = t1::SystemInquiredType::ASSIGNABLE_SETTINGS
            });
        if (mSupport.contains(F::AUTO_POWER_OFF))
            SendCommandACK(t1::GetSystemParam, {
                .systemInquiredType = t1::SystemInquiredType::AUTO_POWER_OFF
            });
        if (mSupport.contains(F::CONTROL_BY_WEARING))
            SendCommandACK(t1::GetSystemParam, {
                .systemInquiredType = t1::SystemInquiredType::CONTROL_BY_WEARING
            });
        if (mSupport.contains(F::SMART_TALKING_MODE))
        {
            SendCommandACK(t1::GetSystemParam, {
                .systemInquiredType = t1::SystemInquiredType::SMART_TALKING_MODE
            });
            SendCommandACK(t1::GetSystemExParam, {
                .systemInquiredType = t1::SystemInquiredType::SMART_TALKING_MODE
            });
        }
        if (mProtocol.hasTable2)
        {
            if (mSupport.contains(F::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
            {
                SendCommandACK(t2::GetPeripheralStatus, {
                    .inquiredType = t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
                });
                SendCommandACK(t2::GetPeripheralParam, {
                    .inquiredType = t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
                });
            }
            if (mSupport.contains(F::VOICE_GUIDANCE))
            {
                SendCommandACK(t2::GetVoiceGuidanceStatus, {
                    .inquiredType = t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING,
                    .statusType = t2::StatusType::ON_OFF
                });
                SendCommandACK(t2::GetVoiceGuidanceParam, {
                    .inquiredType = t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING,
                    .detailedDataType = t2::DetailedDataType::ON_OFF
                });
            }
        }
        mNeutralInitialized = true;
        co_return MDR_EVENT_INITIALIZE_COMPLETE;
    }

    MDRTask MDRHeadphones::RequestSyncV1()
    {
        if (mSupport.contains(t1::FunctionType::BATTERY_LEVEL))
            SendCommandACK(t1::GetBatteryLevel, {.batteryInquiredType = t1::BatteryInquiredType::BATTERY});
        co_return MDR_EVENT_SYNC_COMPLETE;
    }

    MDRTask MDRHeadphones::RequestCommitV1()
    {
        SnapshotProperties();
        using F = t1::FunctionType;

        if (mShutdown.submittedDirty())
        {
            if (mShutdown.submitted && mSupport.contains(F::POWER_OFF))
                SendCommandACK(t1::SetPowerOff);
            mShutdown.commitOneShot(false);
        }

        if (mNcAsmEnabled.submittedDirty() || mNcAsmMode.submittedDirty() ||
            mNcAsmFocusOnVoice.submittedDirty() || mNcAsmAmbientLevel.submittedDirty())
        {
            if (mSupport.contains(F::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE))
            {
                t1::SetNcAsmParamNcAsmParam payload;
                payload.ncAsmEffect =
                    mNcAsmEnabled.submitted ? t1::NcAsmEffect::ON : t1::NcAsmEffect::OFF;
                payload.ncType = t1::NcAsmSettingType::LEVEL_ADJUSTMENT;
                payload.ncValue = V1NcValueForAmbientLevel(mNcAsmAmbientLevel.submitted);
                payload.asmType = t1::AsmSettingType::LEVEL_ADJUSTMENT;
                payload.asmId = mNcAsmFocusOnVoice.submitted ? t1::AsmId::VOICE : t1::AsmId::NORMAL;
                payload.asmValue = static_cast<UInt8>(std::clamp(mNcAsmAmbientLevel.submitted, 0, 20));
                if (mNcAsmMode.submitted == v2::t1::NcAsmMode::NC)
                    payload.asmValue = 0;
                SendCommandACK(t1::SetNcAsmParamNcAsmParam, payload);
            }
            mNcAsmEnabled.commit();
            mNcAsmMode.commit();
            mNcAsmFocusOnVoice.commit();
            mNcAsmAmbientLevel.commit();
        }

        if (mPlayVolume.submittedDirty())
        {
            if (mSupport.contains(F::PLAYBACK_CONTROLLER))
            {
                SendCommandACK(t1::SetPlayParamPlaybackControllerVolumeData, {
                    .volumeValue = static_cast<UInt8>(std::clamp(mPlayVolume.submitted, 0, 30))
                });
            }
            mPlayVolume.commit();
        }
        if (mPlayControl.submittedDirty())
        {
            if (mSupport.contains(F::PLAYBACK_CONTROLLER))
            {
                SendCommandACK(t1::SetPlayStatus, {
                    .type = t1::PlayInquiredType::PLAYBACK_CONTROLLER,
                    .control = ToV1(mPlayControl.submitted)
                });
            }
            mPlayControl.commitOneShot(v2::t1::PlaybackControl::KEY_OFF);
        }

        if (mEqPresetId.submittedDirty() || mEqConfig.submittedDirty() || mEqClearBass.submittedDirty())
        {
            if (mSupport.contains(F::PRESET_EQ))
            {
                t1::SetEqEbbParamEqParam payload;
                payload.presetId = ToV1(mEqPresetId.submitted);
                payload.bandSteps.value.push_back(
                    static_cast<UInt8>(std::clamp(mEqClearBass.submitted, -10, 10) + 10));
                for (const int band : mEqConfig.submitted)
                    payload.bandSteps.value.push_back(static_cast<UInt8>(std::clamp(band, -10, 10) + 10));
                SendCommandACK(t1::SetEqEbbParamEqParam, payload);
            }
            mEqPresetId.commit();
            mEqConfig.commit();
            mEqClearBass.commit();
        }

        if (mUpscalingEnabled.submittedDirty())
        {
            if (mSupport.contains(F::UPSCALING))
            {
                SendCommandACK(t1::SetAudioParamUpscalingParam, {
                    .settingValue = mUpscalingEnabled.submitted
                        ? t1::UpscalingSettingValue::AUTO
                        : t1::UpscalingSettingValue::OFF
                });
            }
            mUpscalingEnabled.commit();
        }

        if (mAudioPriorityMode.submittedDirty())
        {
            if (mSupport.contains(F::CONNECTION_MODE))
            {
                SendCommandACK(t1::SetAudioParamConnectionModeParam, {
                    .settingValue =
                        mAudioPriorityMode.submitted == v2::t1::PriorMode::CONNECTION_QUALITY_PRIOR
                            ? t1::ConnectionModeSettingValue::CONNECTION_QUALITY_PRIOR
                            : t1::ConnectionModeSettingValue::SOUND_QUALITY_PRIOR
                });
            }
            mAudioPriorityMode.commit();
        }

        if (mPowerAutoOff.submittedDirty())
        {
            if (mSupport.contains(F::AUTO_POWER_OFF))
            {
                const auto value = ToV1(mPowerAutoOff.submitted);
                SendCommandACK(t1::SetSystemExParamAutoPowerOffParam, {
                    .activeElementId = value,
                    .selectTimeElementId = value
                });
            }
            mPowerAutoOff.commit();
        }

        if (mAutoPauseEnabled.submittedDirty())
        {
            if (mSupport.contains(F::CONTROL_BY_WEARING))
            {
                SendCommandACK(t1::SetSystemExParamControlByWearingParam, {
                    .settingValue = mAutoPauseEnabled.submitted
                        ? t1::ControlByWearingSettingValue::ON
                        : t1::ControlByWearingSettingValue::OFF
                });
            }
            mAutoPauseEnabled.commit();
        }

        if (mTouchFunctionLeft.submittedDirty() || mTouchFunctionRight.submittedDirty())
        {
            if (mSupport.contains(F::ASSIGNABLE_SETTINGS))
            {
                t1::SetSystemParamAssignableSettingsParam payload;
                payload.presets.value = {
                    ToV1(mTouchFunctionLeft.submitted),
                    ToV1(mTouchFunctionRight.submitted)
                };
                SendCommandACK(t1::SetSystemParamAssignableSettingsParam, payload);
            }
            mTouchFunctionLeft.commit();
            mTouchFunctionRight.commit();
        }

        if (mSpeakToChatEnabled.submittedDirty())
        {
            if (mSupport.contains(F::SMART_TALKING_MODE))
            {
                SendCommandACK(t1::SetSystemParammartTalkingModeSetNtfyParam, {
                    .settingValue = mSpeakToChatEnabled.submitted
                        ? t1::SmartTalkingModeSettingValue::ON
                        : t1::SmartTalkingModeSettingValue::OFF
                });
            }
            mSpeakToChatEnabled.commit();
        }
        if (mSpeakToChatDetectSensitivity.submittedDirty() || mSpeakToModeOutTime.submittedDirty())
        {
            if (mSupport.contains(F::SMART_TALKING_MODE))
            {
                const UInt8 payload[] = {
                    static_cast<UInt8>(t1::Command::SYSTEM_SET_EXTENDED_PARAM),
                    static_cast<UInt8>(t1::SystemInquiredType::SMART_TALKING_MODE),
                    0,
                    static_cast<UInt8>(ToV1(mSpeakToChatDetectSensitivity.submitted)),
                    static_cast<UInt8>(mV1SpeakToChatVoiceFocus),
                    static_cast<UInt8>(ToV1(mSpeakToModeOutTime.submitted))
                };
                SendCommandImpl(payload, MDRDataType::DATA_MDR, mSeqNumber);
                const int result = co_await Await(AWAIT_ACK);
                if (result != MDR_RESULT_OK)
                    co_return SetLastError(result, "Timeout waiting for V1 Speak-to-Chat options");
            }
            mSpeakToChatDetectSensitivity.commit();
            mSpeakToModeOutTime.commit();
        }

        if (mPairingMode.submittedDirty())
        {
            if (mSupport.contains(F::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
            {
                SendCommandACK(t2::SetPeripheralStatus, {
                    .inquiredType = t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT,
                    .bluetoothModeStatus = mPairingMode.submitted
                        ? t2::PeripheralBluetoothModeStatus::INQUIRY_SCAN_MODE
                        : t2::PeripheralBluetoothModeStatus::NORMAL_MODE,
                    .status = CommonStatus::ENABLE
                });
            }
            mPairingMode.commit();
        }

        if (mVoiceGuidanceEnabled.submittedDirty())
        {
            if (mSupport.contains(F::VOICE_GUIDANCE))
            {
                SendCommandACK(t2::SetVoiceGuidanceParamSettingOnOff, {
                    .settingValue = mVoiceGuidanceEnabled.submitted
                        ? t2::VoiceGuidanceSettingValue::ON
                        : t2::VoiceGuidanceSettingValue::OFF
                });
            }
            mVoiceGuidanceEnabled.commit();
        }

        if (mPairedDeviceConnectMac.submittedDirty() ||
            mPairedDeviceDisconnectMac.submittedDirty() ||
            mPairedDeviceUnpairMac.submittedDirty())
        {
            co_return SetLastError(
                MDR_RESULT_ERROR_NOT_SUPPORTED,
                "V1 paired-device mutations require a capture-confirmed address layout");
        }

        co_return MDR_EVENT_APPLY_COMPLETE;
    }
}
