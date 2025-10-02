#pragma once

#include <bitset>
#include <cstdint>
#include <span>

#include "ByteMagic.h"
#include "Constants.h"
#include "Protocol.h"

#pragma pack(push, 1)

namespace THMSGV2T1
{

// Extracted from Sound Connect iOS 11.0.1
enum class Command : uint8_t
{
    CONNECT_GET_PROTOCOL_INFO = 0x00,
    CONNECT_RET_PROTOCOL_INFO = 0x01,

    CONNECT_GET_CAPABILITY_INFO = 0x02,
    CONNECT_RET_CAPABILITY_INFO = 0x03,

    CONNECT_GET_DEVICE_INFO = 0x04,
    CONNECT_RET_DEVICE_INFO = 0x05,

    CONNECT_GET_SUPPORT_FUNCTION = 0x06,
    CONNECT_RET_SUPPORT_FUNCTION = 0x07,

    GET_TEST = 0x0F,

    COMMON_GET_CAPABILITY = 0x10,
    COMMON_RET_CAPABILITY = 0x11,

    COMMON_GET_STATUS = 0x12,
    COMMON_RET_STATUS = 0x13,
    COMMON_NTFY_STATUS = 0x15,

    COMMON_SET_PARAM = 0x18,
    COMMON_NTFY_PARAM = 0x19,

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

    UPDT_GET_CAPABILITY = 0x30,
    UPDT_RET_CAPABILITY = 0x31,

    UPDT_GET_STATUS = 0x32,
    UPDT_RET_STATUS = 0x33,
    UPDT_SET_STATUS = 0x34,
    UPDT_NTFY_STATUS = 0x35,

    UPDT_GET_PARAM = 0x36,
    UPDT_RET_PARAM = 0x37,
    UPDT_SET_PARAM = 0x38,
    UPDT_NTFY_PARAM = 0x39,

    LEA_GET_CAPABILITY = 0x40,
    LEA_RET_CAPABILITY = 0x41,

    LEA_GET_STATUS = 0x42,
    LEA_RET_STATUS = 0x43,
    LEA_NTFY_STATUS = 0x45,

    LEA_GET_PARAM = 0x46,
    LEA_RET_PARAM = 0x47,
    LEA_SET_PARAM = 0x48,
    LEA_NTFY_PARAM = 0x49,

    EQEBB_GET_STATUS = 0x52,
    EQEBB_RET_STATUS = 0x53,
    EQEBB_NTFY_STATUS = 0x55,

    EQEBB_GET_PARAM = 0x56,
    EQEBB_RET_PARAM = 0x57,
    EQEBB_SET_PARAM = 0x58,
    EQEBB_NTFY_PARAM = 0x59,

    EQEBB_GET_EXTENDED_INFO = 0x5A,
    EQEBB_RET_EXTENDED_INFO = 0x5B,

    NCASM_GET_CAPABILITY = 0x60,
    NCASM_RET_CAPABILITY = 0x61,

    NCASM_GET_STATUS = 0x62,
    NCASM_RET_STATUS = 0x63,
    NCASM_SET_STATUS = 0x64,
    NCASM_NTFY_STATUS = 0x65,

    NCASM_GET_PARAM = 0x66,
    NCASM_RET_PARAM = 0x67,
    NCASM_SET_PARAM = 0x68,
    NCASM_NTFY_PARAM = 0x69,

    SENSE_GET_CAPABILITY = 0x70,
    SENSE_RET_CAPABILITY = 0x71,
    SENSE_SET_STATUS = 0x74,
    SENSE_NTFY_STATUS = 0x75,
    SENSE_SET_PARAM = 0x78,
    SENSE_NTFY_PARAM = 0x79,
    SENSE_GET_EXT_INFO = 0x7A,
    SENSE_RET_EXT_INFO = 0x7B,

    OPT_GET_CAPABILITY = 0x80,
    OPT_RET_CAPABILITY = 0x81,
    OPT_GET_STATUS = 0x82,
    OPT_RET_STATUS = 0x83,
    OPT_SET_STATUS = 0x84,
    OPT_NTFY_STATUS = 0x85,
    OPT_GET_PARAM = 0x86,
    OPT_RET_PARAM = 0x87,
    OPT_SET_PARAM = 0x88,
    OPT_NTFY_PARAM = 0x89,

    ALERT_GET_CAPABILITY = 0x90,
    ALERT_RET_CAPABILITY = 0x91,

    ALERT_GET_STATUS = 0x92,
    ALERT_RET_STATUS = 0x93,
    ALERT_SET_STATUS = 0x94,
    ALERT_NTFY_STATUS = 0x95,

    ALERT_SET_PARAM = 0x98,
    ALERT_NTFY_PARAM = 0x99,

    PLAY_GET_CAPABILITY = 0xA0,
    PLAY_RET_CAPABILITY = 0xA1,

    PLAY_GET_STATUS = 0xA2,
    PLAY_RET_STATUS = 0xA3,
    PLAY_SET_STATUS = 0xA4,
    PLAY_NTFY_STATUS = 0xA5,

    PLAY_GET_PARAM = 0xA6,
    PLAY_RET_PARAM = 0xA7,
    PLAY_SET_PARAM = 0xA8,
    PLAY_NTFY_PARAM = 0xA9,

    SAR_AUTO_PLAY_GET_CAPABILITY = 0xB0,
    SAR_AUTO_PLAY_RET_CAPABILITY = 0xB1,

    SAR_AUTO_PLAY_GET_STATUS = 0xB2,
    SAR_AUTO_PLAY_RET_STATUS = 0xB3,
    SAR_AUTO_PLAY_NTFY_STATUS = 0xB5,

    SAR_AUTO_PLAY_GET_PARAM = 0xB6,
    SAR_AUTO_PLAY_RET_PARAM = 0xB7,
    SAR_AUTO_PLAY_SET_PARAM = 0xB8,
    SAR_AUTO_PLAY_NTFY_PARAM = 0xB9,

    LOG_SET_STATUS = 0xC4,
    LOG_NTFY_PARAM = 0xC9,

    GENERAL_SETTING_GET_CAPABILITY = 0xD0,
    GENERAL_SETTING_RET_CAPABILITY = 0xD1,

    GENERAL_SETTING_GET_STATUS = 0xD2,
    GENERAL_SETTING_RET_STATUS = 0xD3,
    GENERAL_SETTING_NTFY_STATUS = 0xD5,

    GENERAL_SETTING_GET_PARAM = 0xD6,
    GENERAL_SETTING_RET_PARAM = 0xD7,
    GENERAL_SETTING_SET_PARAM = 0xD8,
    GENERAL_SETTING_NTNY_PARAM = 0xD9,

    AUDIO_GET_CAPABILITY = 0xE0,
    AUDIO_RET_CAPABILITY = 0xE1,

    AUDIO_GET_STATUS = 0xE2,
    AUDIO_RET_STATUS = 0xE3,
    AUDIO_NTFY_STATUS = 0xE5,

    AUDIO_GET_PARAM = 0xE6,
    AUDIO_RET_PARAM = 0xE7,
    AUDIO_SET_PARAM = 0xE8,
    AUDIO_NTFY_PARAM = 0xE9,

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

    SYSTEM_GET_EXT_PARAM = 0xFA,
    SYSTEM_RET_EXT_PARAM = 0xFB,
    SYSTEM_SET_EXT_PARAM = 0xFC,
    SYSTEM_NTFY_EXT_PARAM = 0xFD,

