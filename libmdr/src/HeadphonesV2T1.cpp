#include <mdr/Headphones.hpp>
#include <algorithm>
namespace mdr
{
    using namespace v2::t1;

    int HandleProtocolInfoT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        ConnectRetProtocolInfo res;
        ConnectRetProtocolInfo::Deserialize(cmd.data(), res);
        self->mProtocol = {
            .version = res.protocolVersion,
            .hasTable1 = res.supportTable1Value == v2::MessageMdrV2EnableDisable::ENABLE,
            .hasTable2 = res.supportTable1Value == v2::MessageMdrV2EnableDisable::ENABLE
        };
        self->Awake(MDRHeadphones::AWAIT_PROTOCOL_INFO);
        return MDR_HEADPHONES_STATE_UPDATE; // <- XXX Change these
    }

    int HandleSupportFunctionT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        ConnectRetSupportFunction res;
        ConnectRetSupportFunction::Deserialize(cmd.data(), res);
        std::ranges::fill(self->mSupport.table1Functions, false);
        for (auto fun : res.supportFunctions)
            self->mSupport.table1Functions[static_cast<UInt8>(fun.table1)] = true;
        self->Awake(MDRHeadphones::AWAIT_SUPPORT_FUNCTION);
        return MDR_HEADPHONES_STATE_UPDATE;
    }

    int HandleCapabilityInfoT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        ConnectRetCapabilityInfo res;
        ConnectRetCapabilityInfo::Deserialize(cmd.data(), res);
        self->mUniqueId = res.uniqueID.value;
        return MDR_HEADPHONES_STATE_UPDATE;
    }

    int HandleDeviceInfoT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        ConnectRetDeviceInfoBase base;
        ConnectRetDeviceInfoBase::Deserialize(cmd.data(), base);
        using enum DeviceInfoType;
        switch (base.type)
        {
        case MODEL_NAME:
        {
            ConnectRetDeviceInfoModelName res;
            ConnectRetDeviceInfoModelName::Deserialize(cmd.data(), res);
            self->mModelName = res.value.value;
            return MDR_HEADPHONES_STATE_UPDATE;
        }
        case FW_VERSION:
        {
            ConnectRetDeviceInfoFwVersion res;
            ConnectRetDeviceInfoFwVersion::Deserialize(cmd.data(), res);
            self->mFWVersion = res.value.value;
            return MDR_HEADPHONES_STATE_UPDATE;
        }
        case SERIES_AND_COLOR_INFO:
        {
            ConnectRetDeviceInfoSeriesAndColor res;
            ConnectRetDeviceInfoSeriesAndColor::Deserialize(cmd.data(), res);
            self->mModelSeries = res.series;
            self->mModelColor = res.color;
            return MDR_HEADPHONES_STATE_UPDATE;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_NO_EVENT;
    }

    int HandleCommonStatusT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        CommonBase base;
        CommonBase::Deserialize(cmd.data(), base);
        using enum CommonInquiredType;
        switch (base.type)
        {
        case AUDIO_CODEC:
        {
            CommonStatusAudioCodec res;
            CommonStatusAudioCodec::Deserialize(cmd.data(), res);
            self->mAudioCodec = res.audioCodec;
            return MDR_HEADPHONES_STATE_UPDATE;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_NO_EVENT;
    }

    int HandleNcAsmParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        NcAsmParamBase base;
        NcAsmParamBase::Deserialize(cmd.data(), base);
        using enum NcAsmInquiredType;
        switch (base.type)
        {
        case MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS:
        {
            if (self->mSupport.contains(
                v2::MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
            {
                NcAsmParamModeNcDualModeSwitchAsmSeamless res;
                NcAsmParamModeNcDualModeSwitchAsmSeamless::Deserialize(cmd.data(), res);
                self->mNcAsmEnabled.overwrite(res.base.ncAsmTotalEffect == NcAsmOnOffValue::ON);
                self->mNcAsmMode.overwrite(res.ncAsmMode);
                self->mNcAsmFocusOnVoice.overwrite(res.ambientSoundMode == AmbientSoundMode::VOICE);
                self->mNcAsmAmbientLevel.overwrite(res.ambientSoundLevelValue);
                return MDR_HEADPHONES_STATE_UPDATE;
            }
            break;
        }
        case MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA:
        {
            if (self->mSupport.contains(
                v2::MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
            {
                NcAsmParamModeNcDualModeSwitchAsmSeamlessNa res;
                NcAsmParamModeNcDualModeSwitchAsmSeamlessNa::Deserialize(cmd.data(), res);
                self->mNcAsmEnabled.overwrite(res.base.ncAsmTotalEffect == NcAsmOnOffValue::ON);
                self->mNcAsmMode.overwrite(res.ncAsmMode);
                self->mNcAsmFocusOnVoice.overwrite(res.ambientSoundMode == AmbientSoundMode::VOICE);
                self->mNcAsmAmbientLevel.overwrite(res.ambientSoundLevelValue);
                self->mNcAsmAutoAsmEnabled.overwrite(res.noiseAdaptiveOnOffValue == NcAsmOnOffValue::ON);
                self->mNcAsmNoiseAdaptiveSensitivity.overwrite(res.noiseAdaptiveSensitivitySettings);
                return MDR_HEADPHONES_STATE_UPDATE;
            }
            break;
        }
        case ASM_SEAMLESS:
        {
            if (self->mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
            {
                NcAsmParamAsmSeamless res;
                NcAsmParamAsmSeamless::Deserialize(cmd.data(), res);
                self->mNcAsmEnabled.overwrite(res.base.ncAsmTotalEffect == NcAsmOnOffValue::ON);
                self->mNcAsmFocusOnVoice.overwrite(res.ambientSoundMode == AmbientSoundMode::VOICE);
                self->mNcAsmAmbientLevel.overwrite(res.ambientSoundLevelValue);
                return MDR_HEADPHONES_STATE_UPDATE;
            }
            break;
        }
        case NC_AMB_TOGGLE:
        {
            if (self->mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_CONTROL_MODE_SELECT))
            {
                NcAsmParamNcAmbToggle res;
                NcAsmParamNcAmbToggle::Deserialize(cmd.data(), res);
                self->mNcAsmButtonFunction.overwrite(res.function);
                return MDR_HEADPHONES_STATE_UPDATE;
            }
            break;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_NO_EVENT;
    }

    int HandlePowerStatusT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        PowerBase base;
        PowerBase::Deserialize(cmd.data(), base);
        using enum PowerInquiredType;
        switch (base.type)
        {
        case AUTO_POWER_OFF:
        {
            if (self->mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF))
            {
                PowerParamAutoPowerOff res;
                PowerParamAutoPowerOff::Deserialize(cmd.data(), res);
                self->mPowerAutoOff.overwrite(res.currentPowerOffElements);
                return MDR_HEADPHONES_STATE_UPDATE;
            }
            break;
        }
        case AUTO_POWER_OFF_WEARING_DETECTION:
        {
            if (self->mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
            {
                PowerParamAutoPowerOffWithWearingDetection res;
                PowerParamAutoPowerOffWithWearingDetection::Deserialize(cmd.data(), res);
                self->mPowerAutoOffWearingDetection.overwrite(res.currentPowerOffElements);
                return MDR_HEADPHONES_STATE_UPDATE;
            }
            break;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_NO_EVENT;
    }

    int HandlePlaybackStatusT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        PlayBase base;
        PlayBase::Deserialize(cmd.data(), base);
        using enum PlayInquiredType;
        switch (base.type)
        {
        case PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT:
        {
            PlayStatusPlaybackController res;
            PlayStatusPlaybackController::Deserialize(cmd.data(), res);
            self->mPlaybackStatus.overwrite(res.playbackStatus);
            return MDR_HEADPHONES_STATE_UPDATE;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_NO_EVENT;
    }

    int HandleGsCapabilityT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        GsRetCapability res;
        GsRetCapability::Deserialize(cmd.data(), res);
        using enum GsInquiredType;
        switch (res.type)
        {
        case GENERAL_SETTING1:
        {
            self->mGsCapability1.overwrite({res.settingType, res.settingInfo});
            return MDR_HEADPHONES_STATE_UPDATE;
        }
        case GENERAL_SETTING2:
        {
            self->mGsCapability2.overwrite({res.settingType, res.settingInfo});
            return MDR_HEADPHONES_STATE_UPDATE;
        }
        case GENERAL_SETTING3:
        {
            self->mGsCapability3.overwrite({res.settingType, res.settingInfo});
            return MDR_HEADPHONES_STATE_UPDATE;
        }
        case GENERAL_SETTING4:
        {
            self->mGsCapability4.overwrite({res.settingType, res.settingInfo});
            return MDR_HEADPHONES_STATE_UPDATE;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_NO_EVENT;
    }

    int HandleGsParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        GsParamBase base;
        GsParamBase::Deserialize(cmd.data(), base);
        using enum GsSettingType;
        using enum GsInquiredType;
        auto Write = [&](MDRProperty<bool>& dstBool) -> int
        {
            switch (base.settingType)
            {
            case BOOLEAN_TYPE:
            {
                GsParamBoolean res;
                GsParamBoolean::Deserialize(cmd.data(), res);
                dstBool.overwrite(res.settingValue == GsSettingValue::ON);
                return MDR_HEADPHONES_STATE_UPDATE;
            }
            default:
                break;
            }
            return MDR_HEADPHONES_NO_EVENT;
        };
        switch (base.type)
        {
        case GENERAL_SETTING1:
        {
            return Write(self->mGsParamBool1);
        }
        case GENERAL_SETTING2:
        {
            return Write(self->mGsParamBool2);
        }
        case GENERAL_SETTING3:
        {
            return Write(self->mGsParamBool3);
        }
        case GENERAL_SETTING4:
        {
            return Write(self->mGsParamBool4);
        }
        default:
            break;
        }
        return MDR_HEADPHONES_NO_EVENT;
    }

    int HandleAudioCapabilityT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        AudioBase base;
        AudioBase::Deserialize(cmd.data(), base);
        using enum AudioInquiredType;
        switch (base.type)
        {
        case UPSCALING:
        {
            if (self->mSupport.contains(v2::MessageMdrV2FunctionType_Table1::UPSCALING_AUTO_OFF))
            {
                AudioRetCapabilityUpscaling res;
                AudioRetCapabilityUpscaling::Deserialize(cmd.data(), res);
                self->mUpscalingType.overwrite(res.upscalingType);
                return MDR_HEADPHONES_STATE_UPDATE;
            }
            return MDR_HEADPHONES_NO_EVENT;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_NO_EVENT;
    }

    int HandleAudioStatusT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        AudioBase base;
        AudioBase::Deserialize(cmd.data(), base);
        using enum AudioInquiredType;
        switch (base.type)
        {
        case UPSCALING:
        {
            if (self->mSupport.contains(v2::MessageMdrV2FunctionType_Table1::UPSCALING_AUTO_OFF))
            {
                AudioStatusCommon res;
                AudioStatusCommon::Deserialize(cmd.data(), res);
                self->mUpscalingAvailable.overwrite(res.status == v2::MessageMdrV2EnableDisable::ENABLE);
                return MDR_HEADPHONES_STATE_UPDATE;
            }
            return MDR_HEADPHONES_NO_EVENT;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_NO_EVENT;
    }

    int HandleAudioParamT1(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        AudioBase base;
        AudioBase::Deserialize(cmd.data(), base);
        using enum AudioInquiredType;
        switch (base.type)
        {
        case CONNECTION_MODE:
        {
            if (self->mSupport.contains(
                v2::MessageMdrV2FunctionType_Table1::CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY))
            {
                AudioParamConnection res;
                AudioParamConnection::Deserialize(cmd.data(), res);
                self->mAudioPriorityMode.overwrite(res.settingValue);
                return MDR_HEADPHONES_STATE_UPDATE;
            }
            return MDR_HEADPHONES_NO_EVENT;
        }
        case UPSCALING:
        {
            if (self->mSupport.contains(v2::MessageMdrV2FunctionType_Table1::UPSCALING_AUTO_OFF))
            {
                AudioParamUpscaling res;
                AudioParamUpscaling::Deserialize(cmd.data(), res);
                self->mUpscalingEnabled.overwrite(res.settingValue == UpscalingTypeAutoOff::AUTO);
                return MDR_HEADPHONES_STATE_UPDATE;
            }
            return MDR_HEADPHONES_NO_EVENT;
        }
        case BGM_MODE:
        {
            if (self->mSupport.contains(v2::MessageMdrV2FunctionType_Table1::LISTENING_OPTION))
            {
                AudioParamBGMMode res;
                AudioParamBGMMode::Deserialize(cmd.data(), res);
                self->mBGMModeEnabled.overwrite(res.onOffSettingValue == v2::MessageMdrV2EnableDisable::ENABLE);
                self->mBGMModeRoomSize.overwrite(res.targetRoomSize);
                return MDR_HEADPHONES_STATE_UPDATE;
            }
            return MDR_HEADPHONES_NO_EVENT;
        }
        case UPMIX_CINEMA:
        {
            if (self->mSupport.contains(v2::MessageMdrV2FunctionType_Table1::LISTENING_OPTION))
            {
                AudioParamUpmixCinema res;
                AudioParamUpmixCinema::Deserialize(cmd.data(), res);
                self->mUpmixCinemaEnabled.overwrite(res.onOffSettingValue == v2::MessageMdrV2EnableDisable::ENABLE);
                return MDR_HEADPHONES_STATE_UPDATE;
            }
            return MDR_HEADPHONES_NO_EVENT;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_NO_EVENT;
    }

    int MDRHeadphones::HandleCommandV2T1(Span<const UInt8> cmd, MDRCommandSeqNumber seq)
    {
        CommandBase base;
        CommandBase::Deserialize(cmd.data(), base);
        using enum Command;
        switch (base.command)
        {
        case CONNECT_RET_PROTOCOL_INFO:
            return HandleProtocolInfoT1(this, cmd);
        case CONNECT_RET_SUPPORT_FUNCTION:
            return HandleSupportFunctionT1(this, cmd);
        case CONNECT_RET_CAPABILITY_INFO:
            return HandleCapabilityInfoT1(this, cmd);
        case CONNECT_RET_DEVICE_INFO:
            return HandleDeviceInfoT1(this, cmd);
        case COMMON_RET_STATUS:
        case COMMON_NTFY_STATUS:
            return HandleCommonStatusT1(this, cmd);
        case NCASM_RET_PARAM:
        case NCASM_NTFY_PARAM:
            return HandleNcAsmParamT1(this, cmd);
        case POWER_RET_STATUS:
        case POWER_NTFY_STATUS:
            return HandlePowerStatusT1(this, cmd);
        case PLAY_RET_STATUS:
        case PLAY_NTFY_STATUS:
            return HandlePlaybackStatusT1(this, cmd);
        case GENERAL_SETTING_RET_CAPABILITY:
            return HandleGsCapabilityT1(this, cmd);
        case GENERAL_SETTING_RET_STATUS:
        case GENERAL_SETTING_NTFY_STATUS:
            return HandleGsParamT1(this, cmd);
        case AUDIO_RET_CAPABILITY:
            return HandleAudioCapabilityT1(this, cmd);
        case AUDIO_RET_STATUS:
        case AUDIO_NTFY_STATUS:
            return HandleAudioStatusT1(this, cmd);
        case AUDIO_RET_PARAM:
        case AUDIO_NTFY_PARAM:
            return HandleAudioParamT1(this, cmd);
        default:
            fmt::println("^^ Unhandled {}", base.command);
        }
        return MDR_HEADPHONES_NO_EVENT;
    }
}
