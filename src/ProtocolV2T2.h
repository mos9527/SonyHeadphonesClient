#pragma once

#include <cstdint>
#include <span>

#include "ByteMagic.h"
#include "Constants.h"

#pragma pack(push, 1)

namespace THMSGV2T2
{

// Extracted from Sound Connect iOS 11.0.1
enum class Command : uint8_t
{
    CONNECT_GET_SUPPORT_FUNCTION = 0x06,
    CONNECT_RET_SUPPORT_FUNCTION = 0x07,

    POWER_GET_CAPABILITY = 0x20,
    POWER_RET_CAPABILITY = 0x21,

    POWER_GET_STATUS = 0x22,
    POWER_RET_STATUS = 0x23,
    POWER_SET_STATUS = 0x24,
    POWER_NTFY_STATUS = 0x25,

    POWER_GET_PARAM = 0x26,
    POWER_RET_PARAM = 0x27,
    POWER_SET_PARAM = 0x28,
    POWER_NTFY_PARAM = 0x29,

    PERI_GET_CAPABILITY = 0x30,
    PERI_RET_CAPABILITY = 0x31,

    PERI_GET_STATUS = 0x32,
    PERI_RET_STATUS = 0x33,
    PERI_SET_STATUS = 0x34,
    PERI_NTFY_STATUS = 0x35,

    PERI_GET_PARAM = 0x36,
    PERI_RET_PARAM = 0x37,
    PERI_SET_PARAM = 0x38,
    PERI_NTFY_PARAM = 0x39,

    MULTIPOINT_DEVICE_GET = 0x3A, // Not in official enum
    MULTIPOINT_DEVICE_RET = 0x3B, // Not in official enum

    PERI_SET_EXTENDED_PARAM = 0x3C,
    PERI_NTFY_EXTENDED_PARAM = 0x3D,

    VOICE_GUIDANCE_GET_CAPABILITY = 0x40,
    VOICE_GUIDANCE_RET_CAPABILITY = 0x41,

    VOICE_GUIDANCE_GET_STATUS = 0x42,
    VOICE_GUIDANCE_RET_STATUS = 0x43,
    VOICE_GUIDANCE_SET_STATUS = 0x44,
    VOICE_GUIDANCE_NTFY_STATUS = 0x45,

    VOICE_GUIDANCE_GET_PARAM = 0x46,
    VOICE_GUIDANCE_RET_PARAM = 0x47,
    VOICE_GUIDANCE_SET_PARAM = 0x48,
    VOICE_GUIDANCE_NTFY_PARAM = 0x49,

    VOICE_GUIDANCE_GET_EXTENDED_PARAM = 0x4A,
    VOICE_GUIDANCE_RET_EXTENDED_PARAM = 0x4B,

    SAFE_LISTENING_GET_CAPABILITY = 0x50,
    SAFE_LISTENING_RET_CAPABILITY = 0x51,

    SAFE_LISTENING_GET_STATUS = 0x52,
    SAFE_LISTENING_RET_STATUS = 0x53,
    SAFE_LISTENING_SET_STATUS = 0x54,
    SAFE_LISTENING_NTFY_STATUS = 0x55,

    SAFE_LISTENING_GET_PARAM = 0x56,
    SAFE_LISTENING_RET_PARAM = 0x57,
    SAFE_LISTENING_SET_PARAM = 0x58,
    SAFE_LISTENING_NTFY_PARAM = 0x59,

    SAFE_LISTENING_GET_EXTENDED_PARAM = 0x5A,
    SAFE_LISTENING_RET_EXTENDED_PARAM = 0x5B,

    LEA_GET_CAPABILITY = 0x60,
    LEA_RET_CAPABILITY = 0x61,

    LEA_GET_STATUS = 0x62,
    LEA_RET_STATUS = 0x63,
    LEA_NTFY_STATUS = 0x65,

    LEA_GET_PARAM = 0x66,
    LEA_RET_PARAM = 0x67,
    LEA_SET_PARAM = 0x68,
    LEA_NTFY_PARAM = 0x69,

    PARTY_GET_CAPABILITY = 0x70,
    PARTY_RET_CAPABILITY = 0x71,

    PARTY_GET_STATUS = 0x72,
    PARTY_RET_STATUS = 0x73,
    PARTY_SET_STATUS = 0x74,
    PARTY_NTFY_STATUS = 0x75,

    PARTY_GET_PARAM = 0x76,
    PARTY_RET_PARAM = 0x77,
    PARTY_SET_PARAM = 0x78,
    PARTY_NTFY_PARAM = 0x79,

    PARTY_SET_EXTENDED_PARAM = 0x7C,

    SYSTEM_GET_CAPABILITY = 0xF0,
    SYSTEM_RET_CAPABILITY = 0xF1,

    SYSTEM_GET_STATUS = 0xF2,
    SYSTEM_RET_STATUS = 0xF3,
    SYSTEM_SET_STATUS = 0xF4,
    SYSTEM_NTFY_STATUS = 0xF5,

    SYSTEM_GET_PARAM = 0xF6,
    SYSTEM_RET_PARAM = 0xF7,
    SYSTEM_SET_PARAM = 0xF8,
    SYSTEM_NTFY_PARAM = 0xF9,

    SYSTEM_GET_EXTENDED_PARAM = 0xFA,
    SYSTEM_RET_EXTENDED_PARAM = 0xFB,
    SYSTEM_SET_EXTENDED_PARAM = 0xFC,
    SYSTEM_NTFY_EXTENDED_PARAM = 0xFD,

    UNKNOWN = 0xFF
};

inline const char* Command_toString(Command cmd)
{
    switch (cmd) {
    case Command::CONNECT_GET_SUPPORT_FUNCTION: return "CONNECT_GET_SUPPORT_FUNCTION";
    case Command::CONNECT_RET_SUPPORT_FUNCTION: return "CONNECT_RET_SUPPORT_FUNCTION";
    case Command::POWER_GET_CAPABILITY: return "POWER_GET_CAPABILITY";
    case Command::POWER_RET_CAPABILITY: return "POWER_RET_CAPABILITY";
    case Command::POWER_GET_STATUS: return "POWER_GET_STATUS";
    case Command::POWER_RET_STATUS: return "POWER_RET_STATUS";
    case Command::POWER_SET_STATUS: return "POWER_SET_STATUS";
    case Command::POWER_NTFY_STATUS: return "POWER_NTFY_STATUS";
    case Command::POWER_GET_PARAM: return "POWER_GET_PARAM";
    case Command::POWER_RET_PARAM: return "POWER_RET_PARAM";
    case Command::POWER_SET_PARAM: return "POWER_SET_PARAM";
    case Command::POWER_NTFY_PARAM: return "POWER_NTFY_PARAM";
    case Command::PERI_GET_CAPABILITY: return "PERI_GET_CAPABILITY";
    case Command::PERI_RET_CAPABILITY: return "PERI_RET_CAPABILITY";
    case Command::PERI_GET_STATUS: return "PERI_GET_STATUS";
    case Command::PERI_RET_STATUS: return "PERI_RET_STATUS";
    case Command::PERI_SET_STATUS: return "PERI_SET_STATUS";
    case Command::PERI_NTFY_STATUS: return "PERI_NTFY_STATUS";
    case Command::PERI_GET_PARAM: return "PERI_GET_PARAM";
    case Command::PERI_RET_PARAM: return "PERI_RET_PARAM";
    case Command::PERI_SET_PARAM: return "PERI_SET_PARAM";
    case Command::PERI_NTFY_PARAM: return "PERI_NTFY_PARAM";
    case Command::MULTIPOINT_DEVICE_GET: return "MULTIPOINT_DEVICE_GET";
    case Command::MULTIPOINT_DEVICE_RET: return "MULTIPOINT_DEVICE_RET";
    case Command::PERI_SET_EXTENDED_PARAM: return "PERI_SET_EXTENDED_PARAM";
    case Command::PERI_NTFY_EXTENDED_PARAM: return "PERI_NTFY_EXTENDED_PARAM";
    case Command::VOICE_GUIDANCE_GET_CAPABILITY: return "VOICE_GUIDANCE_GET_CAPABILITY";
    case Command::VOICE_GUIDANCE_RET_CAPABILITY: return "VOICE_GUIDANCE_RET_CAPABILITY";
    case Command::VOICE_GUIDANCE_GET_STATUS: return "VOICE_GUIDANCE_GET_STATUS";
    case Command::VOICE_GUIDANCE_RET_STATUS: return "VOICE_GUIDANCE_RET_STATUS";
    case Command::VOICE_GUIDANCE_SET_STATUS: return "VOICE_GUIDANCE_SET_STATUS";
    case Command::VOICE_GUIDANCE_NTFY_STATUS: return "VOICE_GUIDANCE_NTFY_STATUS";
    case Command::VOICE_GUIDANCE_GET_PARAM: return "VOICE_GUIDANCE_GET_PARAM";
    case Command::VOICE_GUIDANCE_RET_PARAM: return "VOICE_GUIDANCE_RET_PARAM";
    case Command::VOICE_GUIDANCE_SET_PARAM: return "VOICE_GUIDANCE_SET_PARAM";
    case Command::VOICE_GUIDANCE_NTFY_PARAM: return "VOICE_GUIDANCE_NTFY_PARAM";
    case Command::VOICE_GUIDANCE_GET_EXTENDED_PARAM: return "VOICE_GUIDANCE_GET_EXTENDED_PARAM";
    case Command::VOICE_GUIDANCE_RET_EXTENDED_PARAM: return "VOICE_GUIDANCE_RET_EXTENDED_PARAM";
    case Command::SAFE_LISTENING_GET_CAPABILITY: return "SAFE_LISTENING_GET_CAPABILITY";
    case Command::SAFE_LISTENING_RET_CAPABILITY: return "SAFE_LISTENING_RET_CAPABILITY";
    case Command::SAFE_LISTENING_GET_STATUS: return "SAFE_LISTENING_GET_STATUS";
    case Command::SAFE_LISTENING_RET_STATUS: return "SAFE_LISTENING_RET_STATUS";
    case Command::SAFE_LISTENING_SET_STATUS: return "SAFE_LISTENING_SET_STATUS";
    case Command::SAFE_LISTENING_NTFY_STATUS: return "SAFE_LISTENING_NTFY_STATUS";
    case Command::SAFE_LISTENING_GET_PARAM: return "SAFE_LISTENING_GET_PARAM";
    case Command::SAFE_LISTENING_RET_PARAM: return "SAFE_LISTENING_RET_PARAM";
    case Command::SAFE_LISTENING_SET_PARAM: return "SAFE_LISTENING_SET_PARAM";
    case Command::SAFE_LISTENING_NTFY_PARAM: return "SAFE_LISTENING_NTFY_PARAM";
    case Command::SAFE_LISTENING_GET_EXTENDED_PARAM: return "SAFE_LISTENING_GET_EXTENDED_PARAM";
    case Command::SAFE_LISTENING_RET_EXTENDED_PARAM: return "SAFE_LISTENING_RET_EXTENDED_PARAM";
    case Command::LEA_GET_CAPABILITY: return "LEA_GET_CAPABILITY";
    case Command::LEA_RET_CAPABILITY: return "LEA_RET_CAPABILITY";
    case Command::LEA_GET_STATUS: return "LEA_GET_STATUS";
    case Command::LEA_RET_STATUS: return "LEA_RET_STATUS";
    case Command::LEA_NTFY_STATUS: return "LEA_NTFY_STATUS";
    case Command::LEA_GET_PARAM: return "LEA_GET_PARAM";
    case Command::LEA_RET_PARAM: return "LEA_RET_PARAM";
    case Command::LEA_SET_PARAM: return "LEA_SET_PARAM";
    case Command::LEA_NTFY_PARAM: return "LEA_NTFY_PARAM";
    case Command::PARTY_GET_CAPABILITY: return "PARTY_GET_CAPABILITY";
    case Command::PARTY_RET_CAPABILITY: return "PARTY_RET_CAPABILITY";
    case Command::PARTY_GET_STATUS: return "PARTY_GET_STATUS";
    case Command::PARTY_RET_STATUS: return "PARTY_RET_STATUS";
    case Command::PARTY_SET_STATUS: return "PARTY_SET_STATUS";
    case Command::PARTY_NTFY_STATUS: return "PARTY_NTFY_STATUS";
    case Command::PARTY_GET_PARAM: return "PARTY_GET_PARAM";
    case Command::PARTY_RET_PARAM: return "PARTY_RET_PARAM";
    case Command::PARTY_SET_PARAM: return "PARTY_SET_PARAM";
    case Command::PARTY_NTFY_PARAM: return "PARTY_NTFY_PARAM";
    case Command::PARTY_SET_EXTENDED_PARAM: return "PARTY_SET_EXTENDED_PARAM";
    case Command::SYSTEM_GET_CAPABILITY: return "SYSTEM_GET_CAPABILITY";
    case Command::SYSTEM_RET_CAPABILITY: return "SYSTEM_RET_CAPABILITY";
    case Command::SYSTEM_GET_STATUS: return "SYSTEM_GET_STATUS";
    case Command::SYSTEM_RET_STATUS: return "SYSTEM_RET_STATUS";
    case Command::SYSTEM_SET_STATUS: return "SYSTEM_SET_STATUS";
    case Command::SYSTEM_NTFY_STATUS: return "SYSTEM_NTFY_STATUS";
    case Command::SYSTEM_GET_PARAM: return "SYSTEM_GET_PARAM";
    case Command::SYSTEM_RET_PARAM: return "SYSTEM_RET_PARAM";
    case Command::SYSTEM_SET_PARAM: return "SYSTEM_SET_PARAM";
    case Command::SYSTEM_NTFY_PARAM: return "SYSTEM_NTFY_PARAM";
    case Command::SYSTEM_GET_EXTENDED_PARAM: return "SYSTEM_GET_EXTENDED_PARAM";
    case Command::SYSTEM_RET_EXTENDED_PARAM: return "SYSTEM_RET_EXTENDED_PARAM";
    case Command::SYSTEM_SET_EXTENDED_PARAM: return "SYSTEM_SET_EXTENDED_PARAM";
    case Command::SYSTEM_NTFY_EXTENDED_PARAM: return "SYSTEM_NTFY_EXTENDED_PARAM";
    default: return "???";
    }
}

struct Payload
{
    Command command; // 0x0