    UNKNOWN = 0xFF
};

inline const char* Command_toString(Command cmd)
{
    switch (cmd) {
    case Command::CONNECT_GET_PROTOCOL_INFO: return "CONNECT_GET_PROTOCOL_INFO";
    case Command::CONNECT_RET_PROTOCOL_INFO: return "CONNECT_RET_PROTOCOL_INFO";
    case Command::CONNECT_GET_CAPABILITY_INFO: return "CONNECT_GET_CAPABILITY_INFO";
    case Command::CONNECT_RET_CAPABILITY_INFO: return "CONNECT_RET_CAPABILITY_INFO";
    case Command::CONNECT_GET_DEVICE_INFO: return "CONNECT_GET_DEVICE_INFO";
    case Command::CONNECT_RET_DEVICE_INFO: return "CONNECT_RET_DEVICE_INFO";
    case Command::CONNECT_GET_SUPPORT_FUNCTION: return "CONNECT_GET_SUPPORT_FUNCTION";
    case Command::CONNECT_RET_SUPPORT_FUNCTION: return "CONNECT_RET_SUPPORT_FUNCTION";
    case Command::GET_TEST: return "GET_TEST";
    case Command::COMMON_GET_CAPABILITY: return "COMMON_GET_CAPABILITY";
    case Command::COMMON_RET_CAPABILITY: return "COMMON_RET_CAPABILITY";
    case Command::COMMON_GET_STATUS: return "COMMON_GET_STATUS";
    case Command::COMMON_RET_STATUS: return "COMMON_RET_STATUS";
    case Command::COMMON_NTFY_STATUS: return "COMMON_NTFY_STATUS";
    case Command::COMMON_SET_PARAM: return "COMMON_SET_PARAM";
    case Command::COMMON_NTFY_PARAM: return "COMMON_NTFY_PARAM";
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
    case Command::UPDT_GET_CAPABILITY: return "UPDT_GET_CAPABILITY";
    case Command::UPDT_RET_CAPABILITY: return "UPDT_RET_CAPABILITY";
    case Command::UPDT_GET_STATUS: return "UPDT_GET_STATUS";
    case Command::UPDT_RET_STATUS: return "UPDT_RET_STATUS";
    case Command::UPDT_SET_STATUS: return "UPDT_SET_STATUS";
    case Command::UPDT_NTFY_STATUS: return "UPDT_NTFY_STATUS";
    case Command::UPDT_GET_PARAM: return "UPDT_GET_PARAM";
    case Command::UPDT_RET_PARAM: return "UPDT_RET_PARAM";
    case Command::UPDT_SET_PARAM: return "UPDT_SET_PARAM";
    case Command::UPDT_NTFY_PARAM: return "UPDT_NTFY_PARAM";
    case Command::LEA_GET_CAPABILITY: return "LEA_GET_CAPABILITY";
    case Command::LEA_RET_CAPABILITY: return "LEA_RET_CAPABILITY";
    case Command::LEA_GET_STATUS: return "LEA_GET_STATUS";
    case Command::LEA_RET_STATUS: return "LEA_RET_STATUS";
    case Command::LEA_NTFY_STATUS: return "LEA_NTFY_STATUS";
    case Command::LEA_GET_PARAM: return "LEA_GET_PARAM";
    case Command::LEA_RET_PARAM: return "LEA_RET_PARAM";
    case Command::LEA_SET_PARAM: return "LEA_SET_PARAM";
    case Command::LEA_NTFY_PARAM: return "LEA_NTFY_PARAM";
    case Command::EQEBB_GET_STATUS: return "EQEBB_GET_STATUS";
    case Command::EQEBB_RET_STATUS: return "EQEBB_RET_STATUS";
    case Command::EQEBB_NTFY_STATUS: return "EQEBB_NTFY_STATUS";
    case Command::EQEBB_GET_PARAM: return "EQEBB_GET_PARAM";
    case Command::EQEBB_RET_PARAM: return "EQEBB_RET_PARAM";
    case Command::EQEBB_SET_PARAM: return "EQEBB_SET_PARAM";
    case Command::EQEBB_NTFY_PARAM: return "EQEBB_NTFY_PARAM";
    case Command::EQEBB_GET_EXTENDED_INFO: return "EQEBB_GET_EXTENDED_INFO";
    case Command::EQEBB_RET_EXTENDED_INFO: return "EQEBB_RET_EXTENDED_INFO";
    case Command::NCASM_GET_CAPABILITY: return "NCASM_GET_CAPABILITY";
    case Command::NCASM_RET_CAPABILITY: return "NCASM_RET_CAPABILITY";
    case Command::NCASM_GET_STATUS: return "NCASM_GET_STATUS";
    case Command::NCASM_RET_STATUS: return "NCASM_RET_STATUS";
    case Command::NCASM_SET_STATUS: return "NCASM_SET_STATUS";
    case Command::NCASM_NTFY_STATUS: return "NCASM_NTFY_STATUS";
    case Command::NCASM_GET_PARAM: return "NCASM_GET_PARAM";
    case Command::NCASM_RET_PARAM: return "NCASM_RET_PARAM";
    case Command::NCASM_SET_PARAM: return "NCASM_SET_PARAM";
    case Command::NCASM_NTFY_PARAM: return "NCASM_NTFY_PARAM";
    case Command::SENSE_GET_CAPABILITY: return "SENSE_GET_CAPABILITY";
    case Command::SENSE_RET_CAPABILITY: return "SENSE_RET_CAPABILITY";
    case Command::SENSE_SET_STATUS: return "SENSE_SET_STATUS";
    case Command::SENSE_NTFY_STATUS: return "SENSE_NTFY_STATUS";
    case Command::SENSE_SET_PARAM: return "SENSE_SET_PARAM";
    case Command::SENSE_NTFY_PARAM: return "SENSE_NTFY_PARAM";
    case Command::SENSE_GET_EXT_INFO: return "SENSE_GET_EXT_INFO";
    case Command::SENSE_RET_EXT_INFO: return "SENSE_RET_EXT_INFO";
    case Command::OPT_GET_CAPABILITY: return "OPT_GET_CAPABILITY";
    case Command::OPT_RET_CAPABILITY: return "OPT_RET_CAPABILITY";
    case Command::OPT_GET_STATUS: return "OPT_GET_STATUS";
    case Command::OPT_RET_STATUS: return "OPT_RET_STATUS";
    case Command::OPT_SET_STATUS: return "OPT_SET_STATUS";
    case Command::OPT_NTFY_STATUS: return "OPT_NTFY_STATUS";
    case Command::OPT_GET_PARAM: return "OPT_GET_PARAM";
    case Command::OPT_RET_PARAM: return "OPT_RET_PARAM";
    case Command::OPT_SET_PARAM: return "OPT_SET_PARAM";
    case Command::OPT_NTFY_PARAM: return "OPT_NTFY_PARAM";
    case Command::ALERT_GET_CAPABILITY: return "ALERT_GET_CAPABILITY";
    case Command::ALERT_RET_CAPABILITY: return "ALERT_RET_CAPABILITY";
    case Command::ALERT_GET_STATUS: return "ALERT_GET_STATUS";
    case Command::ALERT_RET_STATUS: return "ALERT_RET_STATUS";
    case Command::ALERT_SET_STATUS: return "ALERT_SET_STATUS";
    case Command::ALERT_NTFY_STATUS: return "ALERT_NTFY_STATUS";
    case Command::ALERT_SET_PARAM: return "ALERT_SET_PARAM";
    case Command::ALERT_NTFY_PARAM: return "ALERT_NTFY_PARAM";
    case Command::PLAY_GET_CAPABILITY: return "PLAY_GET_CAPABILITY";
    case Command::PLAY_RET_CAPABILITY: return "PLAY_RET_CAPABILITY";
    case Command::PLAY_GET_STATUS: return "PLAY_GET_STATUS";
    case Command::PLAY_RET_STATUS: return "PLAY_RET_STATUS";
    case Command::PLAY_SET_STATUS: return "PLAY_SET_STATUS";
    case Command::PLAY_NTFY_STATUS: return "PLAY_NTFY_STATUS";
    case Command::PLAY_GET_PARAM: return "PLAY_GET_PARAM";
    case Command::PLAY_RET_PARAM: return "PLAY_RET_PARAM";
    case Command::PLAY_SET_PARAM: return "PLAY_SET_PARAM";
    case Command::PLAY_NTFY_PARAM: return "PLAY_NTFY_PARAM";
    case Command::SAR_AUTO_PLAY_GET_CAPABILITY: return "SAR_AUTO_PLAY_GET_CAPABILITY";
    case Command::SAR_AUTO_PLAY_RET_CAPABILITY: return "SAR_AUTO_PLAY_RET_CAPABILITY";
    case Command::SAR_AUTO_PLAY_GET_STATUS: return "SAR_AUTO_PLAY_GET_STATUS";
    case Command::SAR_AUTO_PLAY_RET_STATUS: return "SAR_AUTO_PLAY_RET_STATUS";
    case Command::SAR_AUTO_PLAY_NTFY_STATUS: return "SAR_AUTO_PLAY_NTFY_STATUS";
    case Command::SAR_AUTO_PLAY_GET_PARAM: return "SAR_AUTO_PLAY_GET_PARAM";
    case Command::SAR_AUTO_PLAY_RET_PARAM: return "SAR_AUTO_PLAY_RET_PARAM";
    case Command::SAR_AUTO_PLAY_SET_PARAM: return "SAR_AUTO_PLAY_SET_PARAM";
    case Command::SAR_AUTO_PLAY_NTFY_PARAM: return "SAR_AUTO_PLAY_NTFY_PARAM";
    case Command::LOG_SET_STATUS: return "LOG_SET_STATUS";
    case Command::LOG_NTFY_PARAM: return "LOG_NTFY_PARAM";
    case Command::GENERAL_SETTING_GET_CAPABILITY: return "GENERAL_SETTING_GET_CAPABILITY";
    case Command::GENERAL_SETTING_RET_CAPABILITY: return "GENERAL_SETTING_RET_CAPABILITY";
    case Command::GENERAL_SETTING_GET_STATUS: return "GENERAL_SETTING_GET_STATUS";
    case Command::GENERAL_SETTING_RET_STATUS: return "GENERAL_SETTING_RET_STATUS";
    case Command::GENERAL_SETTING_NTFY_STATUS: return "GENERAL_SETTING_NTFY_STATUS";
    case Command::GENERAL_SETTING_GET_PARAM: return "GENERAL_SETTING_GET_PARAM";
    case Command::GENERAL_SETTING_RET_PARAM: return "GENERAL_SETTING_RET_PARAM";
    case Command::GENERAL_SETTING_SET_PARAM: return "GENERAL_SETTING_SET_PARAM";
    case Command::GENERAL_SETTING_NTNY_PARAM: return "GENERAL_SETTING_NTNY_PARAM";
    case Command::AUDIO_GET_CAPABILITY: return "AUDIO_GET_CAPABILITY";
    case Command::AUDIO_RET_CAPABILITY: return "AUDIO_RET_CAPABILITY";
    case Command::AUDIO_GET_STATUS: return "AUDIO_GET_STATUS";
    case Command::AUDIO_RET_STATUS: return "AUDIO_RET_STATUS";
    case Command::AUDIO_NTFY_STATUS: return "AUDIO_NTFY_STATUS";
    case Command::AUDIO_GET_PARAM: return "AUDIO_GET_PARAM";
    case Command::AUDIO_RET_PARAM: return "AUDIO_RET_PARAM";
    case Command::AUDIO_SET_PARAM: return "AUDIO_SET_PARAM";
    case Command::AUDIO_NTFY_PARAM: return "AUDIO_NTFY_PARAM";
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
    case Command::SYSTEM_GET_EXT_PARAM: return "SYSTEM_GET_EXT_PARAM";
    case Command::SYSTEM_RET_EXT_PARAM: return "SYSTEM_RET_EXT_PARAM";
    case Command::SYSTEM_SET_EXT_PARAM: return "SYSTEM_SET_EXT_PARAM";
    case Command::SYSTEM_NTFY_EXT_PARAM: return "SYSTEM_NTFY_EXT_PARAM";
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

// region CONNECT_*_PROTOCOL_INFO

// region CONNECT_GET_PROTOCOL_INFO

struct ConnectGetProtocolInfo : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::CONNECT_RET_PROTOCOL_INFO;

    ConnectInquiredType inquiredType; // 0x1

    ConnectGetProtocolInfo(ConnectInquiredType inquiredType)
        : Payload(Command::CONNECT_GET_PROTOCOL_INFO)
        , inquiredType(inquiredType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(ConnectGetProtocolInfo)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::CONNECT_GET_PROTOCOL_INFO)
            && buf[offsetof(ConnectGetProtocolInfo, inquiredType)] == static_cast<uint8_t>(ConnectInquiredType::FIXED_VALUE);
    }
};

// endregion CONNECT_GET_PROTOCOL_INFO

// region CONNECT_RET_PROTOCOL_INFO

struct ConnectRetProtocolInfo : Payload
{
    ConnectInquiredType inquiredType; // 0x1
    uint32_t protocolVersion_needsByteSwap; // 0x2-0x5
    MessageMdrV2EnableDisable supportTable1Value; // 0x6
    MessageMdrV2EnableDisable supportTable2Value; // 0x7

    ConnectRetProtocolInfo(
        ConnectInquiredType inquiredType, uint32_t protocolVersion, bool supportTable1Value, bool supportTable2Value
    )
        : Payload(Command::CONNECT_RET_PROTOCOL_INFO)
        , inquiredType(inquiredType)
        , protocolVersion_needsByteSwap(byteOrderSwap(protocolVersion))
        , supportTable1Value(supportTable1Value)
        , supportTable2Value(supportTable2Value)
    {}

    uint32_t getProtocolVersion() const
    {
        return byteOrderSwap(protocolVersion_needsByteSwap);
    }

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(ConnectRetProtocolInfo)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::CONNECT_RET_PROTOCOL_INFO)
            && buf[offsetof(ConnectRetProtocolInfo, inquiredType)] == static_cast<uint8_t>(ConnectInquiredType::FIXED_VALUE)
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(ConnectRetProtocolInfo, supportTable1Value)])
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(ConnectRetProtocolInfo, supportTable2Value)]);
    }
};

