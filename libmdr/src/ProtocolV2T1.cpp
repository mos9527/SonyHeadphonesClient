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
            MDR_CHECK(false, "Unsupported INSTRUCTION_GUIDE");
        }
        return ptr - out;
    }

    void ConnectRetDeviceInfo::Deserialize(const UInt8* data, ConnectRetDeviceInfo& out)
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
            MDR_CHECK(false, "Unsupported INSTRUCTION_GUIDE");
        }
    }
    size_t PlayParamPlaybackControllerName::Serialize(const PlayParamPlaybackControllerName& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &ptr);
        for (auto& elem : data.playbackNames)
            PlaybackName::Write(elem, &ptr);
        return ptr - out;
    }

    void PlayParamPlaybackControllerName::Deserialize(const UInt8* data, PlayParamPlaybackControllerName& out)
    {
        MDRPod::Read(&data, out.base);
        for (auto& elem : out.playbackNames)
            PlaybackName::Read(&data, elem);
    }
}
