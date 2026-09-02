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
    if (!CheckSingleFileReplay())
        return 5;
    if (!CheckPacketCollectionZip())
        return 6;
    return 0;
}
