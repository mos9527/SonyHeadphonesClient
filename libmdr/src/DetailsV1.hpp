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
            Vector<v1::t1::FunctionType> order;
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
        String mPlayTrackGenre;
        v1::t1::PlaybackStatus mPlayPause{};

        v1::t1::UpscalingType mUpscalingType{};
        bool mUpscalingAvailable{true};

        struct NCOptimizerState
        {
            v1::t1::RetOptimizerCapability capability{};
            v1::t1::RetOptimizerStatus status{};
            v1::t1::RetOptimizerParam parameters{};
            bool capabilityKnown{};
            bool statusKnown{};
            bool parametersKnown{};

            [[nodiscard]] bool available() const
            {
                return capabilityKnown && statusKnown && status.status == v1::CommonStatus::ENABLE &&
                    (capability.personalType == v1::t1::PersonalMeasureType::PERSONAL ||
                     capability.barometricType == v1::t1::BarometricMeasureType::BAROMETRIC_PRESSURE);
            }

            [[nodiscard]] bool running() const
            {
                using enum v1::t1::OptimizerStatus;
                return statusKnown && (status.optimizerStatus == IN_PROGRESS_OF_PERSONAL ||
                    status.optimizerStatus == IN_PROGRESS_OF_BAROMETRIC_PRESSURE ||
                    status.optimizerStatus == OPTIMIZING);
            }
        } mNCOptimizer;
        // Capability-derived selectors used by Sony's initial holder reloads.
        v1::t1::EqEbbInquiredType mEqType{v1::t1::EqEbbInquiredType::NO_USE};
        UInt8 mEqBandCount{};
        UInt8 mEqStepCount{};
        Vector<UInt8> mEqBandSteps;
        int mEqClearBassBand{-1};
        Vector<v1::t1::EqBandInformation> mEqBandInformation;
        v1::t1::RetEqEbbCapability_EbbCapability mEbbCapability{};
        bool mEbbAvailable{};
        bool mNcAsmAvailable{};
        bool mAudioPriorityAvailable{};
        v1::t1::NcSettingType mNcSettingType{};
        v1::t1::NcAsmSettingType mNcAsmSettingType{};
        v1::t1::AsmSettingType mAsmSettingType{};
        UInt8 mNcStep{};
        Vector<v1::t1::Asm> mAsmCapabilities;
        v1::t1::ConnectionStatus mConnectionL{}, mConnectionR{};
        bool mPlayAvailable{};
        bool mVoiceGuidanceSupportsOnOff{};
        bool mVoiceGuidanceSupportsLanguage{};
        bool mVoiceGuidanceAvailable{};
        v1::t1::TrainingModeAvailableEffectType mTrainingEffect{};
        Vector<v1::t1::AutoPowerOffElementId> mAutoPowerOffCandidates;

        [[nodiscard]] bool wearingPowerPossible() const
        {
            if (!mSupport.contains(v1::t1::FunctionType::AUTO_POWER_OFF))
                return false;
            for (auto value : mAutoPowerOffCandidates)
                if (value == v1::t1::AutoPowerOffElementId::POWER_OFF_WHEN_REMOVED_FROM_EARS)
                    return true;
            return false;
        }
        Array<bool, 256> mSystemAvailable{};
        Array<bool, 3> mGsAvailable{};
        Array<UInt8, 3> mGsListIndex{};

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
        MDRProperty<v1::t1::AutoPowerOffElementId> mPowerAutoOffSelectedTime;
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

} // namespace mdr