    Payload(Command command) : command(command) {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return !buf.empty();
    }

    static constexpr bool VARIABLE_SIZE_NEEDS_SERIALIZATION = false;
    static constexpr bool VARIABLE_SIZE_ONE_ARRAY_AT_END = false;
};

// region CONNECT

enum class ConnectInquiredType : uint8_t
{
    FIXED_VALUE = 0,
};

inline bool ConnectInquiredType_isValidByteCode(uint8_t type)
{
    switch (static_cast<ConnectInquiredType>(type))
    {
    case ConnectInquiredType::FIXED_VALUE:
        return true;
    }
    return false;
}

// CONNECT_GET_SUPPORT_FUNCTION

struct ConnectGetSupportFunction : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::CONNECT_RET_SUPPORT_FUNCTION;

    ConnectInquiredType inquiredType; // 0x1

    ConnectGetSupportFunction(ConnectInquiredType inquiredType)
        : Payload(Command::CONNECT_GET_SUPPORT_FUNCTION)
        , inquiredType(inquiredType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(ConnectGetSupportFunction)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::CONNECT_GET_SUPPORT_FUNCTION)
            && buf[offsetof(ConnectGetSupportFunction, inquiredType)] == static_cast<uint8_t>(ConnectInquiredType::FIXED_VALUE);
    }
};

struct ConnectRetSupportFunction : Payload
{
    ConnectInquiredType inquiredType; // 0x1
    uint8_t numberOfFunction; // 0x2
    MessageMdrV2SupportFunction supportFunctions[]; // 0x3-

private:
    ConnectRetSupportFunction(
        const std::span<const MessageMdrV2SupportFunction>& supportFunctions, ConnectInquiredType inquiredType
    )
        : Payload(Command::CONNECT_RET_SUPPORT_FUNCTION)
        , inquiredType(inquiredType)
        , numberOfFunction(supportFunctions.size())
    {
        std::memcpy(this->supportFunctions, supportFunctions.data(), sizeof(MessageMdrV2SupportFunction) * numberOfFunction);
    }

public:
    VARIABLE_SIZE_PAYLOAD_ONE_ARRAY_AT_END(255);

    std::span<const MessageMdrV2SupportFunction> getSupportFunctions() const
    {
        return { supportFunctions, numberOfFunction };
    }

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::CONNECT_RET_SUPPORT_FUNCTION)
            && buf[offsetof(ConnectRetSupportFunction, inquiredType)] == static_cast<uint8_t>(ConnectInquiredType::FIXED_VALUE)
            && buf.size() >= sizeof(ConnectRetSupportFunction)
            && buf.size() == sizeof(ConnectRetSupportFunction) + sizeof(MessageMdrV2SupportFunction) * buf[offsetof(ConnectRetSupportFunction, numberOfFunction)];
    }
};

// endregion CONNECT

// region PERI

// region PERI_*_PARAM

enum class PeripheralInquiredType : uint8_t
{
    PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT = 0x00,
    SOURCE_SWITCH_CONTROL = 0x01,
    PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE = 0x02,
    MUSIC_HAND_OVER_SETTING = 0x03,
};

inline bool PeripheralInquiredType_isValidByteCode(uint8_t type)
{
    switch (static_cast<PeripheralInquiredType>(type))
    {
    case PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT:
    case PeripheralInquiredType::SOURCE_SWITCH_CONTROL:
    case PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE:
    case PeripheralInquiredType::MUSIC_HAND_OVER_SETTING:
        return true;
    }
    return false;
}

#pragma pack(pop)

struct PeripheralDeviceInfo
{
    static constexpr size_t BLUETOOTH_DEVICE_ADDRESS_LENGTH = 17;

    char btDeviceAddress[BLUETOOTH_DEVICE_ADDRESS_LENGTH]; // MAC address "XX:XX:XX:XX:XX:XX" (17 bytes, no null terminator)
    uint8_t connectedStatus;
    std::string btFriendlyName;

    std::string getBtDeviceAddress() const
    {
        return std::string(btDeviceAddress, BLUETOOTH_DEVICE_ADDRESS_LENGTH);
    }

    static PeripheralDeviceInfo deserialize(std::span<const uint8_t>& buf)
    {
        if (buf.size() < BLUETOOTH_DEVICE_ADDRESS_LENGTH + 2)
            throw std::runtime_error("Buffer too small for PeripheralDeviceInfo");

        PeripheralDeviceInfo info;
        std::memcpy(info.btDeviceAddress, buf.data(), BLUETOOTH_DEVICE_ADDRESS_LENGTH);
        buf = buf.subspan(BLUETOOTH_DEVICE_ADDRESS_LENGTH);

        info.connectedStatus = buf[0];
        buf = buf.subspan(1);

        info.btFriendlyName = readPrefixedString(buf);
        return info;
    }

    void serialize(std::vector<uint8_t>& buf) const
    {
        buf.insert(buf.end(), btDeviceAddress, btDeviceAddress + BLUETOOTH_DEVICE_ADDRESS_LENGTH);
        buf.push_back(connectedStatus);
        writePrefixedString(buf, btFriendlyName);
    }

    size_t countBytes() const
    {
        return BLUETOOTH_DEVICE_ADDRESS_LENGTH + 1 + 1 + btFriendlyName.size();
    }
};

struct PeripheralDeviceInfoWithBluetoothClassOfDevice
{
    static constexpr size_t BLUETOOTH_DEVICE_ADDRESS_LENGTH = 17;
    static constexpr uint32_t BLUETOOTH_CLASS_OF_DEVICE_UNKNOWN = 0xFFFFFF;

    char btDeviceAddress[BLUETOOTH_DEVICE_ADDRESS_LENGTH]; // MAC address "XX:XX:XX:XX:XX:XX" (17 bytes, no null terminator)
    uint8_t connectedStatus;
    uint32_t bluetoothClassOfDevice; // Serialized as 24 byte int big endian
    std::string btFriendlyName;

    std::string getBtDeviceAddress() const
    {
        return std::string(btDeviceAddress, BLUETOOTH_DEVICE_ADDRESS_LENGTH);
    }

    static PeripheralDeviceInfoWithBluetoothClassOfDevice deserialize(std::span<const uint8_t>& buf)
    {
        if (buf.size() < BLUETOOTH_DEVICE_ADDRESS_LENGTH + 1 + 3 + 1)
            throw std::runtime_error("Buffer too small for PeripheralDeviceInfoWithBluetoothClassOfDevice");

        PeripheralDeviceInfoWithBluetoothClassOfDevice info;
        std::memcpy(info.btDeviceAddress, buf.data(), BLUETOOTH_DEVICE_ADDRESS_LENGTH);
        buf = buf.subspan(BLUETOOTH_DEVICE_ADDRESS_LENGTH);

        info.connectedStatus = buf[0];
        buf = buf.subspan(1);

        info.bluetoothClassOfDevice = (static_cast<uint32_t>(buf[0]) << 16)
            | (static_cast<uint32_t>(buf[1]) << 8)
            | static_cast<uint32_t>(buf[2]);
        buf = buf.subspan(3);

        info.btFriendlyName = readPrefixedString(buf);

        return info;
    }

