#include <algorithm>
#include "Details.hpp"

namespace mdr
{
    using namespace v1;

    void MDRHeadphones::RefreshSupportV1()
    {
        auto& state = mDetailsV1;
        state.mProtocol.hasTable2 =
            state.mSupport.contains(t1::FunctionType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT) ||
            state.mSupport.contains(t1::FunctionType::VOICE_GUIDANCE);
    }

    namespace
    {
        UInt8 V1NcValueForAmbientLevel(int level)
        {
            if (level <= -1)
                return static_cast<UInt8>(t1::NcDualSingleValue::DUAL); // Noise Cancelling
            if (level == 0)
                return static_cast<UInt8>(t1::NcDualSingleValue::SINGLE); // Wind Noise Reduction
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
        SendCommandACK(t1::GetDeviceInfo, {.inquiredType = t1::DeviceInfoInquiredType::INSTRUCTION_GUIDE});

        // Following are cached by the official app based on the MAC address
        {
            /* Support Functions */
            SendCommandACK(t1::GetSupportFunction);
            const int supportResult = co_await Await(AWAIT_SUPPORT_FUNCTION);
            if (supportResult != MDR_RESULT_OK)
                co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "Device failed to respond to support function request");

            /* Equalizer */
            if (state.mSupport.contains(t1::FunctionType::PRESET_EQ))
            {
                SendCommandACK(t1::GetEqEbbCapability, {
                    .type = t1::EqEbbInquiredType::PRESET_EQ,
                    .language = t1::DisplayLanguage::ENGLISH
                });
            }

            /* NC/AMB */
            if (state.mSupport.contains(t1::FunctionType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE))
            {
                SendCommandACK(t1::GetNcAsmCapability, {
                    .type = t1::NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE
                });
            }
            else if (state.mSupport.contains(t1::FunctionType::NOISE_CANCELLING))
            {
                SendCommandACK(t1::GetNcAsmCapability, {.type = t1::NcAsmInquiredType::NOISE_CANCELLING});
            }
            else if (state.mSupport.contains(t1::FunctionType::AMBIENT_SOUND_MODE))
            {
                SendCommandACK(t1::GetNcAsmCapability, {.type = t1::NcAsmInquiredType::AMBIENT_SOUND_MODE});
            }

            /* Noise Canceling Optimizer */
            if (state.mSupport.contains(t1::FunctionType::NC_OPTIMIZER))
            {
                SendCommandACK(t1::GetOptimizerCapability, {
                    .optimizerInquiredType = t1::OptimizerInquiredType::NC_OPTIMIZER
                });
            }

            /* Playback */
            if (state.mSupport.contains(t1::FunctionType::PLAYBACK_CONTROLLER))
            {
                SendCommandACK(t1::GetPlayCapability, {.type = t1::PlayInquiredType::PLAYBACK_CONTROLLER});
            }

            /* Sound Quality Mode */
            if (state.mSupport.contains(t1::FunctionType::CONNECTION_MODE))
            {
                SendCommandACK(t1::GetAudioCapability, {.inquiredType = t1::AudioInquiredType::CONNECTION_MODE});
            }

            /* DSEE */
            if (state.mSupport.contains(t1::FunctionType::UPSCALING))
            {
                SendCommandACK(t1::GetAudioCapability, {.inquiredType = t1::AudioInquiredType::UPSCALING});
            }

            /* Pause when headphones are removed */
            if (state.mSupport.contains(t1::FunctionType::CONTROL_BY_WEARING))
            {
                SendCommandACK(t1::GetSystemCapability, {.inquiredType = t1::SystemInquiredType::CONTROL_BY_WEARING});
            }

            /* Auto Power Off */
            if (state.mSupport.contains(t1::FunctionType::AUTO_POWER_OFF))
            {
                SendCommandACK(t1::GetSystemCapability, {.inquiredType = t1::SystemInquiredType::AUTO_POWER_OFF});
            }

            /* Speak-to-Chat */
            if (state.mSupport.contains(t1::FunctionType::SMART_TALKING_MODE))
            {
                SendCommandACK(t1::GetSystemCapability, {.inquiredType = t1::SystemInquiredType::SMART_TALKING_MODE});
            }

            /* WH: CUSTOM Button; WF: Touch Sensor */
            if (state.mSupport.contains(t1::FunctionType::ASSIGNABLE_SETTINGS))
            {
                SendCommandACK(t1::GetSystemCapability, {.inquiredType = t1::SystemInquiredType::ASSIGNABLE_SETTINGS});
            }

            /* General Settings */
            if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING1))
            {
                SendCommandACK(t1::GetGsCapability, {
                    .type = t1::GsInquiredType::GENERAL_SETTING1,
                    .displayLanguage = t1::DisplayLanguage::ENGLISH
                });
            }
            if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING2))
            {
                SendCommandACK(t1::GetGsCapability, {
                    .type = t1::GsInquiredType::GENERAL_SETTING2,
                    .displayLanguage = t1::DisplayLanguage::ENGLISH
                });
            }
            if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING3))
            {
                SendCommandACK(t1::GetGsCapability, {
                    .type = t1::GsInquiredType::GENERAL_SETTING3,
                    .displayLanguage = t1::DisplayLanguage::ENGLISH
                });
            }

            if (state.mSupport.contains(t1::FunctionType::BLE_SETUP))
            {
                SendCommandACK(t1::GetBluetoothDeviceInfo, {
                    .type = t1::BluetoothDeviceInfoType::BLE_HASH_VALUE
                });
                SendCommandACK(t1::GetBluetoothDeviceInfo, {
                    .type = t1::BluetoothDeviceInfoType::BLUETOOTH_DEVICE_ADDRESS
                });
            }

            if (state.mProtocol.hasTable2)
            {
                /* Pairing Management */
                if (state.mSupport.contains(t1::FunctionType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
                {
                    SendCommandACK(t2::GetPeripheralCapability, {
                        .inquiredType = t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
                    });
                }

                /* Voice Guidance */
                if (state.mSupport.contains(t1::FunctionType::VOICE_GUIDANCE))
                {
                    SendCommandACK(t2::GetVoiceGuidanceCapability, {
                        .inquiredType = t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
                    });
                    SendCommandACK(t2::GetVoiceGuidanceParam, {
                        .inquiredType = t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING,
                        .detailedDataType = t2::DetailedDataType::UPDATE_METHOD
                    });
                }
            }
        }

        /* Firmware Update */
        if (state.mSupport.contains(t1::FunctionType::FW_UPDATE))
        {
            SendCommandACK(t1::GetUpdateParam, {
                .command = t1::Command::UPDT_GET_PARAM,
                .updateInquiredType = t1::UpdateInquiredType::BLE_TX_POWER
            });
            SendCommandACK(t1::GetUpdateParam, {
                .command = t1::Command::UPDT_GET_PARAM,
                .updateInquiredType = t1::UpdateInquiredType::BATTERY_POWER_THRESHOLD
            });
            SendCommandACK(t1::GetUpdateParam, {
                .command = t1::Command::UPDT_GET_PARAM,
                .updateInquiredType = t1::UpdateInquiredType::UPDATE_METHOD
            });
            SendCommandACK(t1::GetUpdateParam, {
                .command = t1::Command::UPDT_GET_PARAM,
                .updateInquiredType = t1::UpdateInquiredType::BATTERY_POWER_THRESHOLD_FOR_INTERRUPTIONG_FW_UPDATE
            });
            SendCommandACK(t1::GetUpdateParam, {
                .command = t1::Command::UPDT_GET_PARAM,
                .updateInquiredType = t1::UpdateInquiredType::UNIQUE_ID_FOR_DEVICE_BINDING
            });
            SendCommandACK(t1::GetUpdateParam, {
                .command = t1::Command::UPDT_GET_PARAM,
                .updateInquiredType = t1::UpdateInquiredType::CATEGORY_ID
            });
            SendCommandACK(t1::GetUpdateParam, {
                .command = t1::Command::UPDT_GET_PARAM,
                .updateInquiredType = t1::UpdateInquiredType::SERVICE_ID
            });
            SendCommandACK(t1::GetUpdateParam, {
                .command = t1::Command::UPDT_GET_PARAM,
                .updateInquiredType = t1::UpdateInquiredType::NATION_CODE
            });
            SendCommandACK(t1::GetUpdateParam, {
                .command = t1::Command::UPDT_GET_PARAM,
                .updateInquiredType = t1::UpdateInquiredType::LANGUAGE
            });
            SendCommandACK(t1::GetUpdateParam, {
                .command = t1::Command::UPDT_GET_PARAM,
                .updateInquiredType = t1::UpdateInquiredType::SERIAL_NUMBER
            });
        }

        if (state.mProtocol.hasTable2)
        {
            /* Pairing Management */
            if (state.mSupport.contains(t1::FunctionType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
            {
                /* Pairing Mode */
                SendCommandACK(t2::GetPeripheralStatus, {
                    .inquiredType = t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
                });

                /* Connected Devices */
                SendCommandACK(t2::GetPeripheralParam, {
                    .inquiredType = t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
                });
            }

            /* Voice Guidance */
            if (state.mSupport.contains(t1::FunctionType::VOICE_GUIDANCE))
            {
                SendCommandACK(t2::GetVoiceGuidanceStatus, {
                    .inquiredType = t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING,
                    .statusType = t2::StatusType::ON_OFF
                });
                SendCommandACK(t2::GetVoiceGuidanceParam, {
                    .inquiredType = t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING,
                    .detailedDataType = t2::DetailedDataType::ON_OFF
                });
                SendCommandACK(t2::GetVoiceGuidanceStatus, {
                    .inquiredType = t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING,
                    .statusType = t2::StatusType::LANGUAGE
                });
                SendCommandACK(t2::GetVoiceGuidanceParam, {
                    .inquiredType = t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING,
                    .detailedDataType = t2::DetailedDataType::LANGUAGE
                });
                SendCommandACK(t2::GetVoiceGuidanceParam, {
                    .inquiredType = t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING,
                    .detailedDataType = t2::DetailedDataType::REQUIRED_TIME
                });
                SendCommandACK(t2::GetVoiceGuidanceParam, {
                    .inquiredType = t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING,
                    .detailedDataType = t2::DetailedDataType::DOWNLOAD_SERVER_METHOD
                });
            }
        }

        /* Equalizer */
        if (state.mSupport.contains(t1::FunctionType::PRESET_EQ))
        {
            SendCommandACK(t1::GetEqEbbStatus, {.type = t1::EqEbbInquiredType::PRESET_EQ});
            SendCommandACK(t1::GetEqEbbParam, {.type = t1::EqEbbInquiredType::PRESET_EQ});
            SendCommandACK(t1::GetEqEbbExtendedInfo, {.type = t1::EqEbbInquiredType::PRESET_EQ});
        }

        /* NC/AMB */
        if (state.mSupport.contains(t1::FunctionType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE))
        {
            SendCommandACK(t1::GetNcAsmStatus, {
                .type = t1::NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE
            });
            SendCommandACK(t1::GetNcAsmParam, {
                .type = t1::NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE
            });
        }
        else if (state.mSupport.contains(t1::FunctionType::NOISE_CANCELLING))
        {
            SendCommandACK(t1::GetNcAsmStatus, {.type = t1::NcAsmInquiredType::NOISE_CANCELLING});
            SendCommandACK(t1::GetNcAsmParam, {.type = t1::NcAsmInquiredType::NOISE_CANCELLING});
        }
        else if (state.mSupport.contains(t1::FunctionType::AMBIENT_SOUND_MODE))
        {
            SendCommandACK(t1::GetNcAsmStatus, {.type = t1::NcAsmInquiredType::AMBIENT_SOUND_MODE});
            SendCommandACK(t1::GetNcAsmParam, {.type = t1::NcAsmInquiredType::AMBIENT_SOUND_MODE});
        }

        /* Noise Canceling Optimizer */
        if (state.mSupport.contains(t1::FunctionType::NC_OPTIMIZER))
        {
            SendCommandACK(t1::GetOptimizerStatus, {
                .command = t1::Command::OPT_GET_STATUS,
                .optimizerInquiredType = t1::OptimizerInquiredType::NC_OPTIMIZER
            });

            SendCommandACK(t1::GetOptimizerParam, {
                .command = t1::Command::OPT_GET_PARAM,
                .optimizerInquiredType = t1::OptimizerInquiredType::NC_OPTIMIZER
            });
        }

        /* Playback */
        if (state.mSupport.contains(t1::FunctionType::PLAYBACK_CONTROLLER))
        {
            /* Play/Pause */
            SendCommandACK(t1::GetPlayStatus, {.type = t1::PlayInquiredType::PLAYBACK_CONTROLLER});

            /* Playback Volume */
            SendCommandACK(t1::GetPlayParam, {
                .type = t1::PlayInquiredType::PLAYBACK_CONTROLLER,
                .dataType = t1::PlaybackDetailedDataType::VOLUME
            });

            /* Playback Metadata */
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
                .dataType = t1::PlaybackDetailedDataType::GENRE_NAME
            });
        }

        /* Sound Quality Mode */
        if (state.mSupport.contains(t1::FunctionType::CONNECTION_MODE))
        {
            SendCommandACK(t1::GetAudioStatus, {.audioInquiredType = t1::AudioInquiredType::CONNECTION_MODE});
            SendCommandACK(t1::GetAudioParam, {.audioInquiredType = t1::AudioInquiredType::CONNECTION_MODE});
        }

        /* DSEE */
        if (state.mSupport.contains(t1::FunctionType::UPSCALING))
        {
            SendCommandACK(t1::GetAudioStatus, {.audioInquiredType = t1::AudioInquiredType::UPSCALING});
            SendCommandACK(t1::GetAudioParam, {.audioInquiredType = t1::AudioInquiredType::UPSCALING});
        }

        /* Pause when headphones are removed */
        if (state.mSupport.contains(t1::FunctionType::CONTROL_BY_WEARING))
        {
            SendCommandACK(t1::GetSystemStatus, {.systemInquiredType = t1::SystemInquiredType::CONTROL_BY_WEARING});
            SendCommandACK(t1::GetSystemParam, {.systemInquiredType = t1::SystemInquiredType::CONTROL_BY_WEARING});
        }

        /* Auto Power Off */
        if (state.mSupport.contains(t1::FunctionType::AUTO_POWER_OFF))
        {
            SendCommandACK(t1::GetSystemStatus, {.systemInquiredType = t1::SystemInquiredType::AUTO_POWER_OFF});
            SendCommandACK(t1::GetSystemParam, {.systemInquiredType = t1::SystemInquiredType::AUTO_POWER_OFF});
        }

        /* Speak-to-Chat */
        if (state.mSupport.contains(t1::FunctionType::SMART_TALKING_MODE))
        {
            SendCommandACK(t1::GetSystemStatus, {.systemInquiredType = t1::SystemInquiredType::SMART_TALKING_MODE});
            SendCommandACK(t1::GetSystemParam, {.systemInquiredType = t1::SystemInquiredType::SMART_TALKING_MODE});
            SendCommandACK(t1::GetSystemExParam, {
                .systemInquiredType = t1::SystemInquiredType::SMART_TALKING_MODE
            });
        }

        /* WH: CUSTOM Button; WF: Touch Sensor */
        if (state.mSupport.contains(t1::FunctionType::ASSIGNABLE_SETTINGS))
        {
            SendCommandACK(t1::GetSystemStatus, {.systemInquiredType = t1::SystemInquiredType::ASSIGNABLE_SETTINGS});
            SendCommandACK(t1::GetSystemParam, {.systemInquiredType = t1::SystemInquiredType::ASSIGNABLE_SETTINGS});
        }

        /* Battery level */
        if (state.mSupport.contains(t1::FunctionType::BATTERY_LEVEL))
        {
            SendCommandACK(t1::GetBatteryLevel, {.batteryInquiredType = t1::BatteryInquiredType::BATTERY});
        }

        /* Current DSEE */
        if (state.mSupport.contains(t1::FunctionType::UPSCALING_INDICATOR))
        {
            SendCommandACK(t1::GetUpscalingEffect);
        }

        /* Codec Type */
        if (state.mSupport.contains(t1::FunctionType::CODEC_INDICATOR))
        {
            SendCommandACK(t1::GetAudioCodec);
        }

        /* General Settings */
        if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING1))
        {
            SendCommandACK(t1::GetGsStatus, {.type = t1::GsInquiredType::GENERAL_SETTING1});
            SendCommandACK(t1::GetGsParam, {.type = t1::GsInquiredType::GENERAL_SETTING1});
        }
        if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING2))
        {
            SendCommandACK(t1::GetGsStatus, {.type = t1::GsInquiredType::GENERAL_SETTING2});
            SendCommandACK(t1::GetGsParam, {.type = t1::GsInquiredType::GENERAL_SETTING2});
        }
        if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING3))
        {
            SendCommandACK(t1::GetGsStatus, {.type = t1::GsInquiredType::GENERAL_SETTING3});
            SendCommandACK(t1::GetGsParam, {.type = t1::GsInquiredType::GENERAL_SETTING3});
        }

        /*if (state.mSupport.contains(t1::FunctionType::ACTION_LOG_NOTIFIER))
        {
            SendCommandACK(t1::SetLogStatus, {
                .type = t1::LogInquiredType::ACTION_LOG_NOTIFIER,
                .status = CommonStatus::ENABLE // TODO: Fix this struct; add status field
            });
        }*/

        /* Receive alerts for certain operations like toggling multipoint */
        SendCommandACK(t1::SetAlertStatus, {
            .type = t1::AlertInquiredType::FIXED_MESSAGE,
            .status = CommonStatus::ENABLE
        });

        mInitialized = true;
        co_return MDR_EVENT_INITIALIZE_COMPLETE;
    }

    MDRTask MDRHeadphones::RequestSyncV1()
    {
        // auto& state = mDetailsV1;
        co_return MDR_EVENT_SYNC_COMPLETE;
    }

    void MDRHeadphones::SnapshotPropertiesV1()
    {
        auto& state = mDetailsV1;
        state.mShutdown.submit();
        state.mNcAsmEnabled.submit();
        state.mNcAsmFocusOnVoice.submit();
        state.mNcAsmLevel.submit();
        state.mNcAsmChangingLevel.submit();
        state.mNcAsmButtonFunction.submit();
        state.mPowerAutoOff.submit();
        state.mPowerAutoOffWearingDetection.submit();
        state.mPlayVolume.submit();
        state.mPlayControl.submit();
        state.mGsParamBool[0].submit();
        state.mGsParamBool[1].submit();
        state.mGsParamBool[2].submit();
        state.mUpscalingEnabled.submit();
        state.mAudioPriorityMode.submit();
        state.mBGMModeEnabled.submit();
        state.mBGMModeRoomSize.submit();
        state.mUpmixCinemaEnabled.submit();
        state.mAutoPauseEnabled.submit();
        state.mAssignableSettingsPresets.submit();
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

        if (state.mShutdown.pending())
        {
            if (state.mShutdown.submitted && state.mSupport.contains(t1::FunctionType::POWER_OFF))
                SendCommandACK(t1::SetPowerOff);
            state.mShutdown.override(false);
        }

        if (state.mNcAsmEnabled.pending() || state.mNcAsmFocusOnVoice.pending() || state.mNcAsmLevel.pending() ||
            state.mNcAsmChangingLevel.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE))
            {
                t1::SetNcAsmParamNcAsmParam payload;
                if (state.mNcAsmEnabled.submitted)
                {
                    if (state.mNcAsmChangingLevel.submitted)
                        payload.ncAsmEffect = t1::NcAsmEffect::ADJUSTMENT_IN_PROGRESS;
                    else if (!state.mNcAsmEnabled.pending())
                        payload.ncAsmEffect = t1::NcAsmEffect::ADJUSTMENT_COMPLETION;
                    else
                        payload.ncAsmEffect = t1::NcAsmEffect::ON;
                }
                else
                {
                    payload.ncAsmEffect = t1::NcAsmEffect::OFF;
                }
                payload.ncType = t1::NcAsmSettingType::LEVEL_ADJUSTMENT;
                payload.ncValue = V1NcValueForAmbientLevel(state.mNcAsmLevel.submitted);
                payload.asmType = t1::AsmSettingType::LEVEL_ADJUSTMENT;
                payload.asmId = state.mNcAsmFocusOnVoice.submitted ? t1::AsmId::VOICE : t1::AsmId::NORMAL;
                payload.asmValue = static_cast<UInt8>(
                    state.mNcAsmLevel.submitted <= -1 ? 0 : std::clamp(state.mNcAsmLevel.submitted, 0, 20));
                SendCommandACK(t1::SetNcAsmParamNcAsmParam, payload);
            }
            state.mNcAsmEnabled.commit();
            state.mNcAsmFocusOnVoice.commit();
            state.mNcAsmLevel.commit();
            state.mNcAsmChangingLevel.commit();
        }

        if (state.mPlayVolume.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::PLAYBACK_CONTROLLER))
            {
                SendCommandACK(t1::SetPlayParamPlaybackControllerVolumeData, {
                    .volumeValue = static_cast<UInt8>(std::clamp(state.mPlayVolume.submitted, 0, 30))
                });
            }
            state.mPlayVolume.commit();
        }
        if (state.mPlayControl.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::PLAYBACK_CONTROLLER))
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
            if (state.mSupport.contains(t1::FunctionType::PRESET_EQ))
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
            if (state.mSupport.contains(t1::FunctionType::UPSCALING))
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
            if (state.mSupport.contains(t1::FunctionType::CONNECTION_MODE))
            {
                SendCommandACK(t1::SetAudioParamConnectionModeParam, {
                    .settingValue = state.mAudioPriorityMode.submitted
                });
            }
            state.mAudioPriorityMode.commit();
        }

        if (state.mPowerAutoOff.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::AUTO_POWER_OFF))
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
            if (state.mSupport.contains(t1::FunctionType::CONTROL_BY_WEARING))
            {
                SendCommandACK(t1::SetSystemExParamControlByWearingParam, {
                    .settingValue = state.mAutoPauseEnabled.submitted
                        ? t1::ControlByWearingSettingValue::ON
                        : t1::ControlByWearingSettingValue::OFF
                });
            }
            state.mAutoPauseEnabled.commit();
        }

        if (state.mAssignableSettingsPresets.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::ASSIGNABLE_SETTINGS))
            {
                t1::SetSystemParamAssignableSettingsParam payload;
                payload.presets.value = state.mAssignableSettingsPresets.submitted;
                SendCommandACK(t1::SetSystemParamAssignableSettingsParam, payload);
            }
            state.mAssignableSettingsPresets.commit();
        }

        if (state.mSpeakToChatEnabled.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::SMART_TALKING_MODE))
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
            if (state.mSupport.contains(t1::FunctionType::SMART_TALKING_MODE))
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
            if (state.mSupport.contains(t1::FunctionType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
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
            if (state.mSupport.contains(t1::FunctionType::VOICE_GUIDANCE))
            {
                SendCommandACK(t2::SetVoiceGuidanceParamSettingOnOff, {
                    .settingValue = state.mVoiceGuidanceEnabled.submitted
                        ? t2::VoiceGuidanceSettingValue::ON
                        : t2::VoiceGuidanceSettingValue::OFF
                });
            }
            state.mVoiceGuidanceEnabled.commit();
        }

        if (state.mGsParamBool[0].pending())
        {
            if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING1))
            {
                SendCommandACK(t1::SetGsParamGsBooleanTypeValue, {
                    .type = t1::GsInquiredType::GENERAL_SETTING1,
                    .settingValue = { state.mGsParamBool[0].submitted
                        ? t1::CommonOnOffSettingValue::ON : t1::CommonOnOffSettingValue::OFF }
                });
            }
            state.mGsParamBool[0].commit();
        }
        if (state.mGsParamBool[1].pending())
        {
            if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING2))
            {
                SendCommandACK(t1::SetGsParamGsBooleanTypeValue, {
                    .type = t1::GsInquiredType::GENERAL_SETTING2,
                    .settingValue = { state.mGsParamBool[1].submitted
                        ? t1::CommonOnOffSettingValue::ON : t1::CommonOnOffSettingValue::OFF }
                });
            }
            state.mGsParamBool[1].commit();
        }
        if (state.mGsParamBool[2].pending())
        {
            if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING3))
            {
                SendCommandACK(t1::SetGsParamGsBooleanTypeValue, {
                    .type = t1::GsInquiredType::GENERAL_SETTING3,
                    .settingValue = { state.mGsParamBool[2].submitted
                        ? t1::CommonOnOffSettingValue::ON : t1::CommonOnOffSettingValue::OFF }
                });
            }
            state.mGsParamBool[2].commit();
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
            state.mNcAsmFocusOnVoice.dirty() || state.mNcAsmLevel.dirty() ||
            state.mNcAsmChangingLevel.dirty() || state.mNcAsmButtonFunction.dirty() ||
            state.mPowerAutoOff.dirty() || state.mPowerAutoOffWearingDetection.dirty() ||
            state.mPlayVolume.dirty() || state.mPlayControl.dirty() ||
            state.mGsParamBool[0].dirty() || state.mGsParamBool[1].dirty() ||
            state.mGsParamBool[2].dirty() ||
            state.mUpscalingEnabled.dirty() || state.mAudioPriorityMode.dirty() ||
            state.mBGMModeEnabled.dirty() || state.mBGMModeRoomSize.dirty() ||
            state.mUpmixCinemaEnabled.dirty() || state.mAutoPauseEnabled.dirty() ||
            state.mAssignableSettingsPresets.dirty() ||
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
