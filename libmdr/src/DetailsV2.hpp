#pragma once

#include <mdr/ProtocolV2T1.hpp>
#include <mdr/ProtocolV2T2.hpp>

#include "Property.hpp"

namespace mdr
{
    struct DetailsV2
    {
        struct ProtocolStates
        {
            int version{};
            int hasTable1{};
            int hasTable2{};
        } mProtocol{};

        struct SupportStates
        {
            enum class Provenance
            {
                UNKNOWN,
                ADVERTISED,
            };

            Array<bool, 256> table1Functions{};
            Array<bool, 256> table2Functions{};
            Provenance provenance{Provenance::UNKNOWN};

            [[nodiscard]] constexpr bool contains(v2::t1::FunctionType value) const
            {
                return table1Functions[static_cast<UInt8>(value)];
            }

            [[nodiscard]] constexpr bool contains(v2::t2::FunctionType value) const
            {
                return table2Functions[static_cast<UInt8>(value)];
            }

            [[nodiscard]] constexpr bool containsBGMMode() const
            {
                using F = v2::t1::FunctionType;
                return contains(F::BGM_MODE_SMALL_MIDDLE_LARGE) ||
                    contains(F::BGM_MODE_SMALL_MIDDLE_LARGE_AND_ERRORCODE);
            }

            [[nodiscard]] constexpr bool containsPlaybackController() const
            {
                using F = v2::t1::FunctionType;
                return contains(F::PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT) ||
                    contains(F::PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT_AND_MUTE) ||
                    contains(F::PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE) ||
                    contains(F::PLAYBACK_CONTROLLER_WITH_FUNCTION_CHANGE);
            }

            [[nodiscard]] constexpr bool containsNoiseCancelling() const
            {
                using F = v2::t1::FunctionType;
                return contains(F::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT) ||
                    contains(F::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION);
            }

            [[nodiscard]] constexpr bool containsEqualizer() const
            {
                using F = v2::t1::FunctionType;
                return contains(F::PRESET_EQ) || contains(F::PRESET_EQ_NON_CUSTOMIZABLE) ||
                    contains(F::PRESET_EQ_AND_ULT_MODE) || contains(F::PRESET_EQ_AND_ERRORCODE);
            }

            [[nodiscard]] constexpr bool containsVoiceGuidance() const
            {
                using F = v2::t2::FunctionType;
                return contains(F::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH) ||
                    contains(F::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH) ||
                    contains(
                        F::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT) ||
                    contains(F::VOICE_GUIDANCE_SETTING_SUPPORT_LANGUAGE_SWITCH) ||
                    contains(F::VOICE_GUIDANCE_SETTING_ONLY_ON_OFF_SWITCH);
            }

            [[nodiscard]] constexpr bool containsVoiceGuidanceVolume() const
            {
                using F = v2::t2::FunctionType;
                return contains(
                        F::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT) ||
                    contains(F::VOICE_GUIDANCE_VOLUME_SETTING_MTK_FIXED_TO_5_STEPS);
            }

            [[nodiscard]] constexpr bool containsPairingDeviceManagement() const
            {
                using F = v2::t2::FunctionType;
                return contains(F::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT) ||
                    contains(F::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT) ||
                    contains(F::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE);
            }
        } mSupport{};

        String mUniqueId;
        String mFWVersion;
        String mModelName;
        v2::t1::ModelSeries mModelSeries{};
        v2::ModelColor mModelColor{};
        v2::t1::AudioCodec mAudioCodec{};

        v2::t1::AlertMessageType mLastAlertMessage{};
        bool mAlertAwaitingResponse{};
        String mLastInteractionMessage;
        String mLastDeviceJSONMessage;

        struct PeripheralDevice
        {
            String macAddress;
            String name;
            bool connected{};
            bool playbackDevice{};
        };

        Vector<PeripheralDevice> mPairedDevices;
        UInt8 mPairedDevicesPlaybackDeviceID{};
        int mSafeListeningSoundPressure{};

        struct BatteryState
        {
            UInt8 level{};
            UInt8 threshold{};
            v2::t1::BatteryChargingStatus charging{};
        };

        BatteryState mBatteryL, mBatteryR, mBatteryCase;

        String mPlayTrackTitle;
        String mPlayTrackAlbum;
        String mPlayTrackArtist;
        v2::t1::PlaybackStatus mPlayPause{};

        // Proximity sensor. The device does not push WEARING_STATUS_CHECKER on its own; the
        // operation log's unitRemove / unitWear entries raise MDR_EVENT_NEED_SYNC instead, and
        // RequestSyncV2 re-reads it. OUT_OF_RANGE until the first reply.
        v2::t2::WearingStatusCode mWearingStatus{v2::t2::WearingStatusCode::OUT_OF_RANGE};