    void serialize(std::vector<uint8_t>& buf) const
    {
        buf.insert(buf.end(), btDeviceAddress, btDeviceAddress + BLUETOOTH_DEVICE_ADDRESS_LENGTH);
        buf.push_back(connectedStatus);
        buf.push_back((bluetoothClassOfDevice >> 16) & 0xFF);
        buf.push_back((bluetoothClassOfDevice >> 8) & 0xFF);
        buf.push_back(bluetoothClassOfDevice & 0xFF);
        writePrefixedString(buf, btFriendlyName);
    }

    size_t countBytes() const
    {
        return BLUETOOTH_DEVICE_ADDRESS_LENGTH + 1 + 3 + 1 + btFriendlyName.size();
    }
};

#pragma pack(push, 1)

// region PERI_GET_PARAM

struct PeripheralGetParam : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::PERI_RET_PARAM;

    PeripheralInquiredType inquiredType; // 0x1

    PeripheralGetParam(PeripheralInquiredType inquiredType)
        : Payload(Command::PERI_GET_PARAM)
        , inquiredType(inquiredType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(PeripheralGetParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::PERI_GET_PARAM)
            && PeripheralInquiredType_isValidByteCode(buf[offsetof(PeripheralGetParam, inquiredType)]);
    }
};

// endregion PERI_GET_PARAM

// region PERI_RET_PARAM, PERI_SET_PARAM, PERI_NTFY_PARAM

struct PeripheralParam : Payload
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::PERI_RET_PARAM,
        Command::PERI_SET_PARAM,
        Command::PERI_NTFY_PARAM
    };

    PeripheralInquiredType inquiredType; // 0x1

    PeripheralParam(CommandType ct, PeripheralInquiredType inquiredType)
        : Payload(COMMAND_IDS[ct])
        , inquiredType(inquiredType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return Payload::isValid(buf)
            && buf.size() >= sizeof(PeripheralParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(COMMAND_IDS[ct])
            && PeripheralInquiredType_isValidByteCode(buf[offsetof(PeripheralParam, inquiredType)]);
    }
};

// - PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT

#pragma pack(pop)

struct PeripheralParamPairingDeviceManagementClassicBt : PeripheralParam
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::PERI_RET_PARAM,
        Command::UNKNOWN,
        Command::PERI_NTFY_PARAM
    };

    static constexpr size_t INDEX_NUM_OF_PAIRED_DEVICE = 2;
    static constexpr size_t BLUETOOTH_DEVICE_ADDRESS_LENGTH = 17;
    // App has this set to 1, however the firmware allows the friendly name to be empty.
    // See https://github.com/mos9527/SonyHeadphonesClient/issues/21
    static constexpr size_t MIN_BT_FRIENDLY_NAME_LENGTH = 0;
    static constexpr size_t MAX_BT_FRIENDLY_NAME_LENGTH = 128;

    std::vector<PeripheralDeviceInfo> deviceList;
    uint8_t playbackrightDevice;

    PeripheralParamPairingDeviceManagementClassicBt(
        CommandType ct, const std::span<const PeripheralDeviceInfo> deviceList, uint8_t playbackrightDevice
    )
        : PeripheralParam(ct, PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT)
        , deviceList(deviceList.begin(), deviceList.end())
        , playbackrightDevice(playbackrightDevice)
    {
        if (this->deviceList.size() > 255)
            throw std::invalid_argument("Device list size exceeds 255");
    }

private:
    // For init by deserialization
    PeripheralParamPairingDeviceManagementClassicBt()
        : PeripheralParam(CT_Get /*Any can do*/, PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT)
    {}

public:
    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        if (!PeripheralParam::isValid(buf, ct))
            return false;
        if (buf[offsetof(PeripheralParam, inquiredType)] != static_cast<uint8_t>(PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
            return false;

        size_t indexDevice = INDEX_NUM_OF_PAIRED_DEVICE + 1;
        if (buf.size() < indexDevice)
            return false;

        uint8_t devicesNum = buf[INDEX_NUM_OF_PAIRED_DEVICE];
        for (uint8_t i = 0; i < devicesNum; ++i)
        {
            size_t indexConnectedStatus = indexDevice + BLUETOOTH_DEVICE_ADDRESS_LENGTH;
            if (buf.size() < indexConnectedStatus)
                return false;

            size_t indexBtFriendlyNameLength = indexConnectedStatus + 1;
            if (buf.size() < indexBtFriendlyNameLength)
                return false;

            size_t btFriendlyNameLength = buf[indexBtFriendlyNameLength];
            if (btFriendlyNameLength < MIN_BT_FRIENDLY_NAME_LENGTH || MAX_BT_FRIENDLY_NAME_LENGTH < btFriendlyNameLength)
                return false;

            size_t indexBtFriendlyName = indexBtFriendlyNameLength + 1;
            if (buf.size() < indexBtFriendlyName)
                return false;

            /*if (!isValidBluetoothDeviceAddress(
                    std::string(reinterpret_cast<const char*>(&buf[indexDevice]), BLUETOOTH_DEVICE_ADDRESS_LENGTH)))
                return false;*/

            indexDevice = indexBtFriendlyName + btFriendlyNameLength;
        }

        size_t indexPlaybackrightDevice = indexDevice;
        if (buf.size() != indexPlaybackrightDevice + 1)
            return false;

        return true;
    }

    static PeripheralParamPairingDeviceManagementClassicBt deserialize(std::span<const uint8_t>& buf, CommandType ct)
    {
        if (!isValid(buf, ct))
            throw std::runtime_error("Buffer invalid for PeripheralParamPairingDeviceManagementClassicBt");

        PeripheralParamPairingDeviceManagementClassicBt result;
        result.command = static_cast<Command>(buf[0]);
        result.inquiredType = static_cast<PeripheralInquiredType>(buf[1]);

        uint8_t deviceCount = buf[INDEX_NUM_OF_PAIRED_DEVICE];
        buf = buf.subspan(3);

        result.deviceList.reserve(deviceCount);
        for (uint8_t i = 0; i < deviceCount; ++i)
        {
            result.deviceList.push_back(PeripheralDeviceInfo::deserialize(buf));
        }

        result.playbackrightDevice = buf[0];
        return result;
    }

    void serialize(std::vector<uint8_t>& buf) const
    {
        buf.push_back(static_cast<uint8_t>(command));
        buf.push_back(static_cast<uint8_t>(inquiredType));
        buf.push_back(static_cast<uint8_t>(deviceList.size()));
        for (const PeripheralDeviceInfo& device : deviceList)
        {
            device.serialize(buf);
        }
        buf.push_back(playbackrightDevice);
    }

    size_t countBytes() const
    {
        size_t size = 1 + 1 + 1; // command + inquiredType + device count
        for (const PeripheralDeviceInfo& device : deviceList)
        {
            size += device.countBytes();
        }
        size += 1; // playbackrightDevice
        return size;
    }

    static constexpr bool VARIABLE_SIZE_NEEDS_SERIALIZATION = true;
};

#pragma pack(push, 1)

// - SOURCE_SWITCH_CONTROL

struct PeripheralParamSourceSwitchControl : PeripheralParam
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::PERI_RET_PARAM,
        Command::PERI_SET_PARAM,
        Command::PERI_NTFY_PARAM
    };

    MessageMdrV2OnOffSettingValue sourceKeeping; // 0x2

    PeripheralParamSourceSwitchControl(MessageMdrV2OnOffSettingValue sourceKeeping)
        : PeripheralParam(CT_Get /*Any can do*/, PeripheralInquiredType::SOURCE_SWITCH_CONTROL)
        , sourceKeeping(sourceKeeping)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return PeripheralParam::isValid(buf, ct)
            && buf.size() == sizeof(PeripheralParamSourceSwitchControl)
            && buf[offsetof(PeripheralParamSourceSwitchControl, inquiredType)] == static_cast<uint8_t>(PeripheralInquiredType::SOURCE_SWITCH_CONTROL)
            && MessageMdrV2OnOffSettingValue_isValidByteCode(buf[offsetof(PeripheralParamSourceSwitchControl, sourceKeeping)]);
    }
};

// - PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE

#pragma pack(pop)

struct PeripheralParamPairingDeviceManagementWithBluetoothClassOfDevice : PeripheralParam
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::PERI_RET_PARAM,
        Command::UNKNOWN,
        Command::PERI_NTFY_PARAM
    };

    static constexpr size_t INDEX_NUM_OF_PAIRED_DEVICE = 2;
    static constexpr size_t BLUETOOTH_DEVICE_ADDRESS_LENGTH = 17;
    // App has this set to 1, however the firmware allows the friendly name to be empty.
    // See https://github.com/mos9527/SonyHeadphonesClient/issues/21
    static constexpr size_t MIN_BT_FRIENDLY_NAME_LENGTH = 0;
    static constexpr size_t BLUETOOTH_CLASS_OF_DEVICE_LENGTH = 3;
    static constexpr size_t MAX_BT_FRIENDLY_NAME_LENGTH = 128;

    std::vector<PeripheralDeviceInfoWithBluetoothClassOfDevice> deviceList;
    uint8_t playbackrightDevice;

    PeripheralParamPairingDeviceManagementWithBluetoothClassOfDevice(
        CommandType ct, const std::span<const PeripheralDeviceInfoWithBluetoothClassOfDevice> deviceList,
        uint8_t playbackrightDevice
    )
        : PeripheralParam(ct, PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE)
        , deviceList(deviceList.begin(), deviceList.end())
        , playbackrightDevice(playbackrightDevice)
    {
        if (this->deviceList.size() > 255)
            throw std::invalid_argument("Device list size exceeds 255");
    }

private:
    // For init by deserialization
    PeripheralParamPairingDeviceManagementWithBluetoothClassOfDevice()
        : PeripheralParam(CT_Get /*Any can do*/, PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE)
    {}

