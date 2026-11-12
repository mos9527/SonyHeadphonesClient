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
            std::fill(self->mDetailsV1.mSupport.functions.begin(), self->mDetailsV1.mSupport.functions.end(), false);
            for (const FunctionType function : result.supportFunctions)
                self->mDetailsV1.mSupport.functions[static_cast<UInt8>(function)] = true;
            self->mDetailsV1.mSupport.provenance = DetailsV1::SupportStates::state::ADVERTISED;
            self->mDetailsV1.mProtocol.hasTable2 =
                self->mDetailsV1.mSupport.contains(t1::FunctionType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT) ||
                self->mDetailsV1.mSupport.contains(t1::FunctionType::VOICE_GUIDANCE);
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
                return self->Sync(MDR_HEADPHONES_SYNC_BATTERY, MDR_EVENT_BATTERY_CHANGED);
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
                detailedDataType == PlaybackDetailedDataType::ARTIST_NAME ||
                detailedDataType == PlaybackDetailedDataType::GENRE_NAME)
            {
                String value;
                if (command == Command::PLAY_NTFY_PARAM)
                {
                    // V1 NTFY payloads doesn't contain the actual NameData...
                    // Need a GET to get the actual ones, have the caller to sync.
                    return self->Desync(MDR_HEADPHONES_SYNC_PLAYBACK);
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
                else if (detailedDataType == PlaybackDetailedDataType::ARTIST_NAME)
                    self->mDetailsV1.mPlayTrackArtist = std::move(value);
                else
                    self->mDetailsV1.mPlayTrackGenre = std::move(value);
                return self->Sync(MDR_HEADPHONES_SYNC_PLAYBACK, MDR_EVENT_PLAYBACK_CHANGED);
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

        // ncValue reads per ncType: DUAL_SINGLE_OFF is NC / Wind Noise Reduction / off; ON_OFF is NC on / off.
        NcDualSingleValue NormalizeNcValue(NcAsmSettingType type, UInt8 value)
        {
            switch (type)
            {
            case NcAsmSettingType::DUAL_SINGLE_OFF:
                if (value == static_cast<UInt8>(NcDualSingleValue::DUAL))
                    return NcDualSingleValue::DUAL;
                if (value == static_cast<UInt8>(NcDualSingleValue::SINGLE))
                    return NcDualSingleValue::SINGLE;
                return NcDualSingleValue::OFF;
            default:
                return value != 0 ? NcDualSingleValue::DUAL : NcDualSingleValue::OFF;
            }
        }

        template <typename T>
        void ApplyNcAsm(MDRHeadphones* self, const T& result)
        {
            self->mDetailsV1.mNcAsmEnabled.overwrite(result.ncAsmEffect == NcAsmEffect::ON);
            self->mDetailsV1.mNcAsmFocusOnVoice.overwrite(result.asmId == AsmId::VOICE);
            self->mDetailsV1.mNcAsmNcValue.overwrite(NormalizeNcValue(result.ncType, result.ncValue));
            self->mDetailsV1.mNcAsmAmbientLevel.overwrite(result.asmValue);
        }

        int HandleNcAsmCapability(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            NcAsmInquiredType type{};
            if (!detail::ReadEnumTag(cmd, type))
                return MDR_EVENT_UNHANDLED;
            const auto readAsmStep = [&](const auto& list)
            {
                for (const auto& entry : list)
                    if (entry.id == AsmId::NORMAL)
                        self->mDetailsV1.mAsmStep = entry.step;
            };
            switch (type)
            {
            case NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE:
            {
                // e.g. WH-1000XM3: DUAL_SINGLE_OFF, ambient {NORMAL, VOICE} with 20 steps each
                Deserialize(RetNcAsmCapability_NcAsmCapability, res, cmd);
                self->mDetailsV1.mNcSettingType = res.ncSettingType;
                readAsmStep(res.asmList.value);
                return MDR_EVENT_NOISE_CONTROL_CHANGED;
            }
            case NcAsmInquiredType::AMBIENT_SOUND_MODE:
            {
                Deserialize(RetNcAsmCapability_AsmCapability, res, cmd);
                readAsmStep(res.asmList.value);
                return MDR_EVENT_NOISE_CONTROL_CHANGED;
            }
            default:
                return MDR_EVENT_UNHANDLED;
            }
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
                    self->mDetailsV1.mNcAsmNcValue.overwrite(NcDualSingleValue::OFF);
                    self->mDetailsV1.mNcAsmAmbientLevel.overwrite(result.asmValue);
                }
                else
                {
                    Deserialize(RetNcAsmParamAsmParam, result, cmd);
                    self->mDetailsV1.mNcAsmEnabled.overwrite(result.ncAsmEffect == NcAsmEffect::ON);
                    self->mDetailsV1.mNcAsmFocusOnVoice.overwrite(result.asmId == AsmId::VOICE);
                    self->mDetailsV1.mNcAsmNcValue.overwrite(NcDualSingleValue::OFF);
                    self->mDetailsV1.mNcAsmAmbientLevel.overwrite(result.asmValue);
                }
                return MDR_EVENT_NOISE_CONTROL_CHANGED;
            }
            return MDR_EVENT_UNHANDLED;
        }

        int HandleEqCapability(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            EqEbbInquiredType type{};
            if (!detail::ReadEnumTag(cmd, type) ||
                (type != EqEbbInquiredType::PRESET_EQ && type != EqEbbInquiredType::PRESET_EQ_NONCUSTOMIZABLE))
                return MDR_EVENT_UNHANDLED;
            Deserialize(RetEqEbbCapability_EqCapability, res, cmd);
            self->mDetailsV1.mEqCapabilityBands = res.band;
            self->mDetailsV1.mEqCapabilityStep = res.step;
            self->mDetailsV1.mEqPresets.clear();
            for (const auto& preset : res.presetList)
                self->mDetailsV1.mEqPresets.push_back({preset.presetId, preset.name.value});
            return MDR_EVENT_EQUALIZER_CHANGED;
        }

        int HandleEqExtendedInfo(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            EqEbbInquiredType type{};
            if (!detail::ReadEnumTag(cmd, type) || type != EqEbbInquiredType::PRESET_EQ)
                return MDR_EVENT_UNHANDLED;
            // e.g. WH-1000XM3: {SPECIFIC_INFORMATION 1 (Clear Bass), 400, 1000, 2500, 6300, 16000 Hz}
            Deserialize(RetEqEbbExtendedInfoEqExtendedInfo, res, cmd);
            self->mDetailsV1.mEqBandInfo = std::move(res.bandInfos.value);
            return MDR_EVENT_EQUALIZER_CHANGED;
        }

        int HandlePlayCapability(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            Deserialize(RetPlayCapability, res, cmd);
            // e.g. WH-1000XM3: volumeStep 31, i.e. 0..30
            self->mDetailsV1.mPlayVolumeStep = res.volumeStep;
            return MDR_EVENT_PLAYBACK_CHANGED;
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
                    const int offset = self->mDetailsV1.EqStepOffset();
                    self->mDetailsV1.mEqClearBass.overwrite(static_cast<int>(result.bandSteps.value[0]) - offset);
                    Vector<int> bands;
                    for (size_t i = 1; i < result.bandSteps.size(); ++i)
                        bands.push_back(static_cast<int>(result.bandSteps.value[i]) - offset);
                    self->mDetailsV1.mEqConfig.overwrite(std::move(bands));
                }
            }
            else
            {
                Deserialize(RetEqEbbParamEqParam, result, cmd);
                self->mDetailsV1.mEqPresetId.overwrite(result.presetId);
                if (!result.bandSteps.value.empty())
                {
                    const int offset = self->mDetailsV1.EqStepOffset();
                    self->mDetailsV1.mEqClearBass.overwrite(static_cast<int>(result.bandSteps.value[0]) - offset);
                    Vector<int> bands;
                    for (size_t i = 1; i < result.bandSteps.size(); ++i)
                        bands.push_back(static_cast<int>(result.bandSteps.value[i]) - offset);
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

        int GsSlot(GsInquiredType type)
        {
            switch (type)
            {
            case GsInquiredType::GENERAL_SETTING1:
                return 0;
            case GsInquiredType::GENERAL_SETTING2:
                return 1;
            case GsInquiredType::GENERAL_SETTING3:
                return 2;
            default:
                return -1;
            }
        }

        int HandleGsCapability(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            Deserialize(RetGsCapabilityGsSettingInfo, res, cmd);
            const int slot = GsSlot(res.type);
            if (slot < 0)
                return MDR_EVENT_UNHANDLED;
            auto& capability = self->mDetailsV1.mGsCapability[slot];
            capability = {res.settingType, res.title, {}};
            if (res.settingType == GsSettingType::LIST_TYPE)
            {
                // e.g. WH-1000XM3 GENERAL_SETTING1: ASSIGNABLE_KEY_SETTING {NCASM, GOOGLE_ASSISTANT, AMAZON_ALEXA}
                Deserialize(RetGsCapabilityGsCandidateElementList, list, cmd);
                capability.options = std::move(list.listTypeCapability.elements.value);
            }
            return MDR_EVENT_GENERAL_SETTINGS_CHANGED;
        }

        int HandleGsParam(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            GsInquiredType type{};
            GsSettingType settingType{};
            if (!detail::ReadEnumTag(cmd, type) || !detail::ReadEnumTag(cmd, settingType, 2))
                return MDR_EVENT_UNHANDLED;
            const int slot = GsSlot(type);
            if (slot < 0)
                return MDR_EVENT_UNHANDLED;
            const bool notify = static_cast<Command>(cmd[0]) == Command::GENERAL_SETTING_NTNY_PARAM;
            if (settingType == GsSettingType::LIST_TYPE)
            {
                if (notify)
                {
                    Deserialize(NotifyGsParamGsListTypeValue, result, cmd);
                    self->mDetailsV1.mGsParamList[slot].overwrite(result.currentElementIndex);
                }
                else
                {
                    Deserialize(RetGsParamGsListTypeValue, result, cmd);
                    self->mDetailsV1.mGsParamList[slot].overwrite(result.currentElementIndex);
                }
                return MDR_EVENT_GENERAL_SETTINGS_CHANGED;
            }
            if (notify)
            {
                Deserialize(NotifyGsParamGsBooleanTypeValue, result, cmd);
                self->mDetailsV1.mGsParamBool[slot].overwrite(result.settingValue == CommonOnOffSettingValue::ON);
            }
            else
            {
                Deserialize(RetGsParamGsBooleanTypeValue, result, cmd);
                self->mDetailsV1.mGsParamBool[slot].overwrite(result.settingValue == CommonOnOffSettingValue::ON);
            }
            return MDR_EVENT_GENERAL_SETTINGS_CHANGED;
        }

        int HandleSystemCapability(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            SystemInquiredType type{};
            if (!detail::ReadEnumTag(cmd, type))
                return MDR_EVENT_UNHANDLED;
            switch (type)
            {
            case SystemInquiredType::ASSIGNABLE_SETTINGS:
            {
                Deserialize(RetSystemCapability_AssignableSettingsCapability, result, cmd);
                self->mDetailsV1.mAssignableSettingsKeys = std::move(result.assignableSettingKeyList.elements.value);
                return MDR_EVENT_ASSIGNABLE_CONTROLS_CHANGED;
            }
            case SystemInquiredType::AUTO_POWER_OFF:
            {
                // e.g. WH-1000XM3: {5, 30, 60, 180 min, disabled}, no 15 min
                Deserialize(RetSystemCapability_AutoPowerOffCapability, result, cmd);
                self->mDetailsV1.mAutoPowerOffCandidates = std::move(result.candidateElements.value);
                return MDR_EVENT_POWER_CHANGED;
            }
            default:
                return MDR_EVENT_UNHANDLED;
            }
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
                    self->mDetailsV1.mAutoPauseEnabled.overwrite(result.settingValue ==
                                                                 ControlByWearingSettingValue::ON);
                }
                else
                {
                    Deserialize(RetSystemParamControlByWearingParam, result, cmd);
                    self->mDetailsV1.mAutoPauseEnabled.overwrite(result.settingValue ==
                                                                 ControlByWearingSettingValue::ON);
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
                    self->mDetailsV1.mAssignableSettingsPresets.overwrite(result.presets.value);
                }
                else
                {
                    Deserialize(RetSystemParamAssignableSettingsParam, result, cmd);
                    self->mDetailsV1.mAssignableSettingsPresets.overwrite(result.presets.value);
                }
                return MDR_EVENT_ASSIGNABLE_CONTROLS_CHANGED;
            case SystemInquiredType::SMART_TALKING_MODE:
                if (cmd.size() == sizeof(RetSystemParamSmartTalkingModeRetParam))
                {
                    if (notify)
                    {
                        Deserialize(NotifySystemParamSmartTalkingModeSetNtfyParam, result, cmd);
                        self->mDetailsV1.mSpeakToChatEnabled.overwrite(result.settingValue ==
                                                                       SmartTalkingModeSettingValue::ON);
                    }
                    else
                    {
                        Deserialize(RetSystemParamSmartTalkingModeRetParam, result, cmd);
                        self->mDetailsV1.mSpeakToChatEnabled.overwrite(result.settingValue ==
                                                                       SmartTalkingModeSettingValue::ON);
                    }
                }
                else
                {
                    if (notify)
                    {
                        Deserialize(NotifySystemExParamChildPayloadSmartTalkingModeExParamType1Param, result, cmd);
                        self->mDetailsV1.mSpeakToChatDetectSensitivity.overwrite(result.devectionSensitivity);
                        self->mDetailsV1.mSpeakToChatVoiceFocus = result.voiceFocus;
                        self->mDetailsV1.mSpeakToModeOutTime.overwrite(result.modeOutTime);
                    }
                    else
                    {
                        Deserialize(RetSystemExParamChildPayloadSmartTalkingModeExParamType1Param, result, cmd);
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

        int HandleAlertParam(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            AlertInquiredType type{};
            if (!detail::ReadEnumTag(cmd, type))
                return MDR_EVENT_UNHANDLED;
            using enum AlertInquiredType;
            switch (type)
            {
            case FIXED_MESSAGE:
            {
                Deserialize(NotifyAlertParam, res, cmd);
                using enum AlertActionType;
                switch (res.actionType)
                {
                case POSITIVE_NEGATIVE:
                {
                    self->mDetailsV1.mLastAlertMessage = res.messageType;
                    ++self->mDetailsV1.mAlertSerial;
                    self->mDetailsV1.mAlertAwaitingResponse = true;
                    return MDR_EVENT_ALERT;
                }
                default:
                    break;
                }
                return MDR_EVENT_UNHANDLED;
            }
            default:
                break;
            }
            return MDR_EVENT_UNHANDLED;
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
        case Command::PLAY_RET_CAPABILITY:
            return HandlePlayCapability(self, cmd);
        case Command::PLAY_RET_STATUS:
        case Command::PLAY_NTFY_STATUS:
            return HandlePlayStatus(self, cmd);
        case Command::NCASM_RET_CAPABILITY:
            return HandleNcAsmCapability(self, cmd);
        case Command::NCASM_RET_PARAM:
        case Command::NCASM_NTFY_PARAM:
            return HandleNcAsm(self, cmd);
        case Command::EQEBB_RET_CAPABILITY:
            return HandleEqCapability(self, cmd);
        case Command::EQEBB_RET_EXTENDED_INFO:
            return HandleEqExtendedInfo(self, cmd);
        case Command::EQEBB_RET_PARAM:
        case Command::EQEBB_NTFY_PARAM:
            return HandleEq(self, cmd);
        case Command::AUDIO_RET_PARAM:
        case Command::AUDIO_NTFY_PARAM:
            return HandleAudioParam(self, cmd);
        case Command::GENERAL_SETTING_RET_CAPABILITY:
            return HandleGsCapability(self, cmd);
        case Command::GENERAL_SETTING_RET_PARAM:
        case Command::GENERAL_SETTING_NTNY_PARAM:
            return HandleGsParam(self, cmd);
        case Command::SYSTEM_RET_CAPABILITY:
            return HandleSystemCapability(self, cmd);
        case Command::SYSTEM_RET_PARAM:
        case Command::SYSTEM_NTFY_PARAM:
            return HandleSystemParam(self, cmd);
        case Command::ALERT_NTFY_PARAM:
            return HandleAlertParam(self, cmd);
        default:
            MDR_LOG_DEBUG("** Unhandled V1 T1 {}", static_cast<Command>(cmd[0]));
            return MDR_EVENT_UNHANDLED;
        }
    }
}
