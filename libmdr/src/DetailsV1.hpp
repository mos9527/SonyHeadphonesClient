#pragma once

#include <mdr/ProtocolV1T1.hpp>
#include <mdr/ProtocolV1T2.hpp>

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

        UInt8 mLastAlertMessage{};
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
        bool mUpscalingAvailable{};

        struct GsCapability
        {
            v1::t1::GsSettingType type{};
            v1::t1::GsSettingInfo value{};
        };

        GsCapability mGsCapability1, mGsCapability2, mGsCapability3, mGsCapability4;

        MDRProperty<bool> mShutdown;
        MDRProperty<bool> mNcAsmEnabled;
        MDRProperty<bool> mNcAsmFocusOnVoice;
        MDRProperty<int> mNcAsmAmbientLevel;
        MDRProperty<bool> mNcAsmChangingAsmLevel;
        MDRProperty<UInt8> mNcAsmButtonFunction;
        MDRProperty<UInt8> mNcAsmMode;
        MDRProperty<bool> mNcAsmAutoAsmEnabled;
        MDRProperty<UInt8> mNcAsmNoiseAdaptiveSensitivity;
        MDRProperty<v1::t1::AutoPowerOffElementId> mPowerAutoOff;
        MDRProperty<UInt8> mPowerAutoOffWearingDetection;
        MDRProperty<int> mPlayVolume;
        MDRProperty<v1::t1::PlaybackControl> mPlayControl;
        MDRProperty<bool> mGsParamBool1, mGsParamBool2, mGsParamBool3, mGsParamBool4;
        MDRProperty<bool> mUpscalingEnabled;
        MDRProperty<v1::t1::ConnectionModeSettingValue> mAudioPriorityMode;
        MDRProperty<bool> mBGMModeEnabled;
        MDRProperty<UInt8> mBGMModeRoomSize;
        MDRProperty<bool> mUpmixCinemaEnabled;
        MDRProperty<bool> mAutoPauseEnabled;
        MDRProperty<v1::t1::AssignableSettingsPreset> mTouchFunctionLeft, mTouchFunctionRight;
        MDRProperty<bool> mSpeakToChatEnabled;
        MDRProperty<v1::t1::DetectionSensitivity> mSpeakToChatDetectSensitivity;
        MDRProperty<v1::t1::ModeOutTime> mSpeakToModeOutTime;
        v1::t1::CommonOnOffSettingValue mSpeakToChatVoiceFocus{v1::t1::CommonOnOffSettingValue::OFF};
        MDRProperty<bool> mHeadGestureEnabled;
        MDRProperty<bool> mEqAvailable;
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

} // namespace mdr
