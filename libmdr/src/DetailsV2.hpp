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
                return contains(F::NOISE_CANCELLING_ONOFF) ||
                    contains(F::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_ONOFF) ||
                    contains(F::NOISE_CANCELLING_DUAL_SINGLE_OFF_AND_AMBIENT_SOUND_MODE_ONOFF) ||
                    contains(F::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT) ||
                    contains(F::NOISE_CANCELLING_DUAL_SINGLE_OFF_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT) ||
                    contains(F::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AUTO_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT) ||
                    contains(F::MODE_NC_ASM_NOISE_CANCELLING_DUAL_SINGLE_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT) ||
                    contains(F::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT) ||
                    contains(
                        F::MODE_NC_NCSS_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_WITH_TEST_MODE) ||
                    contains(F::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION);
            }

            [[nodiscard]] constexpr bool containsEqualizer() const
            {
                using F = v2::t1::FunctionType;
                return contains(F::PRESET_EQ) || contains(F::CUSTOM_EQ) ||
                    contains(F::PRESET_EQ_NON_CUSTOMIZABLE) || contains(F::PRESET_EQ_AND_ULT_MODE) ||
                    contains(F::SOUND_EFFECT) || contains(F::TURN_KEY_EQ) ||
                    contains(F::PRESET_EQ_AND_ERRORCODE) || contains(F::CUSTOMIZABLE_SOUND_EFFECT);
            }

            [[nodiscard]] constexpr bool containsVoiceGuidance() const
            {
                using F = v2::t2::FunctionType;
                return contains(F::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH) ||
                    contains(F::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH) ||
                    contains(
                        F::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT) ||
                    contains(F::VOICE_GUIDANCE_VOLUME_SETTING_MTK_FIXED_TO_5_STEPS) ||
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

} // namespace mdr
