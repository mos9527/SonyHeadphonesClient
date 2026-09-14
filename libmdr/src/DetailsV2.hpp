#pragma once

#include <mdr/ProtocolV2T1.hpp>
#include <mdr/ProtocolV2T2.hpp>

#include <mdr-c/Headphones.h>

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
                LEGACY_PROFILE
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
                    contains(F::BGM_MODE_SMALL_MIDDLE_LARGE_AND_ERRORCODE) ||
                    contains(F::BGM_MODE_CANT_BE_USED_WITH_LEA_CONNECTION);
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

        v2::t1::UpscalingType mUpscalingType{};
        bool mUpscalingAvailable{true};

        struct GsCapability
        {
            v2::t1::GsSettingType type{};
            v2::t1::GsSettingInfo value{};
        };

        GsCapability mGsCapability[4];

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
        MDRProperty<bool> mUpmixCinemaEnabled;
        MDRProperty<bool> mVoiceContentsEnabled;
        MDRProperty<bool> mSoundLeakageReductionEnabled;
        MDRProperty<bool> mAutoPauseEnabled;
        MDRProperty<v2::t1::Preset> mTouchFunctionLeft, mTouchFunctionRight;
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
        if (state.mSupport.contains(T1::PRESET_EQ))
            return out = PRESET_EQ, true;
        if (state.mSupport.contains(T1::PRESET_EQ_NON_CUSTOMIZABLE))
            return out = PRESET_EQ_NONCUSTOMIZABLE, true;
        if (state.mSupport.contains(T1::PRESET_EQ_AND_ULT_MODE))
            return out = PRESET_EQ_AND_ULT_MODE, true;
        if (state.mSupport.contains(T1::PRESET_EQ_AND_ERRORCODE))
            return out = PRESET_EQ_AND_ERRORCODE, true;
        return false;
    }

    /**
     * @brief Whether the device advertises @p feature. Also gates the requests in @ref RequestInitV2.
     */
    inline bool SupportsFeature(const DetailsV2& state, MDRFeature feature)
    {
        using T1 = v2::t1::FunctionType;
        using T2 = v2::t2::FunctionType;
        const bool playback =
            state.mSupport.contains(T1::PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT) ||
            state.mSupport.contains(T1::PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT_AND_MUTE) ||
            state.mSupport.contains(T1::PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE) ||
            state.mSupport.contains(T1::PLAYBACK_CONTROLLER_WITH_FUNCTION_CHANGE);
        const bool noise =
            state.mSupport.contains(T1::NOISE_CANCELLING_ONOFF) ||
            state.mSupport.contains(T1::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_ONOFF) ||
            state.mSupport.contains(T1::NOISE_CANCELLING_DUAL_SINGLE_OFF_AND_AMBIENT_SOUND_MODE_ONOFF) ||
            state.mSupport.contains(T1::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT) ||
            state.mSupport.contains(T1::NOISE_CANCELLING_DUAL_SINGLE_OFF_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT) ||
            state.mSupport.contains(T1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AUTO_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT) ||
            state.mSupport.contains(T1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_SINGLE_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT) ||
            state.mSupport.contains(T1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT) ||
            state.mSupport.contains(
                T1::MODE_NC_NCSS_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_WITH_TEST_MODE) ||
            state.mSupport.contains(
                T1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION);
        const bool pairing =
            state.mSupport.contains(T2::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT) ||
            state.mSupport.contains(T2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT) ||
            state.mSupport.contains(T2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE);
        switch (feature)
        {
        case MDR_FEATURE_IDENTITY: return true;
        case MDR_FEATURE_BATTERY_SINGLE:
            return state.mSupport.contains(T1::BATTERY_LEVEL_INDICATOR) ||
                state.mSupport.contains(T1::BATTERY_LEVEL_WITH_THRESHOLD);
        case MDR_FEATURE_BATTERY_LEFT_RIGHT:
            return state.mSupport.contains(T1::LEFT_RIGHT_BATTERY_LEVEL_INDICATOR) ||
                state.mSupport.contains(T1::LR_BATTERY_LEVEL_WITH_THRESHOLD);
        case MDR_FEATURE_BATTERY_CASE:
            return state.mSupport.contains(T1::CRADLE_BATTERY_LEVEL_INDICATOR) ||
                state.mSupport.contains(T1::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD);
        case MDR_FEATURE_PLAYBACK_METADATA:
        case MDR_FEATURE_PLAYBACK_CONTROL:
        case MDR_FEATURE_PLAYBACK_VOLUME: return playback;
        case MDR_FEATURE_NOISE_CANCELLING: return noise;
        case MDR_FEATURE_AMBIENT_SOUND:
            return noise || state.mSupport.contains(T1::AMBIENT_SOUND_MODE_ONOFF) ||
                state.mSupport.contains(T1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT);
        case MDR_FEATURE_ADAPTIVE_AMBIENT_SOUND:
            return state.mSupport.contains(
                T1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION);
        case MDR_FEATURE_SPEAK_TO_CHAT: return state.mSupport.contains(T1::SMART_TALKING_MODE_TYPE2);
        case MDR_FEATURE_LISTENING_MODE: return state.mSupport.contains(T1::LISTENING_OPTION);
        case MDR_FEATURE_LISTENING_BACKGROUND_MUSIC:
            return state.mSupport.contains(T1::LISTENING_OPTION) && state.mSupport.containsBGMMode();
        case MDR_FEATURE_LISTENING_CINEMA:
            return state.mSupport.contains(T1::LISTENING_OPTION) && state.mSupport.contains(T1::UPMIX_CINEMA);
        case MDR_FEATURE_LISTENING_VOICE_BOOST:
            return state.mSupport.contains(T1::LISTENING_OPTION) && state.mSupport.contains(T1::VOICE_CONTENTS);
        case MDR_FEATURE_LISTENING_SOUND_LEAKAGE_REDUCTION:
            return state.mSupport.contains(T1::LISTENING_OPTION) &&
                state.mSupport.contains(T1::SOUND_LEAKAGE_REDUCTION);
        case MDR_FEATURE_EQUALIZER:
            return state.mSupport.contains(T1::PRESET_EQ) || state.mSupport.contains(T1::CUSTOM_EQ) ||
                state.mSupport.contains(T1::PRESET_EQ_NON_CUSTOMIZABLE) ||
                state.mSupport.contains(T1::PRESET_EQ_AND_ULT_MODE) ||
                state.mSupport.contains(T1::SOUND_EFFECT) || state.mSupport.contains(T1::TURN_KEY_EQ) ||
                state.mSupport.contains(T1::PRESET_EQ_AND_ERRORCODE) ||
                state.mSupport.contains(T1::CUSTOMIZABLE_SOUND_EFFECT);
        case MDR_FEATURE_DSEE: return state.mSupport.contains(T1::UPSCALING_AUTO_OFF);
        case MDR_FEATURE_PAIRED_DEVICE_MANAGEMENT:
        case MDR_FEATURE_PAIRING_MODE: return pairing;
        case MDR_FEATURE_GENERAL_SETTINGS:
            return state.mSupport.contains(T1::GENERAL_SETTING_1) ||
                state.mSupport.contains(T1::GENERAL_SETTING_2) ||
                state.mSupport.contains(T1::GENERAL_SETTING_3) ||
                state.mSupport.contains(T1::GENERAL_SETTING_4);
        case MDR_FEATURE_ASSIGNABLE_CONTROLS: return state.mSupport.contains(T1::ASSIGNABLE_SETTING);
        case MDR_FEATURE_NOISE_CONTROL_BUTTON:
            return state.mSupport.contains(T1::AMBIENT_SOUND_CONTROL_MODE_SELECT);
        case MDR_FEATURE_AUTO_POWER_OFF:
            return state.mSupport.contains(T1::AUTO_POWER_OFF) ||
                state.mSupport.contains(T1::AUTO_POWER_OFF_WITH_WEARING_DETECTION);
        case MDR_FEATURE_WEARING_DETECTION:
            return state.mSupport.contains(T1::AUTO_POWER_OFF_WITH_WEARING_DETECTION) ||
                state.mSupport.contains(T1::WEARING_STATUS_DETECTOR);
        case MDR_FEATURE_AUTO_PAUSE:
            return state.mSupport.contains(T1::PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF);
        case MDR_FEATURE_HEAD_GESTURE: return state.mSupport.contains(T1::HEAD_GESTURE_ON_OFF_TRAINING);
        case MDR_FEATURE_VOICE_GUIDANCE:
            return state.mSupport.contains(
                T2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH) ||
                state.mSupport.contains(
                    T2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH) ||
                state.mSupport.contains(
                    T2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT) ||
                state.mSupport.contains(T2::VOICE_GUIDANCE_VOLUME_SETTING_MTK_FIXED_TO_5_STEPS) ||
                state.mSupport.contains(T2::VOICE_GUIDANCE_SETTING_SUPPORT_LANGUAGE_SWITCH) ||
                state.mSupport.contains(T2::VOICE_GUIDANCE_SETTING_ONLY_ON_OFF_SWITCH);
        case MDR_FEATURE_VOICE_GUIDANCE_VOLUME:
            return state.mSupport.contains(
                T2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT) ||
                state.mSupport.contains(T2::VOICE_GUIDANCE_VOLUME_SETTING_MTK_FIXED_TO_5_STEPS);
        case MDR_FEATURE_SHUTDOWN: return state.mSupport.contains(T1::POWER_OFF);
        case MDR_FEATURE_CONNECTION_MODE:
            return state.mSupport.contains(T1::CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY);
        case MDR_FEATURE_SAFE_LISTENING:
            return state.mSupport.contains(T2::SAFE_LISTENING_HBS_1) ||
                state.mSupport.contains(T2::SAFE_LISTENING_HBS_2) ||
                state.mSupport.contains(T2::SAFE_LISTENING_TWS_1) ||
                state.mSupport.contains(T2::SAFE_LISTENING_TWS_2);
        case MDR_FEATURE_SOURCE_SWITCH_CONTROL: return state.mSupport.contains(T2::SOURCE_SWITCH_CONTROL);
        default: return false;
        }
    }

} // namespace mdr
