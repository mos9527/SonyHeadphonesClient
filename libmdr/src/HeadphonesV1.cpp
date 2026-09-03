#include <algorithm>
#include "Details.hpp"

namespace mdr
{
    using namespace v1;

    void MDRHeadphones::RefreshSupportV1()
    {
        auto& state = mDetailsV1;
        using F = t1::FunctionType;
        state.mProtocol.hasTable2 =
            state.mSupport.contains(F::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT) ||
            state.mSupport.contains(F::VOICE_GUIDANCE);
    }

    namespace
    {
        UInt8 V1NcValueForAmbientLevel(int level)
        {
            if (level <= 0)
                return static_cast<UInt8>(t1::NcDualSingleValue::DUAL);
            if (level == 1)
                return static_cast<UInt8>(t1::NcDualSingleValue::SINGLE);
            return static_cast<UInt8>(t1::NcDualSingleValue::OFF);
        }       
    }

    MDRTask MDRHeadphones::RequestInitV1()
    {
        auto& state = mDetailsV1;
        if (mProtocolFamily != ProtocolFamily::V1)
            co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "Device does not use MDR V1");

        state.mProtocol = {};
        SendCommandACK(t1::GetProtocolInfo);
        const int protocolResult = co_await Await(AWAIT_PROTOCOL_INFO);
        if (protocolResult != MDR_RESULT_OK)
            co_return SetLastError(protocolResult, "Unable to initialize MDR V1");

