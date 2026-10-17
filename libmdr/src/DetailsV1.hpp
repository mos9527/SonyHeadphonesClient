#pragma once

#include <mdr/ProtocolV1T1.hpp>
#include <mdr/ProtocolV1T2.hpp>

#include "Property.hpp"

// See also DetailsV2.hpp @ref _DETAILS_V2_PROPS_X
#define _DETAILS_V1_PROPS_X(PROP, ARRAY) \
    PROP(bool, mShutdown) \
    PROP(bool, mNcAsmEnabled) \
    PROP(bool, mNcAsmFocusOnVoice) \
    PROP(int, mNcAsmLevel) \
    PROP(bool, mNcAsmChangingLevel) \
    PROP(v1::t1::NcAsmEffect, mNcAsmEffect) \
    PROP(UInt8, mNcAsmButtonFunction) \
    PROP(v1::t1::AutoPowerOffElementId, mPowerAutoOff) \
    PROP(UInt8, mPowerAutoOffWearingDetection) \
    PROP(int, mPlayVolume) \
    PROP(v1::t1::PlaybackControl, mPlayControl) \
    ARRAY(bool, mGsParamBool, 3) \
    PROP(bool, mUpscalingEnabled) \
    PROP(v1::t1::ConnectionModeSettingValue, mAudioPriorityMode) \
    PROP(bool, mBGMModeEnabled) \
    PROP(UInt8, mBGMModeRoomSize) \
    PROP(bool, mUpmixCinemaEnabled) \
    PROP(bool, mAutoPauseEnabled) \
    PROP(Vector<v1::t1::AssignableSettingsPreset>, mAssignableSettingsPresets) \
    PROP(bool, mSpeakToChatEnabled) \
    PROP(v1::t1::DetectionSensitivity, mSpeakToChatDetectSensitivity) \
    PROP(v1::t1::ModeOutTime, mSpeakToModeOutTime) \
    PROP(bool, mHeadGestureEnabled) \
    PROP(bool, mEqAvailable, {true, true, true}) \
    PROP(v1::t1::EqPresetId, mEqPresetId) \
    PROP(int, mEqClearBass) \
    PROP(Vector<int>, mEqConfig) \
    PROP(bool, mVoiceGuidanceEnabled) \
    PROP(int, mVoiceGuidanceVolume) \
    PROP(bool, mPairingMode) \
    PROP(String, mMultipointDeviceMac) \
    PROP(bool, mSourceSwitchControlEnabled) \
    PROP(String, mPairedDeviceDisconnectMac) \
    PROP(String, mPairedDeviceConnectMac) \
    PROP(String, mPairedDeviceUnpairMac) \
    PROP(bool, mSafeListeningPreviewMode) \
    PROP(AlertResponse, mAlertResponse)

namespace mdr
{
    struct DetailsV1
    {
        struct AlertResponse
        {
            v1::t1::AlertMessageType messageType{};
            v1::t1::AlertAction action{};
            uint64_t serial{};
        };

        struct PeripheralDevice
        {
            String macAddress;
            String name;
            bool connected{};
            bool playbackDevice{};
        };

        struct BatteryState
        {
            UInt8 level{};
            UInt8 threshold{};
            v1::t1::BatteryChargingStatus charging{};
        };

        struct ProtocolStates
        {
            int version{};
            int hasTable1{};
            int hasTable2{};
        } mProtocol{};

        struct GsCapability
        {
            v1::t1::GsSettingType type{};
            v1::t1::GsSettingInfo value{};
        };

        struct EqPresetInfo
        {
            v1::t1::EqPresetId presetId{};
            String name;
        };

        struct SupportStates
        {
            enum class state
            {
                UNKNOWN,
                ADVERTISED,
                LEGACY_PROFILE
            };

            Array<bool, 256> functions{};
            state provenance{state::UNKNOWN};

            [[nodiscard]] constexpr bool contains(v1::t1::FunctionType value) const
            {
                return functions[static_cast<UInt8>(value)];
            }
        } mSupport{};

#define _PROP(TYPE, NAME, ...) MDRProperty<TYPE> NAME __VA_ARGS__;
#define _ARRAY(TYPE, NAME, COUNT, ...) MDRProperty<TYPE> NAME[COUNT] __VA_ARGS__;
        _DETAILS_V1_PROPS_X(_PROP, _ARRAY)
#undef _PROP
#undef _ARRAY

        void submit()
        {
#define _PROP(TYPE, NAME, ...) NAME.submit();
#define _ARRAY(TYPE, NAME, COUNT, ...)                                                                                 \
    for (auto& p : NAME)                                                                                               \
        p.submit();
            _DETAILS_V1_PROPS_X(_PROP, _ARRAY)
#undef _PROP
#undef _ARRAY
        }

        [[nodiscard]] bool dirty() const
        {
#define _PROP(TYPE, NAME, ...)                                                                                         \
    if (NAME.dirty())                                                                                                  \
        return true;
#define _ARRAY(TYPE, NAME, COUNT, ...)                                                                                 \
    for (const auto& p : NAME)                                                                                         \
        if (p.dirty())                                                                                                 \
            return true;
            _DETAILS_V1_PROPS_X(_PROP, _ARRAY)
#undef _PROP
#undef _ARRAY
            return false;
        }

        String mUniqueId;
        String mFWVersion;
        String mModelName;
        v1::t1::ModelSeries mModelSeries{};
        v1::ModelColor mModelColor{};
        v1::t1::AudioCodec mAudioCodec{};

        v1::t1::AlertMessageType mLastAlertMessage{};
        bool mAlertAwaitingResponse{};
        uint64_t mAlertSerial{};

        String mLastInteractionMessage;
        String mLastDeviceJSONMessage;

        Vector<PeripheralDevice> mPairedDevices;
        UInt8 mPairedDevicesPlaybackDeviceID{};
        int mSafeListeningSoundPressure{};

        BatteryState mBatteryL, mBatteryR, mBatteryCase;

        String mPlayTrackTitle;
        String mPlayTrackAlbum;
        String mPlayTrackArtist;
        v1::t1::PlaybackStatus mPlayPause{};

        v1::t1::UpscalingType mUpscalingType{};
        bool mUpscalingAvailable{true};

        Vector<v1::t1::AsCapabilityKey> mAssignableSettingsKeys;

        GsCapability mGsCapability[3];
        v1::t1::CommonOnOffSettingValue mSpeakToChatVoiceFocus{v1::t1::CommonOnOffSettingValue::OFF};

        Vector<EqPresetInfo> mEqPresets;
        UInt8 mSourceSwitchControlResult{};
    };

#undef _DETAILS_V1_PROPS_X

} // namespace mdr
