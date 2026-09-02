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
        } mSupport{};

        String mUniqueId;
        String mFWVersion;
        String mModelName;
        v2::t1::ModelSeries mModelSeries{};
        v2::ModelColor mModelColor{};
        v2::t1::AudioCodec mAudioCodec{};

        v2::t1::AlertMessageType mLastAlertMessage{};
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
        bool mUpscalingAvailable{};

        struct GsCapability
        {
            v2::t1::GsSettingType type{};
            v2::t1::GsSettingInfo value{};
        };

        GsCapability mGsCapability1, mGsCapability2, mGsCapability3, mGsCapability4;

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
        MDRProperty<bool> mGsParamBool1, mGsParamBool2, mGsParamBool3, mGsParamBool4;
        MDRProperty<bool> mUpscalingEnabled;
        MDRProperty<v2::t1::PriorMode> mAudioPriorityMode;
        MDRProperty<bool> mBGMModeEnabled;
        MDRProperty<v2::t1::RoomSize> mBGMModeRoomSize;
        MDRProperty<bool> mUpmixCinemaEnabled;
        MDRProperty<bool> mAutoPauseEnabled;
        MDRProperty<v2::t1::Preset> mTouchFunctionLeft, mTouchFunctionRight;
        MDRProperty<bool> mSpeakToChatEnabled;
        MDRProperty<v2::t1::DetectSensitivity> mSpeakToChatDetectSensitivity;
        MDRProperty<v2::t1::ModeOutTime> mSpeakToModeOutTime;
        UInt8 mSpeakToChatVoiceFocus{};
        MDRProperty<bool> mHeadGestureEnabled;
        MDRProperty<bool> mEqAvailable;
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

} // namespace mdr
