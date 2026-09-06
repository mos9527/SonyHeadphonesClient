#include "Debugger.hpp"
#include "DebuggerDetails.hpp"

#include <mdr/Command.hpp>
#include <mdr/ProtocolV1T1.hpp>
#include <mdr/ProtocolV1T2.hpp>
#include <mdr/ProtocolV2T2.hpp>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

namespace
{
    using namespace client::debugger;

    template <typename Function>
    bool ForEachDescriptor(Function&& function)
    {
        for (const PacketDescriptor& descriptor : Getv1t1Packets())
            if (!function(descriptor))
                return false;
        for (const PacketDescriptor& descriptor : Getv1t2Packets())
            if (!function(descriptor))
                return false;
        for (const PacketDescriptor& descriptor : Getv2t1Packets())
            if (!function(descriptor))
                return false;
        for (const PacketDescriptor& descriptor : Getv2t2Packets())
            if (!function(descriptor))
                return false;
        return true;
    }

    template <typename Payload, size_t Size>
    bool EncodesExactly(const Payload& payload, const mdr::UInt8 (&expected)[Size])
    {
        mdr::UInt8 bytes[Size]{};
        const auto result = Payload::Serialize(payload, bytes, sizeof(bytes));
        return result && result.value == Size && std::memcmp(bytes, expected, Size) == 0;
    }

    bool CheckDefaultRoundTrips()
    {
        size_t count{};
        const bool passed = ForEachDescriptor(
            [&](const PacketDescriptor& descriptor)
            {
                ++count;
                PacketInstance source{descriptor};
                mdr::UInt8 firstBytes[mdr::kMDRMaxPacketSize];
                const auto first = descriptor.encode(source.Value(), firstBytes, sizeof(firstBytes));
                if (!first)
                {
                    MDR_LOG("Unable to encode default {}", descriptor.name);
                    return false;
                }

                PacketInstance decoded{descriptor};
                const auto decode = descriptor.decode(decoded.Value(), {firstBytes, first.value});
                if (!decode)
                {
                    MDR_LOG("Unable to decode default {}", descriptor.name);
                    return false;
                }

                mdr::UInt8 secondBytes[mdr::kMDRMaxPacketSize];
                const auto second = descriptor.encode(decoded.Value(), secondBytes, sizeof(secondBytes));
                if (!second || second.value != first.value || std::memcmp(firstBytes, secondBytes, first.value) != 0)
                {
                    MDR_LOG("Round trip changed {}", descriptor.name);
                    return false;
                }
                return true;
            });
        if (count == 0)
        {
            MDR_LOG("No debugger packet descriptors were generated");
            return false;
        }
        return passed;
    }

    bool CheckInvalidValuesRemainEditable()
    {
        const PacketDescriptor* selected{};
        ForEachDescriptor(
            [&](const PacketDescriptor& descriptor)
            {
                if (std::string_view(descriptor.name) == "mdr::v2::t1::ConnectRetProtocolInfo")
                {
                    selected = &descriptor;
                }
                return true;
            });
        if (!selected)
            return false;

        PacketInstance packet{*selected};
        mdr::UInt8 bytes[mdr::kMDRMaxPacketSize];
        auto encoded = selected->encode(packet.Value(), bytes, sizeof(bytes));
        if (!encoded || encoded.value == 0)
            return false;
        bytes[0] ^= 0x7f;

        PacketInstance invalid{*selected};
        if (!selected->decode(invalid.Value(), {bytes, encoded.value}))
            return false;
        if (selected->validate(invalid.Value()))
        {
            MDR_LOG("Invalid debugger packet unexpectedly validated");
            return false;
        }

        mdr::UInt8 roundTrip[mdr::kMDRMaxPacketSize];
        const auto invalidEncoded = selected->encode(invalid.Value(), roundTrip, sizeof(roundTrip));
        return invalidEncoded && invalidEncoded.value == encoded.value &&
            std::memcmp(bytes, roundTrip, encoded.value) == 0;
    }

