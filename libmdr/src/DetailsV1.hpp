#pragma once

#include <mdr/ProtocolV1T1.hpp>
#include <mdr/ProtocolV1T2.hpp>

#include <mdr-c/Headphones.h>

#include "Property.hpp"

namespace mdr
{
    struct DetailsV1
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

            Array<bool, 256> functions{};
            Provenance provenance{Provenance::UNKNOWN};

            [[nodiscard]] constexpr bool contains(v1::t1::FunctionType value) const
            {
                return functions[static_cast<UInt8>(value)];
            }
        } mSupport{};

        String mUniqueId;
        String mFWVersion;
        String mModelName;
        v1::t1::ModelSeries mModelSeries{};
        v1::ModelColor mModelColor{};
        v1::t1::AudioCodec mAudioCodec{};

        v1::t1::AlertMessageType mLastAlertMessage{};
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
            v1::t1::BatteryChargingStatus charging{};
        };

        BatteryState mBatteryL, mBatteryR, mBatteryCase;

        String mPlayTrackTitle;
        String mPlayTrackAlbum;
        String mPlayTrackArtist;
        v1::t1::PlaybackStatus mPlayPause{};

        v1::t1::UpscalingType mUpscalingType{};
        bool mUpscalingAvailable{true};

        Vector<v1::t1::AsCapabilityKey> mAssignableSettingsKeys;

        struct GsCapability
        {
            v1::t1::GsSettingType type{};
            v1::t1::GsSettingInfo value{};
        };

        GsCapability mGsCapability[3];

        MDRProperty<bool> mShutdown;
        MDRProperty<bool> mNcAsmEnabled;
        MDRProperty<bool> mNcAsmFocusOnVoice;
        MDRProperty<int> mNcAsmLevel;
        MDRProperty<bool> mNcAsmChangingLevel;
        MDRProperty<UInt8> mNcAsmButtonFunction;
        MDRProperty<v1::t1::AutoPowerOffElementId> mPowerAutoOff;
        MDRProperty<UInt8> mPowerAutoOffWearingDetection;
        MDRProperty<int> mPlayVolume;
        MDRProperty<v1::t1::PlaybackControl> mPlayControl;
        MDRProperty<bool> mGsParamBool[3];
        MDRProperty<bool> mUpscalingEnabled;
        MDRProperty<v1::t1::ConnectionModeSettingValue> mAudioPriorityMode;
        MDRProperty<bool> mBGMModeEnabled;
        MDRProperty<UInt8> mBGMModeRoomSize;
        MDRProperty<bool> mUpmixCinemaEnabled;
        MDRProperty<bool> mAutoPauseEnabled;
        MDRProperty<Vector<v1::t1::AssignableSettingsPreset>> mAssignableSettingsPresets;
        MDRProperty<bool> mSpeakToChatEnabled;
        MDRProperty<v1::t1::DetectionSensitivity> mSpeakToChatDetectSensitivity;
        MDRProperty<v1::t1::ModeOutTime> mSpeakToModeOutTime;
        v1::t1::CommonOnOffSettingValue mSpeakToChatVoiceFocus{v1::t1::CommonOnOffSettingValue::OFF};
        MDRProperty<bool> mHeadGestureEnabled;
        struct EqPresetInfo
        {
            v1::t1::EqPresetId presetId{};
            String name;
        };

        Vector<EqPresetInfo> mEqPresets;
        MDRProperty<bool> mEqAvailable{true, true, true};
        MDRProperty<v1::t1::EqPresetId> mEqPresetId;
        MDRProperty<int> mEqClearBass;
        MDRProperty<Vector<int>> mEqConfig;
        MDRProperty<bool> mVoiceGuidanceEnabled;
        MDRProperty<int> mVoiceGuidanceVolume;
        MDRProperty<bool> mPairingMode;
        MDRProperty<String> mMultipointDeviceMac;
        MDRProperty<String> mPairedDeviceDisconnectMac, mPairedDeviceConnectMac, mPairedDeviceUnpairMac;
        MDRProperty<bool> mSourceSwitchControlEnabled;
        UInt8 mSourceSwitchControlResult{};
        MDRProperty<bool> mSafeListeningPreviewMode;
    };

    /**
     * @brief Whether the device advertises @p feature. Also gates the requests in @ref RequestInitV1.
     */
    inline bool SupportsFeature(const DetailsV1& state, MDRFeature feature)
    {
        using F = v1::t1::FunctionType;
        switch (feature)
        {
        case MDR_FEATURE_IDENTITY: return true;
        case MDR_FEATURE_BATTERY_SINGLE: return state.mSupport.contains(F::BATTERY_LEVEL);
        case MDR_FEATURE_PLAYBACK_METADATA:
        case MDR_FEATURE_PLAYBACK_CONTROL:
        case MDR_FEATURE_PLAYBACK_VOLUME: return state.mSupport.contains(F::PLAYBACK_CONTROLLER);
        case MDR_FEATURE_NOISE_CANCELLING:
            return state.mSupport.contains(F::NOISE_CANCELLING) ||
                state.mSupport.contains(F::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE);
        case MDR_FEATURE_AMBIENT_SOUND:
            return state.mSupport.contains(F::AMBIENT_SOUND_MODE) ||
                state.mSupport.contains(F::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE);
        case MDR_FEATURE_SPEAK_TO_CHAT: return state.mSupport.contains(F::SMART_TALKING_MODE);
        case MDR_FEATURE_EQUALIZER:
            return state.mSupport.contains(F::PRESET_EQ) || state.mSupport.contains(F::EBB) ||
                state.mSupport.contains(F::PRESET_EQ_NONCUSTOMIZABLE);
        case MDR_FEATURE_DSEE: return state.mSupport.contains(F::UPSCALING);
        case MDR_FEATURE_PAIRED_DEVICE_MANAGEMENT:
        case MDR_FEATURE_PAIRING_MODE:
            return state.mSupport.contains(F::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT);
        case MDR_FEATURE_GENERAL_SETTINGS:
            return state.mSupport.contains(F::GENERAL_SETTING1) ||
                state.mSupport.contains(F::GENERAL_SETTING2) ||
                state.mSupport.contains(F::GENERAL_SETTING3);
        case MDR_FEATURE_ASSIGNABLE_CONTROLS: return state.mSupport.contains(F::ASSIGNABLE_SETTINGS);
        case MDR_FEATURE_AUTO_POWER_OFF: return state.mSupport.contains(F::AUTO_POWER_OFF);
        case MDR_FEATURE_WEARING_DETECTION:
        case MDR_FEATURE_AUTO_PAUSE: return state.mSupport.contains(F::CONTROL_BY_WEARING);
        case MDR_FEATURE_VOICE_GUIDANCE: return state.mSupport.contains(F::VOICE_GUIDANCE);
        case MDR_FEATURE_SHUTDOWN: return state.mSupport.contains(F::POWER_OFF);
        case MDR_FEATURE_CONNECTION_MODE: return state.mSupport.contains(F::CONNECTION_MODE);
        default: return false;
        }
    }

} // namespace mdr
