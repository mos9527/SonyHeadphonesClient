#include <algorithm>

#include "Headphones.hpp"
using namespace v2::t1;

void HandleProtocolInfoT1(MDRHeadphones* self, Span<const UInt8> cmd)
{
    ConnectRetProtocolInfo res;
    ConnectRetProtocolInfo::Deserialize(cmd.data(), res);
    self->mProtocol = {
        .version = res.protocolVersion,
        .hasTable1 = res.supportTable1Value == v2::MessageMdrV2EnableDisable::ENABLE,
        .hasTable2 = res.supportTable1Value == v2::MessageMdrV2EnableDisable::ENABLE
    };
}

void HandleSupportFunctionT1(MDRHeadphones* self, Span<const UInt8> cmd)
{
    ConnectRetSupportFunction res;
    ConnectRetSupportFunction::Deserialize(cmd.data(), res);
    std::ranges::fill(self->mSupport.table1Functions, 0);
    for (auto fun : res.supportFunctions)
        self->mSupport.table1Functions[static_cast<UInt8>(fun.table1)] = true;
}

void HandleCapabilityInfoT1(MDRHeadphones* self, Span<const UInt8> cmd)
{
    ConnectRetCapabilityInfo res;
    ConnectRetCapabilityInfo::Deserialize(cmd.data(), res);
    self->mUniqueId = res.uniqueID.value;
}

void HandleDeviceInfoT1(MDRHeadphones* self, Span<const UInt8> cmd)
{
    ConnectRetDeviceInfo res;
    ConnectRetDeviceInfo::Deserialize(cmd.data(), res);
    std::visit(
        Visitor(
            [self](ConnectRetDeviceInfoModelName const& v)
            {
                self->mModelName = v.value.value;
            },
            [self](ConnectRetDeviceInfoFwVersion const& v)
            {
                self->mFWVersion = v.value.value;
            },
            [self](ConnectRetDeviceInfoSeriesAndColor const& v)
            {
                self->mModelColor = v.color;
                self->mModelSeries = v.series;
            }
            ), res.info);
}

void HandleCommonStatusT1(MDRHeadphones* self, Span<const UInt8> cmd)
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
        break;
    }
    default:
        break;
    }
}

void HandleNcAsmT1(MDRHeadphones* self, Span<const UInt8> cmd)
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
            self->mPropertyNcAsmEnabled.overwrite(res.base.ncAsmTotalEffect == NcAsmOnOffValue::ON);
            self->mPropertyNcAsmMode.overwrite(res.ncAsmMode);
            self->mPropertyNcAsmFocusOnVoice.overwrite(res.ambientSoundMode == AmbientSoundMode::VOICE);
            self->mPropertyNcAsmAmbientLevel.overwrite(res.ambientSoundLevelValue);
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
            self->mPropertyNcAsmEnabled.overwrite(res.base.ncAsmTotalEffect == NcAsmOnOffValue::ON);
            self->mPropertyNcAsmMode.overwrite(res.ncAsmMode);
            self->mPropertyNcAsmFocusOnVoice.overwrite(res.ambientSoundMode == AmbientSoundMode::VOICE);
            self->mPropertyNcAsmAmbientLevel.overwrite(res.ambientSoundLevelValue);
            self->mPropertyNcAsmAutoAsmEnabled.overwrite(res.noiseAdaptiveOnOffValue == NcAsmOnOffValue::ON);
            self->mPropertyNcAsmNoiseAdaptiveSensitivity.overwrite(res.noiseAdaptiveSensitivitySettings);
        }
        break;
    }
    case ASM_SEAMLESS:
    {
        if (self->mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            NcAsmParamAsmSeamless res;
            NcAsmParamAsmSeamless::Deserialize(cmd.data(), res);
            self->mPropertyNcAsmEnabled.overwrite(res.base.ncAsmTotalEffect == NcAsmOnOffValue::ON);
            self->mPropertyNcAsmFocusOnVoice.overwrite(res.ambientSoundMode == AmbientSoundMode::VOICE);
            self->mPropertyNcAsmAmbientLevel.overwrite(res.ambientSoundLevelValue);
        }
        break;
    }
    case NC_AMB_TOGGLE:
    {
        if (self->mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_CONTROL_MODE_SELECT))
        {
            NcAsmParamNcAmbToggle res;
            NcAsmParamNcAmbToggle::Deserialize(cmd.data(), res);
            self->mPropertyNcAsmButtonFunction.overwrite(res.function);
        }
        break;
    }
    default:
        break;
    }
}

void HandlePower(MDRHeadphones* self, Span<const UInt8> cmd)
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
            self->mPropertyPowerAutoOff.overwrite(res.currentPowerOffElements);
        }
        break;
    }
    case AUTO_POWER_OFF_WEARING_DETECTION:
    {
        if (self->mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
        {
            PowerParamAutoPowerOffWithWearingDetection res;
            PowerParamAutoPowerOffWithWearingDetection::Deserialize(cmd.data(), res);
            self->mPropertyPowerAutoOffWearingDetection.overwrite(res.currentPowerOffElements);
        }
        break;
    }
    default:
        break;
    }
}

int MDRHeadphones::HandleCommandV2T1(Span<const UInt8> cmd, MDRCommandSeqNumber seq)
{
    CommandBase base;
    CommandBase::Deserialize(cmd.data(), base);
    using enum Command;
    switch (base.command)
    {
    case CONNECT_RET_PROTOCOL_INFO:
        HandleProtocolInfoT1(this, cmd);
        Awake(AWAIT_PROTOCOL_INFO);
        return MDR_HEADPHONES_STATE_UPDATE;
    case CONNECT_RET_SUPPORT_FUNCTION:
        HandleSupportFunctionT1(this, cmd);
        Awake(AWAIT_SUPPORT_FUNCTION);
        return MDR_HEADPHONES_STATE_UPDATE;
    case CONNECT_RET_CAPABILITY_INFO:
        HandleCapabilityInfoT1(this, cmd);
        return MDR_HEADPHONES_STATE_UPDATE;
    case CONNECT_RET_DEVICE_INFO:
        HandleDeviceInfoT1(this, cmd);
        return MDR_HEADPHONES_STATE_UPDATE;
    case COMMON_RET_STATUS:
    case COMMON_NTFY_STATUS:
        HandleCommonStatusT1(this, cmd);
        return MDR_HEADPHONES_STATE_UPDATE;
    case NCASM_RET_PARAM:
    case NCASM_NTFY_PARAM:
        HandleNcAsmT1(this, cmd);
        return MDR_HEADPHONES_STATE_UPDATE;
    case POWER_RET_STATUS:
    case POWER_NTFY_STATUS:
        HandlePower(this, cmd);
        return MDR_HEADPHONES_STATE_UPDATE;
    default:
        fmt::println("^^ Unhandled {}", base.command);
    }
    return MDR_HEADPHONES_NO_EVENT;
}