public:
    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        if (!PeripheralParam::isValid(buf, ct))
            return false;
        if (buf[offsetof(PeripheralParam, inquiredType)] != static_cast<uint8_t>(PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE))
            return false;

        size_t indexDevice = INDEX_NUM_OF_PAIRED_DEVICE + 1;
        if (buf.size() < indexDevice)
            return false;

        uint8_t devicesNum = buf[INDEX_NUM_OF_PAIRED_DEVICE];
        for (uint8_t i = 0; i < devicesNum; ++i)
        {
            size_t indexConnectedStatus = indexDevice + BLUETOOTH_DEVICE_ADDRESS_LENGTH;
            if (buf.size() < indexConnectedStatus)
                return false;

            size_t indexBluetoothClassOfDevice = indexConnectedStatus + 1;
            if (buf.size() < indexBluetoothClassOfDevice)
                return false;

            size_t indexBtFriendlyNameLength = indexBluetoothClassOfDevice + BLUETOOTH_CLASS_OF_DEVICE_LENGTH;
            if (buf.size() < indexBtFriendlyNameLength)
                return false;

            size_t btFriendlyNameLength = buf[indexBtFriendlyNameLength];
            if (btFriendlyNameLength < MIN_BT_FRIENDLY_NAME_LENGTH || MAX_BT_FRIENDLY_NAME_LENGTH < btFriendlyNameLength)
                return false;

            size_t indexBtFriendlyName = indexBtFriendlyNameLength + 1;
            if (buf.size() < indexBtFriendlyName)
                return false;

            /*if (!isValidBluetoothDeviceAddress(
                    std::string(reinterpret_cast<const char*>(&buf[indexDevice]), BLUETOOTH_DEVICE_ADDRESS_LENGTH)))
                return false;*/

            indexDevice = indexBtFriendlyName + btFriendlyNameLength;
        }

        size_t indexPlaybackrightDevice = indexDevice;
        if (buf.size() != indexPlaybackrightDevice + 1)
            return false;

        return true;
    }

    static PeripheralParamPairingDeviceManagementWithBluetoothClassOfDevice deserialize(std::span<const uint8_t>& buf, CommandType ct)
    {
        if (!isValid(buf, ct))
            throw std::runtime_error("Buffer invalid for PeripheralParamPairingDeviceManagementWithBluetoothClassOfDevice");

        PeripheralParamPairingDeviceManagementWithBluetoothClassOfDevice result;
        result.command = static_cast<Command>(buf[0]);
        result.inquiredType = static_cast<PeripheralInquiredType>(buf[1]);

        uint8_t deviceCount = buf[INDEX_NUM_OF_PAIRED_DEVICE];
        buf = buf.subspan(3);

        result.deviceList.reserve(deviceCount);
        for (uint8_t i = 0; i < deviceCount; ++i)
        {
            result.deviceList.push_back(PeripheralDeviceInfoWithBluetoothClassOfDevice::deserialize(buf));
        }

        result.playbackrightDevice = buf[0];
        return result;
    }

    void serialize(std::vector<uint8_t>& buf) const
    {
        buf.push_back(static_cast<uint8_t>(command));
        buf.push_back(static_cast<uint8_t>(inquiredType));
        buf.push_back(static_cast<uint8_t>(deviceList.size()));
        for (const PeripheralDeviceInfoWithBluetoothClassOfDevice& device : deviceList)
        {
            device.serialize(buf);
        }
        buf.push_back(playbackrightDevice);
    }

    size_t countBytes() const
    {
        size_t size = 1 + 1 + 1; // command + inquiredType + device count
        for (const PeripheralDeviceInfoWithBluetoothClassOfDevice& device : deviceList)
        {
            size += device.countBytes();
        }
        size += 1; // playbackrightDevice
        return size;
    }

    static constexpr bool VARIABLE_SIZE_NEEDS_SERIALIZATION = true;
};

#pragma pack(push, 1)

// - MUSIC_HAND_OVER_SETTING

struct PeripheralParamMusicHandOverSetting : PeripheralParam
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::PERI_RET_PARAM,
        Command::PERI_SET_PARAM,
        Command::PERI_NTFY_PARAM
    };

    MessageMdrV2OnOffSettingValue isOn; // 0x2

    PeripheralParamMusicHandOverSetting(CommandType ct, MessageMdrV2OnOffSettingValue isOn)
        : PeripheralParam(ct, PeripheralInquiredType::MUSIC_HAND_OVER_SETTING)
        , isOn(isOn)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return PeripheralParam::isValid(buf, ct)
            && buf.size() == sizeof(PeripheralParamMusicHandOverSetting)
            && buf[offsetof(PeripheralParamMusicHandOverSetting, inquiredType)] == static_cast<uint8_t>(PeripheralInquiredType::MUSIC_HAND_OVER_SETTING)
            && MessageMdrV2OnOffSettingValue_isValidByteCode(buf[offsetof(PeripheralParamMusicHandOverSetting, isOn)]);
    }
};

// endregion PERI_RET_PARAM, PERI_SET_PARAM, PERI_NTFY_PARAM

// endregion PERI_*_PARAM

// endregion PERI

// region SAFE_LISTENING

// region SAFE_LISTENING Common Enums

enum class SafeListeningInquiredType : uint8_t
{
    SAFE_LISTENING_HBS_1 = 0x0,
    SAFE_LISTENING_TWS_1 = 0x1,
    SAFE_LISTENING_HBS_2 = 0x2,
    SAFE_LISTENING_TWS_2 = 0x3,
    SAFE_VOLUME_CONTROL = 0x4,
};

inline bool SafeListeningInquiredType_isValidByteCode(uint8_t type)
{
    switch (static_cast<SafeListeningInquiredType>(type))
    {
    case SafeListeningInquiredType::SAFE_LISTENING_HBS_1:
    case SafeListeningInquiredType::SAFE_LISTENING_TWS_1:
    case SafeListeningInquiredType::SAFE_LISTENING_HBS_2:
    case SafeListeningInquiredType::SAFE_LISTENING_TWS_2:
    case SafeListeningInquiredType::SAFE_VOLUME_CONTROL:
        return true;
    }
    return false;
}

enum class SafeListeningErrorCause : uint8_t
{
    NOT_PLAYING = 0x0,
    IN_CALL = 0x1,
    DETACHED = 0x2,
};

inline bool SafeListeningErrorCause_isValidByteCode(uint8_t cause)
{
    switch (static_cast<SafeListeningErrorCause>(cause))
    {
    case SafeListeningErrorCause::NOT_PLAYING:
    case SafeListeningErrorCause::IN_CALL:
    case SafeListeningErrorCause::DETACHED:
        return true;
    }
    return false;
}

// endregion SAFE_LISTENING Common Enums

// region SAFE_LISTENING_*_CAPABILITY

// region SAFE_LISTENING_GET_CAPABILITY

struct SafeListeningGetCapability : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::SAFE_LISTENING_RET_CAPABILITY;

    SafeListeningInquiredType inquiredType; // 0x1

    SafeListeningGetCapability(SafeListeningInquiredType inquiredType)
        : Payload(Command::SAFE_LISTENING_GET_CAPABILITY)
        , inquiredType(inquiredType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(SafeListeningGetCapability)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::SAFE_LISTENING_GET_CAPABILITY)
            && isValidInquiredType(static_cast<SafeListeningInquiredType>(buf[offsetof(SafeListeningGetCapability, inquiredType)]));
    }

    static bool isValidInquiredType(SafeListeningInquiredType type)
    {
        return type == SafeListeningInquiredType::SAFE_LISTENING_HBS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_HBS_2
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
    }
};

// endregion SAFE_LISTENING_GET_CAPABILITY

// region SAFE_LISTENING_RET_CAPABILITY

struct SafeListeningRetCapability : Payload
{
    SafeListeningInquiredType inquiredType; // 0x1
    uint8_t roundBase; // 0x2
    uint32_t timestampBase_needsByteSwap; // 0x3-0x6
    uint8_t minimumInterval; // 0x7
    uint8_t logCapacity; // 0x8

    SafeListeningRetCapability(
        SafeListeningInquiredType inquiredType, uint8_t roundBase, uint32_t timestampBase, uint8_t minimumInterval,
        uint8_t logCapacity
    )
        : Payload(Command::SAFE_LISTENING_RET_CAPABILITY)
        , inquiredType(inquiredType)
        , roundBase(roundBase)
        , timestampBase_needsByteSwap(byteOrderSwap(timestampBase))
        , minimumInterval(minimumInterval)
        , logCapacity(logCapacity)
    {}

    uint32_t getTimestampBase() const
    {
        return byteOrderSwap(timestampBase_needsByteSwap);
    }

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(SafeListeningRetCapability)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::SAFE_LISTENING_RET_CAPABILITY)
            && isValidInquiredType(static_cast<SafeListeningInquiredType>(buf[offsetof(SafeListeningRetCapability, inquiredType)]));
    }

    static bool isValidInquiredType(SafeListeningInquiredType type)
    {
        return type == SafeListeningInquiredType::SAFE_LISTENING_HBS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_HBS_2
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
    }
};

// endregion SAFE_LISTENING_RET_CAPABILITY

// endregion SAFE_LISTENING_*_CAPABILITY

// region SAFE_LISTENING_*_STATUS

// region SAFE_LISTENING_*_STATUS Common Structs and Enums

enum class SafeListeningTargetType : uint8_t
{
    HBS = 0x00,
    TWS_L = 0x01,
    TWS_R = 0x02,
};

inline bool SafeListeningTargetType_isValidByteCode(uint8_t type)
{
    switch (static_cast<SafeListeningTargetType>(type))
    {
    case SafeListeningTargetType::HBS:
    case SafeListeningTargetType::TWS_L:
    case SafeListeningTargetType::TWS_R:
        return true;
    }
    return false;
}

struct SafeListeningData
{
    SafeListeningTargetType targetType; // 0x0
    uint32_t timestamp_needsByteSwap; // 0x1-0x4
    uint16_t rtcRc_needsByteSwap; // 0x5-0x6
    uint8_t viewTime; // 0x7
    uint32_t soundPressure_needsByteSwap; // 0x8-0xB

protected:
    SafeListeningData() {}

    SafeListeningData(
        SafeListeningTargetType targetType, uint32_t timestamp, uint16_t rtcRc, uint8_t viewTime, uint32_t soundPressure
    )
        : targetType(targetType)
        , timestamp_needsByteSwap(byteOrderSwap(timestamp))
        , rtcRc_needsByteSwap(byteOrderSwap(rtcRc))
        , viewTime(viewTime)
        , soundPressure_needsByteSwap(byteOrderSwap(soundPressure))
    {}

public:
    uint32_t getTimestamp() const
    {
        return byteOrderSwap(timestamp_needsByteSwap);
    }