        if (!state.mProtocol.hasTable1)
            co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "Device doesn't support MDR V1 Table 1");

        SendCommandACK(t1::GetCapabilityInfo);
        SendCommandACK(t1::GetDeviceInfo, {.inquiredType = t1::DeviceInfoInquiredType::MODEL_NAME});
        SendCommandACK(t1::GetDeviceInfo, {.inquiredType = t1::DeviceInfoInquiredType::FW_VERSION});
        SendCommandACK(t1::GetDeviceInfo, {.inquiredType = t1::DeviceInfoInquiredType::SERIES_AND_COLOR_INFO});

        SendCommandACK(t1::GetSupportFunction);
        const int supportResult = co_await Await(AWAIT_SUPPORT_FUNCTION);
        if (supportResult != MDR_RESULT_OK)
            co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "Device failed to respond to support function request");

        using F = t1::FunctionType;
        if (state.mSupport.contains(F::CODEC_INDICATOR))
            SendCommandACK(t1::GetAudioCodec);
        if (state.mSupport.contains(F::PLAYBACK_CONTROLLER))
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
        if (state.mSupport.contains(F::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE))
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
            if (state.mSupport.contains(F::NOISE_CANCELLING))
                SendCommandACK(t1::GetNcAsmParam, {.type = t1::NcAsmInquiredType::NOISE_CANCELLING});
            if (state.mSupport.contains(F::AMBIENT_SOUND_MODE))
                SendCommandACK(t1::GetNcAsmParam, {.type = t1::NcAsmInquiredType::AMBIENT_SOUND_MODE});
        }
        if (state.mSupport.contains(F::PRESET_EQ))
        {
            SendCommandACK(t1::GetEqEbbCapability, {
                .type = t1::EqEbbInquiredType::PRESET_EQ,
                .language = t1::DisplayLanguage::ENGLISH
            });
            SendCommandACK(t1::GetEqEbbParam, {.type = t1::EqEbbInquiredType::PRESET_EQ});
        }
        if (state.mSupport.contains(F::UPSCALING))
        {
            SendCommandACK(t1::GetAudioCapability, {.inquiredType = t1::AudioInquiredType::UPSCALING});
            SendCommandACK(t1::GetAudioParam, {.audioInquiredType = t1::AudioInquiredType::UPSCALING});
        }
        if (state.mSupport.contains(F::CONNECTION_MODE))
            SendCommandACK(t1::GetAudioParam, {.audioInquiredType = t1::AudioInquiredType::CONNECTION_MODE});
        if (state.mSupport.contains(F::GENERAL_SETTING1))
        {
            SendCommandACK(t1::GetGsCapability, {
                .type = t1::GsInquiredType::GENERAL_SETTING1,
                .displayLanguage = t1::DisplayLanguage::ENGLISH
            });
            SendCommandACK(t1::GetGsParam, {.type = t1::GsInquiredType::GENERAL_SETTING1});
        }
        if (state.mSupport.contains(F::GENERAL_SETTING2))
        {
            SendCommandACK(t1::GetGsCapability, {
                .type = t1::GsInquiredType::GENERAL_SETTING2,
                .displayLanguage = t1::DisplayLanguage::ENGLISH
            });
            SendCommandACK(t1::GetGsParam, {.type = t1::GsInquiredType::GENERAL_SETTING2});
        }
        if (state.mSupport.contains(F::GENERAL_SETTING3))
        {
            SendCommandACK(t1::GetGsCapability, {
                .type = t1::GsInquiredType::GENERAL_SETTING3,
                .displayLanguage = t1::DisplayLanguage::ENGLISH
            });
            SendCommandACK(t1::GetGsParam, {.type = t1::GsInquiredType::GENERAL_SETTING3});
        }
        if (state.mSupport.contains(F::ASSIGNABLE_SETTINGS))
            SendCommandACK(t1::GetSystemParam, {
                .systemInquiredType = t1::SystemInquiredType::ASSIGNABLE_SETTINGS
            });
        if (state.mSupport.contains(F::AUTO_POWER_OFF))
            SendCommandACK(t1::GetSystemParam, {
                .systemInquiredType = t1::SystemInquiredType::AUTO_POWER_OFF
            });
        if (state.mSupport.contains(F::CONTROL_BY_WEARING))
            SendCommandACK(t1::GetSystemParam, {
                .systemInquiredType = t1::SystemInquiredType::CONTROL_BY_WEARING
            });
        if (state.mSupport.contains(F::SMART_TALKING_MODE))
        {
            SendCommandACK(t1::GetSystemParam, {
                .systemInquiredType = t1::SystemInquiredType::SMART_TALKING_MODE
            });
            SendCommandACK(t1::GetSystemExParam, {
                .systemInquiredType = t1::SystemInquiredType::SMART_TALKING_MODE
            });
        }
        if (state.mProtocol.hasTable2)
        {
            if (state.mSupport.contains(F::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
            {
                SendCommandACK(t2::GetPeripheralStatus, {
                    .inquiredType = t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
                });
                SendCommandACK(t2::GetPeripheralParam, {
                    .inquiredType = t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
                });
            }
            if (state.mSupport.contains(F::VOICE_GUIDANCE))
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
        mInitialized = true;
        co_return MDR_EVENT_INITIALIZE_COMPLETE;
    }

    MDRTask MDRHeadphones::RequestSyncV1()
    {
        auto& state = mDetailsV1;
        if (state.mSupport.contains(t1::FunctionType::BATTERY_LEVEL))
            SendCommandACK(t1::GetBatteryLevel, {.batteryInquiredType = t1::BatteryInquiredType::BATTERY});
        co_return MDR_EVENT_SYNC_COMPLETE;
    }

    void MDRHeadphones::SnapshotPropertiesV1()
    {
        auto& state = mDetailsV1;
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

    MDRTask MDRHeadphones::RequestCommitV1()
    {
        auto& state = mDetailsV1;
        SnapshotPropertiesV1();
        using F = t1::FunctionType;

        if (state.mShutdown.pending())
        {
            if (state.mShutdown.submitted && state.mSupport.contains(F::POWER_OFF))
                SendCommandACK(t1::SetPowerOff);
            state.mShutdown.override(false);
        }

        if (state.mNcAsmEnabled.pending() || state.mNcAsmMode.pending() ||
            state.mNcAsmFocusOnVoice.pending() || state.mNcAsmAmbientLevel.pending())
        {
            if (state.mSupport.contains(F::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE))
            {
                t1::SetNcAsmParamNcAsmParam payload;
                payload.ncAsmEffect =
                    state.mNcAsmEnabled.submitted ? t1::NcAsmEffect::ON : t1::NcAsmEffect::OFF;
                payload.ncType = t1::NcAsmSettingType::LEVEL_ADJUSTMENT;
                payload.ncValue = V1NcValueForAmbientLevel(state.mNcAsmAmbientLevel.submitted);
                payload.asmType = t1::AsmSettingType::LEVEL_ADJUSTMENT;
                payload.asmId = state.mNcAsmFocusOnVoice.submitted ? t1::AsmId::VOICE : t1::AsmId::NORMAL;
                payload.asmValue = static_cast<UInt8>(std::clamp(state.mNcAsmAmbientLevel.submitted, 0, 20));
                if (state.mNcAsmMode.submitted == 0)
                    payload.asmValue = 0;
                SendCommandACK(t1::SetNcAsmParamNcAsmParam, payload);
            }
            state.mNcAsmEnabled.commit();
            state.mNcAsmMode.commit();
            state.mNcAsmFocusOnVoice.commit();
            state.mNcAsmAmbientLevel.commit();
        }

        if (state.mPlayVolume.pending())
        {
            if (state.mSupport.contains(F::PLAYBACK_CONTROLLER))
            {
                SendCommandACK(t1::SetPlayParamPlaybackControllerVolumeData, {
                    .volumeValue = static_cast<UInt8>(std::clamp(state.mPlayVolume.submitted, 0, 30))
                });
            }
            state.mPlayVolume.commit();
        }
        if (state.mPlayControl.pending())
        {
            if (state.mSupport.contains(F::PLAYBACK_CONTROLLER))
            {
                SendCommandACK(t1::SetPlayStatus, {
                    .type = t1::PlayInquiredType::PLAYBACK_CONTROLLER,
                    .control = state.mPlayControl.submitted
                });
            }
            state.mPlayControl.override(t1::PlaybackControl::KEY_OFF);
        }

        if (state.mEqPresetId.pending() || state.mEqConfig.pending() || state.mEqClearBass.pending())
        {
            if (state.mSupport.contains(F::PRESET_EQ))
            {
                t1::SetEqEbbParamEqParam payload;
                payload.presetId = state.mEqPresetId.submitted;
                payload.bandSteps.value.push_back(
                    static_cast<UInt8>(std::clamp(state.mEqClearBass.submitted, -10, 10) + 10));
                for (const int band : state.mEqConfig.submitted)
                    payload.bandSteps.value.push_back(static_cast<UInt8>(std::clamp(band, -10, 10) + 10));
                SendCommandACK(t1::SetEqEbbParamEqParam, payload);
            }
            state.mEqPresetId.commit();
            state.mEqConfig.commit();
            state.mEqClearBass.commit();
        }

        if (state.mUpscalingEnabled.pending())
        {
            if (state.mSupport.contains(F::UPSCALING))
            {
                SendCommandACK(t1::SetAudioParamUpscalingParam, {
                    .settingValue = state.mUpscalingEnabled.submitted
                        ? t1::UpscalingSettingValue::AUTO
                        : t1::UpscalingSettingValue::OFF
                });
            }
            state.mUpscalingEnabled.commit();
        }

        if (state.mAudioPriorityMode.pending())
        {
            if (state.mSupport.contains(F::CONNECTION_MODE))
            {
                SendCommandACK(t1::SetAudioParamConnectionModeParam, {
                    .settingValue = state.mAudioPriorityMode.submitted
                });
            }
            state.mAudioPriorityMode.commit();
        }

        if (state.mPowerAutoOff.pending())
        {
            if (state.mSupport.contains(F::AUTO_POWER_OFF))
            {
                SendCommandACK(t1::SetSystemExParamAutoPowerOffParam, {
                    .activeElementId = state.mPowerAutoOff.submitted,
                    .selectTimeElementId = state.mPowerAutoOff.submitted
                });
            }
            state.mPowerAutoOff.commit();
        }

        if (state.mAutoPauseEnabled.pending())
        {
            if (state.mSupport.contains(F::CONTROL_BY_WEARING))
            {
                SendCommandACK(t1::SetSystemExParamControlByWearingParam, {
                    .settingValue = state.mAutoPauseEnabled.submitted
                        ? t1::ControlByWearingSettingValue::ON
                        : t1::ControlByWearingSettingValue::OFF
                });
            }
            state.mAutoPauseEnabled.commit();
        }

        if (state.mTouchFunctionLeft.pending() || state.mTouchFunctionRight.pending())
        {
            if (state.mSupport.contains(F::ASSIGNABLE_SETTINGS))
            {
                t1::SetSystemParamAssignableSettingsParam payload;
                payload.presets.value = {
                    state.mTouchFunctionLeft.submitted,
                    state.mTouchFunctionRight.submitted
                };
                SendCommandACK(t1::SetSystemParamAssignableSettingsParam, payload);
            }
            state.mTouchFunctionLeft.commit();
            state.mTouchFunctionRight.commit();
        }

        if (state.mSpeakToChatEnabled.pending())
        {
            if (state.mSupport.contains(F::SMART_TALKING_MODE))
            {
                SendCommandACK(t1::SetSystemParammartTalkingModeSetNtfyParam, {
                    .settingValue = state.mSpeakToChatEnabled.submitted
                        ? t1::SmartTalkingModeSettingValue::ON
                        : t1::SmartTalkingModeSettingValue::OFF
                });
            }
            state.mSpeakToChatEnabled.commit();
        }
        if (state.mSpeakToChatDetectSensitivity.pending() || state.mSpeakToModeOutTime.pending())
        {
            if (state.mSupport.contains(F::SMART_TALKING_MODE))
            {
                const UInt8 payload[] = {
                    static_cast<UInt8>(t1::Command::SYSTEM_SET_EXTENDED_PARAM),
                    static_cast<UInt8>(t1::SystemInquiredType::SMART_TALKING_MODE),
                    0,
                    static_cast<UInt8>(state.mSpeakToChatDetectSensitivity.submitted),
                    static_cast<UInt8>(state.mSpeakToChatVoiceFocus),
                    static_cast<UInt8>(state.mSpeakToModeOutTime.submitted)
                };
                SendCommandImpl(payload, MDRDataType::DATA_MDR, mSeqNumber);
                const int result = co_await Await(AWAIT_ACK);
                if (result != MDR_RESULT_OK)
                    co_return SetLastError(result, "Timeout waiting for V1 Speak-to-Chat options");
            }
            state.mSpeakToChatDetectSensitivity.commit();
            state.mSpeakToModeOutTime.commit();
        }

        if (state.mPairingMode.pending())
        {
            if (state.mSupport.contains(F::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
            {
                SendCommandACK(t2::SetPeripheralStatus, {
                    .inquiredType = t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT,
                    .bluetoothModeStatus = state.mPairingMode.submitted
                        ? t2::PeripheralBluetoothModeStatus::INQUIRY_SCAN_MODE
                        : t2::PeripheralBluetoothModeStatus::NORMAL_MODE,
                    .status = CommonStatus::ENABLE
                });
            }
            state.mPairingMode.commit();
        }

        if (state.mVoiceGuidanceEnabled.pending())
        {
            if (state.mSupport.contains(F::VOICE_GUIDANCE))
            {
                SendCommandACK(t2::SetVoiceGuidanceParamSettingOnOff, {
                    .settingValue = state.mVoiceGuidanceEnabled.submitted
                        ? t2::VoiceGuidanceSettingValue::ON
                        : t2::VoiceGuidanceSettingValue::OFF
                });
            }
            state.mVoiceGuidanceEnabled.commit();
        }

        if (state.mPairedDeviceConnectMac.pending() ||
            state.mPairedDeviceDisconnectMac.pending() ||
            state.mPairedDeviceUnpairMac.pending())
        {
            co_return SetLastError(
                MDR_RESULT_ERROR_NOT_SUPPORTED,
                "V1 paired-device mutations require a capture-confirmed address layout");
        }

        co_return MDR_EVENT_APPLY_COMPLETE;
    }

    int MDRHeadphones::HandleProtocolInfoV1(Span<const UInt8> command)
    {
        auto& state = mDetailsV1;
        if (command.size() != sizeof(t1::RetProtocolInfo))
            return SetLastError(MDR_RESULT_ERROR_MALFORMED_PAYLOAD, "MDR V1 protocol info size is not valid");
        const auto result = (t1::RetProtocolInfo::Deserialize)(command.data(), command.size());
        if (!result)
            return SetLastError(
                result.error,
                result.errMessage ? result.errMessage : "Unable to deserialize MDR V1 protocol info");
        state.mProtocol = {
            .version = result.value.protocolVersion,
            .hasTable1 = true,
            .hasTable2 = false
        };
        Awake(AWAIT_PROTOCOL_INFO);
        return MDR_EVENT_IDENTITY_CHANGED;
    }

    bool MDRHeadphones::IsDirtyV1() const
    {
        const auto& state = mDetailsV1;
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

}
