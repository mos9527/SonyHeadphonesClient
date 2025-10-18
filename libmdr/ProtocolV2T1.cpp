#include "ProtocolV2T1.hpp"

namespace mdr::v2::t1
{

    size_t ConnectRetDeviceInfo::Serialize(const ConnectRetDeviceInfo& data, UInt8* out)
    {
        using enum DeviceInfoType;
        UInt8* ptr = out;
        MDRPod::Write(data.command, &ptr);
        MDRPod::Write(data.type, &ptr);
        switch (data.type)
        {
        case MODEL_NAME:
            MDRPrefixedString::Write(std::get<ConnectRetDeviceInfoModelName>(data.info).value, &ptr);
            break;
        case FW_VERSION:
            MDRPrefixedString::Write(std::get<ConnectRetDeviceInfoFwVersion>(data.info).value, &ptr);
            break;
        case SERIES_AND_COLOR_INFO:
            MDRPod::Write(std::get<ConnectRetDeviceInfoSeriesAndColor>(data.info), &ptr);
            break;
        default:
        case INSTRUCTION_GUIDE:
            [[unlikely]]
                throw std::runtime_error("Unsupported INSTRUCTION_GUIDE");
        }
        return ptr - out;
    }

    void ConnectRetDeviceInfo::Deserialize(UInt8* data, ConnectRetDeviceInfo& out)
    {
        using enum DeviceInfoType;
        MDRPod::Read(&data, out.command);
        MDRPod::Read(&data, out.type);
        switch (out.type)
        {
        case MODEL_NAME:
            MDRPrefixedString::Read(&data, out.info.emplace<ConnectRetDeviceInfoModelName>().value);
            break;
        case FW_VERSION:
            MDRPrefixedString::Read(&data, out.info.emplace<ConnectRetDeviceInfoFwVersion>().value);
            break;
        case SERIES_AND_COLOR_INFO:
            MDRPod::Read(&data, out.info.emplace<ConnectRetDeviceInfoSeriesAndColor>());
            break;
        default:
        case INSTRUCTION_GUIDE:
            [[unlikely]]
                throw std::runtime_error("Unsupported INSTRUCTION_GUIDE");
        }
    }