// endregion CONNECT_RET_PROTOCOL_INFO

// endregion CONNECT_*_PROTOCOL_INFO

// region CONNECT_*_CAPABILITY_INFO

// region CONNECT_GET_CAPABILITY_INFO

struct ConnectGetCapabilityInfo : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::CONNECT_RET_CAPABILITY_INFO;

    ConnectInquiredType inquiredType; // 0x1

    ConnectGetCapabilityInfo(ConnectInquiredType inquiredType)
        : Payload(Command::CONNECT_GET_CAPABILITY_INFO)
        , inquiredType(inquiredType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(ConnectGetCapabilityInfo)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::CONNECT_GET_CAPABILITY_INFO)
            && buf[offsetof(ConnectGetCapabilityInfo, inquiredType)] == static_cast<uint8_t>(ConnectInquiredType::FIXED_VALUE);
    }
};

// endregion CONNECT_GET_CAPABILITY_INFO

// endregion CONNECT_*_CAPABILITY_INFO

// region CONNECT_*_DEVICE_INFO

enum class DeviceInfoType : uint8_t
{
    MODEL_NAME = 1,
    FW_VERSION = 2,
    SERIES_AND_COLOR_INFO = 3,
    INSTRUCTION_GUIDE = 4,
};

inline bool DeviceInfoType_isValidByteCode(uint8_t type)
{
    switch (static_cast<DeviceInfoType>(type))
    {
    case DeviceInfoType::MODEL_NAME:
    case DeviceInfoType::FW_VERSION:
    case DeviceInfoType::SERIES_AND_COLOR_INFO:
    case DeviceInfoType::INSTRUCTION_GUIDE:
        return true;
    }
    return false;
}

enum class ModelSeries : uint8_t
{
    NO_SERIES = 0,
    EXTRA_BASS = 0x10,
    ULT_POWER_SOUND = 0x11,
    HEAR = 0x20,
    PREMIUM = 0x30,
    SPORTS = 0x40,
    CASUAL = 0x50,
    LINK_BUDS = 0x60,
    NECKBAND = 0x70,
    LINKPOD = 0x80,
    GAMING = 0x90,
};

inline bool ModelSeries_isValidByteCode(uint8_t type)
{
    switch (static_cast<ModelSeries>(type))
    {
    case ModelSeries::NO_SERIES:
    case ModelSeries::EXTRA_BASS:
    case ModelSeries::ULT_POWER_SOUND:
    case ModelSeries::HEAR:
    case ModelSeries::PREMIUM:
    case ModelSeries::SPORTS:
    case ModelSeries::CASUAL:
    case ModelSeries::LINK_BUDS:
    case ModelSeries::NECKBAND:
    case ModelSeries::LINKPOD:
    case ModelSeries::GAMING:
        return true;
    }
    return false;
}

// region CONNECT_GET_DEVICE_INFO

struct ConnectGetDeviceInfo : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::CONNECT_RET_DEVICE_INFO;

    DeviceInfoType deviceInfoType; // 0x1

    ConnectGetDeviceInfo(DeviceInfoType infoType)
        : Payload(Command::CONNECT_GET_DEVICE_INFO)
        , deviceInfoType(infoType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(ConnectGetDeviceInfo)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::CONNECT_GET_DEVICE_INFO)
            && DeviceInfoType_isValidByteCode(buf[offsetof(ConnectGetDeviceInfo, deviceInfoType)]);
    }
};

// endregion CONNECT_GET_DEVICE_INFO

// region CONNECT_RET_DEVICE_INFO

struct ConnectRetDeviceInfo : Payload
{
    DeviceInfoType type; // 0x1

    ConnectRetDeviceInfo(DeviceInfoType type)
        : Payload(Command::CONNECT_RET_DEVICE_INFO)
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::CONNECT_RET_DEVICE_INFO)
            && buf.size() >= sizeof(ConnectRetDeviceInfo)
            && DeviceInfoType_isValidByteCode(buf[offsetof(ConnectRetDeviceInfo, type)]);
    }
};

// - MODEL_NAME

struct ConnectRetDeviceInfoModelName : ConnectRetDeviceInfo
{
    uint8_t modelNameLength; // 0x2
    char modelName[]; // 0x3-

private:
    ConnectRetDeviceInfoModelName(const std::span<const char>& modelName)
        : ConnectRetDeviceInfo(DeviceInfoType::MODEL_NAME)
        , modelNameLength(modelName.size())
    {
        memcpy(this->modelName, modelName.data(), modelNameLength);
    }

public:
    VARIABLE_SIZE_PAYLOAD_ONE_ARRAY_AT_END(127);

    std::string getModelName() const
    {
        return std::string(modelName, modelNameLength);
    }

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return ConnectRetDeviceInfo::isValid(buf)
            && buf.size() > sizeof(ConnectRetDeviceInfoModelName)
            && (uint8_t)(buf.size() - sizeof(ConnectRetDeviceInfoModelName)) == buf[offsetof(ConnectRetDeviceInfoModelName, modelNameLength)]
            && buf[offsetof(ConnectRetDeviceInfoModelName, modelNameLength)] <= ARRAY_AT_END_MAX_SIZE; // new StringData_Factory().isValid(copyStringBytes(buf))
    }
};

static_assert(sizeof(ConnectRetDeviceInfoModelName) == 3);

// - FW_VERSION

struct ConnectRetDeviceInfoFwVersion : ConnectRetDeviceInfo
{
    uint8_t fwVersionLength; // 0x2
    char fwVersion[]; // 0x3-

private:
    ConnectRetDeviceInfoFwVersion(const std::span<const char>& fwVersion)
        : ConnectRetDeviceInfo(DeviceInfoType::FW_VERSION)
        , fwVersionLength(fwVersion.size())
    {
        memcpy(this->fwVersion, fwVersion.data(), fwVersionLength);
    }

public:
    VARIABLE_SIZE_PAYLOAD_ONE_ARRAY_AT_END(127);

    std::string getFwVersion() const
    {
        return std::string(fwVersion, fwVersionLength);
    }

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return ConnectRetDeviceInfo::isValid(buf)
            && buf.size() > sizeof(ConnectRetDeviceInfoFwVersion)
            && (uint8_t)(buf.size() - sizeof(ConnectRetDeviceInfoFwVersion)) == buf[offsetof(ConnectRetDeviceInfoFwVersion, fwVersionLength)]
            && buf[offsetof(ConnectRetDeviceInfoFwVersion, fwVersionLength)] <= ARRAY_AT_END_MAX_SIZE; // new StringData_Factory().isValid(copyStringBytes(buf))
    }
};

// - SERIES_AND_COLOR_INFO

struct ConnectRetDeviceInfoSeriesAndColor : ConnectRetDeviceInfo
{
    ModelSeries modelSeries; // 0x2
    ModelColor modelColor; // 0x3

    ConnectRetDeviceInfoSeriesAndColor(DeviceInfoType type, ModelSeries series, ModelColor color)
        : ConnectRetDeviceInfo(type)
        , modelSeries(series)
        , modelColor(color)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return ConnectRetDeviceInfo::isValid(buf)
            && buf.size() == sizeof(ConnectRetDeviceInfoSeriesAndColor)
            /*&& ModelSeries_isValidByteCode(buf[offsetof(ConnectRetDeviceInfoSeriesAndColor, modelSeries)])
            && ModelColor_isValidByteCode(buf[offsetof(ConnectRetDeviceInfoSeriesAndColor, modelColor)])*/;
    }
};

// endregion CONNECT_RET_DEVICE_INFO

// endregion CONNECT_*_DEVICE_INFO

// region CONNECT_*_SUPPORT_FUNCTION

// region CONNECT_GET_SUPPORT_FUNCTION

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

// endregion CONNECT_GET_SUPPORT_FUNCTION

// region CONNECT_RET_SUPPORT_FUNCTION

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
        memcpy(this->supportFunctions, supportFunctions.data(), sizeof(MessageMdrV2SupportFunction) * numberOfFunction);
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

// endregion CONNECT_RET_SUPPORT_FUNCTION

// endregion CONNECT_*_SUPPORT_FUNCTION

// endregion CONNECT

// region POWER

// region POWER_*_STATUS

enum class PowerInquiredType : uint8_t
{
    BATTERY = 0x00,
    LEFT_RIGHT_BATTERY = 0x01,
    CRADLE_BATTERY = 0x02,
    POWER_OFF = 0x03,
    AUTO_POWER_OFF = 0x04,
    AUTO_POWER_OFF_WEARING_DETECTION = 0x05,
    POWER_SAVE_MODE = 0x06,
    LINK_CONTROL = 0x07,
    BATTERY_WITH_THRESHOLD = 0x08,
    LR_BATTERY_WITH_THRESHOLD = 0x09,
    CRADLE_BATTERY_WITH_THRESHOLD = 0xA,
    BATTERY_SAFE_MODE = 0xB,
    CARING_CHARGE = 0xC,
    BT_STANDBY = 0xD,
    STAMINA = 0xE,
    AUTOMATIC_TOUCH_PANEL_BACKLIGHT_TURN_OFF = 0xF,
};

inline bool PowerInquiredType_isValidByteCode(uint8_t type)
{
    switch (static_cast<PowerInquiredType>(type))
    {
    case PowerInquiredType::BATTERY:
    case PowerInquiredType::LEFT_RIGHT_BATTERY:
    case PowerInquiredType::CRADLE_BATTERY:
    case PowerInquiredType::POWER_OFF:
    case PowerInquiredType::AUTO_POWER_OFF:
    case PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION:
    case PowerInquiredType::POWER_SAVE_MODE:
    case PowerInquiredType::LINK_CONTROL:
    case PowerInquiredType::BATTERY_WITH_THRESHOLD:
    case PowerInquiredType::LR_BATTERY_WITH_THRESHOLD:
    case PowerInquiredType::CRADLE_BATTERY_WITH_THRESHOLD:
    case PowerInquiredType::BATTERY_SAFE_MODE:
    case PowerInquiredType::CARING_CHARGE:
    case PowerInquiredType::BT_STANDBY:
    case PowerInquiredType::STAMINA:
    case PowerInquiredType::AUTOMATIC_TOUCH_PANEL_BACKLIGHT_TURN_OFF:
        return true;
    }
    return false;
}

// region POWER_GET_STATUS

struct PowerGetStatus : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::POWER_RET_STATUS;

    PowerInquiredType type; // 0x1

    PowerGetStatus(PowerInquiredType type)
        : Payload(Command::POWER_GET_STATUS)
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(PowerGetStatus)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::POWER_GET_STATUS)
            && isValidType(buf);
    }

    static bool isValidType(const std::span<const uint8_t>& buf)
    {
        uint8_t typeB = buf[offsetof(PowerGetStatus, type)];
        if (!PowerInquiredType_isValidByteCode(typeB))
            return false;
        auto type = static_cast<PowerInquiredType>(typeB);
        return type != PowerInquiredType::POWER_OFF
            && type != PowerInquiredType::LINK_CONTROL
            && type != PowerInquiredType::BATTERY_SAFE_MODE;
    }
};