    uint16_t getRtcRc() const
    {
        return byteOrderSwap(rtcRc_needsByteSwap);
    }

    uint32_t getSoundPressure() const
    {
        return byteOrderSwap(soundPressure_needsByteSwap);
    }

protected:
    static bool isValid(const std::span<const uint8_t>& buf)
    {
        // Size checked by leaf structs
        return SafeListeningTargetType_isValidByteCode(buf[offsetof(SafeListeningData, targetType)]);
    }
};

struct SafeListeningData1 : SafeListeningData
{
    SafeListeningData1() {}

    SafeListeningData1(
        SafeListeningTargetType targetType, uint32_t timestamp, uint16_t rtcRc, uint8_t viewTime, uint32_t soundPressure
    )
        : SafeListeningData(targetType, timestamp, rtcRc, viewTime, soundPressure)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return buf.size() == sizeof(SafeListeningData1)
            && SafeListeningData::isValid(buf);
    }
};

struct SafeListeningData2 : SafeListeningData
{
    uint8_t ambientTime; // 0xC

    SafeListeningData2() {}

    SafeListeningData2(
        SafeListeningTargetType targetType, uint32_t timestamp, uint16_t rtcRc, uint8_t viewTime, uint32_t soundPressure,
        uint8_t ambientTime
    )
        : SafeListeningData(targetType, timestamp, rtcRc, viewTime, soundPressure)
        , ambientTime(ambientTime)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return buf.size() == sizeof(SafeListeningData2)
            && SafeListeningData::isValid(buf);
    }
};

struct SafeListeningStatus
{
    uint32_t timestamp_needsByteSwap; // 0x0-0x3
    uint16_t rtcRc_needsByteSwap; // 0x4-0x5

    SafeListeningStatus(uint32_t timestamp, uint16_t rtcRc)
        : timestamp_needsByteSwap(byteOrderSwap(timestamp))
        , rtcRc_needsByteSwap(byteOrderSwap(rtcRc))
    {}

    uint32_t getTimestamp() const
    {
        return byteOrderSwap(timestamp_needsByteSwap);
    }

    uint16_t getRtcRc() const
    {
        return byteOrderSwap(rtcRc_needsByteSwap);
    }

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return buf.size() == sizeof(SafeListeningStatus);
    }
};

enum class SafeListeningLogDataStatus : uint8_t
{
    DISCONNECTED = 0x00,
    SENDING = 0x01,
    COMPLETED = 0x02,
    NOT_SENDING = 0x03,
    ERROR = 0x04,
};

inline bool SafeListeningLogDataStatus_isValidByteCode(uint8_t status)
{
    switch (static_cast<SafeListeningLogDataStatus>(status))
    {
        case SafeListeningLogDataStatus::DISCONNECTED:
        case SafeListeningLogDataStatus::SENDING:
        case SafeListeningLogDataStatus::COMPLETED:
        case SafeListeningLogDataStatus::NOT_SENDING:
        case SafeListeningLogDataStatus::ERROR:
            return true;
    }
    return false;
}

enum class SafeListeningWHOStandardLevel : uint8_t
{
    NORMAL = 0x0,
    SENSITIVE = 0x1,
};

inline bool SafeListeningWHOStandardLevel_isValidByteCode(uint8_t level)
{
    switch (static_cast<SafeListeningWHOStandardLevel>(level))
    {
        case SafeListeningWHOStandardLevel::NORMAL:
        case SafeListeningWHOStandardLevel::SENSITIVE:
            return true;
    }
    return false;
}

// endregion SAFE_LISTENING_*_STATUS Common Structs and Enums

// region SAFE_LISTENING_GET_STATUS

struct SafeListeningGetStatus : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::SAFE_LISTENING_RET_STATUS;

    SafeListeningInquiredType inquiredType; // 0x1

    SafeListeningGetStatus(SafeListeningInquiredType inquiredType)
        : Payload(Command::SAFE_LISTENING_GET_STATUS)
        , inquiredType(inquiredType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(SafeListeningGetStatus)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::SAFE_LISTENING_GET_STATUS)
            && isValidInquiredType(static_cast<SafeListeningInquiredType>(buf[offsetof(SafeListeningGetStatus, inquiredType)]));
    }

    static bool isValidInquiredType(SafeListeningInquiredType type)
    {
        return type == SafeListeningInquiredType::SAFE_LISTENING_HBS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_HBS_2
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
    }
};

// endregion SAFE_LISTENING_GET_STATUS

// region SAFE_LISTENING_RET_STATUS

struct SafeListeningRetStatus : Payload
{
    SafeListeningInquiredType inquiredType; // 0x1

protected:
    SafeListeningRetStatus(SafeListeningInquiredType inquiredType)
        : Payload(Command::SAFE_LISTENING_RET_STATUS)
        , inquiredType(inquiredType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() >= sizeof(SafeListeningRetStatus)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::SAFE_LISTENING_RET_STATUS)
            && SafeListeningInquiredType_isValidByteCode(buf[offsetof(SafeListeningRetStatus, inquiredType)]);
    }
};

// - SAFE_LISTENING_HBS_*, SAFE_LISTENING_TWS_*

struct SafeListeningRetStatusSL : SafeListeningRetStatus
{
protected:
    SafeListeningRetStatusSL(SafeListeningInquiredType inquiredType)
        : SafeListeningRetStatus(inquiredType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningRetStatus::isValid(buf)
            && isValidInquiredType(static_cast<SafeListeningInquiredType>(buf[offsetof(SafeListeningRetStatusSL, inquiredType)]));
    }

    static bool isValidInquiredType(SafeListeningInquiredType type)
    {
        return type == SafeListeningInquiredType::SAFE_LISTENING_HBS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_HBS_2
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
    }
};

// - SAFE_LISTENING_HBS_*

struct SafeListeningRetStatusHbs : SafeListeningRetStatusSL
{
    SafeListeningLogDataStatus logDataStatus; // 0x2

protected:
    SafeListeningRetStatusHbs(
        SafeListeningInquiredType inquiredType, SafeListeningLogDataStatus logDataStatus
    )
        : SafeListeningRetStatusSL(inquiredType)
        , logDataStatus(logDataStatus)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningRetStatusSL::isValid(buf)
            && SafeListeningLogDataStatus_isValidByteCode(buf[offsetof(SafeListeningRetStatusHbs, logDataStatus)]);
    }
};

// - SAFE_LISTENING_HBS_1

struct SafeListeningRetStatusHbs1 : SafeListeningRetStatusHbs
{
    SafeListeningData1 currentData; // 0x3-0xF

    SafeListeningRetStatusHbs1(
        SafeListeningLogDataStatus logDataStatus, const SafeListeningData1& currentData
    )
        : SafeListeningRetStatusHbs(SafeListeningInquiredType::SAFE_LISTENING_HBS_1, logDataStatus)
        , currentData(currentData)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningRetStatusHbs::isValid(buf)
            && buf.size() == sizeof(SafeListeningRetStatusHbs1)
            && buf[offsetof(SafeListeningRetStatusHbs1, inquiredType)] == static_cast<uint8_t>(SafeListeningInquiredType::SAFE_LISTENING_HBS_1)
            && SafeListeningData1::isValid(buf.subspan(offsetof(SafeListeningRetStatusHbs1, currentData), sizeof(SafeListeningData1)));
    }
};

// - SAFE_LISTENING_HBS_2

struct SafeListeningRetStatusHbs2 : SafeListeningRetStatusHbs
{
    SafeListeningData2 currentData; // 0x3-0x10

    SafeListeningRetStatusHbs2(
        SafeListeningLogDataStatus logDataStatus, const SafeListeningData2& currentData
    )
        : SafeListeningRetStatusHbs(SafeListeningInquiredType::SAFE_LISTENING_HBS_2, logDataStatus)
        , currentData(currentData)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningRetStatusHbs::isValid(buf)
            && buf.size() == sizeof(SafeListeningRetStatusHbs2)
            && buf[offsetof(SafeListeningRetStatusHbs2, inquiredType)] == static_cast<uint8_t>(SafeListeningInquiredType::SAFE_LISTENING_HBS_2)
            && SafeListeningData2::isValid(buf.subspan(offsetof(SafeListeningRetStatusHbs2, currentData), sizeof(SafeListeningData2)));
    }
};

// - SAFE_LISTENING_TWS_*

struct SafeListeningRetStatusTws : SafeListeningRetStatusSL
{
    SafeListeningLogDataStatus logDataStatusLeft; // 0x2
    SafeListeningLogDataStatus logDataStatusRight; // 0x3

protected:
    SafeListeningRetStatusTws(
        SafeListeningInquiredType inquiredType, SafeListeningLogDataStatus logDataStatusLeft,
        SafeListeningLogDataStatus logDataStatusRight
    )
        : SafeListeningRetStatusSL(inquiredType)
        , logDataStatusLeft(logDataStatusLeft)
        , logDataStatusRight(logDataStatusRight)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningRetStatusSL::isValid(buf)
            && buf.size() >= sizeof(SafeListeningRetStatusTws)
            && SafeListeningLogDataStatus_isValidByteCode(buf[offsetof(SafeListeningRetStatusTws, logDataStatusLeft)])
            && SafeListeningLogDataStatus_isValidByteCode(buf[offsetof(SafeListeningRetStatusTws, logDataStatusRight)]);
    }
};

// - SAFE_LISTENING_TWS_1

struct SafeListeningRetStatusTws1 : SafeListeningRetStatusTws
{
    SafeListeningData1 currentDataLeft; // 0x4-0xF
    SafeListeningData1 currentDataRight; // 0x10-0x1B

    SafeListeningRetStatusTws1(
        SafeListeningLogDataStatus logDataStatusLeft, SafeListeningLogDataStatus logDataStatusRight,
        const SafeListeningData1& currentDataLeft, const SafeListeningData1& currentDataRight
    )
        : SafeListeningRetStatusTws(SafeListeningInquiredType::SAFE_LISTENING_TWS_1, logDataStatusLeft, logDataStatusRight)
        , currentDataLeft(currentDataLeft)
        , currentDataRight(currentDataRight)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningRetStatusTws::isValid(buf)
            && buf.size() == sizeof(SafeListeningRetStatusTws1)
            && buf[offsetof(SafeListeningRetStatusTws1, inquiredType)] == static_cast<uint8_t>(SafeListeningInquiredType::SAFE_LISTENING_TWS_1)
            && SafeListeningData1::isValid(buf.subspan(offsetof(SafeListeningRetStatusTws1, currentDataLeft), sizeof(SafeListeningData1)))
            && SafeListeningData1::isValid(buf.subspan(offsetof(SafeListeningRetStatusTws1, currentDataRight), sizeof(SafeListeningData1)));
    }
};