        [[nodiscard]] bool wearingSensorPossible() const
        {
            return mProtocol.hasTable2 && mSupport.contains(v2::t2::FunctionType::WEARING_STATUS_CHECKER);
        }

        v2::t1::UpscalingType mUpscalingType{};
        bool mUpscalingAvailable{true};

        Vector<v2::t1::AssignableSettingsKey> mAssignableSettingsKeys;

        struct GsCapability
        {
            v2::t1::GsSettingType type{};
            v2::t1::GsSettingInfo value{};
        };

        GsCapability mGsCapability[4];
        Vector<v2::t1::AutoPowerOffElements> mAutoPowerOffCandidates;

        Array<v2::t1::GsInquiredType, 4> mGsDiscoveryOrder{};
        UInt8 mGsDiscoveryOrderSize{};
        Array<bool, 3> mVoiceGuidanceOnOffSupported{};
        v2::t2::VoiceGuidanceLanguage mVoiceGuidanceLanguage{};
        UInt16 mSvaReceivedLength{};
        UInt8 mSvaPartLength{};
        v2::t2::DataContinue mSvaDataContinue{v2::t2::DataContinue::NOT_CONTINUE_DATA};

        [[nodiscard]] bool voiceGuidanceOnOffPossible() const
        {
            using F = v2::t2::FunctionType;
            return mSupport.contains(F::VOICE_GUIDANCE_SETTING_ONLY_ON_OFF_SWITCH) ||
                (mSupport.contains(F::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH) && mVoiceGuidanceOnOffSupported[0]) ||
                ((mSupport.contains(F::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH) ||
                  mSupport.contains(F::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT)) && mVoiceGuidanceOnOffSupported[1]) ||
                (mSupport.contains(F::VOICE_GUIDANCE_SETTING_SUPPORT_LANGUAGE_SWITCH) && mVoiceGuidanceOnOffSupported[2]);
        }
        Array<bool, 2> mAssignableHasCustomizableActions{};
        bool mEqCustomizable{};
        bool mEqCanUseUltMode{};
        bool mEqErrorCodeMode{};
        UInt8 mEqErrorCodeCount{};
        v2::t1::EqUltModeStatus mEqUltMode{};

        MDRProperty<bool> mShutdown;
        MDRProperty<bool> mNcAsmEnabled;
        MDRProperty<bool> mNcAsmFocusOnVoice;
        MDRProperty<int> mNcAsmAmbientLevel;
        MDRProperty<bool> mNcAsmChangingAsmLevel;
        MDRProperty<v2::t1::Function> mNcAsmButtonFunction;
        MDRProperty<v2::t1::NcAsmMode> mNcAsmMode;
        MDRProperty<bool> mNcAsmAutoAsmEnabled;
        MDRProperty<v2::t1::NoiseAdaptiveSensitivity> mNcAsmNoiseAdaptiveSensitivity;
        MDRProperty<v2::t1::AutoPowerOffElements> mPowerAutoOff;
        MDRProperty<v2::t1::AutoPowerOffWearingDetectionElements> mPowerAutoOffWearingDetection;
        MDRProperty<int> mPlayVolume;
        MDRProperty<v2::t1::PlaybackControl> mPlayControl;
        MDRProperty<bool> mGsParamBool[4];
        MDRProperty<bool> mUpscalingEnabled;
        MDRProperty<v2::t1::PriorMode> mAudioPriorityMode;
        MDRProperty<bool> mBGMModeEnabled;
        MDRProperty<v2::t1::RoomSize> mBGMModeRoomSize;
        bool mBGMModeAvailable{};
        UInt8 mBGMErrorCodeCount{};
        MDRProperty<bool> mUpmixCinemaEnabled;
        MDRProperty<bool> mVoiceContentsEnabled;
        MDRProperty<bool> mSoundLeakageReductionEnabled;
        MDRProperty<bool> mAutoPauseEnabled;
        MDRProperty<Vector<v2::t1::Preset>> mAssignableSettingsPresets;
        MDRProperty<bool> mSpeakToChatEnabled;
        MDRProperty<v2::t1::DetectSensitivity> mSpeakToChatDetectSensitivity;
        MDRProperty<v2::t1::ModeOutTime> mSpeakToModeOutTime;
        UInt8 mSpeakToChatVoiceFocus{};
        MDRProperty<bool> mHeadGestureEnabled;
        struct EqPresetInfo
        {
            v2::t1::EqPresetId presetId{};
            String name;
        };