// endregion POWER_GET_STATUS

// region POWER_RET_STATUS

struct PowerRetStatus : Payload
{
    PowerInquiredType type; // 0x1

    PowerRetStatus(PowerInquiredType type)
        : Payload(Command::POWER_RET_STATUS)
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() >= sizeof(PowerRetStatus)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::POWER_RET_STATUS);
    }
};

enum class BatteryChargingStatus : uint8_t
{
    NOT_CHARGING = 0,
    CHARGING = 1,
    UNKNOWN = 2,
    CHARGED = 3,
};

inline bool BatteryChargingStatus_isValidByteCode(uint8_t status)
{
    switch (static_cast<BatteryChargingStatus>(status))
    {
    case BatteryChargingStatus::NOT_CHARGING:
    case BatteryChargingStatus::CHARGING:
    case BatteryChargingStatus::UNKNOWN:
    case BatteryChargingStatus::CHARGED:
        return true;
    }
    return false;
}

struct PowerRetStatusBatteryBase : PowerRetStatus
{
    uint8_t batteryLevel; // 0x2, 0-100
    BatteryChargingStatus chargingStatus; // 0x3

    PowerRetStatusBatteryBase(PowerInquiredType type, uint8_t batteryLevel, BatteryChargingStatus chargingStatus)
        : PowerRetStatus(type)
        , batteryLevel(batteryLevel)
        , chargingStatus(chargingStatus)
    {}

    static bool isValid(const std::span<const uint8_t>& buf) = delete;

    static bool isValid(
        const std::span<const uint8_t>& buf, size_t payloadLength, PowerInquiredType inquiredType)
    {
        return buf.size() == payloadLength
            && buf[offsetof(PowerRetStatusBatteryBase, type)] == static_cast<uint8_t>(inquiredType)
            && isValidChargingStatus(buf)
            && PowerRetStatus::isValid(buf);
    }

    static bool isValidChargingStatus(const std::span<const uint8_t>& buf)
    {
        if (!BatteryChargingStatus_isValidByteCode(buf[offsetof(PowerRetStatusBatteryBase, chargingStatus)]))
            return false;
        uint8_t batteryLevel = buf[offsetof(PowerRetStatusBatteryBase, batteryLevel)];
        return /*batteryLevel >= 0 &&*/ batteryLevel <= 100;
    }
};

struct PowerRetStatusLeftRightBatteryBase : PowerRetStatus
{
    uint8_t leftBatteryLevel; // 0x2, 0-100
    BatteryChargingStatus leftChargingStatus; // 0x3
    uint8_t rightBatteryLevel; // 0x4, 0-100
    BatteryChargingStatus rightChargingStatus; // 0x5

    PowerRetStatusLeftRightBatteryBase(
        PowerInquiredType type, uint8_t leftBatteryLevel, BatteryChargingStatus leftChargingStatus,
        uint8_t rightBatteryLevel, BatteryChargingStatus rightChargingStatus
    )
        : PowerRetStatus(type)
        , leftBatteryLevel(leftBatteryLevel)
        , leftChargingStatus(leftChargingStatus)
        , rightBatteryLevel(rightBatteryLevel)
        , rightChargingStatus(rightChargingStatus)
    {}

    static bool isValid(const std::span<const uint8_t>& buf) = delete;

    static bool isValid(
        const std::span<const uint8_t>& buf, size_t payloadLength, PowerInquiredType inquiredType)
    {
        return buf.size() == payloadLength
            && buf[offsetof(PowerRetStatusLeftRightBatteryBase, type)] == static_cast<uint8_t>(inquiredType)
            && isValidChargingStatusLR(buf)
            && PowerRetStatus::isValid(buf);
    }

    static bool isValidChargingStatusLR(const std::span<const uint8_t>& buf)
    {
        if (!BatteryChargingStatus_isValidByteCode(buf[offsetof(PowerRetStatusLeftRightBatteryBase, leftChargingStatus)]))
            return false;
        if (!BatteryChargingStatus_isValidByteCode(buf[offsetof(PowerRetStatusLeftRightBatteryBase, rightChargingStatus)]))
            return false;
        uint8_t leftBatteryLevel = buf[offsetof(PowerRetStatusLeftRightBatteryBase, leftBatteryLevel)];
        uint8_t rightBatteryLevel = buf[offsetof(PowerRetStatusLeftRightBatteryBase, rightBatteryLevel)];
        return /*leftBatteryLevel >= 0 &&*/ leftBatteryLevel <= 100
            && /*rightBatteryLevel >= 0 &&*/ rightBatteryLevel <= 100;
    }
};

struct PowerRetStatusBatteryThresholdBase : PowerRetStatusBatteryBase
{
    uint8_t batteryThreshold; // 0x4, 0-100

    PowerRetStatusBatteryThresholdBase(
        PowerInquiredType type, uint8_t batteryLevel, BatteryChargingStatus chargingStatus, uint8_t batteryThreshold
    )
        : PowerRetStatusBatteryBase(type, batteryLevel, chargingStatus)
        , batteryThreshold(batteryThreshold)
    {}

    static bool isValid(const std::span<const uint8_t>& buf) = delete;

    static bool isValid(
        const std::span<const uint8_t>& buf, size_t payloadLength, PowerInquiredType inquiredType)
    {
        if (!PowerRetStatusBatteryBase::isValid(buf, payloadLength, inquiredType))
            return false;
        uint8_t batteryThreshold = buf[offsetof(PowerRetStatusBatteryThresholdBase, batteryThreshold)];
        return /*batteryThreshold >= 0 &&*/ batteryThreshold <= 100;
    }
};

// - BATTERY

struct PowerRetStatusBattery : PowerRetStatusBatteryBase
{
    PowerRetStatusBattery(uint8_t batteryLevel, BatteryChargingStatus chargingStatus)
        : PowerRetStatusBatteryBase(PowerInquiredType::BATTERY, batteryLevel, chargingStatus)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return PowerRetStatusBatteryBase::isValid(buf, sizeof(PowerRetStatusBattery), PowerInquiredType::BATTERY);
    }
};

// - LEFT_RIGHT_BATTERY

struct PowerRetStatusLeftRightBattery : PowerRetStatusLeftRightBatteryBase
{
    PowerRetStatusLeftRightBattery(
        uint8_t leftBatteryLevel, BatteryChargingStatus leftChargingStatus,
        uint8_t rightBatteryLevel, BatteryChargingStatus rightChargingStatus
    )
        : PowerRetStatusLeftRightBatteryBase(
            PowerInquiredType::LEFT_RIGHT_BATTERY,
            leftBatteryLevel, leftChargingStatus,
            rightBatteryLevel, rightChargingStatus)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return PowerRetStatusLeftRightBatteryBase::isValid(buf, sizeof(PowerRetStatusLeftRightBattery), PowerInquiredType::LEFT_RIGHT_BATTERY);
    }
};

// - CRADLE_BATTERY

struct PowerRetStatusCradleBattery : PowerRetStatusBatteryBase
{
    PowerRetStatusCradleBattery(uint8_t batteryLevel, BatteryChargingStatus chargingStatus)
        : PowerRetStatusBatteryBase(PowerInquiredType::CRADLE_BATTERY, batteryLevel, chargingStatus)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return PowerRetStatusBatteryBase::isValid(buf, sizeof(PowerRetStatusCradleBattery), PowerInquiredType::CRADLE_BATTERY);
    }
};

// - BATTERY_WITH_THRESHOLD

struct PowerRetStatusBatteryThreshold : PowerRetStatusBatteryThresholdBase
{
    PowerRetStatusBatteryThreshold(
        uint8_t batteryLevel, BatteryChargingStatus chargingStatus, uint8_t batteryThreshold
    )
        : PowerRetStatusBatteryThresholdBase(
            PowerInquiredType::BATTERY_WITH_THRESHOLD, batteryLevel, chargingStatus, batteryThreshold)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return PowerRetStatusBatteryThresholdBase::isValid(buf, sizeof(PowerRetStatusBatteryThreshold), PowerInquiredType::BATTERY_WITH_THRESHOLD);
    }
};

// - LR_BATTERY_WITH_THRESHOLD

struct PowerRetStatusLeftRightBatteryThreshold : PowerRetStatusLeftRightBatteryBase
{
    uint8_t leftBatteryThreshold; // 0x6, 0-100
    uint8_t rightBatteryThreshold; // 0x7, 0-100

    PowerRetStatusLeftRightBatteryThreshold(
        uint8_t leftBatteryLevel, BatteryChargingStatus leftChargingStatus, uint8_t leftBatteryThreshold,
        uint8_t rightBatteryLevel, BatteryChargingStatus rightChargingStatus, uint8_t rightBatteryThreshold
    )
        : PowerRetStatusLeftRightBatteryBase(
            PowerInquiredType::LR_BATTERY_WITH_THRESHOLD,
            leftBatteryLevel, leftChargingStatus,
            rightBatteryLevel, rightChargingStatus)
        , leftBatteryThreshold(leftBatteryThreshold)
        , rightBatteryThreshold(rightBatteryThreshold)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        if (!PowerRetStatusLeftRightBatteryBase::isValid(buf, sizeof(PowerRetStatusLeftRightBatteryThreshold), PowerInquiredType::LR_BATTERY_WITH_THRESHOLD))
            return false;
        uint8_t leftBatteryThreshold = buf[offsetof(PowerRetStatusLeftRightBatteryThreshold, leftBatteryThreshold)];
        uint8_t rightBatteryThreshold = buf[offsetof(PowerRetStatusLeftRightBatteryThreshold, rightBatteryThreshold)];
        return /*leftBatteryThreshold >= 0 &&*/ leftBatteryThreshold <= 100
            && /*rightBatteryThreshold >= 0 &&*/ rightBatteryThreshold <= 100;
    }
};

// - CRADLE_BATTERY_WITH_THRESHOLD

struct PowerRetStatusCradleBatteryThreshold : PowerRetStatusBatteryThresholdBase
{
    PowerRetStatusCradleBatteryThreshold(
        uint8_t batteryLevel, BatteryChargingStatus chargingStatus, uint8_t batteryThreshold
    )
        : PowerRetStatusBatteryThresholdBase(
            PowerInquiredType::CRADLE_BATTERY_WITH_THRESHOLD, batteryLevel, chargingStatus, batteryThreshold)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return PowerRetStatusBatteryThresholdBase::isValid(buf, sizeof(PowerRetStatusCradleBatteryThreshold), PowerInquiredType::CRADLE_BATTERY_WITH_THRESHOLD);
    }
};

// endregion POWER_RET_STATUS