// - SAFE_LISTENING_TWS_2

struct SafeListeningRetStatusTws2 : SafeListeningRetStatusTws
{
    SafeListeningData2 currentDataLeft; // 0x4-0x10
    SafeListeningData2 currentDataRight; // 0x11-0x1D

    SafeListeningRetStatusTws2(
        SafeListeningLogDataStatus logDataStatusLeft, SafeListeningLogDataStatus logDataStatusRight,
        const SafeListeningData2& currentDataLeft, const SafeListeningData2& currentDataRight
    )
        : SafeListeningRetStatusTws(SafeListeningInquiredType::SAFE_LISTENING_TWS_2, logDataStatusLeft, logDataStatusRight)
        , currentDataLeft(currentDataLeft)
        , currentDataRight(currentDataRight)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningRetStatusTws::isValid(buf)
            && buf.size() == sizeof(SafeListeningRetStatusTws2)
            && buf[offsetof(SafeListeningRetStatusTws2, inquiredType)] == static_cast<uint8_t>(SafeListeningInquiredType::SAFE_LISTENING_TWS_2)
            && SafeListeningData2::isValid(buf.subspan(offsetof(SafeListeningRetStatusTws2, currentDataLeft), sizeof(SafeListeningData2)))
            && SafeListeningData2::isValid(buf.subspan(offsetof(SafeListeningRetStatusTws2, currentDataRight), sizeof(SafeListeningData2)));
    }
};

// endregion SAFE_LISTENING_RET_STATUS

// region SAFE_LISTENING_SET_STATUS

struct SafeListeningSetStatus : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::SAFE_LISTENING_NTFY_STATUS;

    SafeListeningInquiredType inquiredType; // 0x1

protected:
    SafeListeningSetStatus(SafeListeningInquiredType inquiredType)
        : Payload(Command::SAFE_LISTENING_SET_STATUS)
        , inquiredType(inquiredType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        // Size checked by leaf structs
        return Payload::isValid(buf)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::SAFE_LISTENING_SET_STATUS)
            && SafeListeningInquiredType_isValidByteCode(buf[offsetof(SafeListeningSetStatus, inquiredType)]);
    }
};

// - SAFE_LISTENING_HBS_*, SAFE_LISTENING_TWS_*

struct SafeListeningSetStatusSL : SafeListeningSetStatus
{
protected:
    SafeListeningSetStatusSL(SafeListeningInquiredType inquiredType)
        : SafeListeningSetStatus(inquiredType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        // Size checked by leaf structs
        return SafeListeningSetStatus::isValid(buf)
            && isValidInquiredType(static_cast<SafeListeningInquiredType>(buf[offsetof(SafeListeningSetStatusSL, inquiredType)]));
    }

    static bool isValidInquiredType(SafeListeningInquiredType type)
    {
        return type == SafeListeningInquiredType::SAFE_LISTENING_HBS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_HBS_2
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
    }
};

// - SAFE_LISTENING_HBS_1, SAFE_LISTENING_HBS_2

struct SafeListeningSetStatusHbs : SafeListeningSetStatusSL
{
    SafeListeningLogDataStatus logDataStatus; // 0x2
    SafeListeningStatus status; // 0x3-0x8

    SafeListeningSetStatusHbs(
        SafeListeningInquiredType inquiredType, SafeListeningLogDataStatus logDataStatus,
        const SafeListeningStatus& status
    )
        : SafeListeningSetStatusSL(inquiredType)
        , logDataStatus(logDataStatus)
        , status(status)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningSetStatusSL::isValid(buf)
            && buf.size() == sizeof(SafeListeningSetStatusHbs)
            && isValidInquiredType(static_cast<SafeListeningInquiredType>(buf[offsetof(SafeListeningSetStatusHbs, inquiredType)]))
            && SafeListeningLogDataStatus_isValidByteCode(buf[offsetof(SafeListeningSetStatusHbs, logDataStatus)])
            && SafeListeningStatus::isValid(buf.subspan(offsetof(SafeListeningSetStatusHbs, status), sizeof(SafeListeningStatus)));
    }

    static bool isValidInquiredType(SafeListeningInquiredType type)
    {
        return type == SafeListeningInquiredType::SAFE_LISTENING_HBS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_HBS_2;
    }
};

// - SAFE_LISTENING_TWS_1, SAFE_LISTENING_TWS_2

struct SafeListeningSetStatusTws : SafeListeningSetStatusSL
{
    SafeListeningLogDataStatus logDataStatusLeft; // 0x2
    SafeListeningLogDataStatus logDataStatusRight; // 0x3
    SafeListeningStatus statusLeft; // 0x4-0x9
    SafeListeningStatus statusRight; // 0xA-0xF

    SafeListeningSetStatusTws(
        SafeListeningInquiredType inquiredType, SafeListeningLogDataStatus logDataStatusLeft,
        SafeListeningLogDataStatus logDataStatusRight, const SafeListeningStatus& statusLeft,
        const SafeListeningStatus& statusRight
    )
        : SafeListeningSetStatusSL(inquiredType)
        , logDataStatusLeft(logDataStatusLeft)
        , logDataStatusRight(logDataStatusRight)
        , statusLeft(statusLeft)
        , statusRight(statusRight)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningSetStatusSL::isValid(buf)
            && buf.size() == sizeof(SafeListeningSetStatusTws)
            && isValidInquiredType(static_cast<SafeListeningInquiredType>(buf[offsetof(SafeListeningSetStatusTws, inquiredType)]))
            && SafeListeningLogDataStatus_isValidByteCode(buf[offsetof(SafeListeningSetStatusTws, logDataStatusLeft)])
            && SafeListeningLogDataStatus_isValidByteCode(buf[offsetof(SafeListeningSetStatusTws, logDataStatusRight)])
            && SafeListeningStatus::isValid(buf.subspan(offsetof(SafeListeningSetStatusTws, statusLeft), sizeof(SafeListeningStatus)))
            && SafeListeningStatus::isValid(buf.subspan(offsetof(SafeListeningSetStatusTws, statusRight), sizeof(SafeListeningStatus)));
    }

    static bool isValidInquiredType(SafeListeningInquiredType type)
    {
        return type == SafeListeningInquiredType::SAFE_LISTENING_TWS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
    }
};

// - SAFE_VOLUME_CONTROL

struct SafeListeningSetStatusSVC : SafeListeningSetStatus
{
    SafeListeningWHOStandardLevel whoStandardLevel; // 0x2

    SafeListeningSetStatusSVC(SafeListeningWHOStandardLevel whoStandardLevel)
        : SafeListeningSetStatus(SafeListeningInquiredType::SAFE_VOLUME_CONTROL)
        , whoStandardLevel(whoStandardLevel)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningSetStatus::isValid(buf)
            && buf.size() == sizeof(SafeListeningSetStatusSVC)
            && buf[offsetof(SafeListeningSetStatusSVC, inquiredType)] == static_cast<uint8_t>(SafeListeningInquiredType::SAFE_VOLUME_CONTROL)
            && SafeListeningWHOStandardLevel_isValidByteCode(buf[offsetof(SafeListeningSetStatusSVC, whoStandardLevel)]);
    }
};

// endregion SAFE_LISTENING_SET_STATUS

// region SAFE_LISTENING_NTFY_STATUS

struct SafeListeningNotifyStatus : Payload
{
    SafeListeningInquiredType inquiredType; // 0x1

protected:
    SafeListeningNotifyStatus(SafeListeningInquiredType inquiredType)
        : Payload(Command::SAFE_LISTENING_NTFY_STATUS)
        , inquiredType(inquiredType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() >= sizeof(SafeListeningNotifyStatus)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::SAFE_LISTENING_NTFY_STATUS)
            && SafeListeningInquiredType_isValidByteCode(buf[offsetof(SafeListeningNotifyStatus, inquiredType)]);
    }
};

// - SAFE_LISTENING_HBS_*, SAFE_LISTENING_TWS_*

struct SafeListeningNotifyStatusSL : SafeListeningNotifyStatus
{
protected:
    SafeListeningNotifyStatusSL(SafeListeningInquiredType inquiredType)
        : SafeListeningNotifyStatus(inquiredType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningNotifyStatus::isValid(buf)
            && isValidInquiredType(static_cast<SafeListeningInquiredType>(buf[offsetof(SafeListeningNotifyStatusSL, inquiredType)]));
    }

    static bool isValidInquiredType(SafeListeningInquiredType type)
    {
        return type == SafeListeningInquiredType::SAFE_LISTENING_HBS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_HBS_2
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
    }
};

// - SAFE_LISTENING_HBS_1, SAFE_LISTENING_HBS_2

struct SafeListeningNotifyStatusHbs : SafeListeningNotifyStatusSL
{
    SafeListeningLogDataStatus logDataStatus; // 0x2

protected:
    SafeListeningNotifyStatusHbs(
        SafeListeningInquiredType inquiredType, SafeListeningLogDataStatus logDataStatus
    )
        : SafeListeningNotifyStatusSL(inquiredType)
        , logDataStatus(logDataStatus)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningNotifyStatusSL::isValid(buf)
            && buf.size() >= sizeof(SafeListeningNotifyStatusHbs)
            && isValidInquiredType(static_cast<SafeListeningInquiredType>(buf[offsetof(SafeListeningNotifyStatusHbs, inquiredType)]))
            && SafeListeningLogDataStatus_isValidByteCode(buf[offsetof(SafeListeningNotifyStatusHbs, logDataStatus)]);
    }

    static bool isValidInquiredType(SafeListeningInquiredType type)
    {
        return type == SafeListeningInquiredType::SAFE_LISTENING_HBS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_HBS_2;
    }
};

// - SAFE_LISTENING_HBS_1

struct SafeListeningNotifyStatusHbs1 : SafeListeningNotifyStatusHbs
{
    uint8_t logDataNum; // 0x3
    SafeListeningData1 logData[]; // 0x4-

private:
    SafeListeningNotifyStatusHbs1(
        SafeListeningLogDataStatus logDataStatus, const std::span<const SafeListeningData1>& logDataSpan
    )
        : SafeListeningNotifyStatusHbs(SafeListeningInquiredType::SAFE_LISTENING_HBS_1, logDataStatus)
        , logDataNum(logDataSpan.size())
    {
        std::memcpy(logData, logDataSpan.data(), sizeof(SafeListeningData1) * logDataNum);
    }

public:
    VARIABLE_SIZE_PAYLOAD_ONE_ARRAY_AT_END(255);

