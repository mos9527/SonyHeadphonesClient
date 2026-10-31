#pragma once

#include <mdr/ProtocolV2T1.hpp>
#include <mdr/ProtocolV2T2.hpp>

#include "Property.hpp"

/*
* X-macro madness.
*
* All `MDRProperty<T>` fields are defined here. As to why, this is to support `DetailsV2::submit` and `DetailsV2::dirty`,
* which believe it or not is used to be written *by hand*.
* 
* For both V1/V2 Details implemementation:
* - Define `MDRProperty<T> FIELD_NAME`, ALWAYS in `_DETAILS_V2_PROPS_X`/`_DETAILS_V1_PROPS_X`, with PROP(T, FIELD_NAME)
* - Define `MDRProperty<T> FIELD_NAME[COUNT]`, ALWAYS in `_DETAILS_V2_PROPS_X`/`_DETAILS_V1_PROPS_X`, with ARRAY(T, FIELD_NAME, COUNT)
* 
* Furthermore on coding style, which should also ALWAYS be followed, in this order:
* - Declare `struct`s for all fields
* - Declare macro expansions [`MDRProperty` expansion -> `submit` expansion -> `dirty` expansion] (always present)
* - Declare Read-only fields
* - Declare Functions.
*/
#define _DETAILS_V2_PROPS_X(PROP, ARRAY) \
    PROP(bool, mShutdown) \
    PROP(bool, mNcAsmEnabled) \
    PROP(bool, mNcAsmFocusOnVoice) \
    PROP(int, mNcAsmAmbientLevel) \
    PROP(bool, mNcAsmChangingAsmLevel) \
    PROP(v2::t1::Function, mNcAsmButtonFunction) \
    PROP(v2::t1::NcAsmMode, mNcAsmMode) \
    PROP(bool, mNcAsmAutoAsmEnabled) \
    PROP(v2::t1::NoiseAdaptiveSensitivity, mNcAsmNoiseAdaptiveSensitivity) \
    PROP(v2::t1::AutoPowerOffElements, mPowerAutoOff) \
    PROP(v2::t1::AutoPowerOffWearingDetectionElements, mPowerAutoOffWearingDetection) \
    PROP(int, mPlayVolume) \
    PROP(v2::t1::PlaybackControl, mPlayControl) \
    ARRAY(bool, mGsParamBool, 4) \
    PROP(bool, mUpscalingEnabled) \
    PROP(v2::t1::PriorMode, mAudioPriorityMode) \
    PROP(bool, mBGMModeEnabled) \
    PROP(v2::t1::RoomSize, mBGMModeRoomSize) \
    PROP(bool, mUpmixCinemaEnabled) \
    PROP(bool, mVoiceContentsEnabled) \
    PROP(bool, mSoundLeakageReductionEnabled) \
    PROP(bool, mAutoPauseEnabled) \
    PROP(Vector<v2::t1::Preset>, mAssignableSettingsPresets) \
    PROP(bool, mSpeakToChatEnabled) \
    PROP(v2::t1::DetectSensitivity, mSpeakToChatDetectSensitivity) \
    PROP(v2::t1::ModeOutTime, mSpeakToModeOutTime) \
    PROP(bool, mHeadGestureEnabled) \
    PROP(bool, mEqAvailable, {true, true, true}) \
    PROP(v2::t1::EqPresetId, mEqPresetId) \
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
    struct DetailsV2
    {
        struct AlertResponse
        {
            v2::t1::AlertMessageType messageType{};
            v2::t1::AlertAction action{};
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
            v2::t1::BatteryChargingStatus charging{};
        };

        struct ProtocolStates
        {
            int version{};
            int hasTable1{};
            int hasTable2{};
        } mProtocol{};

        struct GsCapability
        {
            v2::t1::GsSettingType type{};
            v2::t1::GsSettingInfo value{};
        };

        struct EqPresetInfo
        {
            v2::t1::EqPresetId presetId{};
            String name;
        };

        
        struct SupportStates
        {
            enum State
            {
                UNKNOWN,
                ADVERTISED,
                LEGACY_PROFILE
            };

            Array<bool, 256> table1Functions{};
            Array<bool, 256> table2Functions{};
            State state{UNKNOWN};

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
                return contains(F::PRESET_EQ) || contains(F::CUSTOM_EQ) || contains(F::PRESET_EQ_NON_CUSTOMIZABLE) ||
                    contains(F::PRESET_EQ_AND_ULT_MODE) || contains(F::SOUND_EFFECT) || contains(F::TURN_KEY_EQ) ||
                    contains(F::PRESET_EQ_AND_ERRORCODE) || contains(F::CUSTOMIZABLE_SOUND_EFFECT);
            }

            [[nodiscard]] constexpr bool containsVoiceGuidance() const
            {
                using F = v2::t2::FunctionType;
                return contains(
                           F::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH) ||
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

#define _PROP(TYPE, NAME, ...) MDRProperty<TYPE> NAME __VA_ARGS__;
#define _ARRAY(TYPE, NAME, COUNT, ...) MDRProperty<TYPE> NAME[COUNT] __VA_ARGS__;
        _DETAILS_V2_PROPS_X(_PROP, _ARRAY)
#undef _PROP
#undef _ARRAY

        void submit()
        {
#define _PROP(TYPE, NAME, ...) NAME.submit();
#define _ARRAY(TYPE, NAME, COUNT, ...)                                                                                 \
    for (auto& p : NAME)                                                                                               \
        p.submit();
            _DETAILS_V2_PROPS_X(_PROP, _ARRAY)
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
            _DETAILS_V2_PROPS_X(_PROP, _ARRAY)
#undef _PROP
#undef _ARRAY
            return false;
        }

        String mUniqueId;
        String mFWVersion;
        String mModelName;
        v2::t1::ModelSeries mModelSeries{};
        v2::ModelColor mModelColor{};
        v2::t1::AudioCodec mAudioCodec{};

        v2::t1::AlertMessageType mLastAlertMessage{};
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
        v2::t1::PlaybackStatus mPlayPause{};
        v2::t2::WearingStatusCode mWearingStatus{v2::t2::WearingStatusCode::OUT_OF_RANGE};

        v2::t1::UpscalingType mUpscalingType{};
        bool mUpscalingAvailable{true};

        Vector<v2::t1::AssignableSettingsKey> mAssignableSettingsKeys;

        GsCapability mGsCapability[4];
        UInt8 mSpeakToChatVoiceFocus{};

        Vector<EqPresetInfo> mEqPresets;
        v2::t2::SourceSwitchControlResult mSourceSwitchControlResult{v2::t2::SourceSwitchControlResult::SUCCESS};

        [[nodiscard]] bool wearingSensorPossible() const
        {
            // See https://github.com/mos9527/SonyHeadphonesClient/pull/63 as to why `WEARING_STATUS_CHECKER` is not
            // enough
            return mProtocol.hasTable2 &&
                (mSupport.contains(v2::t2::FunctionType::WEARING_STATUS_CHECKER) ||
                 mSupport.contains(v2::t1::FunctionType::WEARING_STATUS_DETECTOR) ||
                 mSupport.contains(v2::t1::FunctionType::PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF));
        }

    };

#undef _DETAILS_V2_PROPS_X

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

} // namespace mdr