// region POWER_SET_STATUS

struct PowerSetStatus : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::POWER_NTFY_STATUS;

    PowerInquiredType type; // 0x1

    PowerSetStatus(PowerInquiredType type)
        : Payload(Command::POWER_SET_STATUS)
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() >= sizeof(PowerSetStatus)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::POWER_SET_STATUS);
    }
};

// - POWER_OFF

enum class PowerOffSettingValue : uint8_t
{
    USER_POWER_OFF = 0x01,
    FACTORY_POWER_OFF = 0xFF,
};

inline bool PowerOffSettingValue_isValidByteCode(uint8_t value)
{
    switch (static_cast<PowerOffSettingValue>(value))
    {
    case PowerOffSettingValue::USER_POWER_OFF:
    case PowerOffSettingValue::FACTORY_POWER_OFF:
        return true;
    }
    return false;
}

struct PowerSetStatusPowerOff : PowerSetStatus
{
    PowerOffSettingValue powerOffSettingValue; // 0x2

    PowerSetStatusPowerOff(PowerOffSettingValue powerOffSettingValue)
        : PowerSetStatus(PowerInquiredType::POWER_OFF)
        , powerOffSettingValue(powerOffSettingValue)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return PowerSetStatus::isValid(buf)
            && buf.size() == sizeof(PowerSetStatusPowerOff)
            && buf[offsetof(PowerSetStatusPowerOff, type)] == static_cast<uint8_t>(PowerInquiredType::POWER_OFF)
            && buf[offsetof(PowerSetStatusPowerOff, powerOffSettingValue)] == static_cast<uint8_t>(PowerOffSettingValue::USER_POWER_OFF);
    }
};

// endregion POWER_SET_STATUS

// endregion POWER_*_STATUS

// region POWER_*_PARAM

// region POWER_GET_PARAM

struct PowerGetParam : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::POWER_RET_PARAM;

    PowerInquiredType type; // 0x1

    PowerGetParam(PowerInquiredType type)
        : Payload(Command::POWER_GET_PARAM)
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(PowerGetParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::POWER_GET_PARAM)
            && isValidType(buf);
    }

    static bool isValidType(const std::span<const uint8_t>& buf)
    {
        uint8_t typeB = buf[offsetof(PowerGetParam, type)];
        if (!PowerInquiredType_isValidByteCode(typeB))
            return false;
        auto type = static_cast<PowerInquiredType>(typeB);
        return type == PowerInquiredType::AUTO_POWER_OFF
            || type == PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION
            || type == PowerInquiredType::POWER_SAVE_MODE
            || type == PowerInquiredType::BATTERY_SAFE_MODE
            || type == PowerInquiredType::CARING_CHARGE
            || type == PowerInquiredType::BT_STANDBY
            || type == PowerInquiredType::STAMINA
            || type == PowerInquiredType::AUTOMATIC_TOUCH_PANEL_BACKLIGHT_TURN_OFF;
    }
};

// endregion POWER_GET_PARAM

// region POWER_RET_PARAM, POWER_SET_PARAM, POWER_NTFY_PARAM

struct PowerParam : Payload
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::POWER_RET_PARAM,
        Command::POWER_SET_PARAM,
        Command::POWER_NTFY_PARAM,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::POWER_NTFY_PARAM,
        Command::UNKNOWN,
    };

    PowerInquiredType type; // 0x1

    PowerParam(CommandType ct, PowerInquiredType type)
        : Payload(COMMAND_IDS[ct])
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return Payload::isValid(buf)
            && buf.size() >= sizeof(PowerParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(COMMAND_IDS[ct]);
    }
};

// - AUTO_POWER_OFF

enum class AutoPowerOffElements : uint8_t
{
    POWER_OFF_IN_5_MIN = 0x00,
    POWER_OFF_IN_30_MIN = 0x01,
    POWER_OFF_IN_60_MIN = 0x02,
    POWER_OFF_IN_180_MIN = 0x03,
    POWER_OFF_IN_15_MIN = 0x04,
    POWER_OFF_DISABLE = 0x11,
};

inline bool AutoPowerOffElements_isValidByteCode(uint8_t value)
{
    switch (static_cast<AutoPowerOffElements>(value))
    {
    case AutoPowerOffElements::POWER_OFF_IN_5_MIN:
    case AutoPowerOffElements::POWER_OFF_IN_30_MIN:
    case AutoPowerOffElements::POWER_OFF_IN_60_MIN:
    case AutoPowerOffElements::POWER_OFF_IN_180_MIN:
    case AutoPowerOffElements::POWER_OFF_IN_15_MIN:
    case AutoPowerOffElements::POWER_OFF_DISABLE:
        return true;
    }
    return false;
}

struct PowerParamAutoPowerOff : PowerParam
{
    AutoPowerOffElements currentPowerOffElements; // 0x2
    AutoPowerOffElements lastSelectPowerOffElements; // 0x3

    PowerParamAutoPowerOff(CommandType ct, AutoPowerOffElements current, AutoPowerOffElements lastSelect)
        : PowerParam(ct, PowerInquiredType::AUTO_POWER_OFF)
        , currentPowerOffElements(current)
        , lastSelectPowerOffElements(lastSelect)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return PowerParam::isValid(buf, ct)
            && buf.size() == sizeof(PowerParamAutoPowerOff)
            && buf[offsetof(PowerParamAutoPowerOff, type)] == static_cast<uint8_t>(PowerInquiredType::AUTO_POWER_OFF)
            && AutoPowerOffElements_isValidByteCode(buf[offsetof(PowerParamAutoPowerOff, currentPowerOffElements)])
            && AutoPowerOffElements_isValidByteCode(buf[offsetof(PowerParamAutoPowerOff, lastSelectPowerOffElements)]);
    }
};

// - AUTO_POWER_OFF_WEARING_DETECTION

enum class AutoPowerOffWearingDetectionElements : uint8_t
{
    POWER_OFF_IN_5_MIN = 0x00,
    POWER_OFF_IN_30_MIN = 0x01,
    POWER_OFF_IN_60_MIN = 0x02,
    POWER_OFF_IN_180_MIN = 0x03,
    POWER_OFF_IN_15_MIN = 0x04,
    POWER_OFF_WHEN_REMOVED_FROM_EARS = 0x10,
    POWER_OFF_DISABLE = 0x11,
};

inline bool AutoPowerOffWearingDetectionElements_isValidByteCode(uint8_t value)
{
    switch (static_cast<AutoPowerOffWearingDetectionElements>(value))
    {
    case AutoPowerOffWearingDetectionElements::POWER_OFF_IN_5_MIN:
    case AutoPowerOffWearingDetectionElements::POWER_OFF_IN_30_MIN:
    case AutoPowerOffWearingDetectionElements::POWER_OFF_IN_60_MIN:
    case AutoPowerOffWearingDetectionElements::POWER_OFF_IN_180_MIN:
    case AutoPowerOffWearingDetectionElements::POWER_OFF_IN_15_MIN:
    case AutoPowerOffWearingDetectionElements::POWER_OFF_WHEN_REMOVED_FROM_EARS:
    case AutoPowerOffWearingDetectionElements::POWER_OFF_DISABLE:
        return true;
    }
    return false;
}

struct PowerParamAutoPowerOffWithWearingDetection : PowerParam
{
    AutoPowerOffWearingDetectionElements currentPowerOffElements; // 0x2
    AutoPowerOffWearingDetectionElements lastSelectPowerOffElements; // 0x3

    PowerParamAutoPowerOffWithWearingDetection(
        CommandType ct, AutoPowerOffWearingDetectionElements current, AutoPowerOffWearingDetectionElements lastSelect
    )
        : PowerParam(ct, PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION)
        , currentPowerOffElements(current)
        , lastSelectPowerOffElements(lastSelect)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return PowerParam::isValid(buf, ct)
            && buf.size() == sizeof(PowerParamAutoPowerOffWithWearingDetection)
            && buf[offsetof(PowerParamAutoPowerOffWithWearingDetection, type)] == static_cast<uint8_t>(PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION)
            && AutoPowerOffWearingDetectionElements_isValidByteCode(buf[offsetof(PowerParamAutoPowerOffWithWearingDetection, currentPowerOffElements)])
            && AutoPowerOffWearingDetectionElements_isValidByteCode(buf[offsetof(PowerParamAutoPowerOffWithWearingDetection, lastSelectPowerOffElements)]);
    }
};

// - POWER_SAVE_MODE, CARING_CHARGE, BT_STANDBY, STAMINA, AUTOMATIC_TOUCH_PANEL_BACKLIGHT_TURN_OFF

struct PowerParamSettingOnOff : PowerParam
{
    MessageMdrV2OnOffSettingValue onOffSetting; // 0x2

    PowerParamSettingOnOff(CommandType ct, PowerInquiredType type, MessageMdrV2OnOffSettingValue onOffSetting)
        : PowerParam(ct, type)
        , onOffSetting(onOffSetting)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return PowerParam::isValid(buf, ct)
            && buf.size() == sizeof(PowerParamSettingOnOff)
            && isValidInquiredType(static_cast<PowerInquiredType>(buf[offsetof(PowerParamSettingOnOff, type)]))
            && MessageMdrV2OnOffSettingValue_isValidByteCode(buf[offsetof(PowerParamSettingOnOff, onOffSetting)]);
    }

    static bool isValidInquiredType(PowerInquiredType type)
    {
        return type == PowerInquiredType::POWER_SAVE_MODE
            || type == PowerInquiredType::CARING_CHARGE
            || type == PowerInquiredType::BT_STANDBY
            || type == PowerInquiredType::STAMINA
            || type == PowerInquiredType::AUTOMATIC_TOUCH_PANEL_BACKLIGHT_TURN_OFF;
    }
};

// - BATTERY_SAFE_MODE

struct PowerParamBatterySafeMode : PowerParam
{
    MessageMdrV2OnOffSettingValue onOffSettingValue; // 0x2
    MessageMdrV2OnOffSettingValue effectStatus; // 0x3

    PowerParamBatterySafeMode(
        CommandType ct, MessageMdrV2OnOffSettingValue onOffSettingValue, MessageMdrV2OnOffSettingValue effectStatus
    )
        : PowerParam(ct, PowerInquiredType::BATTERY_SAFE_MODE)
        , onOffSettingValue(onOffSettingValue)
        , effectStatus(effectStatus)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return PowerParam::isValid(buf, ct)
            && buf.size() == sizeof(PowerParamBatterySafeMode)
            && buf[offsetof(PowerParamBatterySafeMode, type)] == static_cast<uint8_t>(PowerInquiredType::BATTERY_SAFE_MODE)
            && MessageMdrV2OnOffSettingValue_isValidByteCode(buf[offsetof(PowerParamBatterySafeMode, onOffSettingValue)])
            && MessageMdrV2OnOffSettingValue_isValidByteCode(buf[offsetof(PowerParamBatterySafeMode, effectStatus)]);
    }
};