    std::span<const SafeListeningData1> getLogDataSpan() const
    {
        return { logData, logDataNum };
    }

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningNotifyStatusHbs::isValid(buf)
            && buf.size() >= sizeof(SafeListeningNotifyStatusHbs1)
            && buf.size() == sizeof(SafeListeningNotifyStatusHbs1)
                + sizeof(SafeListeningData1) * buf[offsetof(SafeListeningNotifyStatusHbs1, logDataNum)]
            && buf[offsetof(SafeListeningNotifyStatusHbs1, inquiredType)] == static_cast<uint8_t>(SafeListeningInquiredType::SAFE_LISTENING_HBS_1)
            && isValidLogData(buf);
    }

private:
    static bool isValidLogData(const std::span<const uint8_t>& buf)
    {
        size_t logDataNum = buf[offsetof(SafeListeningNotifyStatusHbs1, logDataNum)];
        for (size_t i = 0; i < logDataNum; i++)
        {
            size_t logDataIndex = offsetof(SafeListeningNotifyStatusHbs1, logData) + sizeof(SafeListeningData1) * i;
            if (!SafeListeningTargetType_isValidByteCode(buf[logDataIndex + offsetof(SafeListeningData1, targetType)]))
            {
                return false;
            }
        }
        return true;
    }
};

// - SAFE_LISTENING_HBS_2

struct SafeListeningNotifyStatusHbs2 : SafeListeningNotifyStatusHbs
{
    uint8_t logDataNum; // 0x3
    SafeListeningData2 logData[]; // 0x4-

private:
    SafeListeningNotifyStatusHbs2(
        SafeListeningLogDataStatus logDataStatus, const std::span<const SafeListeningData2>& logDataSpan
    )
        : SafeListeningNotifyStatusHbs(SafeListeningInquiredType::SAFE_LISTENING_HBS_2, logDataStatus)
        , logDataNum(logDataSpan.size())
    {
        std::memcpy(logData, logDataSpan.data(), sizeof(SafeListeningData2) * logDataNum);
    }

public:
    VARIABLE_SIZE_PAYLOAD_ONE_ARRAY_AT_END(255);

    std::span<const SafeListeningData2> getLogDataSpan() const
    {
        return { logData, logDataNum };
    }

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningNotifyStatusHbs::isValid(buf)
            && buf.size() >= sizeof(SafeListeningNotifyStatusHbs2)
            && buf.size() == sizeof(SafeListeningNotifyStatusHbs2)
                + sizeof(SafeListeningData2) * buf[offsetof(SafeListeningNotifyStatusHbs2, logDataNum)]
            && buf[offsetof(SafeListeningNotifyStatusHbs2, inquiredType)] == static_cast<uint8_t>(SafeListeningInquiredType::SAFE_LISTENING_HBS_2)
            && isValidLogData(buf);
    }

private:
    static bool isValidLogData(const std::span<const uint8_t>& buf)
    {
        size_t logDataNum = buf[offsetof(SafeListeningNotifyStatusHbs2, logDataNum)];
        for (size_t i = 0; i < logDataNum; i++)
        {
            size_t logDataIndex = offsetof(SafeListeningNotifyStatusHbs2, logData) + sizeof(SafeListeningData2) * i;
            if (!SafeListeningTargetType_isValidByteCode(buf[logDataIndex + offsetof(SafeListeningData2, targetType)]))
            {
                return false;
            }
        }
        return true;
    }
};

// - SAFE_LISTENING_TWS_1, SAFE_LISTENING_TWS_2

struct SafeListeningNotifyStatusTws : SafeListeningNotifyStatusSL
{
    SafeListeningLogDataStatus logDataStatusLeft; // 0x2
    SafeListeningLogDataStatus logDataStatusRight; // 0x3

protected:
    SafeListeningNotifyStatusTws(
        SafeListeningInquiredType inquiredType, SafeListeningLogDataStatus logDataStatusLeft,
        SafeListeningLogDataStatus logDataStatusRight
    )
        : SafeListeningNotifyStatusSL(inquiredType)
        , logDataStatusLeft(logDataStatusLeft)
        , logDataStatusRight(logDataStatusRight)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningNotifyStatusSL::isValid(buf)
            && buf.size() >= sizeof(SafeListeningNotifyStatusTws)
            && isValidInquiredType(static_cast<SafeListeningInquiredType>(buf[offsetof(SafeListeningNotifyStatusTws, inquiredType)]))
            && SafeListeningLogDataStatus_isValidByteCode(buf[offsetof(SafeListeningNotifyStatusTws, logDataStatusLeft)])
            && SafeListeningLogDataStatus_isValidByteCode(buf[offsetof(SafeListeningNotifyStatusTws, logDataStatusRight)]);
    }

    static bool isValidInquiredType(SafeListeningInquiredType type)
    {
        return type == SafeListeningInquiredType::SAFE_LISTENING_TWS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
    }
};

// - SAFE_LISTENING_TWS_1

struct SafeListeningNotifyStatusTws1 : SafeListeningNotifyStatusTws
{
    uint8_t logDataNum; // 0x4
    SafeListeningData1 logData[]; // 0x5-

private:
    SafeListeningNotifyStatusTws1(
        SafeListeningLogDataStatus logDataStatusLeft, SafeListeningLogDataStatus logDataStatusRight,
        const std::span<const SafeListeningData1>& logDataSpan
    )
        : SafeListeningNotifyStatusTws(SafeListeningInquiredType::SAFE_LISTENING_TWS_1, logDataStatusLeft, logDataStatusRight)
        , logDataNum(logDataSpan.size())
    {
        std::memcpy(logData, logDataSpan.data(), sizeof(SafeListeningData1) * logDataNum);
    }

public:
    VARIABLE_SIZE_PAYLOAD_ONE_ARRAY_AT_END(255);

    std::span<const SafeListeningData1> getLogDataSpan() const
    {
        return { logData, logDataNum };
    }

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningNotifyStatusTws::isValid(buf)
            && buf.size() >= sizeof(SafeListeningNotifyStatusTws1)
            && buf.size() == sizeof(SafeListeningNotifyStatusTws1)
                + sizeof(SafeListeningData1) * buf[offsetof(SafeListeningNotifyStatusTws1, logDataNum)]
            && buf[offsetof(SafeListeningNotifyStatusTws1, inquiredType)] == static_cast<uint8_t>(SafeListeningInquiredType::SAFE_LISTENING_TWS_1)
            && isValidLogData(buf);
    }

private:
    static bool isValidLogData(const std::span<const uint8_t>& buf)
    {
        size_t logDataNum = buf[offsetof(SafeListeningNotifyStatusTws1, logDataNum)];
        for (size_t i = 0; i < logDataNum; i++)
        {
            size_t logDataIndex = offsetof(SafeListeningNotifyStatusTws1, logData) + sizeof(SafeListeningData1) * i;
            if (!SafeListeningTargetType_isValidByteCode(buf[logDataIndex + offsetof(SafeListeningData1, targetType)]))
            {
                return false;
            }
        }
        return true;
    }
};

// - SAFE_LISTENING_TWS_2

struct SafeListeningNotifyStatusTws2 : SafeListeningNotifyStatusTws
{
    uint8_t logDataNum; // 0x4
    SafeListeningData2 logData[]; // 0x5-

private:
    SafeListeningNotifyStatusTws2(
        SafeListeningLogDataStatus logDataStatusLeft, SafeListeningLogDataStatus logDataStatusRight,
        const std::span<const SafeListeningData2>& logDataSpan
    )
        : SafeListeningNotifyStatusTws(SafeListeningInquiredType::SAFE_LISTENING_TWS_2, logDataStatusLeft, logDataStatusRight)
        , logDataNum(logDataSpan.size())
    {
        std::memcpy(logData, logDataSpan.data(), sizeof(SafeListeningData2) * logDataNum);
    }

public:
    VARIABLE_SIZE_PAYLOAD_ONE_ARRAY_AT_END(255);

    std::span<const SafeListeningData2> getLogDataSpan() const
    {
        return { logData, logDataNum };
    }

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningNotifyStatusTws::isValid(buf)
            && buf.size() >= sizeof(SafeListeningNotifyStatusTws2)
            && buf.size() == sizeof(SafeListeningNotifyStatusTws2)
                + sizeof(SafeListeningData2) * buf[offsetof(SafeListeningNotifyStatusTws2, logDataNum)]
            && buf[offsetof(SafeListeningNotifyStatusTws2, inquiredType)] == static_cast<uint8_t>(SafeListeningInquiredType::SAFE_LISTENING_TWS_2)
            && isValidLogData(buf);
    }

private:
    static bool isValidLogData(const std::span<const uint8_t>& buf)
    {
        size_t logDataNum = buf[offsetof(SafeListeningNotifyStatusTws2, logDataNum)];
        for (size_t i = 0; i < logDataNum; i++)
        {
            size_t logDataIndex = offsetof(SafeListeningNotifyStatusTws2, logData) + sizeof(SafeListeningData2) * i;
            if (!SafeListeningTargetType_isValidByteCode(buf[logDataIndex + offsetof(SafeListeningData2, targetType)]))
            {
                return false;
            }
        }
        return true;
    }
};

// - SAFE_VOLUME_CONTROL

struct SafeListeningNotifyStatusSVC : SafeListeningNotifyStatus
{
    SafeListeningWHOStandardLevel whoStandardLevel; // 0x2

    SafeListeningNotifyStatusSVC(SafeListeningWHOStandardLevel whoStandardLevel)
        : SafeListeningNotifyStatus(SafeListeningInquiredType::SAFE_VOLUME_CONTROL)
        , whoStandardLevel(whoStandardLevel)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningNotifyStatus::isValid(buf)
            && buf.size() == sizeof(SafeListeningNotifyStatusSVC)
            && buf[offsetof(SafeListeningNotifyStatusSVC, inquiredType)] == static_cast<uint8_t>(SafeListeningInquiredType::SAFE_VOLUME_CONTROL)
            && SafeListeningWHOStandardLevel_isValidByteCode(buf[offsetof(SafeListeningNotifyStatusSVC, whoStandardLevel)]);
    }
};

// endregion SAFE_LISTENING_NTFY_STATUS

// endregion SAFE_LISTENING_*_STATUS

// region SAFE_LISTENING_*_PARAM