    bool CheckV1PlaybackNameWireLayout()
    {
        using namespace mdr;
        using namespace mdr::v1;
        using namespace mdr::v1::t1;

        constexpr UInt8 payload[]{
            0xA7,
            0x01,
            0x00,
            0x02,
            0x00,
        };
        const auto decoded = RetPlayParamPlaybackControllerNameData::Deserialize(
            payload, sizeof(payload)
        );
        if (
            !decoded
            || decoded.value.playbackName.nameStatus
                != PlaybackNameStatus::SETTLED
            || !decoded.value.playbackName.name.value.empty()
        )
        {
            MDR_LOG("V1 playback name response wire layout is incorrect");
            return false;
        }

        UInt8 encoded[sizeof(payload)]{};
        const auto serialized =
            RetPlayParamPlaybackControllerNameData::Serialize(
                decoded.value, encoded, sizeof(encoded)
            );
        return serialized && serialized.value == sizeof(payload)
            && std::memcmp(payload, encoded, sizeof(payload)) == 0;
    }

    bool CheckCorrectedV1WireLayouts()
    {
        using namespace mdr;
        using namespace mdr::v1;
        using namespace mdr::v1::t1;

        RetOptimizerCapability optimizer{};
        UInt8 optimizerBytes[16]{};
        const auto optimizerResult =
            RetOptimizerCapability::Serialize(optimizer, optimizerBytes, sizeof(optimizerBytes));
        if (!optimizerResult || optimizerBytes[0] != static_cast<UInt8>(Command::OPT_RET_CAPABILITY))
            return false;

        NotifyUpdateStatus update{};
        UInt8 updateBytes[8]{};
        const auto updateResult = NotifyUpdateStatus::Serialize(update, updateBytes, sizeof(updateBytes));
        if (!updateResult || updateBytes[0] != static_cast<UInt8>(Command::UPDT_NTFY_STATUS))
            return false;

        RetPlayCapability capability{};
        capability.inquiredType = PlayInquiredType::PLAYBACK_CONTROLLER;
        capability.volumeStep = 0x1F;
        capability.playbackControlType = PlaybackControlType::PLAY_PAUSE_TRACKUP_TRACKDOWN;
        capability.metaDataDisplayType = MetaDataDisplayType::TRACK_ALBUM_ARTIST_GENRE_PLAYER;
        const UInt8 capabilityExpected[]{0xA1, 0x01, 0x1F, 0x01, 0x01};
        if (!EncodesExactly(capability, capabilityExpected))
            return false;

        SetPlayStatus play{};
        play.type = PlayInquiredType::PLAYBACK_CONTROLLER;
        play.status = CommonStatus::ENABLE;
        play.control = PlaybackControl::PLAY;
        const UInt8 playExpected[]{
            static_cast<UInt8>(Command::PLAY_SET_STATUS),
            static_cast<UInt8>(PlayInquiredType::PLAYBACK_CONTROLLER),
            static_cast<UInt8>(CommonStatus::ENABLE),
            static_cast<UInt8>(PlaybackControl::PLAY),
        };
        if (!EncodesExactly(play, playExpected))
            return false;

        SetLogStatus logStatus{};
        const UInt8 logStatusExpected[]{
            static_cast<UInt8>(Command::LOG_SET_STATUS),
            static_cast<UInt8>(LogInquiredType::ACTION_LOG_NOTIFIER),
            static_cast<UInt8>(CommonStatus::ENABLE),
        };
        if (!EncodesExactly(logStatus, logStatusExpected))
            return false;

        NotifyPlayParamPlaybackControllerNotifyNameData notifyName{};
        const UInt8 notifyNameExpected[]{
            static_cast<UInt8>(Command::PLAY_NTFY_PARAM),
            static_cast<UInt8>(PlayInquiredType::PLAYBACK_CONTROLLER),
            static_cast<UInt8>(PlaybackDetailedDataType::TRACK_NAME),
        };
        if (!EncodesExactly(notifyName, notifyNameExpected))
            return false;

        RetSystemStatusControlByWearingStatus wearing{};
        const UInt8 wearingExpected[]{
            static_cast<UInt8>(Command::SYSTEM_RET_STATUS),
            static_cast<UInt8>(SystemInquiredType::CONTROL_BY_WEARING),
            static_cast<UInt8>(CommonStatus::ENABLE),
        };
        if (!EncodesExactly(wearing, wearingExpected))
            return false;

        RetSystemStatusAutoPowerOffStatus powerOff{};
        const UInt8 powerOffExpected[]{
            static_cast<UInt8>(Command::SYSTEM_RET_STATUS),
            static_cast<UInt8>(SystemInquiredType::AUTO_POWER_OFF),
            static_cast<UInt8>(CommonStatus::ENABLE),
        };
        if (!EncodesExactly(powerOff, powerOffExpected))
            return false;

        RetSystemStatusAssignableSettingsStatus assignable{};
        assignable.allStatus.value.push_back(CommonStatus::ENABLE);
        const UInt8 assignableExpected[]{
            static_cast<UInt8>(Command::SYSTEM_RET_STATUS),
            static_cast<UInt8>(SystemInquiredType::ASSIGNABLE_SETTINGS),
            0x01,
            static_cast<UInt8>(CommonStatus::ENABLE),
        };
        if (!EncodesExactly(assignable, assignableExpected))
            return false;

        NotifyLogParam log{};
        log.data.value = "{}";
        const UInt8 logExpected[]{
            static_cast<UInt8>(Command::LOG_NTFY_PARAM),
            static_cast<UInt8>(LogInquiredType::ACTION_LOG_NOTIFIER),
            0x00,
            0x02,
            '{',
            '}',
        };
        if (!EncodesExactly(log, logExpected))
            return false;

        EqBandInformation band{};
        band.infoType = EqBandInformationType::HZ;
        band.valueAsFrequency = 400;
        UInt8 bandBytes[3]{};
        UInt8* bandPtr = bandBytes;
        const auto bandResult = EqBandInformation::Write(band, &bandPtr, sizeof(bandBytes));
        const UInt8 bandExpected[]{0x01, 0x01, 0x90};
        if (!bandResult || bandResult.value != sizeof(bandExpected) ||
            std::memcmp(bandBytes, bandExpected, sizeof(bandExpected)) != 0)
        {
            return false;
        }

        using namespace mdr::v1::t2;
        RetVoiceGuidanceParamSettingOnOff voiceParam{};
        const UInt8 voiceParamExpected[]{
            static_cast<UInt8>(mdr::v1::t2::Command::VOICE_GUIDANCE_RET_PARAM),
            static_cast<UInt8>(VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING),
            static_cast<UInt8>(DetailedDataType::ON_OFF),
            static_cast<UInt8>(VoiceGuidanceSettingValue::OFF),
        };
        if (!EncodesExactly(voiceParam, voiceParamExpected))
            return false;

        RetVoiceGuidanceStatusSettingOnOff voiceStatus{};
        const UInt8 voiceStatusExpected[]{
            static_cast<UInt8>(mdr::v1::t2::Command::VOICE_GUIDANCE_RET_STATUS),
            static_cast<UInt8>(VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING),
            static_cast<UInt8>(StatusType::ON_OFF),
            static_cast<UInt8>(CommonStatus::ENABLE),
        };
        return EncodesExactly(voiceStatus, voiceStatusExpected);
    }