// endregion POWER_RET_PARAM, POWER_SET_PARAM, POWER_NTFY_PARAM

// endregion POWER_*_PARAM

// endregion POWER

// region NC_ASM

enum class NcAsmInquiredType : uint8_t
{
    NC_ON_OFF = 0x1,
    NC_ON_OFF_AND_ASM_ON_OFF = 0x11,
    NC_MODE_SWITCH_AND_ASM_ON_OFF = 0x12,
    NC_ON_OFF_AND_ASM_SEAMLESS = 0x13,
    NC_MODE_SWITCH_AND_ASM_SEAMLESS = 0x14,
    MODE_NC_ASM_AUTO_NC_MODE_SWITCH_AND_ASM_SEAMLESS = 0x15,
    MODE_NC_ASM_DUAL_SINGLE_NC_MODE_SWITCH_AND_ASM_SEAMLESS = 0x16,
    MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS = 0x17,
    MODE_NC_NCSS_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS = 0x18,
    MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA = 0x19,
    ASM_ON_OFF = 0x21,
    ASM_SEAMLESS = 0x22,
    NC_AMB_TOGGLE = 0x30,
    NC_TEST_MODE = 0x40,
};

constexpr std::bitset<256> NcAsmInquiredType_values = []
{
    std::bitset<256> bs;
    bs.set(static_cast<uint8_t>(NcAsmInquiredType::NC_ON_OFF));
    bs.set(static_cast<uint8_t>(NcAsmInquiredType::NC_ON_OFF_AND_ASM_ON_OFF));
    bs.set(static_cast<uint8_t>(NcAsmInquiredType::NC_MODE_SWITCH_AND_ASM_ON_OFF));
    bs.set(static_cast<uint8_t>(NcAsmInquiredType::NC_ON_OFF_AND_ASM_SEAMLESS));
    bs.set(static_cast<uint8_t>(NcAsmInquiredType::NC_MODE_SWITCH_AND_ASM_SEAMLESS));
    bs.set(static_cast<uint8_t>(NcAsmInquiredType::MODE_NC_ASM_AUTO_NC_MODE_SWITCH_AND_ASM_SEAMLESS));
    bs.set(static_cast<uint8_t>(NcAsmInquiredType::MODE_NC_ASM_DUAL_SINGLE_NC_MODE_SWITCH_AND_ASM_SEAMLESS));
    bs.set(static_cast<uint8_t>(NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS));
    bs.set(static_cast<uint8_t>(NcAsmInquiredType::MODE_NC_NCSS_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS));
    bs.set(static_cast<uint8_t>(NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA));
    bs.set(static_cast<uint8_t>(NcAsmInquiredType::ASM_ON_OFF));
    bs.set(static_cast<uint8_t>(NcAsmInquiredType::ASM_SEAMLESS));
    bs.set(static_cast<uint8_t>(NcAsmInquiredType::NC_AMB_TOGGLE));
    bs.set(static_cast<uint8_t>(NcAsmInquiredType::NC_TEST_MODE));
    return bs;
}();

inline bool NcAsmInquiredType_isValidByteCode(uint8_t type)
{
    return NcAsmInquiredType_values[type];
}

enum class ValueChangeStatus : uint8_t
{
    UNDER_CHANGING = 0,
    CHANGED = 1,
};

inline bool ValueChangeStatus_isValidByteCode(uint8_t status)
{
    switch (static_cast<ValueChangeStatus>(status))
    {
    case ValueChangeStatus::UNDER_CHANGING:
    case ValueChangeStatus::CHANGED:
        return true;
    }
    return false;
}

enum class NcAsmOnOffValue : uint8_t
{
    OFF = 0,
    ON = 1,
};

inline bool NcAsmOnOffValue_isValidByteCode(uint8_t value)
{
    switch (static_cast<NcAsmOnOffValue>(value))
    {
    case NcAsmOnOffValue::OFF:
    case NcAsmOnOffValue::ON:
        return true;
    }
    return false;
}

enum class NcAsmMode : uint8_t
{
    NC = 0,
    ASM = 1,
};

inline bool NcAsmMode_isValidByteCode(uint8_t mode)
{
    switch (static_cast<NcAsmMode>(mode))
    {
    case NcAsmMode::NC:
    case NcAsmMode::ASM:
        return true;
    }
    return false;
}

enum class AmbientSoundMode : uint8_t
{
    NORMAL = 0,
    VOICE = 1,
};

inline bool AmbientSoundMode_isValidByteCode(uint8_t mode)
{
    switch (static_cast<AmbientSoundMode>(mode))
    {
    case AmbientSoundMode::NORMAL:
    case AmbientSoundMode::VOICE:
        return true;
    }
    return false;
}

enum class NoiseAdaptiveSensitivity : uint8_t
{
    STANDARD = 0,
    HIGH = 1,
    LOW = 2,
};

inline bool NoiseAdaptiveSensitivity_isValidByteCode(uint8_t sensitivity)
{
    switch (static_cast<NoiseAdaptiveSensitivity>(sensitivity))
    {
    case NoiseAdaptiveSensitivity::STANDARD:
    case NoiseAdaptiveSensitivity::HIGH:
    case NoiseAdaptiveSensitivity::LOW:
        return true;
    }
    return false;
}

enum class Function : uint8_t
{
    NO_FUNCTION = 0x00,
    NC_ASM_OFF = 0x01,
    NC_ASM = 0x02,
    NC_OFF = 0x03,
    ASM_OFF = 0x04,
    QUICK_ATTENTION = 0x10,
    NC_OPTIMIZER = 0x11,
    PLAY_PAUSE = 0x20,
    NEXT_TRACK = 0x21,
    PREV_TRACK = 0x22,
    VOLUME_UP = 0x23,
    VOLUME_DOWN = 0x24,
    VOICE_RECOGNITION = 0x30,
    GET_YOUR_NOTIFICATION = 0x31,
    TALK_TO_GOOGLE_ASSISTANT = 0x32,
    STOP_GOOGLE_ASSISTANT = 0x33,
    VOICE_INPUT_CANCEL = 0x34,
    TALK_TO_TENCENT_XIAOWEI = 0x35,
    CANCEL_VOICE_RECOGNITION = 0x36,
    VOICE_INPUT_AMAZON_ALEXA = 0x37,
    CANCEL_AMAZON_ALEXA = 0x38,
    CANCEL_TENCENT_XIAOWEI = 0x39,
    LAUNCH_MLP = 0x40,
    TALK_TO_YOUR_MLP = 0x41,
    SPTF_ONE_TOUCH = 0x42,
    QUICK_ACCESS1 = 0x43,
    QUICK_ACCESS2 = 0x44,
    TALK_TO_TENCENT_XIAOWEI_CANCEL = 0x45,
    Q_MSC_ONE_TOUCH = 0x46,
    TEAMS = 0x47,
    TEAMS_VOICE_SKILLS = 0x48,
    NC_NCSS_ASM_OFF = 0x50,
    NC_NCSS_ASM = 0x51,
    NC_NCSS_OFF = 0x52,
    NCSS_ASM_OFF = 0x53,
    NC_NCSS = 0x54,
    NCSS_ASM = 0x55,
    NCSS_OFF = 0x56,
    AMB_SETTING = 0x57,
    STANDARD_VOICE_SOUND = 0x58,
    MIC_MUTE = 0x70,
    GAME_UP = 0x71,
    CHAT_UP = 0x72,
};

inline bool Function_isValidByteCode(uint8_t func)
{
    switch (static_cast<Function>(func))
    {
    case Function::NO_FUNCTION:
    case Function::NC_ASM_OFF:
    case Function::NC_ASM:
    case Function::NC_OFF:
    case Function::ASM_OFF:
    case Function::QUICK_ATTENTION:
    case Function::NC_OPTIMIZER:
    case Function::PLAY_PAUSE:
    case Function::NEXT_TRACK:
    case Function::PREV_TRACK:
    case Function::VOLUME_UP:
    case Function::VOLUME_DOWN:
    case Function::VOICE_RECOGNITION:
    case Function::GET_YOUR_NOTIFICATION:
    case Function::TALK_TO_GOOGLE_ASSISTANT:
    case Function::STOP_GOOGLE_ASSISTANT:
    case Function::VOICE_INPUT_CANCEL:
    case Function::TALK_TO_TENCENT_XIAOWEI:
    case Function::CANCEL_VOICE_RECOGNITION:
    case Function::VOICE_INPUT_AMAZON_ALEXA:
    case Function::CANCEL_AMAZON_ALEXA:
    case Function::CANCEL_TENCENT_XIAOWEI:
    case Function::LAUNCH_MLP:
    case Function::TALK_TO_YOUR_MLP:
    case Function::SPTF_ONE_TOUCH:
    case Function::QUICK_ACCESS1:
    case Function::QUICK_ACCESS2:
    case Function::TALK_TO_TENCENT_XIAOWEI_CANCEL:
    case Function::Q_MSC_ONE_TOUCH:
    case Function::TEAMS:
    case Function::TEAMS_VOICE_SKILLS:
    case Function::NC_NCSS_ASM_OFF:
    case Function::NC_NCSS_ASM:
    case Function::NC_NCSS_OFF:
    case Function::NCSS_ASM_OFF:
    case Function::NC_NCSS:
    case Function::NCSS_ASM:
    case Function::NCSS_OFF:
    case Function::AMB_SETTING:
    case Function::STANDARD_VOICE_SOUND:
    case Function::MIC_MUTE:
    case Function::GAME_UP:
    case Function::CHAT_UP:
        return true;
    }
    return false;
}

inline bool NcAmbButtonMode_FromStates(bool nc, bool amb, bool off, Function* ret)
{
    if (nc && amb && off) *ret = Function::NC_ASM_OFF; // all enabled -> cycle through all 3 modes
    else if (nc && amb && !off) *ret = Function::NC_ASM; // cycle between NC and AMB
    else if (nc && !amb && off) *ret = Function::NC_OFF; // cycle between NC and Off
    else if (!nc && amb && off) *ret = Function::ASM_OFF; // cycle between AMB and Off
    else return false;
    return true;
}

inline void NcAmbButtonMode_ToStates(Function mode, bool* nc, bool* amb, bool* off)
{
    switch (mode)
    {
        case Function::NC_ASM_OFF:
            *nc = true;
            *amb = true;
            *off = true;
            break;
        case Function::NC_ASM:
            *nc = true;
            *amb = true;
            *off = false;
            break;
        case Function::NC_OFF:
            *nc = true;
            *amb = false;
            *off = true;
            break;
        case Function::ASM_OFF:
            *nc = false;
            *amb = true;
            *off = true;
            break;
        default:
            *nc = true;
            *amb = true;
            *off = false;
            break;
    }
}

// region NCASM_*_PARAM