// region SAFE_LISTENING_GET_PARAM

struct SafeListeningGetParam : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::SAFE_LISTENING_RET_PARAM;

    SafeListeningInquiredType inquiredType; // 0x1

    SafeListeningGetParam(SafeListeningInquiredType inquiredType)
        : Payload(Command::SAFE_LISTENING_GET_PARAM)
        , inquiredType(inquiredType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(SafeListeningGetParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::SAFE_LISTENING_GET_PARAM)
            && isValidInquiredType(static_cast<SafeListeningInquiredType>(buf[offsetof(SafeListeningGetParam, inquiredType)]));
    }

    static bool isValidInquiredType(SafeListeningInquiredType type)
    {
        return type == SafeListeningInquiredType::SAFE_LISTENING_HBS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_HBS_2
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
    }
};

// endregion SAFE_LISTENING_GET_PARAM

// region SAFE_LISTENING_RET_PARAM

struct SafeListeningRetParam : Payload
{
    SafeListeningInquiredType inquiredType; // 0x1
    MessageMdrV2EnableDisable availability; // 0x2

    SafeListeningRetParam(
        SafeListeningInquiredType inquiredType, MessageMdrV2EnableDisable availability
    )
        : Payload(Command::SAFE_LISTENING_RET_PARAM)
        , inquiredType(inquiredType)
        , availability(availability)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(SafeListeningRetParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::SAFE_LISTENING_RET_PARAM)
            && isValidInquiredType(static_cast<SafeListeningInquiredType>(buf[offsetof(SafeListeningRetParam, inquiredType)]))
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(SafeListeningRetParam, availability)]);
    }

    static bool isValidInquiredType(SafeListeningInquiredType type)
    {
        return type == SafeListeningInquiredType::SAFE_LISTENING_HBS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_HBS_2
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
    }
};

// endregion SAFE_LISTENING_RET_PARAM

// region SAFE_LISTENING_SET_PARAM

struct SafeListeningSetParam : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::SAFE_LISTENING_NTFY_PARAM;

    SafeListeningInquiredType inquiredType; // 0x1

protected:
    SafeListeningSetParam(SafeListeningInquiredType inquiredType)
        : Payload(Command::SAFE_LISTENING_SET_PARAM)
        , inquiredType(inquiredType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() >= sizeof(SafeListeningSetParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::SAFE_LISTENING_SET_PARAM)
            && SafeListeningInquiredType_isValidByteCode(buf[offsetof(SafeListeningSetParam, inquiredType)]);
    }
};

// - SAFE_LISTENING_*

struct SafeListeningSetParamSL : SafeListeningSetParam
{
    MessageMdrV2EnableDisable safeListeningMode; // 0x2
    MessageMdrV2EnableDisable previewMode; // 0x3

    SafeListeningSetParamSL(
        SafeListeningInquiredType inquiredType, MessageMdrV2EnableDisable safeListeningMode,
        MessageMdrV2EnableDisable previewMode
    )
        : SafeListeningSetParam(inquiredType)
        , safeListeningMode(safeListeningMode)
        , previewMode(previewMode)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningSetParam::isValid(buf)
            && buf.size() == sizeof(SafeListeningSetParamSL)
            && isValidInquiredType(static_cast<SafeListeningInquiredType>(buf[offsetof(SafeListeningSetParamSL, inquiredType)]))
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(SafeListeningSetParamSL, safeListeningMode)])
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(SafeListeningSetParamSL, previewMode)]);
    }

    static bool isValidInquiredType(SafeListeningInquiredType type)
    {
        return type == SafeListeningInquiredType::SAFE_LISTENING_HBS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_HBS_2
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
    }
};

// - SAFE_VOLUME_CONTROL

struct SafeListeningSetParamSVC : SafeListeningSetParam
{
    MessageMdrV2EnableDisable volumeLimitationMode; // 0x2
    MessageMdrV2EnableDisable safeVolumeControlMode; // 0x3

    SafeListeningSetParamSVC(
        MessageMdrV2EnableDisable volumeLimitationMode, MessageMdrV2EnableDisable safeVolumeControlMode
    )
        : SafeListeningSetParam(SafeListeningInquiredType::SAFE_VOLUME_CONTROL)
        , volumeLimitationMode(volumeLimitationMode)
        , safeVolumeControlMode(safeVolumeControlMode)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningSetParam::isValid(buf)
            && buf.size() == sizeof(SafeListeningSetParamSVC)
            && buf[offsetof(SafeListeningSetParamSVC, inquiredType)] == static_cast<uint8_t>(SafeListeningInquiredType::SAFE_VOLUME_CONTROL)
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(SafeListeningSetParamSVC, volumeLimitationMode)])
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(SafeListeningSetParamSVC, safeVolumeControlMode)]);
    }
};

// endregion SAFE_LISTENING_SET_PARAM

// region SAFE_LISTENING_NTFY_PARAM

struct SafeListeningNotifyParam : Payload
{
    SafeListeningInquiredType inquiredType; // 0x1

    SafeListeningNotifyParam(SafeListeningInquiredType inquiredType)
        : Payload(Command::SAFE_LISTENING_NTFY_PARAM)
        , inquiredType(inquiredType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() >= sizeof(SafeListeningNotifyParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::SAFE_LISTENING_NTFY_PARAM)
            && SafeListeningInquiredType_isValidByteCode(buf[offsetof(SafeListeningNotifyParam, inquiredType)]);
    }
};

// - SAFE_LISTENING_*

struct SafeListeningNotifyParamSL : SafeListeningNotifyParam
{
    MessageMdrV2EnableDisable safeListeningMode; // 0x2
    MessageMdrV2EnableDisable previewMode; // 0x3

    SafeListeningNotifyParamSL(
        SafeListeningInquiredType inquiredType, MessageMdrV2EnableDisable safeListeningMode,
        MessageMdrV2EnableDisable previewMode
    )
        : SafeListeningNotifyParam(inquiredType)
        , safeListeningMode(safeListeningMode)
        , previewMode(previewMode)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningNotifyParam::isValid(buf)
            && buf.size() == sizeof(SafeListeningNotifyParamSL)
            && isValidInquiredType(static_cast<SafeListeningInquiredType>(buf[offsetof(SafeListeningNotifyParamSL, inquiredType)]))
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(SafeListeningNotifyParamSL, safeListeningMode)])
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(SafeListeningNotifyParamSL, previewMode)]);
    }

    static bool isValidInquiredType(SafeListeningInquiredType type)
    {
        return type == SafeListeningInquiredType::SAFE_LISTENING_HBS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_HBS_2
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
    }
};

// - SAFE_VOLUME_CONTROL

struct SafeListeningNotifyParamSVC : SafeListeningNotifyParam
{
    MessageMdrV2EnableDisable volumeLimitationMode; // 0x2
    MessageMdrV2EnableDisable safeVolumeControlMode; // 0x3

    SafeListeningNotifyParamSVC(
        MessageMdrV2EnableDisable volumeLimitationMode, MessageMdrV2EnableDisable safeVolumeControlMode
    )
        : SafeListeningNotifyParam(SafeListeningInquiredType::SAFE_VOLUME_CONTROL)
        , volumeLimitationMode(volumeLimitationMode)
        , safeVolumeControlMode(safeVolumeControlMode)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SafeListeningNotifyParam::isValid(buf)
            && buf.size() == sizeof(SafeListeningNotifyParamSVC)
            && buf[offsetof(SafeListeningNotifyParamSVC, inquiredType)] == static_cast<uint8_t>(SafeListeningInquiredType::SAFE_VOLUME_CONTROL)
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(SafeListeningNotifyParamSVC, volumeLimitationMode)])
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(SafeListeningNotifyParamSVC, safeVolumeControlMode)]);
    }
};

// endregion SAFE_LISTENING_NTFY_PARAM

// endregion SAFE_LISTENING_*_PARAM

// region SAFE_LISTENING_*_EXTENDED_PARAM

// region SAFE_LISTENING_GET_EXTENDED_PARAM

struct SafeListeningGetExtendedParam : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::SAFE_LISTENING_RET_EXTENDED_PARAM;

    SafeListeningInquiredType inquiredType; // 0x1

    SafeListeningGetExtendedParam(SafeListeningInquiredType inquiredType)
        : Payload(Command::SAFE_LISTENING_GET_EXTENDED_PARAM)
        , inquiredType(inquiredType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(SafeListeningGetExtendedParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::SAFE_LISTENING_GET_EXTENDED_PARAM)
            && isValidInquiredType(static_cast<SafeListeningInquiredType>(buf[offsetof(SafeListeningGetExtendedParam, inquiredType)]));
    }

    static bool isValidInquiredType(SafeListeningInquiredType type)
    {
        return type == SafeListeningInquiredType::SAFE_LISTENING_HBS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_HBS_2
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
    }
};

// endregion SAFE_LISTENING_GET_EXTENDED_PARAM

// region SAFE_LISTENING_RET_EXTENDED_PARAM

struct SafeListeningRetExtendedParam : Payload
{
    SafeListeningInquiredType inquiredType; // 0x1
    uint8_t levelPerPeriod; // 0x2
    SafeListeningErrorCause errorCause; // 0x3

    SafeListeningRetExtendedParam(
        SafeListeningInquiredType inquiredType, uint8_t levelPerPeriod, SafeListeningErrorCause errorCause
    )
        : Payload(Command::SAFE_LISTENING_RET_EXTENDED_PARAM)
        , inquiredType(inquiredType)
        , levelPerPeriod(levelPerPeriod)
        , errorCause(errorCause)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(SafeListeningRetExtendedParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::SAFE_LISTENING_RET_EXTENDED_PARAM)
            && SafeListeningInquiredType_isValidByteCode(buf[offsetof(SafeListeningRetExtendedParam, inquiredType)])
            && (buf[offsetof(SafeListeningRetExtendedParam, levelPerPeriod)] != 0xFF
                || SafeListeningErrorCause_isValidByteCode(buf[offsetof(SafeListeningRetExtendedParam, errorCause)]));
    }

    static bool isValidInquiredType(SafeListeningInquiredType type)
    {
        return type == SafeListeningInquiredType::SAFE_LISTENING_HBS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_1
            || type == SafeListeningInquiredType::SAFE_LISTENING_HBS_2
            || type == SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
    }
};

// endregion SAFE_LISTENING_RET_EXTENDED_PARAM

// endregion SAFE_LISTENING_*_EXTENDED_PARAM

// endregion SAFE_LISTENING

} // namespace THMSGV2T2

#pragma pack(pop)