    bool CheckV2AssignableSettingsCapabilityWireLayout()
    {
        using namespace mdr;
        using namespace mdr::v2::t1;

        constexpr UInt8 payload[]{
            0xF1, 0x03, 0x01,
            0x00, 0x01, 0x35, 0x01,
            0x35, 0x01, 0x01,
            0x00, 0x04,
            0x01, 0x43, 0x02, 0x44, 0x24,
        };
        const auto decoded =
            SystemRetCapabilityAssignableSettings::Deserialize(
                payload, sizeof(payload)
            );
        if (!decoded || decoded.value.keys.size() != 1)
            return false;

        const auto& presets =
            decoded.value.keys.value[0].assignableSettingsPreset;
        if (presets.size() != 1)
            return false;
        const auto& preset = presets.value[0];
        if (
            preset.settingsActions.size() != 1
            || preset.settingsCustomizableActions.size() != 1
        )
        {
            return false;
        }
        const auto& customizable =
            preset.settingsCustomizableActions.value[0];
        if (
            customizable.action != Action::DOUBLE_TAP
            || customizable.defaultFunction != Function::QUICK_ACCESS1
            || customizable.functions.size() != 2
            || customizable.functions.value[0]
                != Function::QUICK_ACCESS2
            || customizable.functions.value[1] != Function::VOLUME_DOWN
        )
        {
            return false;
        }
        return EncodesExactly(decoded.value, payload);
    }