// region NCASM_GET_PARAM

struct NcAsmGetParam : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::NCASM_RET_PARAM;

    NcAsmInquiredType type; // 0x1

    NcAsmGetParam(NcAsmInquiredType type)
        : Payload(Command::NCASM_GET_PARAM)
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(NcAsmGetParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::NCASM_GET_PARAM)
            && NcAsmInquiredType_isValidByteCode(buf[offsetof(NcAsmGetParam, type)]);
    }
};

// endregion NCASM_GET_PARAM

// region NCASM_RET_PARAM, NCASM_SET_PARAM, NCASM_NOTIFY_PARAM

struct NcAsmParam : Payload
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::NCASM_RET_PARAM,
        Command::NCASM_SET_PARAM,
        Command::NCASM_NTFY_PARAM,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::NCASM_NTFY_PARAM,
        Command::UNKNOWN,
    };

    NcAsmInquiredType type; // 0x1

    NcAsmParam(CommandType ct, NcAsmInquiredType type)
        : Payload(COMMAND_IDS[ct])
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return Payload::isValid(buf)
            && buf.size() >= sizeof(NcAsmParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(COMMAND_IDS[ct])
            && NcAsmInquiredType_isValidByteCode(buf[offsetof(NcAsmParam, type)]);
    }
};

struct NcAsmParamBase : NcAsmParam
{
    ValueChangeStatus valueChangeStatus; // 0x2
    NcAsmOnOffValue ncAsmTotalEffect; // 0x3

    NcAsmParamBase(
        CommandType ct, NcAsmInquiredType type, ValueChangeStatus valueChangeStatus, NcAsmOnOffValue ncAsmTotalEffect
    )
        : NcAsmParam(ct, type)
        , valueChangeStatus(valueChangeStatus)
        , ncAsmTotalEffect(ncAsmTotalEffect)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return NcAsmParam::isValid(buf, ct)
            && buf.size() >= sizeof(NcAsmParamBase)
            && ValueChangeStatus_isValidByteCode(buf[offsetof(NcAsmParamBase, valueChangeStatus)])
            && NcAsmOnOffValue_isValidByteCode(buf[offsetof(NcAsmParamBase, ncAsmTotalEffect)]);
    }
};

// - MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS

struct NcAsmParamModeNcDualModeSwitchAsmSeamless : NcAsmParamBase
{
    NcAsmMode ncAsmMode; // 0x4
    AmbientSoundMode ambientSoundMode; // 0x5
    uint8_t ambientSoundLevelValue; // 0x6

    NcAsmParamModeNcDualModeSwitchAsmSeamless(
        CommandType ct, ValueChangeStatus valueChangeStatus, NcAsmOnOffValue ncAsmTotalEffect, NcAsmMode ncAsmMode,
        AmbientSoundMode ambientSoundMode, uint8_t ambientSoundLevelValue
    )
        : NcAsmParamBase(
            ct, NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS, valueChangeStatus,
            ncAsmTotalEffect)
        , ncAsmMode(ncAsmMode)
        , ambientSoundMode(ambientSoundMode)
        , ambientSoundLevelValue(ambientSoundLevelValue)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return NcAsmParamBase::isValid(buf, ct)
            && buf.size() == sizeof(NcAsmParamModeNcDualModeSwitchAsmSeamless)
            && buf[offsetof(NcAsmParamModeNcDualModeSwitchAsmSeamless, type)] == static_cast<uint8_t>(NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS)
            && NcAsmMode_isValidByteCode(buf[offsetof(NcAsmParamModeNcDualModeSwitchAsmSeamless, ncAsmMode)])
            && AmbientSoundMode_isValidByteCode(buf[offsetof(NcAsmParamModeNcDualModeSwitchAsmSeamless, ambientSoundMode)]);
    }
};

// - MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA

struct NcAsmParamModeNcDualModeSwitchAsmSeamlessNa : NcAsmParamBase
{
    NcAsmMode ncAsmMode; // 0x4
    AmbientSoundMode ambientSoundMode; // 0x5
    uint8_t ambientSoundLevelValue; // 0x6
    NcAsmOnOffValue noiseAdaptiveOnOffValue; // 0x7
    NoiseAdaptiveSensitivity noiseAdaptiveSensitivitySettings; // 0x8

    NcAsmParamModeNcDualModeSwitchAsmSeamlessNa(
        CommandType ct, ValueChangeStatus valueChangeStatus, NcAsmOnOffValue ncAsmTotalEffect, NcAsmMode ncAsmMode,
        AmbientSoundMode ambientSoundMode, uint8_t ambientSoundLevelValue, NcAsmOnOffValue noiseAdaptiveOnOffValue,
        NoiseAdaptiveSensitivity noiseAdaptiveSensitivitySettings
    )
        : NcAsmParamBase(
            ct, NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA, valueChangeStatus,
            ncAsmTotalEffect)
        , ncAsmMode(ncAsmMode)
        , ambientSoundMode(ambientSoundMode)
        , ambientSoundLevelValue(ambientSoundLevelValue)
        , noiseAdaptiveOnOffValue(noiseAdaptiveOnOffValue)
        , noiseAdaptiveSensitivitySettings(noiseAdaptiveSensitivitySettings)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return NcAsmParamBase::isValid(buf, ct)
            && buf.size() == sizeof(NcAsmParamModeNcDualModeSwitchAsmSeamlessNa)
            && buf[offsetof(NcAsmParamModeNcDualModeSwitchAsmSeamlessNa, type)] == static_cast<uint8_t>(NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA)
            && NcAsmMode_isValidByteCode(buf[offsetof(NcAsmParamModeNcDualModeSwitchAsmSeamlessNa, ncAsmMode)])
            && AmbientSoundMode_isValidByteCode(buf[offsetof(NcAsmParamModeNcDualModeSwitchAsmSeamlessNa, ambientSoundMode)])
            && NcAsmOnOffValue_isValidByteCode(buf[offsetof(NcAsmParamModeNcDualModeSwitchAsmSeamlessNa, noiseAdaptiveOnOffValue)])
            && NoiseAdaptiveSensitivity_isValidByteCode(buf[offsetof(NcAsmParamModeNcDualModeSwitchAsmSeamlessNa, noiseAdaptiveSensitivitySettings)]);
    }
};

// - ASM_ON_OFF

struct NcAsmParamAsmOnOff : NcAsmParamBase
{
    AmbientSoundMode ambientSoundMode; // 0x4
    NcAsmOnOffValue ambientSoundValue; // 0x5

    NcAsmParamAsmOnOff(
        CommandType ct, ValueChangeStatus valueChangeStatus, NcAsmOnOffValue ncAsmTotalEffect,
        AmbientSoundMode ambientSoundMode, NcAsmOnOffValue ambientSoundValue
    )
        : NcAsmParamBase(ct, NcAsmInquiredType::ASM_ON_OFF, valueChangeStatus, ncAsmTotalEffect)
        , ambientSoundMode(ambientSoundMode)
        , ambientSoundValue(ambientSoundValue)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return NcAsmParamBase::isValid(buf, ct)
            && buf.size() == sizeof(NcAsmParamAsmOnOff)
            && buf[offsetof(NcAsmParamAsmOnOff, type)] == static_cast<uint8_t>(NcAsmInquiredType::ASM_ON_OFF)
            && AmbientSoundMode_isValidByteCode(buf[offsetof(NcAsmParamAsmOnOff, ambientSoundMode)])
            && NcAsmOnOffValue_isValidByteCode(buf[offsetof(NcAsmParamAsmOnOff, ambientSoundValue)]);
    }
};

// - ASM_SEAMLESS

struct NcAsmParamAsmSeamless : NcAsmParamBase
{
    AmbientSoundMode ambientSoundMode; // 0x4
    uint8_t ambientSoundLevelValue; // 0x5

    NcAsmParamAsmSeamless(
        CommandType ct, ValueChangeStatus valueChangeStatus, NcAsmOnOffValue ncAsmTotalEffect,
        AmbientSoundMode ambientSoundMode, uint8_t ambientSoundLevelValue
    )
        : NcAsmParamBase(ct, NcAsmInquiredType::ASM_SEAMLESS, valueChangeStatus, ncAsmTotalEffect)
        , ambientSoundMode(ambientSoundMode)
        , ambientSoundLevelValue(ambientSoundLevelValue)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return NcAsmParamBase::isValid(buf, ct)
            && buf.size() == sizeof(NcAsmParamAsmSeamless)
            && buf[offsetof(NcAsmParamAsmSeamless, type)] == static_cast<uint8_t>(NcAsmInquiredType::ASM_SEAMLESS)
            && AmbientSoundMode_isValidByteCode(buf[offsetof(NcAsmParamAsmSeamless, ambientSoundMode)]);
    }
};

// - NC_AMB_TOGGLE

struct NcAsmParamNcAmbToggle : NcAsmParam
{
    Function function; // 0x2

    NcAsmParamNcAmbToggle(CommandType ct, Function function)
        : NcAsmParam(ct, NcAsmInquiredType::NC_AMB_TOGGLE)
        , function(function)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return NcAsmParam::isValid(buf, ct)
            && buf.size() == sizeof(NcAsmParamNcAmbToggle)
            && buf[offsetof(NcAsmParamNcAmbToggle, type)] == static_cast<uint8_t>(NcAsmInquiredType::NC_AMB_TOGGLE)
            && Function_isValidByteCode(buf[offsetof(NcAsmParamNcAmbToggle, function)])
            && isValidFunction(static_cast<Function>(buf[offsetof(NcAsmParamNcAmbToggle, function)]));
    }

    static bool isValidFunction(Function func)
    {
        return func == Function::NC_ASM_OFF
            || func == Function::NC_ASM
            || func == Function::NC_OFF
            || func == Function::ASM_OFF;
    }
};

// endregion NCASM_RET_PARAM, NCASM_SET_PARAM, NCASM_NOTIFY_PARAM

// endregion NCASM_*_PARAM

// endregion NC_ASM

// region GENERAL_SETTING

enum class GsInquiredType : uint8_t
{
    GENERAL_SETTING1 = 0xD1, ///< Capture Voice During a Phone Call
    GENERAL_SETTING2 = 0xD2, ///< Multipoint
    GENERAL_SETTING3 = 0xD3, ///< Touch sensor control panel
    GENERAL_SETTING4 = 0xD4,
};

inline bool GsInquiredType_isValidByteCode(uint8_t type)
{
    switch (static_cast<GsInquiredType>(type))
    {
    case GsInquiredType::GENERAL_SETTING1:
    case GsInquiredType::GENERAL_SETTING2:
    case GsInquiredType::GENERAL_SETTING3:
    case GsInquiredType::GENERAL_SETTING4:
        return true;
    }
    return false;
}

enum class GsSettingType : uint8_t
{
    BOOLEAN_TYPE = 0x00,
    LIST_TYPE = 0x01,
};