        // Empty means unknown, not none.
        Vector<EqPresetInfo> mEqPresets;
        MDRProperty<bool> mEqAvailable{true, true, true};
        MDRProperty<v2::t1::EqPresetId> mEqPresetId;
        MDRProperty<int> mEqClearBass;
        MDRProperty<Vector<int>> mEqConfig;
        MDRProperty<bool> mVoiceGuidanceEnabled;
        MDRProperty<int> mVoiceGuidanceVolume;
        MDRProperty<bool> mPairingMode;
        MDRProperty<String> mMultipointDeviceMac;
        MDRProperty<String> mPairedDeviceDisconnectMac, mPairedDeviceConnectMac, mPairedDeviceUnpairMac;
        MDRProperty<bool> mSourceSwitchControlEnabled;
        v2::t2::SourceSwitchControlResult mSourceSwitchControlResult{v2::t2::SourceSwitchControlResult::SUCCESS};
        MDRProperty<bool> mSafeListeningPreviewMode;
    };

    /**
     * @brief The advertised preset EQ variant as an inquired type.
     * @return false if the device has no variant whose capability carries a preset list.
     */
    inline bool EqPresetInquiredType(const DetailsV2& state, v2::t1::EqEbbInquiredType& out)
    {
        using T1 = v2::t1::FunctionType;
        using enum v2::t1::EqEbbInquiredType;
        if (state.mEqCanUseUltMode)
            return out = PRESET_EQ_AND_ULT_MODE, true;
        if (state.mEqErrorCodeMode)
            return out = PRESET_EQ_AND_ERRORCODE, true;
        if (state.mSupport.containsEqualizer())
            return out = state.mEqCustomizable ? PRESET_EQ : PRESET_EQ_NONCUSTOMIZABLE, true;
        return false;
    }

    inline bool AssignableSettingsInquiredType(const DetailsV2& state, v2::t1::SystemInquiredType& out)
    {
        using T1 = v2::t1::FunctionType;
        using enum v2::t1::SystemInquiredType;
        if (state.mSupport.contains(T1::ASSIGNABLE_SETTING_WITH_LIMITATION))
            return out = ASSIGNABLE_SETTINGS_WITH_LIMITATION, true;
        if (state.mSupport.contains(T1::ASSIGNABLE_SETTING))
            return out = ASSIGNABLE_SETTINGS, true;
        return false;
    }

    inline v2::t1::PlayInquiredType PlaybackInquiredType(const DetailsV2& state)
    {
        using F = v2::t1::FunctionType;
        using T = v2::t1::PlayInquiredType;
        if (state.mSupport.contains(F::PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE))
            return T::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE;
        if (state.mSupport.contains(F::PLAYBACK_CONTROLLER_WITH_FUNCTION_CHANGE))
            return T::PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE;
        return T::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT;
    }

    inline v2::t1::PlayInquiredType MusicVolumeInquiredType(const DetailsV2& state)
    {
        return state.mSupport.contains(v2::t1::FunctionType::PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT_AND_MUTE)
            ? v2::t1::PlayInquiredType::MUSIC_VOLUME_WITH_MUTE : v2::t1::PlayInquiredType::MUSIC_VOLUME;
    }

    inline v2::t1::AudioInquiredType BGMInquiredType(const DetailsV2& state)
    {
        return state.mSupport.contains(v2::t1::FunctionType::BGM_MODE_SMALL_MIDDLE_LARGE_AND_ERRORCODE)
            ? v2::t1::AudioInquiredType::BGM_MODE_AND_ERRORCODE : v2::t1::AudioInquiredType::BGM_MODE;
    }

    inline v2::t2::VoiceGuidanceInquiredType VoiceGuidanceSettingInquiredType(const DetailsV2& state)
    {
        using F = v2::t2::FunctionType;
        using T = v2::t2::VoiceGuidanceInquiredType;
        if ((state.mSupport.contains(F::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH) ||
             state.mSupport.contains(F::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT)) &&
            state.mVoiceGuidanceOnOffSupported[1])
            return T::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH;
        if (state.mSupport.contains(F::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH) &&
            state.mVoiceGuidanceOnOffSupported[0])
            return T::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH;
        if (state.mSupport.contains(F::VOICE_GUIDANCE_SETTING_SUPPORT_LANGUAGE_SWITCH) && state.mVoiceGuidanceOnOffSupported[2])
            return T::SUPPORT_LANGUAGE_SWITCH;
        return T::ONLY_ON_OFF_SETTING;
    }

    inline v2::t2::VoiceGuidanceInquiredType VoiceGuidanceVolumeInquiredType(const DetailsV2& state)
    {
        return state.mSupport.contains(v2::t2::FunctionType::VOICE_GUIDANCE_VOLUME_SETTING_MTK_FIXED_TO_5_STEPS)
            ? v2::t2::VoiceGuidanceInquiredType::VOLUME_SETTING_FIXED_TO_5_STEPS : v2::t2::VoiceGuidanceInquiredType::VOLUME;
    }

} // namespace mdr