    bool CheckPeripheralDeviceInfoWireLayouts()
    {
        using namespace mdr;

        UInt8 v1Bytes[64]{};
        v1::t2::NotifyPeripheralParamPairingDeviceManagementClassicBt v1Payload{};
        auto& v1Device = v1Payload.deviceInfo.value.emplace_back();
        v1Device.btDeviceAddress.fill(0xAA);
        v1Device.connectedStatus = 1;
        v1Payload.playbackrightDevice = 2;
        const auto v1Result =
            v1::t2::NotifyPeripheralParamPairingDeviceManagementClassicBt::Serialize(
                v1Payload, v1Bytes, sizeof(v1Bytes)
            );
        if (
            !v1Result
            || v1Result.value != 23
            || v1Bytes[1] != static_cast<UInt8>(
                v1::t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
            )
            || v1Bytes[21] != 0
            || v1Bytes[22] != 2
        )
        {
            MDR_LOG("V1 ClassicBt device info wire layout is incorrect");
            return false;
        }

        using namespace mdr::v2::t2;

        UInt8 classicBytes[64]{};
        PeripheralNotifyParamPairingDeviceManagementClassicBt classic{};
        auto& classicDevice = classic.deviceInfo.value.emplace_back();
        classicDevice.btDeviceAddress.fill(0xAA);
        classicDevice.connectedStatus = 1;
        classic.playbackrightDevice = 2;
        const auto classicResult =
            PeripheralNotifyParamPairingDeviceManagementClassicBt::Serialize(
                classic, classicBytes, sizeof(classicBytes)
            );
        if (
            !classicResult
            || classicResult.value != 23
            || classicBytes[1] != static_cast<UInt8>(
                PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
            )
            || classicBytes[21] != 0
            || classicBytes[22] != 2
        )
        {
            MDR_LOG("ClassicBt device info wire layout is incorrect");
            return false;
        }

        UInt8 classOfDeviceBytes[64]{};
        PeripheralNotifyParamPairingDeviceManagementWithBluetoothClassOfDevice
            classOfDevice{};
        auto& classOfDeviceDevice =
            classOfDevice.deviceInfo.value.emplace_back();
        classOfDeviceDevice.btDeviceAddress.fill(0xAA);
        classOfDeviceDevice.connectedStatus = 1;
        classOfDeviceDevice.bluetoothClassOfDevice = 0x123456;
        classOfDevice.playbackrightDevice = 2;
        const auto classOfDeviceResult =
            PeripheralNotifyParamPairingDeviceManagementWithBluetoothClassOfDevice::
                Serialize(
                    classOfDevice,
                    classOfDeviceBytes,
                    sizeof(classOfDeviceBytes)
                );
        if (
            !classOfDeviceResult
            || classOfDeviceResult.value != 26
            || classOfDeviceBytes[1] != static_cast<UInt8>(
                PeripheralInquiredType::
                    PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE
            )
            || classOfDeviceBytes[21] != 0x12
            || classOfDeviceBytes[22] != 0x34
            || classOfDeviceBytes[23] != 0x56
            || classOfDeviceBytes[24] != 0
            || classOfDeviceBytes[25] != 2
        )
        {
            MDR_LOG("Bluetooth class-of-device wire layout is incorrect");
            return false;
        }
        return true;
    }