inline bool GsSettingType_isValidByteCode(uint8_t type)
{
    switch (static_cast<GsSettingType>(type))
    {
    case GsSettingType::BOOLEAN_TYPE:
    case GsSettingType::LIST_TYPE:
        return true;
    }
    return false;
}

enum class GsSettingValue : uint8_t
{
    ON = 0x00,
    OFF = 0x01,
};

inline bool GsSettingValue_isValidByteCode(uint8_t value)
{
    switch (static_cast<GsSettingValue>(value))
    {
    case GsSettingValue::ON:
    case GsSettingValue::OFF:
        return true;
    }
    return false;
}

// region GENERAL_SETTING_*_CAPABILITY

enum class DisplayLanguage : uint8_t
{
    UNDEFINED_LANGUAGE = 0x00,
    ENGLISH = 0x01,
    FRENCH = 0x02,
    GERMAN = 0x03,
    SPANISH = 0x04,
    ITALIAN = 0x05,
    PORTUGUESE = 0x06,
    DUTCH = 0x07,
    SWEDISH = 0x08,
    FINNISH = 0x09,
    RUSSIAN = 0x0A,
    JAPANESE = 0x0B,
    SIMPLIFIED_CHINESE = 0x0C,
    BRAZILIAN_PORTUGUESE = 0x0D,
    TRADITIONAL_CHINESE = 0x0E,
    KOREAN = 0x0F,
    TURKISH = 0x10,
};

inline bool DisplayLanguage_isValidByteCode(uint8_t lang)
{
    switch (static_cast<DisplayLanguage>(lang))
    {
    case DisplayLanguage::UNDEFINED_LANGUAGE:
    case DisplayLanguage::ENGLISH:
    case DisplayLanguage::FRENCH:
    case DisplayLanguage::GERMAN:
    case DisplayLanguage::SPANISH:
    case DisplayLanguage::ITALIAN:
    case DisplayLanguage::PORTUGUESE:
    case DisplayLanguage::DUTCH:
    case DisplayLanguage::SWEDISH:
    case DisplayLanguage::FINNISH:
    case DisplayLanguage::RUSSIAN:
    case DisplayLanguage::JAPANESE:
    case DisplayLanguage::SIMPLIFIED_CHINESE:
    case DisplayLanguage::BRAZILIAN_PORTUGUESE:
    case DisplayLanguage::TRADITIONAL_CHINESE:
    case DisplayLanguage::KOREAN:
    case DisplayLanguage::TURKISH:
        return true;
    }
    return false;
}

// region GENERAL_SETTING_GET_CAPABILITY

struct GsGetCapability : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::GENERAL_SETTING_RET_CAPABILITY;

    GsInquiredType type; // 0x1
    DisplayLanguage displayLanguage; // 0x2

    GsGetCapability(GsInquiredType type, DisplayLanguage displayLanguage)
        : Payload(Command::GENERAL_SETTING_GET_CAPABILITY)
        , type(type)
        , displayLanguage(displayLanguage)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(GsGetCapability)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::GENERAL_SETTING_GET_CAPABILITY)
            && GsInquiredType_isValidByteCode(buf[offsetof(GsGetCapability, type)])
            && DisplayLanguage_isValidByteCode(buf[offsetof(GsGetCapability, displayLanguage)]);
    }
};

// endregion GENERAL_SETTING_GET_CAPABILITY

// region GENERAL_SETTING_RET_CAPABILITY

#pragma pack(pop)

enum class GsStringFormat
{
    RAW_NAME = 0x00,
    ENUM_NAME = 0x01,
};

struct GsSettingInfo
{
    GsStringFormat stringFormat;
    std::string subject;
    std::string summary;

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        size_t subjectLenIndex = 1 /*stringFormat*/;
        if (buf.size() <= subjectLenIndex)
            return false;
        size_t subjectLen = buf[subjectLenIndex];
        size_t summaryLenIndex = subjectLenIndex + 1 /*subject.len*/ + subjectLen;
        if (buf.size() <= summaryLenIndex)
            return false;
        size_t expectedTotalSize = summaryLenIndex + 1 /*summary.len*/ + static_cast<size_t>(buf[summaryLenIndex]);
        return buf.size() == expectedTotalSize;
    }

    static GsSettingInfo deserialize(std::span<const uint8_t> buf)
    {
        if (buf.empty())
            throw std::runtime_error("Buffer too small for GsSettingInfo");

        GsSettingInfo info;
        info.stringFormat = static_cast<GsStringFormat>(buf[0]); buf = buf.subspan(1);
        info.subject = readPrefixedString(buf);
        info.summary = readPrefixedString(buf);
        return info;
    }

    void serialize(std::vector<uint8_t>& buf)
    {
        buf.push_back(static_cast<uint8_t>(stringFormat));
        writePrefixedString(buf, subject);
        writePrefixedString(buf, summary);
    }
};

struct GsRetCapability : Payload
{
    GsInquiredType type; // 0x1
    GsSettingType settingType; // 0x2
    GsSettingInfo settingInfo; // 0x3-...

    GsRetCapability(GsInquiredType type, GsSettingType settingType, GsSettingInfo settingInfo)
        : Payload(Command::GENERAL_SETTING_RET_CAPABILITY)
        , type(type)
        , settingType(settingType)
        , settingInfo(std::move(settingInfo))
    {}

private:
    // ReSharper disable once CppPossiblyUninitializedMember
    GsRetCapability() // Don't initialize members
        : Payload(Command::GENERAL_SETTING_RET_CAPABILITY)
    {
    }

public:
    static bool isValid(const std::span<const uint8_t>& buf)
    {
        if (!Payload::isValid(buf))
            return false;
        if (buf[offsetof(Payload, command)] != static_cast<uint8_t>(Command::GENERAL_SETTING_RET_CAPABILITY))
            return false;
        size_t settingInfoSubjectLenIndex =
            1 /*command*/ + 1 /*type*/ + 1 /*settingType*/ + 1 /*stringFormat*/;
        if (buf.size() <= settingInfoSubjectLenIndex)
            return false;
        size_t settingInfoSubjectLen = buf[settingInfoSubjectLenIndex];
        size_t settingInfoSummaryLenIndex =
            settingInfoSubjectLenIndex + 1 /*subject.len*/ + settingInfoSubjectLen;
        if (buf.size() <= settingInfoSummaryLenIndex)
            return false;
        size_t expectedTotalSize
            = settingInfoSummaryLenIndex + 1 /*summary.len*/ + static_cast<size_t>(buf[settingInfoSummaryLenIndex]);
        if (buf.size() < expectedTotalSize)
            return false;
        return GsSettingInfo::isValid(buf.subspan(3, expectedTotalSize - 3));
    }

    static GsRetCapability deserialize(const std::span<const uint8_t>& buf)
    {
        if (!isValid(buf))
            throw std::runtime_error("Buffer invalid for GsRetCapability");

        GsRetCapability cap;
        cap.type = static_cast<GsInquiredType>(buf[1]);
        cap.settingType = static_cast<GsSettingType>(buf[2]);
        cap.settingInfo = GsSettingInfo::deserialize(buf.subspan(3));
        return cap;
    }

    void serialize(std::vector<uint8_t>& buf)
    {
        buf.push_back(static_cast<uint8_t>(Command::GENERAL_SETTING_RET_CAPABILITY));
        buf.push_back(static_cast<uint8_t>(type));
        buf.push_back(static_cast<uint8_t>(settingType));
        settingInfo.serialize(buf);
    }

    static constexpr bool VARIABLE_SIZE_NEEDS_SERIALIZATION = true;
};

#pragma pack(push, 1)

// endregion GENERAL_SETTING_RET_CAPABILITY

// endregion GENERAL_SETTING_*_CAPABILITY

// region GENERAL_SETTING_*_PARAM

// region GENERAL_SETTING_GET_PARAM

struct GsGetParam : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::GENERAL_SETTING_RET_PARAM;

    GsInquiredType type; // 0x1

    GsGetParam(GsInquiredType type)
        : Payload(Command::GENERAL_SETTING_GET_PARAM)
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(GsGetParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::GENERAL_SETTING_GET_PARAM)
            && GsInquiredType_isValidByteCode(buf[offsetof(GsGetParam, type)]);
    }
};

// endregion GENERAL_SETTING_GET_PARAM

// region GENERAL_SETTING_RET_PARAM, GENERAL_SETTING_SET_PARAM, GENERAL_SETTING_NTNY_PARAM

struct GsParam : Payload
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::GENERAL_SETTING_RET_PARAM,
        Command::GENERAL_SETTING_SET_PARAM,
        Command::GENERAL_SETTING_NTNY_PARAM,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::GENERAL_SETTING_NTNY_PARAM,
        Command::UNKNOWN,
    };

    GsInquiredType type; // 0x1
    GsSettingType settingType; // 0x2

    GsParam(CommandType ct, GsInquiredType type, GsSettingType settingType)
        : Payload(COMMAND_IDS[ct])
        , type(type)
        , settingType(settingType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return Payload::isValid(buf)
            && buf.size() >= sizeof(GsParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(COMMAND_IDS[ct])
            && GsInquiredType_isValidByteCode(buf[offsetof(GsParam, type)]);
    }
};

// - BOOLEAN_TYPE

struct GsParamBoolean : GsParam
{
    GsSettingValue settingValue; // 0x3

    GsParamBoolean(CommandType ct, GsInquiredType type, GsSettingValue settingValue)
        : GsParam(ct, type, GsSettingType::BOOLEAN_TYPE)
        , settingValue(settingValue)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return GsParam::isValid(buf, ct)
            && buf.size() == sizeof(GsParamBoolean)
            && buf[offsetof(GsParamBoolean, settingType)] == static_cast<uint8_t>(GsSettingType::BOOLEAN_TYPE)
            && GsSettingValue_isValidByteCode(buf[offsetof(GsParamBoolean, settingValue)]);
    }
};

// - LIST_TYPE

struct GsParamList : GsParam
{
    uint8_t currentElementIndex; // 0x3, 0-63

    GsParamList(CommandType ct, GsInquiredType type, uint8_t currentElementIndex)
        : GsParam(ct, type, GsSettingType::LIST_TYPE)
        , currentElementIndex(currentElementIndex)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return GsParam::isValid(buf, ct)
            && buf.size() == sizeof(GsParamList)
            && buf[offsetof(GsParamList, settingType)] == static_cast<uint8_t>(GsSettingType::LIST_TYPE)
            && buf[offsetof(GsParamList, currentElementIndex)] <= 63;
    }
};

// endregion GENERAL_SETTING_RET_PARAM, GENERAL_SETTING_SET_PARAM, GENERAL_SETTING_NTNY_PARAM

// endregion GENERAL_SETTING_*_PARAM

// endregion GENERAL_SETTING

} // namespace THMSGV2T1

#pragma pack(pop)