    size_t ConnectRetSupportFunction::Serialize(const ConnectRetSupportFunction& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.inquiredType, &ptr);
        MDRPodArray<MessageMdrV2SupportFunction>::Write(data.supportFunctions, &ptr);
        return ptr - out;
    }

    void ConnectRetSupportFunction::Deserialize(UInt8* data, ConnectRetSupportFunction& out)
    {
        MDRPod::Read(&data, out.command);
        MDRPod::Read(&data, out.inquiredType);
        MDRPodArray<MessageMdrV2SupportFunction>::Read(&data, out.supportFunctions);
    }

    size_t EqEbbStatusErrorCode::Serialize(const EqEbbStatusErrorCode& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &ptr);
        MDRPod::Write(data.value, &ptr);
        MDRPodArray<PresetEqErrorCodeType>::Write(data.errors, &ptr);
        return ptr - out;
    }

    void EqEbbStatusErrorCode::Deserialize(UInt8* data, EqEbbStatusErrorCode& out)
    {
        MDRPod::Read(&data, out.base);
        MDRPod::Read(&data, out.value);
        MDRPodArray<PresetEqErrorCodeType>::Read(&data, out.errors);
    }

    size_t EqEbbParamEq::Serialize(const EqEbbParamEq& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &ptr);
        MDRPod::Write(data.presetId, &ptr);
        MDRPodArray<UInt8>::Write(data.bands, &ptr);
        return ptr - out;
    }

    void EqEbbParamEq::Deserialize(UInt8* data, EqEbbParamEq& out)
    {
        MDRPod::Read(&data, out.base);
        MDRPod::Read(&data, out.presetId);
        MDRPodArray<UInt8>::Read(&data, out.bands);
        // TODO: Check bands count?
    }

    size_t EqEbbParamEqAndUltMode::Serialize(const EqEbbParamEqAndUltMode& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &ptr);
        MDRPod::Write(data.presetId, &ptr);
        MDRPod::Write(data.eqUltModeStatus, &ptr);
        MDRPodArray<UInt8>::Write(data.bandSteps, &ptr);
        return ptr - out;
    }

    void EqEbbParamEqAndUltMode::Deserialize(UInt8* data, EqEbbParamEqAndUltMode& out)
    {
        MDRPod::Read(&data, out.base);
        MDRPod::Read(&data, out.presetId);
        MDRPod::Read(&data, out.eqUltModeStatus);
        MDRPodArray<UInt8>::Read(&data, out.bandSteps);
    }

    size_t EqEbbParamCustomEq::Serialize(const EqEbbParamCustomEq& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &ptr);
        MDRPodArray<UInt8>::Write(data.bandSteps, &ptr);
        return ptr - out;
    }

    void EqEbbParamCustomEq::Deserialize(UInt8* data, EqEbbParamCustomEq& out)
    {
        MDRPod::Read(&data, out.base);
        MDRPodArray<UInt8>::Read(&data, out.bandSteps);
    }

    size_t AlertRetStatusVoiceAssistant::Serialize(const AlertRetStatusVoiceAssistant& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &ptr);
        MDRPodArray<VoiceAssistantType>::Write(data.voiceAssistants, &ptr);
        return ptr - out;
    }

    void AlertRetStatusVoiceAssistant::Deserialize(UInt8* data, AlertRetStatusVoiceAssistant& out)
    {
        MDRPod::Read(&data, out.base);
        MDRPodArray<VoiceAssistantType>::Read(&data, out.voiceAssistants);
    }

    size_t PlaybackName::Write(const PlaybackName& data, UInt8** ppDstBuffer)
    {
        UInt8* ptr = *ppDstBuffer;
        MDRPod::Write(data.playbackNameStatus, ppDstBuffer);
        MDRPrefixedString::Write(data.playbackName, ppDstBuffer);
        return ptr - *ppDstBuffer;
    }

    void PlaybackName::Read(UInt8** ppSrcBuffer, PlaybackName& out, size_t maxSize)
    {
        MDRPod::Read(ppSrcBuffer, out.playbackNameStatus, maxSize);
        MDRPrefixedString::Read(ppSrcBuffer, out.playbackName, maxSize);
    }

    size_t PlayParamPlaybackControllerName::Serialize(const PlayParamPlaybackControllerName& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &ptr);
        for (auto& elem : data.playbackNames)
            PlaybackName::Write(elem, &ptr);
        return ptr - out;
    }

    void PlayParamPlaybackControllerName::Deserialize(UInt8* data, PlayParamPlaybackControllerName& out)
    {
        MDRPod::Read(&data, out.base);
        for (auto& elem : out.playbackNames)
            PlaybackName::Read(&data, elem);
    }

    void GsSettingInfo::Read(UInt8** ppSrcBuffer, GsSettingInfo& out, size_t maxSize)
    {
        MDRPod::Read(ppSrcBuffer, out.stringFormat, maxSize);
        MDRPrefixedString::Read(ppSrcBuffer, out.subject, maxSize);
        MDRPrefixedString::Read(ppSrcBuffer, out.summary, maxSize);
    }

    size_t GsSettingInfo::Write(const GsSettingInfo& data, UInt8** ppDstBuffer)
    {
        UInt8* ptr = *ppDstBuffer;
        MDRPod::Write(data.stringFormat, ppDstBuffer);
        MDRPrefixedString::Write(data.subject, ppDstBuffer);
        MDRPrefixedString::Write(data.summary, ppDstBuffer);
        return *ppDstBuffer - ptr;
    }

    size_t GsRetCapability::Serialize(const GsRetCapability& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.command, &ptr);
        MDRPod::Write(data.type, &ptr);
        MDRPod::Write(data.settingType, &ptr);
        GsSettingInfo::Write(data.settingInfo, &ptr);
        return ptr - out;
    }

    void GsRetCapability::Deserialize(UInt8* data, GsRetCapability& out)
    {
        MDRPod::Read(&data, out.command);
        MDRPod::Read(&data, out.type);
        MDRPod::Read(&data, out.settingType);
        GsSettingInfo::Read(&data, out.settingInfo);
    }

    size_t AudioParamListeningOptionAssignCustomizableItem::Serialize(
        const AudioParamListeningOptionAssignCustomizableItem& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &ptr);
        MDRPodArray<ListeningOptionAssignCustomizableItem>::Write(data.items, &ptr);
        return ptr - out;
    }

    void AudioParamListeningOptionAssignCustomizableItem::Deserialize(UInt8* data,
                                                                      AudioParamListeningOptionAssignCustomizableItem&
                                                                      out)
    {
        MDRPod::Read(&data, out.base);
        MDRPodArray<ListeningOptionAssignCustomizableItem>::Read(&data, out.items);
    }

    size_t SystemParamAssignableSettings::Serialize(const SystemParamAssignableSettings& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &ptr);
        MDRPodArray<Preset>::Write(data.presets, &ptr);
        return ptr - out;
    }

    void SystemParamAssignableSettings::Deserialize(UInt8* data, SystemParamAssignableSettings& out)
    {
        MDRPod::Read(&data, out.base);
        MDRPodArray<Preset>::Read(&data, out.presets);
    }

    size_t SystemParamAssignableSettingsWithLimit::Serialize(const SystemParamAssignableSettingsWithLimit& data,
                                                             UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &ptr);
        MDRPodArray<Preset>::Write(data.presets, &ptr);
        return ptr - out;
    }

    void SystemParamAssignableSettingsWithLimit::Deserialize(UInt8* data,
                                                             SystemParamAssignableSettingsWithLimit& out)
    {
        MDRPod::Read(&data, out.base);
        MDRPodArray<Preset>::Read(&data, out.presets);
    }

    void AssignableSettingsPreset::Read(UInt8** ppSrcBuffer, AssignableSettingsPreset& out, size_t maxSize)
    {
        MDRPod::Read(ppSrcBuffer, out.preset, maxSize);
        MDRPodArray<AssignableSettingsAction>::Read(ppSrcBuffer, out.actions, maxSize);
    }

    size_t AssignableSettingsPreset::Write(const AssignableSettingsPreset& data, UInt8** ppDstBuffer)
    {
        UInt8* ptr = *ppDstBuffer;
        MDRPod::Write(data.preset, ppDstBuffer);
        MDRPodArray<AssignableSettingsAction>::Write(data.actions, ppDstBuffer);
        return *ppDstBuffer - ptr;
    }

    size_t SystemExtParamAssignableSettings::Serialize(const SystemExtParamAssignableSettings& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &ptr);
        MDRArray<AssignableSettingsPreset>::Write(data.presets, &ptr);
        return ptr - out;
    }

    void SystemExtParamAssignableSettings::Deserialize(UInt8* data,
                                                       SystemExtParamAssignableSettings& out)
    {
        MDRPod::Read(&data, out.base);
        MDRArray<AssignableSettingsPreset>::Read(&data, out.presets);
    }

    size_t SystemExtParamAssignableSettingsWithLimit::Serialize(const SystemExtParamAssignableSettingsWithLimit& data,
                                                                UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &ptr);
        MDRArray<AssignableSettingsPreset>::Write(data.presets, &ptr);
        return ptr - out;
    }

    void SystemExtParamAssignableSettingsWithLimit::Deserialize(UInt8* data,
                                                                SystemExtParamAssignableSettingsWithLimit& out)
    {
        MDRPod::Read(&data, out.base);
        MDRArray<AssignableSettingsPreset>::Read(&data, out.presets);
    }
}