    bool CheckSingleFileReplay()
    {
        using namespace mdr;
        using namespace mdr::v2::t2;

        ConnectGetSupportFunction packet{};
        UInt8 payload[kMDRMaxPacketSize];
        const auto serialized =
            ConnectGetSupportFunction::Serialize(packet, payload, sizeof(payload));
        if (!serialized)
            return false;

        const MDRBuffer frame = MDRPackCommand(
            MDRDataType::DATA_MDR_NO2,
            0,
            {payload, serialized.value});
        const std::filesystem::path path =
            std::filesystem::temp_directory_path() / "mdr-debugger-single-packet-test.bin";
        {
            std::ofstream output(path, std::ios::binary | std::ios::trunc);
            output.write(
                reinterpret_cast<const char*>(frame.data()),
                static_cast<std::streamsize>(frame.size()));
            if (!output)
                return false;
        }

        size_t packetCount{};
        const std::string pathString = path.string();
        const bool replayed = clientDebuggerReplayPath(pathString.c_str(), &packetCount);
        const std::filesystem::path exportPath =
            std::filesystem::temp_directory_path() / "mdr-debugger-export-test.bin";
        const bool exported = clientDebuggerWritePacketFile(
            exportPath.string().c_str(), frame.data(), frame.size());
        const std::vector<char> exportedBytes = [&]
        {
            std::ifstream input(exportPath, std::ios::binary);
            return std::vector<char>(
                std::istreambuf_iterator<char>(input),
                std::istreambuf_iterator<char>());
        }();
        std::error_code removeError;
        std::filesystem::remove(path, removeError);
        std::filesystem::remove(exportPath, removeError);
        if (!replayed || packetCount != 1 || !exported ||
            !(exportedBytes.size() == frame.size() &&
              std::memcmp(exportedBytes.data(), frame.data(), frame.size()) == 0))
        {
            return false;
        }

        const std::filesystem::path invalidPath =
            std::filesystem::temp_directory_path() / "mdr-debugger-invalid-packet-test.bin";
        MDRBuffer invalidFrame = frame;
        invalidFrame.back() ^= 0x01;
        {
            std::ofstream output(invalidPath, std::ios::binary | std::ios::trunc);
            output.write(
                reinterpret_cast<const char*>(invalidFrame.data()),
                static_cast<std::streamsize>(invalidFrame.size()));
            if (!output)
                return false;
        }
        packetCount = 0;
        const std::string invalidPathString = invalidPath.string();
        const bool invalidReplayed =
            clientDebuggerReplayPath(invalidPathString.c_str(), &packetCount);
        std::filesystem::remove(invalidPath, removeError);
        return invalidReplayed && packetCount == 1 && clientDebuggerHasPackets();
    }

    bool CheckPacketCollectionZip()
    {
        using namespace mdr;

        const UInt8 payload[]{0x01};
        const MDRBuffer additionalFrame =
            MDRPackCommand(MDRDataType::DATA_MDR, 1, payload);
        clientDebuggerObservePacket(
            MDR_PACKET_DIRECTION_TX,
            additionalFrame.data(),
            static_cast<int>(additionalFrame.size()));

        const std::filesystem::path path =
            std::filesystem::temp_directory_path() / "mdr-debugger-packets-test.zip";
        const std::string pathString = path.string();
        if (!clientDebuggerWritePacketCollectionFile(pathString.c_str()))
            return false;

        std::ifstream input(path, std::ios::binary);
        const std::vector<UInt8> bytes{
            std::istreambuf_iterator<char>(input),
            std::istreambuf_iterator<char>()};
        input.close();
        std::error_code removeError;
        std::filesystem::remove(path, removeError);
        if (bytes.size() < 52 || removeError)
            return false;

        const auto read16 = [&](size_t offset)
        {
            return static_cast<std::uint16_t>(bytes[offset] | (bytes[offset + 1] << 8));
        };
        const auto read32 = [&](size_t offset)
        {
            return static_cast<std::uint32_t>(
                bytes[offset] | (bytes[offset + 1] << 8) | (bytes[offset + 2] << 16) |
                (bytes[offset + 3] << 24));
        };

        const size_t endOffset = bytes.size() - 22;
        if (read32(0) != 0x04034b50u || read16(8) != 0 ||
            read32(endOffset) != 0x06054b50u || read16(endOffset + 10) != 2)
        {
            return false;
        }
        const std::uint32_t centralOffset = read32(endOffset + 16);
        return centralOffset + 46 <= endOffset &&
            read32(centralOffset) == 0x02014b50u &&
            read16(centralOffset + 10) == 0;
    }
} // namespace

int main()
{
    if (!CheckDefaultRoundTrips())
        return 1;
    if (!CheckInvalidValuesRemainEditable())
        return 2;
    if (!CheckV1PlaybackNameWireLayout())
        return 3;
    if (!CheckPeripheralDeviceInfoWireLayouts())
        return 4;
    if (!CheckCorrectedV1WireLayouts())
        return 5;
    if (!CheckV2AssignableSettingsCapabilityWireLayout())
        return 6;
    if (!CheckSingleFileReplay())
        return 7;
    if (!CheckPacketCollectionZip())
        return 8;
    return 0;
}
