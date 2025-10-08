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
        std::memcpy(this->modelName, modelName.data(), modelNameLength);
    }

public:
    VARIABLE_SIZE_PAYLOAD_ONE_ARRAY_AT_END(char, 127);

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
        std::memcpy(this->fwVersion, fwVersion.data(), fwVersionLength);
    }

public:
    VARIABLE_SIZE_PAYLOAD_ONE_ARRAY_AT_END(char, 127);

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
        std::memcpy(this->supportFunctions, supportFunctions.data(), sizeof(MessageMdrV2SupportFunction) * numberOfFunction);
    }

public:
    VARIABLE_SIZE_PAYLOAD_ONE_ARRAY_AT_END(MessageMdrV2SupportFunction, 255);

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
    CRADLE_BATTERY_WITH_THRESHOLD = 0x0A,
    BATTERY_SAFE_MODE = 0x0B,
    CARING_CHARGE = 0x0C,
    BT_STANDBY = 0x0D,
    STAMINA = 0x0E,
    AUTOMATIC_TOUCH_PANEL_BACKLIGHT_TURN_OFF = 0x0F,
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

// region PLAY

enum class PlayInquiredType : uint8_t
{
    // STATUS: [R N] PlayStatusPlaybackController
    //         [ S ] SetPlayStatusPlaybackController
    // PARAM:  [R N] PlayParamPlaybackControllerName
    PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT = 0x1,
    // STATUS: [R N] PlayStatusPlaybackControlWithCallVolumeAdjustmentAndFunctionChange
    //         [ S ] SetPlayStatusPlaybackController
    // PARAM:  [R N] PlayParamPlaybackControllerName
    PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE = 0x2,
    // STATUS: [R N] PlayStatusPlaybackControlWithFunctionChange
    //         [ S ] SetPlayStatusPlaybackController
    // PARAM:  [R N] PlayParamPlaybackControllerName
    PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE = 0x3,

    // STATUS: [   ] None
    // PARAM:  [RSN] PlayParamPlaybackControllerVolume
    MUSIC_VOLUME = 0x20,
    // STATUS: [   ] None
    // PARAM:  [RSN] PlayParamPlaybackControllerVolume
    CALL_VOLUME = 0x21,

    // STATUS: [   ] None
    // PARAM:  [R N] PlayParamPlaybackControllerVolumeWithMute
    //         [ S ] PlayParamPlaybackControllerVolume
    MUSIC_VOLUME_WITH_MUTE = 0x30,
    // STATUS: [   ] None
    // PARAM:  [R N] PlayParamPlaybackControllerVolumeWithMute
    //         [ S ] PlayParamPlaybackControllerVolume
    CALL_VOLUME_WITH_MUTE = 0x31,

    // STATUS: [R N] PlayStatusCommon
    // PARAM:  [RSN] PlayParamPlayMode
    PLAY_MODE = 0x40,
};

inline bool PlayInquiredType_isValidByteCode(uint8_t type)
{
    switch (static_cast<PlayInquiredType>(type))
    {
    case PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT:
    case PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE:
    case PlayInquiredType::PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE:
    case PlayInquiredType::MUSIC_VOLUME:
    case PlayInquiredType::CALL_VOLUME:
    case PlayInquiredType::MUSIC_VOLUME_WITH_MUTE:
    case PlayInquiredType::CALL_VOLUME_WITH_MUTE:
    case PlayInquiredType::PLAY_MODE:
        return true;
    }
    return false;
}

enum class PlaybackStatus : uint8_t
{
    UNSETTLED = 0x00,
    PLAY = 0x01,
    PAUSE = 0x02,
    STOP = 0x03,
};

inline bool PlaybackStatus_isValidByteCode(uint8_t status)
{
    switch (static_cast<PlaybackStatus>(status))
    {
    case PlaybackStatus::UNSETTLED:
    case PlaybackStatus::PLAY:
    case PlaybackStatus::PAUSE:
    case PlaybackStatus::STOP:
        return true;
    }
    return false;
}

enum class MusicCallStatus : uint8_t
{
    MUSIC = 0x0,
    CALL = 0x1,
};

inline bool MusicCallStatus_isValidByteCode(uint8_t status)
{
    switch (static_cast<MusicCallStatus>(status))
    {
    case MusicCallStatus::MUSIC:
    case MusicCallStatus::CALL:
        return true;
    }
    return false;
}

enum class PlaybackControl : uint8_t
{
    KEY_OFF = 0x00,
    PAUSE = 0x01,
    TRACK_UP = 0x02,
    TRACK_DOWN = 0x03,
    GROUP_UP = 0x04,
    GROUP_DOWN = 0x05,
    STOP = 0x06,
    PLAY = 0x07,
    FAST_FORWARD = 0x08,
    FAST_REWIND = 0x09,
};

inline bool PlaybackControl_isValidByteCode(uint8_t control)
{
    switch (static_cast<PlaybackControl>(control))
    {
    case PlaybackControl::KEY_OFF:
    case PlaybackControl::PAUSE:
    case PlaybackControl::TRACK_UP:
    case PlaybackControl::TRACK_DOWN:
    case PlaybackControl::GROUP_UP:
    case PlaybackControl::GROUP_DOWN:
    case PlaybackControl::STOP:
    case PlaybackControl::PLAY:
    case PlaybackControl::FAST_FORWARD:
    case PlaybackControl::FAST_REWIND:
        return true;
    }
    return false;
}

// region PLAY_*_STATUS

// region PLAY_GET_STATUS

struct GetPlayStatus : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::PLAY_RET_STATUS;

    PlayInquiredType type; // 0x1

    GetPlayStatus(PlayInquiredType type)
        : Payload(Command::PLAY_GET_STATUS)
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(GetPlayStatus)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::PLAY_GET_STATUS)
            && PlayInquiredType_isValidByteCode(buf[offsetof(GetPlayStatus, type)]);
    }
};

// endregion PLAY_GET_STATUS

// region PLAY_RET_STATUS, PLAY_NTFY_STATUS

struct PlayStatus : Payload
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::PLAY_RET_STATUS,
        Command::UNKNOWN,
        Command::PLAY_NTFY_STATUS,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
    };

    PlayInquiredType playInquiredType; // 0x1

    PlayStatus(CommandType ct, PlayInquiredType type)
        : Payload(COMMAND_IDS[ct])
        , playInquiredType(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return Payload::isValid(buf)
            && buf.size() >= sizeof(PlayStatus)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(COMMAND_IDS[ct])
            && PlayInquiredType_isValidByteCode(buf[offsetof(PlayStatus, playInquiredType)]);
    }
};

// - PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT

struct PlayStatusPlaybackController : PlayStatus
{
    MessageMdrV2EnableDisable status; // 0x2
    PlaybackStatus playbackStatus; // 0x3
    MusicCallStatus musicCallStatus; // 0x4

    PlayStatusPlaybackController(
        CommandType ct, MessageMdrV2EnableDisable status, PlaybackStatus playbackStatus, MusicCallStatus musicCallStatus
    )
        : PlayStatus(ct, PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT)
        , status(status)
        , playbackStatus(playbackStatus)
        , musicCallStatus(musicCallStatus)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return PlayStatus::isValid(buf, ct)
            && buf.size() == sizeof(PlayStatusPlaybackController)
            && isValidInquiredType(static_cast<PlayInquiredType>(buf[offsetof(PlayStatusPlaybackController, playInquiredType)]))
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(PlayStatusPlaybackController, status)])
            && PlaybackStatus_isValidByteCode(buf[offsetof(PlayStatusPlaybackController, playbackStatus)])
            && MusicCallStatus_isValidByteCode(buf[offsetof(PlayStatusPlaybackController, musicCallStatus)]);
    }

    static bool isValidInquiredType(PlayInquiredType type)
    {
        return type == PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT;
    }
};

// - PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE

struct PlayStatusPlaybackControlWithCallVolumeAdjustmentAndFunctionChange : PlayStatus
{
    MessageMdrV2EnableDisable status; // 0x2
    PlaybackStatus playbackStatus; // 0x3
    MusicCallStatus musicCallStatus; // 0x4
    MessageMdrV2EnableDisable playbackControlStatus; // 0x5

    PlayStatusPlaybackControlWithCallVolumeAdjustmentAndFunctionChange(
        CommandType ct, MessageMdrV2EnableDisable status, PlaybackStatus playbackStatus,
        MusicCallStatus musicCallStatus, MessageMdrV2EnableDisable playbackControlStatus
    )
        : PlayStatus(ct, PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE)
        , status(status)
        , playbackStatus(playbackStatus)
        , musicCallStatus(musicCallStatus)
        , playbackControlStatus(playbackControlStatus)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return PlayStatus::isValid(buf, ct)
            && buf.size() == sizeof(PlayStatusPlaybackControlWithCallVolumeAdjustmentAndFunctionChange)
            && isValidInquiredType(static_cast<PlayInquiredType>(buf[offsetof(PlayStatusPlaybackControlWithCallVolumeAdjustmentAndFunctionChange, playInquiredType)]))
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(PlayStatusPlaybackControlWithCallVolumeAdjustmentAndFunctionChange, status)])
            && PlaybackStatus_isValidByteCode(buf[offsetof(PlayStatusPlaybackControlWithCallVolumeAdjustmentAndFunctionChange, playbackStatus)])
            && MusicCallStatus_isValidByteCode(buf[offsetof(PlayStatusPlaybackControlWithCallVolumeAdjustmentAndFunctionChange, musicCallStatus)])
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(PlayStatusPlaybackControlWithCallVolumeAdjustmentAndFunctionChange, playbackControlStatus)]);
    }

    static bool isValidInquiredType(PlayInquiredType type)
    {
        return type == PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE;
    }
};

// - PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE

struct PlayStatusPlaybackControlWithFunctionChange : PlayStatus
{
    MessageMdrV2EnableDisable status; // 0x2
    PlaybackStatus playbackStatus; // 0x3
    MessageMdrV2EnableDisable playbackControlStatus; // 0x4

    PlayStatusPlaybackControlWithFunctionChange(
        CommandType ct, MessageMdrV2EnableDisable status, PlaybackStatus playbackStatus,
        MessageMdrV2EnableDisable playbackControlStatus
    )
        : PlayStatus(ct, PlayInquiredType::PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE)
        , status(status)
        , playbackStatus(playbackStatus)
        , playbackControlStatus(playbackControlStatus)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return PlayStatus::isValid(buf, ct)
            && buf.size() == sizeof(PlayStatusPlaybackControlWithFunctionChange)
            && isValidInquiredType(static_cast<PlayInquiredType>(buf[offsetof(PlayStatusPlaybackControlWithFunctionChange, playInquiredType)]))
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(PlayStatusPlaybackControlWithFunctionChange, status)])
            && PlaybackStatus_isValidByteCode(buf[offsetof(PlayStatusPlaybackControlWithFunctionChange, playbackStatus)])
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(PlayStatusPlaybackControlWithFunctionChange, playbackControlStatus)]);
    }

    static bool isValidInquiredType(PlayInquiredType type)
    {
        return type == PlayInquiredType::PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE;
    }
};

// - PLAY_MODE

struct PlayStatusCommon : PlayStatus
{
    MessageMdrV2EnableDisable status; // 0x2

    PlayStatusCommon(CommandType ct, MessageMdrV2EnableDisable status)
        : PlayStatus(ct, PlayInquiredType::PLAY_MODE)
        , status(status)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return PlayStatus::isValid(buf, ct)
            && buf.size() == sizeof(PlayStatusCommon)
            && isValidInquiredType(static_cast<PlayInquiredType>(buf[offsetof(PlayStatusCommon, playInquiredType)]))
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(PlayStatusCommon, status)]);
    }

    static bool isValidInquiredType(PlayInquiredType type)
    {
        return type == PlayInquiredType::PLAY_MODE;
    }
};

// endregion PLAY_RET_STATUS, PLAY_NTFY_STATUS

// region PLAY_SET_STATUS

struct SetPlayStatus : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::PLAY_NTFY_STATUS;

    PlayInquiredType type; // 0x1

protected:
    SetPlayStatus(PlayInquiredType type)
        : Payload(Command::PLAY_SET_STATUS)
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() >= sizeof(SetPlayStatus)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::PLAY_SET_STATUS)
            && PlayInquiredType_isValidByteCode(buf[offsetof(SetPlayStatus, type)]);
    }
};

// - PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT, PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE, PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE

struct SetPlayStatusPlaybackController : SetPlayStatus
{
    MessageMdrV2EnableDisable status; // 0x2
    PlaybackControl control; // 0x3

    SetPlayStatusPlaybackController(PlayInquiredType type, MessageMdrV2EnableDisable status, PlaybackControl control)
        : SetPlayStatus(type)
        , status(status)
        , control(control)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return SetPlayStatus::isValid(buf)
            && buf.size() == sizeof(SetPlayStatusPlaybackController)
            && isValidInquiredType(static_cast<PlayInquiredType>(buf[offsetof(SetPlayStatusPlaybackController, type)]))
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(SetPlayStatusPlaybackController, status)])
            && PlaybackControl_isValidByteCode(buf[offsetof(SetPlayStatusPlaybackController, control)]);
    }

    static bool isValidInquiredType(PlayInquiredType type)
    {
        return type == PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT
            || type == PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE
            || type == PlayInquiredType::PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE;
    }
};

// endregion PLAY_SET_STATUS

// endregion PLAY_*_STATUS

// region PLAY_*_PARAM

// region PLAY_GET_PARAM

struct GetPlayParam : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::PLAY_RET_PARAM;

    PlayInquiredType type; // 0x1

    GetPlayParam(PlayInquiredType type)
        : Payload(Command::PLAY_GET_PARAM)
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(GetPlayParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::PLAY_GET_PARAM)
            && PlayInquiredType_isValidByteCode(buf[offsetof(GetPlayParam, type)]);
    }
};

// endregion PLAY_GET_PARAM

// region PLAY_RET_PARAM, PLAY_SET_PARAM, PLAY_NTFY_PARAM

struct PlayParam : Payload
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::PLAY_RET_PARAM,
        Command::PLAY_SET_PARAM,
        Command::PLAY_NTFY_PARAM,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::PLAY_NTFY_PARAM,
        Command::UNKNOWN,
    };

    PlayInquiredType playInquiredType; // 0x1

    PlayParam(CommandType ct, PlayInquiredType type)
        : Payload(COMMAND_IDS[ct])
        , playInquiredType(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return Payload::isValid(buf)
            && buf.size() >= sizeof(PlayParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(COMMAND_IDS[ct])
            && PlayInquiredType_isValidByteCode(buf[offsetof(PlayParam, playInquiredType)]);
    }
};

// - PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT, PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE, PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE

#pragma pack(pop)

enum class PlaybackNameStatus
{
    UNSETTLED = 0,
    NOTHING = 1,
    SETTLED = 2,
};

inline bool PlaybackNameStatus_isValidByteCode(uint8_t status)
{
    switch (static_cast<PlaybackNameStatus>(status))
    {
    case PlaybackNameStatus::UNSETTLED:
    case PlaybackNameStatus::NOTHING:
    case PlaybackNameStatus::SETTLED:
        return true;
    }
    return false;
}

struct PlaybackName
{
    static constexpr size_t MAX_NAME_LENGTH = 128;

    PlaybackNameStatus playbackNameStatus;
    std::string name;

    static PlaybackName deserialize(std::span<const uint8_t>& buf)
    {
        if (buf.size() < 2)
            throw std::runtime_error("Buffer too small for PlaybackName");

        PlaybackName pn;
        pn.playbackNameStatus = static_cast<PlaybackNameStatus>(buf[0]);
        buf = buf.subspan(1);

        pn.name = readPrefixedString(buf);
        return pn;
    }

    void serialize(std::vector<uint8_t>& buf) const
    {
        buf.push_back(static_cast<uint8_t>(playbackNameStatus));
        writePrefixedString(buf, name);
    }

    size_t countBytes() const
    {
        return 1 + 1 + name.size();
    }
};

struct PlayParamPlaybackControllerName : PlayParam
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::PLAY_RET_PARAM,
        Command::UNKNOWN,
        Command::PLAY_NTFY_PARAM,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
    };

    static constexpr size_t NUM_OF_PLAYBACK_NAMES = 4;

    std::array<PlaybackName, NUM_OF_PLAYBACK_NAMES> playbackNames; // playbackNames: 0x2-

    PlayParamPlaybackControllerName(CommandType ct, std::array<PlaybackName, NUM_OF_PLAYBACK_NAMES>&& playbackNames)
        : PlayParam(ct, PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT)
        , playbackNames(std::move(playbackNames))
    {}

private:
    // For init by deserialization
    PlayParamPlaybackControllerName(PlayInquiredType type)
        : PlayParam(CT_Get /*Any can do*/, type)
    {}

public:
    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        if (!PlayParam::isValid(buf, ct))
            return false;
        if (!isValidInquiredType(static_cast<PlayInquiredType>(buf[offsetof(PlayParam, playInquiredType)])))
            return false;

        size_t bufSize = buf.size();
        size_t indexPlaybackName = 1 + 1; // command + playInquiredType
        uint8_t playbackNamesNum = 0;
        while (indexPlaybackName < bufSize)
        {
            size_t indexNameLength = indexPlaybackName + 1;
            if (bufSize < indexNameLength)
                return false;

            size_t nameLength = buf[indexNameLength];
            if (nameLength > PlaybackName::MAX_NAME_LENGTH)
                return false;

            size_t indexName = indexNameLength + 1;
            if (bufSize < indexName)
                return false;

            indexPlaybackName = indexName + nameLength;
            ++playbackNamesNum;
        }

        if (bufSize != indexPlaybackName)
            return false;
        if (playbackNamesNum != NUM_OF_PLAYBACK_NAMES)
            return false;

        return true;
    }

    static bool isValidInquiredType(PlayInquiredType type)
    {
        return type == PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT
            || type == PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE
            || type == PlayInquiredType::PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE;
    }

    static PlayParamPlaybackControllerName deserialize(std::span<const uint8_t>& buf, CommandType ct)
    {
        if (!isValid(buf, ct))
            throw std::runtime_error("Buffer invalid for PlayParamPlaybackControllerName");

        PlayInquiredType type = static_cast<PlayInquiredType>(buf[offsetof(PlayParam, playInquiredType)]);
        PlayParamPlaybackControllerName result(type);
        result.command = static_cast<Command>(buf[offsetof(Payload, command)]);
        buf = buf.subspan(sizeof(PlayParam));

        for (size_t i = 0; i < NUM_OF_PLAYBACK_NAMES; ++i)
        {
            result.playbackNames[i] = PlaybackName::deserialize(buf);
        }

        return result;
    }

    void serialize(std::vector<uint8_t>& buf) const
    {
        buf.push_back(static_cast<uint8_t>(command));
        buf.push_back(static_cast<uint8_t>(playInquiredType));
        // Size not encoded, always 4
        for (const PlaybackName& pn : playbackNames)
        {
            pn.serialize(buf);
        }
    }

    size_t countBytes() const
    {
        size_t size = 1 + 1; // command + playInquiredType
        for (const PlaybackName& pn : playbackNames)
        {
            size += pn.countBytes();
        }
        return size;
    }

    static constexpr bool VARIABLE_SIZE_NEEDS_SERIALIZATION = true;
};

#pragma pack(push, 1)

// - MUSIC_VOLUME, CALL_VOLUME

struct PlayParamPlaybackControllerVolume : PlayParam
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::PLAY_RET_PARAM,
        Command::PLAY_SET_PARAM,
        Command::PLAY_NTFY_PARAM,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::PLAY_NTFY_PARAM,
        Command::UNKNOWN,
    };

    uint8_t volumeValue; // 0x2

    PlayParamPlaybackControllerVolume(CommandType ct, PlayInquiredType type, uint8_t volumeValue)
        : PlayParam(ct, type)
        , volumeValue(volumeValue)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return PlayParam::isValid(buf, ct)
            && buf.size() == sizeof(PlayParamPlaybackControllerVolume)
            && isValidInquiredType(static_cast<PlayInquiredType>(buf[offsetof(PlayParam, playInquiredType)]), ct);
    }

    static bool isValidInquiredType(PlayInquiredType type, CommandType ct)
    {
        if (ct == CT_Set)
        {
            return type == PlayInquiredType::MUSIC_VOLUME
                || type == PlayInquiredType::CALL_VOLUME
                || type == PlayInquiredType::MUSIC_VOLUME_WITH_MUTE
                || type == PlayInquiredType::CALL_VOLUME_WITH_MUTE;
        }
        else
        {
            return type == PlayInquiredType::MUSIC_VOLUME
                || type == PlayInquiredType::CALL_VOLUME;
        }
    }
};

// - MUSIC_VOLUME_WITH_MUTE, CALL_VOLUME_WITH_MUTE

struct PlayParamPlaybackControllerVolumeWithMute : PlayParam
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::PLAY_RET_PARAM,
        Command::UNKNOWN,
        Command::PLAY_NTFY_PARAM,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
    };

    uint8_t volumeValue; // 0x2
    MessageMdrV2EnableDisable muteSetting; // 0x3

    PlayParamPlaybackControllerVolumeWithMute(CommandType ct, PlayInquiredType type, uint8_t volumeValue, MessageMdrV2EnableDisable muteSetting)
        : PlayParam(ct, type)
        , volumeValue(volumeValue)
        , muteSetting(muteSetting)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return PlayParam::isValid(buf, ct)
            && buf.size() == sizeof(PlayParamPlaybackControllerVolumeWithMute)
            && isValidInquiredType(static_cast<PlayInquiredType>(buf[offsetof(PlayParam, playInquiredType)]), ct)
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(PlayParamPlaybackControllerVolumeWithMute, muteSetting)]);
    }

    static bool isValidInquiredType(PlayInquiredType type, CommandType ct)
    {
        if (ct == CT_Set)
            return false;
        return type == PlayInquiredType::MUSIC_VOLUME_WITH_MUTE
            || type == PlayInquiredType::CALL_VOLUME_WITH_MUTE;
    }
};

// - PLAY_MODE

enum class PlayMode : uint8_t
{
    PLAY_MODE_OFF = 0x00,
    PLAY_FOLDER = 0x01,
    REPEAT_ALL = 0x02,
    REPEAT_FOLDER = 0x03,
    REPEAT_TRACK = 0x04,
    SHUFFLE_ALL = 0x05,
};

inline bool PlayMode_isValidByteCode(uint8_t mode)
{
    switch (static_cast<PlayMode>(mode))
    {
    case PlayMode::PLAY_MODE_OFF:
    case PlayMode::PLAY_FOLDER:
    case PlayMode::REPEAT_ALL:
    case PlayMode::REPEAT_FOLDER:
    case PlayMode::REPEAT_TRACK:
    case PlayMode::SHUFFLE_ALL:
        return true;
    }
    return false;
}

struct PlayParamPlayMode : PlayParam
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::PLAY_RET_PARAM,
        Command::PLAY_SET_PARAM,
        Command::PLAY_NTFY_PARAM,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::PLAY_NTFY_PARAM,
        Command::UNKNOWN,
    };

    PlayMode playMode; // 0x2

    PlayParamPlayMode(CommandType ct, PlayMode playMode)
        : PlayParam(ct, PlayInquiredType::PLAY_MODE)
        , playMode(playMode)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return PlayParam::isValid(buf, ct)
            && buf.size() == sizeof(PlayParamPlayMode)
            && buf[offsetof(PlayParamPlayMode, playInquiredType)] == static_cast<uint8_t>(PlayInquiredType::PLAY_MODE)
            && PlayMode_isValidByteCode(buf[offsetof(PlayParamPlayMode, playMode)]);
    }
};

// endregion PLAY_RET_PARAM, PLAY_SET_PARAM, PLAY_NTFY_PARAM

// endregion PLAY_*_PARAM

// endregion PLAY

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

    static GsSettingInfo deserialize(std::span<const uint8_t>& buf)
    {
        if (buf.empty())
            throw std::runtime_error("Buffer too small for GsSettingInfo");

        GsSettingInfo info;
        info.stringFormat = static_cast<GsStringFormat>(buf[0]); buf = buf.subspan(1);
        info.subject = readPrefixedString(buf);
        info.summary = readPrefixedString(buf);
        return info;
    }

    void serialize(std::vector<uint8_t>& buf) const
    {
        buf.push_back(static_cast<uint8_t>(stringFormat));
        writePrefixedString(buf, subject);
        writePrefixedString(buf, summary);
    }

    size_t countBytes() const
    {
        return 1 /*stringFormat*/ + 1 /*subject.len*/ + subject.size() + 1 /*summary.len*/ + summary.size();
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

    static GsRetCapability deserialize(std::span<const uint8_t>& buf)
    {
        if (!isValid(buf))
            throw std::runtime_error("Buffer invalid for GsRetCapability");

        GsRetCapability cap;
        cap.command = static_cast<Command>(buf[0]);
        cap.type = static_cast<GsInquiredType>(buf[1]);
        cap.settingType = static_cast<GsSettingType>(buf[2]);
        buf = buf.subspan(3);
        cap.settingInfo = GsSettingInfo::deserialize(buf);
        return cap;
    }

    void serialize(std::vector<uint8_t>& buf) const
    {
        buf.push_back(static_cast<uint8_t>(command));
        buf.push_back(static_cast<uint8_t>(type));
        buf.push_back(static_cast<uint8_t>(settingType));
        settingInfo.serialize(buf);
    }

    size_t countBytes() const
    {
        return 1 /*command*/ + 1 /*type*/ + 1 /*settingType*/ + settingInfo.countBytes();
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

// region AUDIO

enum class AudioInquiredType : uint8_t
{
    // PARAM:  [RSN] AudioParamConnection
    CONNECTION_MODE = 0x00,
    // PARAM:  [RSN] AudioParamUpscaling
    UPSCALING = 0x01,
    // PARAM:  [RSN] AudioParamConnectionWithLdacStatus
    CONNECTION_MODE_WITH_LDAC_STATUS = 0x02,
    // PARAM:  [RSN] AudioParamBGMMode
    BGM_MODE = 0x03,
    // PARAM:  [RSN] AudioParamUpmixCinema
    UPMIX_CINEMA = 0x04,
    // PARAM:  [R  ] AudioRetParamConnectionModeClassicAudioLeAudio
    //         [ S ] AudioSetParamConnectionModeClassicAudioLeAudio
    //         [  N] AudioNtfyParamConnectionModeClassicAudioLeAudio
    CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO = 0x05,
    // PARAM:  [RSN] AudioParamVoiceContents
    VOICE_CONTENTS = 0x06,
    // PARAM:  [RSN] AudioParamSoundLeakageReduction
    SOUND_LEAKAGE_REDUCTION = 0x07,
    // PARAM:  [RSN] AudioParamListeningOptionAssignCustomizable
    LISTENING_OPTION_ASSIGN_CUSTOMIZABLE = 0x08,
    // PARAM:  [RSN] AudioParamBGMMode
    BGM_MODE_AND_ERRORCODE = 0x09,
    // PARAM:  [RSN] AudioParamUpmixSeries
    UPMIX_SERIES = 0x0A,
};

inline bool AudioInquiredType_isValidByteCode(uint8_t type)
{
    switch (static_cast<AudioInquiredType>(type))
    {
    case AudioInquiredType::CONNECTION_MODE:
    case AudioInquiredType::UPSCALING:
    case AudioInquiredType::CONNECTION_MODE_WITH_LDAC_STATUS:
    case AudioInquiredType::BGM_MODE:
    case AudioInquiredType::UPMIX_CINEMA:
    case AudioInquiredType::CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO:
    case AudioInquiredType::VOICE_CONTENTS:
    case AudioInquiredType::SOUND_LEAKAGE_REDUCTION:
    case AudioInquiredType::LISTENING_OPTION_ASSIGN_CUSTOMIZABLE:
    case AudioInquiredType::BGM_MODE_AND_ERRORCODE:
    case AudioInquiredType::UPMIX_SERIES:
        return true;
    }
    return false;
}

enum class PriorMode : uint8_t
{
    SOUND_QUALITY_PRIOR = 0x00,
    CONNECTION_QUALITY_PRIOR = 0x01,
    LOW_LATENCY_PRIOR_BETA = 0x02,
};

inline bool PriorMode_isValidByteCode(uint8_t mode)
{
    switch (static_cast<PriorMode>(mode))
    {
    case PriorMode::SOUND_QUALITY_PRIOR:
    case PriorMode::CONNECTION_QUALITY_PRIOR:
    case PriorMode::LOW_LATENCY_PRIOR_BETA:
        return true;
    }
    return false;
}

enum class UpscalingTypeAutoOff : uint8_t
{
    OFF = 0x00,
    AUTO = 0x01,
};

inline bool UpscalingTypeAutoOff_isValidByteCode(uint8_t type)
{
    switch (static_cast<UpscalingTypeAutoOff>(type))
    {
    case UpscalingTypeAutoOff::OFF:
    case UpscalingTypeAutoOff::AUTO:
        return true;
    }
    return false;
}

enum class RoomSize : uint8_t
{
    SMALL = 0x00,
    MIDDLE = 0x01,
    LARGE = 0x02,
};

inline bool RoomSize_isValidByteCode(uint8_t size)
{
    switch (static_cast<RoomSize>(size))
    {
    case RoomSize::SMALL:
    case RoomSize::MIDDLE:
    case RoomSize::LARGE:
        return true;
    }
    return false;
}

enum class ListeningOptionAssignCustomizableItem : uint8_t
{
    STANDARD = 0x00,
    BGM = 0x01,
    UPMIX_CINEMA = 0x02,
    UPMIX_GAME = 0x03,
    UPMIX_MUSIC = 0x04,
};

inline bool ListeningOptionAssignCustomizableItem_isValidByteCode(uint8_t item)
{
    switch (static_cast<ListeningOptionAssignCustomizableItem>(item))
    {
    case ListeningOptionAssignCustomizableItem::STANDARD:
    case ListeningOptionAssignCustomizableItem::BGM:
    case ListeningOptionAssignCustomizableItem::UPMIX_CINEMA:
    case ListeningOptionAssignCustomizableItem::UPMIX_GAME:
    case ListeningOptionAssignCustomizableItem::UPMIX_MUSIC:
        return true;
    }
    return false;
}

enum class UpmixItemId : uint8_t
{
    NONE = 0x00,
    CINEMA = 0x01,
    GAME = 0x02,
    MUSIC = 0x03,
};

inline bool UpmixItemId_isValidByteCode(uint8_t id)
{
    switch (static_cast<UpmixItemId>(id))
    {
    case UpmixItemId::NONE:
    case UpmixItemId::CINEMA:
    case UpmixItemId::GAME:
    case UpmixItemId::MUSIC:
        return true;
    }
    return false;
}

enum class SwitchingStream : uint8_t
{
    NONE = 0x00,
    LE_AUDIO = 0x01,
    CLASSIC_AUDIO = 0x02,
};

inline bool SwitchingStream_isValidByteCode(uint8_t stream)
{
    switch (static_cast<SwitchingStream>(stream))
    {
    case SwitchingStream::NONE:
    case SwitchingStream::LE_AUDIO:
    case SwitchingStream::CLASSIC_AUDIO:
        return true;
    }
    return false;
}

// region AUDIO_*_CAPABILITY

// region AUDIO_GET_CAPABILITY

struct AudioGetCapability : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::AUDIO_RET_CAPABILITY;

    AudioInquiredType type; // 0x1

    AudioGetCapability(AudioInquiredType type)
        : Payload(Command::AUDIO_GET_CAPABILITY)
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(AudioGetCapability)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::AUDIO_GET_CAPABILITY)
            && AudioInquiredType_isValidByteCode(buf[offsetof(AudioGetCapability, type)]);
    }
};

// endregion AUDIO_GET_CAPABILITY

// region AUDIO_RET_CAPABILITY

// Not implemented

// endregion AUDIO_RET_CAPABILITY

// endregion AUDIO_*_CAPABILITY

// region AUDIO_*_STATUS

// region AUDIO_GET_STATUS

struct AudioGetStatus : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::AUDIO_RET_STATUS;

    AudioInquiredType type; // 0x1

    AudioGetStatus(AudioInquiredType type)
        : Payload(Command::AUDIO_GET_STATUS)
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(AudioGetStatus)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::AUDIO_GET_STATUS)
            && AudioInquiredType_isValidByteCode(buf[offsetof(AudioGetStatus, type)]);
    }
};

// endregion AUDIO_GET_STATUS

// region AUDIO_RET_STATUS, AUDIO_SET_STATUS, AUDIO_NTFY_STATUS

// Not implemented

// endregion AUDIO_RET_STATUS, AUDIO_SET_STATUS, AUDIO_NTFY_STATUS

// endregion AUDIO_*_STATUS

// region AUDIO_*_PARAM

// region AUDIO_GET_PARAM

struct AudioGetParam : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::AUDIO_RET_PARAM;

    AudioInquiredType type; // 0x1

    AudioGetParam(AudioInquiredType type)
        : Payload(Command::AUDIO_GET_PARAM)
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(AudioGetParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::AUDIO_GET_PARAM)
            && AudioInquiredType_isValidByteCode(buf[offsetof(AudioGetParam, type)]);
    }
};

// endregion AUDIO_GET_PARAM

// region AUDIO_RET_PARAM, AUDIO_SET_PARAM, AUDIO_NTFY_PARAM

struct AudioParam : Payload
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::AUDIO_RET_PARAM,
        Command::AUDIO_SET_PARAM,
        Command::AUDIO_NTFY_PARAM,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::AUDIO_NTFY_PARAM,
        Command::UNKNOWN,
    };

    AudioInquiredType type; // 0x1

    AudioParam(CommandType ct, AudioInquiredType type)
        : Payload(COMMAND_IDS[ct])
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return Payload::isValid(buf)
            && buf.size() >= sizeof(AudioParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(COMMAND_IDS[ct])
            && AudioInquiredType_isValidByteCode(buf[offsetof(AudioParam, type)]);
    }
};

// - CONNECTION_MODE

struct AudioParamConnection : AudioParam
{
    PriorMode settingValue; // 0x2

    AudioParamConnection(CommandType ct, PriorMode settingValue)
        : AudioParam(ct, AudioInquiredType::CONNECTION_MODE)
        , settingValue(settingValue)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return AudioParam::isValid(buf, ct)
            && buf.size() == sizeof(AudioParamConnection)
            && buf[offsetof(AudioParamConnection, type)] == static_cast<uint8_t>(AudioInquiredType::CONNECTION_MODE)
            && PriorMode_isValidByteCode(buf[offsetof(AudioParamConnection, settingValue)]);
    }
};

// - UPSCALING

struct AudioParamUpscaling : AudioParam
{
    UpscalingTypeAutoOff settingValue; // 0x2

    AudioParamUpscaling(CommandType ct, UpscalingTypeAutoOff settingValue)
        : AudioParam(ct, AudioInquiredType::UPSCALING)
        , settingValue(settingValue)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return AudioParam::isValid(buf, ct)
            && buf.size() == sizeof(AudioParamUpscaling)
            && buf[offsetof(AudioParamUpscaling, type)] == static_cast<uint8_t>(AudioInquiredType::UPSCALING)
            && UpscalingTypeAutoOff_isValidByteCode(buf[offsetof(AudioParamUpscaling, settingValue)]);
    }
};

// - CONNECTION_MODE_WITH_LDAC_STATUS

struct AudioParamConnectionWithLdacStatus : AudioParam
{
    PriorMode settingValue; // 0x2

    AudioParamConnectionWithLdacStatus(CommandType ct, PriorMode settingValue)
        : AudioParam(ct, AudioInquiredType::CONNECTION_MODE_WITH_LDAC_STATUS)
        , settingValue(settingValue)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return AudioParam::isValid(buf, ct)
            && buf.size() == sizeof(AudioParamConnectionWithLdacStatus)
            && buf[offsetof(AudioParamConnectionWithLdacStatus, type)] == static_cast<uint8_t>(AudioInquiredType::CONNECTION_MODE_WITH_LDAC_STATUS)
            && PriorMode_isValidByteCode(buf[offsetof(AudioParamConnectionWithLdacStatus, settingValue)]);
    }
};

// - CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO

struct AudioRetParamConnectionModeClassicAudioLeAudio : AudioParam
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::AUDIO_RET_PARAM,
        Command::UNKNOWN,
        Command::UNKNOWN,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
    };

    PriorMode settingValue; // 0x2

    AudioRetParamConnectionModeClassicAudioLeAudio(CommandType ct, PriorMode settingValue)
        : AudioParam(ct, AudioInquiredType::CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO)
        , settingValue(settingValue)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return AudioParam::isValid(buf, ct)
            && buf.size() == sizeof(AudioRetParamConnectionModeClassicAudioLeAudio)
            && buf[offsetof(AudioRetParamConnectionModeClassicAudioLeAudio, type)] == static_cast<uint8_t>(AudioInquiredType::CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO)
            && PriorMode_isValidByteCode(buf[offsetof(AudioRetParamConnectionModeClassicAudioLeAudio, settingValue)]);
    }
};

// - BGM_MODE, BGM_MODE_AND_ERRORCODE

struct AudioParamBGMMode : AudioParam
{
    MessageMdrV2EnableDisable onOffSettingValue; // 0x2
    RoomSize targetRoomSize; // 0x3

    AudioParamBGMMode(CommandType ct, AudioInquiredType type, MessageMdrV2EnableDisable onOffSettingValue, RoomSize targetRoomSize)
        : AudioParam(ct, type)
        , onOffSettingValue(onOffSettingValue)
        , targetRoomSize(targetRoomSize)
    {
    }

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return AudioParam::isValid(buf, ct)
            && buf.size() == sizeof(AudioParamBGMMode)
            && isValidInquiredType(static_cast<AudioInquiredType>(buf[offsetof(AudioParamBGMMode, type)]))
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(AudioParamBGMMode, onOffSettingValue)])
            && RoomSize_isValidByteCode(buf[offsetof(AudioParamBGMMode, targetRoomSize)]);
    }

    static bool isValidInquiredType(AudioInquiredType type)
    {
        return type == AudioInquiredType::BGM_MODE
            || type == AudioInquiredType::BGM_MODE_AND_ERRORCODE;
    }
};

// - UPMIX_CINEMA

struct AudioParamUpmixCinema : AudioParam
{
    MessageMdrV2EnableDisable onOffSettingValue; // 0x2

    AudioParamUpmixCinema(CommandType ct, MessageMdrV2EnableDisable onOffSettingValue)
        : AudioParam(ct, AudioInquiredType::UPMIX_CINEMA)
        , onOffSettingValue(onOffSettingValue)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return AudioParam::isValid(buf, ct)
            && buf.size() == sizeof(AudioParamUpmixCinema)
            && buf[offsetof(AudioParamUpmixCinema, type)] == static_cast<uint8_t>(AudioInquiredType::UPMIX_CINEMA)
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(AudioParamUpmixCinema, onOffSettingValue)]);
    }
};

// - VOICE_CONTENTS

struct AudioParamVoiceContents : AudioParam
{
    MessageMdrV2EnableDisable onOffSettingValue; // 0x2

    AudioParamVoiceContents(CommandType ct, MessageMdrV2EnableDisable onOffSettingValue)
        : AudioParam(ct, AudioInquiredType::VOICE_CONTENTS)
        , onOffSettingValue(onOffSettingValue)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return AudioParam::isValid(buf, ct)
            && buf.size() == sizeof(AudioParamVoiceContents)
            && buf[offsetof(AudioParamVoiceContents, type)] == static_cast<uint8_t>(AudioInquiredType::VOICE_CONTENTS)
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(AudioParamVoiceContents, onOffSettingValue)]);
    }
};

// - SOUND_LEAKAGE_REDUCTION

struct AudioParamSoundLeakageReduction : AudioParam
{
    MessageMdrV2EnableDisable onOffSettingValue; // 0x2

    AudioParamSoundLeakageReduction(CommandType ct, MessageMdrV2EnableDisable onOffSettingValue)
        : AudioParam(ct, AudioInquiredType::SOUND_LEAKAGE_REDUCTION)
        , onOffSettingValue(onOffSettingValue)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return AudioParam::isValid(buf, ct)
            && buf.size() == sizeof(AudioParamSoundLeakageReduction)
            && buf[offsetof(AudioParamSoundLeakageReduction, type)] == static_cast<uint8_t>(AudioInquiredType::SOUND_LEAKAGE_REDUCTION)
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(AudioParamSoundLeakageReduction, onOffSettingValue)]);
    }
};

// - LISTENING_OPTION_ASSIGN_CUSTOMIZABLE

struct AudioParamListeningOptionAssignCustomizableItem : AudioParam
{
    uint8_t numberOfAssignedItem; // 0x2
    ListeningOptionAssignCustomizableItem assignedItems[]; // 0x3-

private:
    AudioParamListeningOptionAssignCustomizableItem(
        CommandType ct, const std::span<const ListeningOptionAssignCustomizableItem>& assignedItems
    )
        : AudioParam(ct, AudioInquiredType::LISTENING_OPTION_ASSIGN_CUSTOMIZABLE)
        , numberOfAssignedItem(assignedItems.size())
    {
        std::memcpy(this->assignedItems, assignedItems.data(), sizeof(ListeningOptionAssignCustomizableItem) * numberOfAssignedItem);
    }

public:
    VARIABLE_SIZE_PAYLOAD_ONE_ARRAY_AT_END(ListeningOptionAssignCustomizableItem, 255);

    std::span<const ListeningOptionAssignCustomizableItem> getAssignedItems() const
    {
        return { assignedItems, numberOfAssignedItem };
    }

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return AudioParam::isValid(buf, ct)
            && buf[offsetof(AudioParamListeningOptionAssignCustomizableItem, type)] == static_cast<uint8_t>(AudioInquiredType::LISTENING_OPTION_ASSIGN_CUSTOMIZABLE)
            && buf.size() >= sizeof(AudioParamListeningOptionAssignCustomizableItem)
            && buf.size() == sizeof(AudioParamListeningOptionAssignCustomizableItem) + sizeof(ListeningOptionAssignCustomizableItem) * buf[offsetof(ConnectRetSupportFunction, numberOfFunction)];
        // Not validating every entry for simplicity
    }
};

// - UPMIX_SERIES

struct AudioParamUpmixSeries : AudioParam
{
    UpmixItemId upmixItemId; // 0x2

    AudioParamUpmixSeries(CommandType ct, UpmixItemId upmixItemId)
        : AudioParam(ct, AudioInquiredType::UPMIX_SERIES)
        , upmixItemId(upmixItemId)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return AudioParam::isValid(buf, ct)
            && buf.size() == sizeof(AudioParamUpmixSeries)
            && buf[offsetof(AudioParamUpmixSeries, type)] == static_cast<uint8_t>(AudioInquiredType::UPMIX_SERIES)
            && UpmixItemId_isValidByteCode(buf[offsetof(AudioParamUpmixSeries, upmixItemId)]);
    }
};

// endregion AUDIO_RET_PARAM, AUDIO_SET_PARAM, AUDIO_NTFY_PARAM

// region AUDIO_SET_PARAM

// - CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO

struct AudioSetParamConnectionModeClassicAudioLeAudio : AudioParam
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::AUDIO_SET_PARAM,
        Command::UNKNOWN,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::AUDIO_NTFY_PARAM,
        Command::UNKNOWN,
    };

    PriorMode settingValue; // 0x2
    MessageMdrV2EnableDisable alertConfirmation; // 0x3

    AudioSetParamConnectionModeClassicAudioLeAudio(CommandType ct, PriorMode settingValue, MessageMdrV2EnableDisable alertConfirmation)
        : AudioParam(ct, AudioInquiredType::CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO)
        , settingValue(settingValue)
        , alertConfirmation(alertConfirmation)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return AudioParam::isValid(buf, ct)
            && buf.size() == sizeof(AudioSetParamConnectionModeClassicAudioLeAudio)
            && buf[offsetof(AudioSetParamConnectionModeClassicAudioLeAudio, type)] == static_cast<uint8_t>(AudioInquiredType::CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO)
            && PriorMode_isValidByteCode(buf[offsetof(AudioSetParamConnectionModeClassicAudioLeAudio, settingValue)])
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(AudioSetParamConnectionModeClassicAudioLeAudio, alertConfirmation)]);
    }
};

// endregion AUDIO_SET_PARAM

// region AUDIO_NTFY_PARAM

// - CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO

struct AudioNtfyParamConnectionModeClassicAudioLeAudio : AudioParam
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::AUDIO_NTFY_PARAM,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
    };

    PriorMode settingValue; // 0x2
    SwitchingStream switchingStream; // 0x3

    AudioNtfyParamConnectionModeClassicAudioLeAudio(CommandType ct, PriorMode settingValue, SwitchingStream switchingStream)
        : AudioParam(ct, AudioInquiredType::CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO)
        , settingValue(settingValue)
        , switchingStream(switchingStream)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return AudioParam::isValid(buf, ct)
            && buf.size() == sizeof(AudioNtfyParamConnectionModeClassicAudioLeAudio)
            && buf[offsetof(AudioNtfyParamConnectionModeClassicAudioLeAudio, type)] == static_cast<uint8_t>(AudioInquiredType::CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO)
            && PriorMode_isValidByteCode(buf[offsetof(AudioNtfyParamConnectionModeClassicAudioLeAudio, settingValue)])
            && SwitchingStream_isValidByteCode(buf[offsetof(AudioNtfyParamConnectionModeClassicAudioLeAudio, switchingStream)]);
    }
};

// endregion AUDIO_NTFY_PARAM

// endregion AUDIO_*_PARAM

// endregion AUDIO

// region SYSTEM

enum class SystemInquiredType : uint8_t
{
    // PARAM:  [RSN] SystemParamCommon
    VIBRATOR = 0x00,
    // PARAM:  [RSN] SystemParamCommon
    PLAYBACK_CONTROL_BY_WEARING = 0x01,
    // PARAM:  [RSN] SystemParamSmartTalking
    SMART_TALKING_MODE_TYPE1 = 0x02,
    // PARAM:  [RSN] SystemParamAssignableSettings
    ASSIGNABLE_SETTINGS = 0x03,
    // PARAM:  [RSN] SystemParamVoiceAssistantSettings
    VOICE_ASSISTANT_SETTINGS = 0x04,
    // PARAM:  [RSN] SystemParamCommon
    VOICE_ASSISTANT_WAKE_WORD = 0x05,
    // PARAM:  [R N] SystemParamWearingStatusDetector
    //         [ S ] SystemSetParamWearingStatusDetector
    WEARING_STATUS_DETECTOR = 0x06,
    // PARAM:  [RSN] SystemParamEarpieceSelection
    EARPIECE_SELECTION = 0x07,
    // PARAM:  [RSN] SystemParamCallSettings
    CALL_SETTINGS = 0x08,
    // PARAM:  [ S ] SystemSetParamResetSettings
    //         [  N] SystemNotifyParamResetSettings
    RESET_SETTINGS = 0x09,
    // PARAM:  [RSN] SystemParamCommon
    AUTO_VOLUME = 0x0A,
    // PARAM:  [  N] SystemNotifyParamFaceTapTestMode
    FACE_TAP_TEST_MODE = 0x0B,
    // PARAM:  [RSN] SystemParamSmartTalking
    SMART_TALKING_MODE_TYPE2 = 0x0C,
    // PARAM:  [R N] SystemParamQuickAccess
    //         [ S ] SystemSetParamQuickAccess
    QUICK_ACCESS = 0x0D,
    // PARAM:  [RSN] SystemParamAssignableSettingsWithLimit
    ASSIGNABLE_SETTINGS_WITH_LIMITATION = 0x0E,
    // PARAM:  [RSN] SystemParamCommon
    HEAD_GESTURE_ON_OFF = 0x0F,
    // PARAM:  [RSN] SystemParamHeadGestureTraining
    HEAD_GESTURE_TRAINING = 0x10,
};

inline bool SystemInquiredType_isValidByteCode(uint8_t type)
{
    switch (static_cast<SystemInquiredType>(type))
    {
    case SystemInquiredType::VIBRATOR:
    case SystemInquiredType::PLAYBACK_CONTROL_BY_WEARING:
    case SystemInquiredType::SMART_TALKING_MODE_TYPE1:
    case SystemInquiredType::ASSIGNABLE_SETTINGS:
    case SystemInquiredType::VOICE_ASSISTANT_SETTINGS:
    case SystemInquiredType::VOICE_ASSISTANT_WAKE_WORD:
    case SystemInquiredType::WEARING_STATUS_DETECTOR:
    case SystemInquiredType::EARPIECE_SELECTION:
    case SystemInquiredType::CALL_SETTINGS:
    case SystemInquiredType::RESET_SETTINGS:
    case SystemInquiredType::AUTO_VOLUME:
    case SystemInquiredType::FACE_TAP_TEST_MODE:
    case SystemInquiredType::SMART_TALKING_MODE_TYPE2:
    case SystemInquiredType::QUICK_ACCESS:
    case SystemInquiredType::ASSIGNABLE_SETTINGS_WITH_LIMITATION:
    case SystemInquiredType::HEAD_GESTURE_ON_OFF:
    case SystemInquiredType::HEAD_GESTURE_TRAINING:
        return true;
    }
    return false;
}

enum class Preset : uint8_t
{
    AMBIENT_SOUND_CONTROL = 0x00,
    VOLUME_CONTROL = 0x10,
    PLAYBACK_CONTROL = 0x20,
    TRACK_CONTROL = 0x21,
    PLAYBACK_CONTROL_VOICE_ASSISTANT_LIMITATION = 0x22,
    VOICE_RECOGNITION = 0x30,
    GOOGLE_ASSIST = 0x31,
    AMAZON_ALEXA = 0x32,
    TENCENT_XIAOWEI = 0x33,
    MS = 0x34,
    AMBIENT_SOUND_CONTROL_QUICK_ACCESS = 0x35,
    QUICK_ACCESS = 0x36,
    TENCENT_XIAOWEI_Q_MSC = 0x37,
    TEAMS = 0x38,
    GOOGLE_ASSISTANT_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION = 0x39,
    AMAZON_ALEXA_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION = 0x40,
    TENCENT_XIAOWEI_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION = 0x41,
    QUICK_ACCESS_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION = 0x42,
    AMBIENT_SOUND_CONTROL_QUICK_ACCESS_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION = 0x43,
    TENCENT_XIAOWEI_Q_MSC_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION = 0x44,
    AMBIENT_SOUND_CONTROL_MIC = 0x45,
    LISTENING_MODE_QUICK_ACCESS = 0x46,
    CHAT_MIX = 0x70,
    CUSTOM1 = 0x71,
    CUSTOM2 = 0x72,
    NO_FUNCTION = 0xFF,
};

inline bool Preset_isValidByteCode(uint8_t preset)
{
    switch (static_cast<Preset>(preset))
    {
    case Preset::AMBIENT_SOUND_CONTROL:
    case Preset::VOLUME_CONTROL:
    case Preset::PLAYBACK_CONTROL:
    case Preset::TRACK_CONTROL:
    case Preset::PLAYBACK_CONTROL_VOICE_ASSISTANT_LIMITATION:
    case Preset::VOICE_RECOGNITION:
    case Preset::GOOGLE_ASSIST:
    case Preset::AMAZON_ALEXA:
    case Preset::TENCENT_XIAOWEI:
    case Preset::MS:
    case Preset::AMBIENT_SOUND_CONTROL_QUICK_ACCESS:
    case Preset::QUICK_ACCESS:
    case Preset::TENCENT_XIAOWEI_Q_MSC:
    case Preset::TEAMS:
    case Preset::GOOGLE_ASSISTANT_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION:
    case Preset::AMAZON_ALEXA_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION:
    case Preset::TENCENT_XIAOWEI_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION:
    case Preset::QUICK_ACCESS_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION:
    case Preset::AMBIENT_SOUND_CONTROL_QUICK_ACCESS_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION:
    case Preset::TENCENT_XIAOWEI_Q_MSC_WITH_AVAILABLE_ONLY_ON_BT_CLASSIC_CONNECTION_CAUTION:
    case Preset::AMBIENT_SOUND_CONTROL_MIC:
    case Preset::LISTENING_MODE_QUICK_ACCESS:
    case Preset::CHAT_MIX:
    case Preset::CUSTOM1:
    case Preset::CUSTOM2:
    case Preset::NO_FUNCTION:
        return true;
    }
    return false;
}

enum class VoiceAssistant : uint8_t
{
    VOICE_RECOGNITION = 0x30,
    GOOGLE_ASSISTANT = 0x31,
    AMAZON_ALEXA = 0x32,
    TENCENT_XIAOWEI = 0x33,
    SONY_VOICE_ASSISTANT = 0x34,
    VOICE_ASSISTANT_ENABLED_IN_OTHER_DEVICE = 0x3F,
    NO_FUNCTION = 0xFF,
};

inline bool VoiceAssistant_isValidByteCode(uint8_t va)
{
    switch (static_cast<VoiceAssistant>(va))
    {
    case VoiceAssistant::VOICE_RECOGNITION:
    case VoiceAssistant::GOOGLE_ASSISTANT:
    case VoiceAssistant::AMAZON_ALEXA:
    case VoiceAssistant::TENCENT_XIAOWEI:
    case VoiceAssistant::SONY_VOICE_ASSISTANT:
    case VoiceAssistant::VOICE_ASSISTANT_ENABLED_IN_OTHER_DEVICE:
    case VoiceAssistant::NO_FUNCTION:
        return true;
    }
    return false;
}

enum class EarpieceFittingDetectionOperationStatus : uint8_t
{
    DETECTION_IS_NOT_STARTED = 0x00,
    DETECTION_STARTED = 0x01,
    DETECTION_COMPLETED_SUCCESSFULLY = 0x02,
    DETECTION_COMPLETED_UNSUCCESSFULLY = 0x03,
};

inline bool EarpieceFittingDetectionOperationStatus_isValidByteCode(uint8_t status)
{
    switch (static_cast<EarpieceFittingDetectionOperationStatus>(status))
    {
    case EarpieceFittingDetectionOperationStatus::DETECTION_IS_NOT_STARTED:
    case EarpieceFittingDetectionOperationStatus::DETECTION_STARTED:
    case EarpieceFittingDetectionOperationStatus::DETECTION_COMPLETED_SUCCESSFULLY:
    case EarpieceFittingDetectionOperationStatus::DETECTION_COMPLETED_UNSUCCESSFULLY:
        return true;
    }
    return false;
}

enum class EarpieceFittingDetectionOperationErrorCode : uint8_t
{
    NO_ERROR = 0x00,
    LEFT_CONNECTION_ERROR = 0x01,
    RIGHT_CONNECTION_ERROR = 0x02,
    FUNCTION_UNAVAILABLE_ERROR = 0x03,
    LEFT_FITTING_ERROR = 0x04,
    RIGHT_FITTING_ERROR = 0x05,
    BOTH_FITTING_ERROR = 0x06,
    MEASURING_ERROR = 0x07,
};

inline bool EarpieceFittingDetectionOperationErrorCode_isValidByteCode(uint8_t code)
{
    switch (static_cast<EarpieceFittingDetectionOperationErrorCode>(code))
    {
    case EarpieceFittingDetectionOperationErrorCode::NO_ERROR:
    case EarpieceFittingDetectionOperationErrorCode::LEFT_CONNECTION_ERROR:
    case EarpieceFittingDetectionOperationErrorCode::RIGHT_CONNECTION_ERROR:
    case EarpieceFittingDetectionOperationErrorCode::FUNCTION_UNAVAILABLE_ERROR:
    case EarpieceFittingDetectionOperationErrorCode::LEFT_FITTING_ERROR:
    case EarpieceFittingDetectionOperationErrorCode::RIGHT_FITTING_ERROR:
    case EarpieceFittingDetectionOperationErrorCode::BOTH_FITTING_ERROR:
    case EarpieceFittingDetectionOperationErrorCode::MEASURING_ERROR:
        return true;
    }
    return false;
}

enum class EarpieceSeries : uint8_t
{
    OTHER = 0x00,
    POLYURETHANE = 0x01,
    HYBRID = 0x02,
    SOFT_FITTING_FOR_LINKBUDS_FIT = 0x03,
    NOT_DETERMINED = 0xFF,
};

inline bool EarpieceSeries_isValidByteCode(uint8_t series)
{
    switch (static_cast<EarpieceSeries>(series))
    {
    case EarpieceSeries::OTHER:
    case EarpieceSeries::POLYURETHANE:
    case EarpieceSeries::HYBRID:
    case EarpieceSeries::SOFT_FITTING_FOR_LINKBUDS_FIT:
    case EarpieceSeries::NOT_DETERMINED:
        return true;
    }
    return false;
}

enum class EarpieceSize : uint8_t
{
    SS = 0x00,
    S = 0x01,
    M = 0x02,
    L = 0x03,
    LL = 0x04,
    NOT_DETERMINED = 0xFF,
};

inline bool EarpieceSize_isValidByteCode(uint8_t size)
{
    switch (static_cast<EarpieceSize>(size))
    {
    case EarpieceSize::SS:
    case EarpieceSize::S:
    case EarpieceSize::M:
    case EarpieceSize::L:
    case EarpieceSize::LL:
    case EarpieceSize::NOT_DETERMINED:
        return true;
    }
    return false;
}

enum class EarpieceFittingDetectionOperation : uint8_t
{
    DETECTION_START = 0x00,
    DETECTION_CANCEL = 0x01,
    DETECTION_START_FORCEFUL = 0x02,
};

inline bool EarpieceFittingDetectionOperation_isValidByteCode(uint8_t op)
{
    switch (static_cast<EarpieceFittingDetectionOperation>(op))
    {
    case EarpieceFittingDetectionOperation::DETECTION_START:
    case EarpieceFittingDetectionOperation::DETECTION_CANCEL:
    case EarpieceFittingDetectionOperation::DETECTION_START_FORCEFUL:
        return true;
    }
    return false;
}

enum class ResetType : uint8_t
{
    SETTINGS_ONLY = 0x00,
    FACTORY_RESET = 0x01,
};

inline bool ResetType_isValidByteCode(uint32_t type)
{
    switch (static_cast<ResetType>(type))
    {
    case ResetType::SETTINGS_ONLY:
    case ResetType::FACTORY_RESET:
        return true;
    }
    return false;
}

enum class ResetResult : uint8_t
{
    SUCCESS = 0x00,
    ERROR_CONNECTION_LEFT = 0x01,
    ERROR_CONNECTION_RIGHT = 0x02,
};

inline bool ResetResult_isValidByteCode(uint8_t result)
{
    switch (static_cast<ResetResult>(result))
    {
    case ResetResult::SUCCESS:
    case ResetResult::ERROR_CONNECTION_LEFT:
    case ResetResult::ERROR_CONNECTION_RIGHT:
        return true;
    }
    return false;
}

enum class FaceTapKey : uint8_t
{
    LEFT_SIDE_KEY = 0x00,
    RIGHT_SIDE_KEY = 0x01,
};

inline bool FaceTapKey_isValidByteCode(uint8_t key)
{
    switch (static_cast<FaceTapKey>(key))
    {
    case FaceTapKey::LEFT_SIDE_KEY:
    case FaceTapKey::RIGHT_SIDE_KEY:
        return true;
    }
    return false;
}

enum class FaceTapAction : uint8_t
{
    DOUBLE_TAP = 0x00,
    TRIPLE_TAP = 0x01,
};

inline bool FaceTapAction_isValidByteCode(uint8_t action)
{
    switch (static_cast<FaceTapAction>(action))
    {
    case FaceTapAction::DOUBLE_TAP:
    case FaceTapAction::TRIPLE_TAP:
        return true;
    }
    return false;
}

enum class HeadGestureAction : uint8_t
{
    NOD = 0x00,
    SWING = 0x01,
};

inline bool HeadGestureAction_isValidByteCode(uint8_t action)
{
    switch (static_cast<HeadGestureAction>(action))
    {
    case HeadGestureAction::NOD:
    case HeadGestureAction::SWING:
        return true;
    }
    return false;
}

// region SYSTEM_*_CAPABILITY

// Not implemented

// endregion SYSTEM_*_CAPABILITY

// region SYSTEM_*_STATUS

// Not implemented

// endregion SYSTEM_*_STATUS

// region SYSTEM_*_PARAM

// region SYSTEM_GET_PARAM

struct SystemGetParam : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::SYSTEM_RET_PARAM;

    SystemInquiredType type; // 0x1

    SystemGetParam(SystemInquiredType type)
        : Payload(Command::SYSTEM_GET_PARAM)
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(SystemGetParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::SYSTEM_GET_PARAM)
            && SystemInquiredType_isValidByteCode(buf[offsetof(SystemGetParam, type)]);
    }
};

// endregion SYSTEM_GET_PARAM

// region SYSTEM_RET_PARAM, SYSTEM_SET_PARAM, SYSTEM_NTFY_PARAM

struct SystemParam : Payload
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::SYSTEM_RET_PARAM,
        Command::SYSTEM_SET_PARAM,
        Command::SYSTEM_NTFY_PARAM,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::SYSTEM_NTFY_PARAM,
        Command::UNKNOWN,
    };

    SystemInquiredType type; // 0x1

    SystemParam(CommandType ct, SystemInquiredType type)
        : Payload(COMMAND_IDS[ct])
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return Payload::isValid(buf)
            && buf.size() >= sizeof(SystemParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(COMMAND_IDS[ct])
            && SystemInquiredType_isValidByteCode(buf[offsetof(SystemParam, type)]);
    }
};

// - VIBRATOR, PLAYBACK_CONTROL_BY_WEARING, VOICE_ASSISTANT_WAKE_WORD, AUTO_VOLUME, HEAD_GESTURE_ON_OFF

struct SystemParamCommon : SystemParam
{
    MessageMdrV2EnableDisable settingValue; // 0x2

    SystemParamCommon(CommandType ct, SystemInquiredType type, MessageMdrV2EnableDisable settingValue)
        : SystemParam(ct, type)
        , settingValue(settingValue)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return SystemParam::isValid(buf, ct)
            && buf.size() == sizeof(SystemParamCommon)
            && isValidInquiredType(static_cast<SystemInquiredType>(buf[offsetof(SystemParamCommon, type)]))
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(SystemParamCommon, settingValue)]);
    }

    static bool isValidInquiredType(SystemInquiredType type)
    {
        return type == SystemInquiredType::VIBRATOR
            || type == SystemInquiredType::PLAYBACK_CONTROL_BY_WEARING
            || type == SystemInquiredType::VOICE_ASSISTANT_WAKE_WORD
            || type == SystemInquiredType::AUTO_VOLUME
            || type == SystemInquiredType::HEAD_GESTURE_ON_OFF;
    }
};

// - SMART_TALKING_MODE_TYPE1, SMART_TALKING_MODE_TYPE2

struct SystemParamSmartTalking : SystemParam
{
    MessageMdrV2EnableDisable onOffValue; // 0x2
    MessageMdrV2EnableDisable previewModeOnOffValue; // 0x3

    SystemParamSmartTalking(
        CommandType ct, SystemInquiredType type, MessageMdrV2EnableDisable onOffValue,
        MessageMdrV2EnableDisable previewModeOnOffValue
    )
        : SystemParam(ct, type)
        , onOffValue(onOffValue)
        , previewModeOnOffValue(previewModeOnOffValue)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return SystemParam::isValid(buf, ct)
            && buf.size() == sizeof(SystemParamSmartTalking)
            && isValidInquiredType(static_cast<SystemInquiredType>(buf[offsetof(SystemParamSmartTalking, type)]))
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(SystemParamSmartTalking, onOffValue)])
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(SystemParamSmartTalking, previewModeOnOffValue)]);
    }

    static bool isValidInquiredType(SystemInquiredType type)
    {
        return type == SystemInquiredType::SMART_TALKING_MODE_TYPE1
            || type == SystemInquiredType::SMART_TALKING_MODE_TYPE2;
    }
};

// - ASSIGNABLE_SETTINGS

struct SystemParamAssignableSettings : SystemParam
{
    uint8_t numberOfPreset; // 0x2
    Preset presets[]; // 0x3-

private:
    SystemParamAssignableSettings(CommandType ct, const std::span<const Preset>& presets)
        : SystemParam(ct, SystemInquiredType::ASSIGNABLE_SETTINGS)
        , numberOfPreset(presets.size())
    {
        std::memcpy(this->presets, presets.data(), sizeof(Preset) * numberOfPreset);
    }

public:
    VARIABLE_SIZE_PAYLOAD_ONE_ARRAY_AT_END(Preset, 255);

    std::span<const Preset> getPresets() const
    {
        return { presets, numberOfPreset };
    }

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return SystemParam::isValid(buf, ct)
            && buf[offsetof(SystemParamAssignableSettings, type)] == static_cast<uint8_t>(SystemInquiredType::ASSIGNABLE_SETTINGS)
            && buf.size() >= sizeof(SystemParamAssignableSettings)
            && buf.size() == sizeof(SystemParamAssignableSettings) + sizeof(Preset) * buf[offsetof(SystemParamAssignableSettings, numberOfPreset)];
        // Not validating every entry for simplicity
    }
};

// - VOICE_ASSISTANT_SETTINGS

struct SystemParamVoiceAssistantSettings : SystemParam
{
    VoiceAssistant voiceAssistant; // 0x2

    SystemParamVoiceAssistantSettings(CommandType ct, VoiceAssistant voiceAssistant)
        : SystemParam(ct, SystemInquiredType::VOICE_ASSISTANT_SETTINGS)
        , voiceAssistant(voiceAssistant)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return SystemParam::isValid(buf, ct)
            && buf.size() == sizeof(SystemParamVoiceAssistantSettings)
            && buf[offsetof(SystemParamVoiceAssistantSettings, type)] == static_cast<uint8_t>(SystemInquiredType::VOICE_ASSISTANT_SETTINGS)
            && VoiceAssistant_isValidByteCode(buf[offsetof(SystemParamVoiceAssistantSettings, voiceAssistant)]);
    }
};

// - WEARING_STATUS_DETECTOR

struct SystemParamWearingStatusDetector : SystemParam
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::SYSTEM_RET_PARAM,
        Command::UNKNOWN,
        Command::SYSTEM_NTFY_PARAM,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
    };

    EarpieceFittingDetectionOperationStatus operationStatus; // 0x2
    EarpieceFittingDetectionOperationErrorCode errorCode; // 0x3
    uint8_t numOfSelectedEarpieces; // 0x4
    uint8_t indexOfCurrentDetection; // 0x5
    EarpieceSeries currentDetectingSeries; // 0x6
    EarpieceSize earpieceSize; // 0x7

    SystemParamWearingStatusDetector(
        CommandType ct, EarpieceFittingDetectionOperationStatus operationStatus,
        EarpieceFittingDetectionOperationErrorCode errorCode, uint8_t numOfSelectedEarpieces,
        uint8_t indexOfCurrentDetection, EarpieceSeries currentDetectingSeries, EarpieceSize earpieceSize
    )
        : SystemParam(ct, SystemInquiredType::WEARING_STATUS_DETECTOR)
        , operationStatus(operationStatus)
        , errorCode(errorCode)
        , numOfSelectedEarpieces(numOfSelectedEarpieces)
        , indexOfCurrentDetection(indexOfCurrentDetection)
        , currentDetectingSeries(currentDetectingSeries)
        , earpieceSize(earpieceSize)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return SystemParam::isValid(buf, ct)
            && buf.size() == sizeof(SystemParamWearingStatusDetector)
            && buf[offsetof(SystemParamWearingStatusDetector, type)] == static_cast<uint8_t>(SystemInquiredType::WEARING_STATUS_DETECTOR)
            && EarpieceFittingDetectionOperationStatus_isValidByteCode(buf[offsetof(SystemParamWearingStatusDetector, operationStatus)])
            && EarpieceFittingDetectionOperationErrorCode_isValidByteCode(buf[offsetof(SystemParamWearingStatusDetector, errorCode)])
            && EarpieceSeries_isValidByteCode(buf[offsetof(SystemParamWearingStatusDetector, currentDetectingSeries)])
            && EarpieceSize_isValidByteCode(buf[offsetof(SystemParamWearingStatusDetector, earpieceSize)]);
    }
};

// - EARPIECE_SELECTION

struct SystemParamEarpieceSelection : SystemParam
{
    EarpieceSeries series; // 0x2

    SystemParamEarpieceSelection(CommandType ct, EarpieceSeries series)
        : SystemParam(ct, SystemInquiredType::EARPIECE_SELECTION)
        , series(series)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return SystemParam::isValid(buf, ct)
            && buf.size() == sizeof(SystemParamEarpieceSelection)
            && buf[offsetof(SystemParamEarpieceSelection, type)] == static_cast<uint8_t>(SystemInquiredType::EARPIECE_SELECTION)
            && EarpieceSeries_isValidByteCode(buf[offsetof(SystemParamEarpieceSelection, series)]);
    }
};

// - CALL_SETTINGS

struct SystemParamCallSettings : SystemParam
{
    MessageMdrV2EnableDisable selfVoiceOnOff; // 0x2
    uint8_t selfVoiceVolume; // 0x3
    uint8_t callVoiceVolume; // 0x4

    SystemParamCallSettings(
        CommandType ct, MessageMdrV2EnableDisable selfVoiceOnOff, uint8_t selfVoiceVolume, uint8_t callVoiceVolume
    )
        : SystemParam(ct, SystemInquiredType::CALL_SETTINGS)
        , selfVoiceOnOff(selfVoiceOnOff)
        , selfVoiceVolume(selfVoiceVolume)
        , callVoiceVolume(callVoiceVolume)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return SystemParam::isValid(buf, ct)
            && buf.size() == sizeof(SystemParamCallSettings)
            && buf[offsetof(SystemParamCallSettings, type)] == static_cast<uint8_t>(SystemInquiredType::CALL_SETTINGS)
            && MessageMdrV2EnableDisable_isValidByteCode(buf[offsetof(SystemParamCallSettings, selfVoiceOnOff)]);
    }
};

// - QUICK_ACCESS

// Not implemented, variable size

// - ASSIGNABLE_SETTINGS_WITH_LIMITATION

struct SystemParamAssignableSettingsWithLimit : SystemParam
{
    uint8_t numberOfPreset; // 0x2
    Preset presets[]; // 0x3-

private:
    SystemParamAssignableSettingsWithLimit(CommandType ct, const std::span<const Preset>& presets)
        : SystemParam(ct, SystemInquiredType::ASSIGNABLE_SETTINGS_WITH_LIMITATION)
        , numberOfPreset(presets.size())
    {
        std::memcpy(this->presets, presets.data(), sizeof(Preset) * numberOfPreset);
    }

public:
    VARIABLE_SIZE_PAYLOAD_ONE_ARRAY_AT_END(Preset, 255);

    std::span<const Preset> getPresets() const
    {
        return { presets, numberOfPreset };
    }

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return SystemParam::isValid(buf, ct)
            && buf[offsetof(SystemParamAssignableSettingsWithLimit, type)] == static_cast<uint8_t>(SystemInquiredType::ASSIGNABLE_SETTINGS_WITH_LIMITATION)
            && buf.size() >= sizeof(SystemParamAssignableSettingsWithLimit)
            && buf.size() == sizeof(SystemParamAssignableSettingsWithLimit) + sizeof(Preset) * buf[offsetof(SystemParamAssignableSettingsWithLimit, numberOfPreset)];
        // Not validating every entry for simplicity
    }
};

// - HEAD_GESTURE_TRAINING

struct SystemParamHeadGestureTraining : SystemParam
{
    HeadGestureAction headGestureAction; // 0x2

    SystemParamHeadGestureTraining(CommandType ct, HeadGestureAction headGestureAction)
        : SystemParam(ct, SystemInquiredType::HEAD_GESTURE_TRAINING)
        , headGestureAction(headGestureAction)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return SystemParam::isValid(buf, ct)
            && buf.size() == sizeof(SystemParamHeadGestureTraining)
            && buf[offsetof(SystemParamHeadGestureTraining, type)] == static_cast<uint8_t>(SystemInquiredType::HEAD_GESTURE_TRAINING)
            && HeadGestureAction_isValidByteCode(buf[offsetof(SystemParamHeadGestureTraining, headGestureAction)]);
    }
};

// endregion SYSTEM_RET_PARAM, SYSTEM_SET_PARAM, SYSTEM_NTFY_PARAM

// region SYSTEM_SET_PARAM

// - WEARING_STATUS_DETECTOR

struct SystemSetParamWearingStatusDetector : SystemParam
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::SYSTEM_SET_PARAM,
        Command::UNKNOWN,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::SYSTEM_NTFY_PARAM,
        Command::UNKNOWN,
    };

    EarpieceFittingDetectionOperation operation; // 0x2
    uint8_t indexOfCurrentDetection; // 0x3
    EarpieceSeries currentDetectionSeries; // 0x4
    EarpieceSize currentDetectionSize; // 0x5

    SystemSetParamWearingStatusDetector(
        CommandType ct, EarpieceFittingDetectionOperation operation, uint8_t indexOfCurrentDetection,
        EarpieceSeries currentDetectionSeries, EarpieceSize currentDetectionSize
    )
        : SystemParam(ct, SystemInquiredType::WEARING_STATUS_DETECTOR)
        , operation(operation)
        , indexOfCurrentDetection(indexOfCurrentDetection)
        , currentDetectionSeries(currentDetectionSeries)
        , currentDetectionSize(currentDetectionSize)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return SystemParam::isValid(buf, ct)
            && buf.size() == sizeof(SystemSetParamWearingStatusDetector)
            && buf[offsetof(SystemSetParamWearingStatusDetector, type)] == static_cast<uint8_t>(SystemInquiredType::WEARING_STATUS_DETECTOR)
            && EarpieceFittingDetectionOperation_isValidByteCode(buf[offsetof(SystemSetParamWearingStatusDetector, operation)])
            && EarpieceSeries_isValidByteCode(buf[offsetof(SystemSetParamWearingStatusDetector, currentDetectionSeries)])
            && EarpieceSize_isValidByteCode(buf[offsetof(SystemSetParamWearingStatusDetector, currentDetectionSize)]);
    }
};

// - RESET_SETTINGS

struct SystemSetParamResetSettings : SystemParam
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::SYSTEM_SET_PARAM,
        Command::UNKNOWN,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::SYSTEM_NTFY_PARAM,
        Command::UNKNOWN,
    };

    ResetType resetType; // 0x2

    SystemSetParamResetSettings(CommandType ct, ResetType resetType)
        : SystemParam(ct, SystemInquiredType::RESET_SETTINGS)
        , resetType(resetType)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return SystemParam::isValid(buf, ct)
            && buf.size() == sizeof(SystemSetParamResetSettings)
            && buf[offsetof(SystemSetParamResetSettings, type)] == static_cast<uint8_t>(SystemInquiredType::RESET_SETTINGS)
            && ResetType_isValidByteCode(buf[offsetof(SystemSetParamResetSettings, resetType)]);
    }
};

// endregion SYSTEM_SET_PARAM

// region SYSTEM_NTFY_PARAM

// - RESET_SETTINGS

struct SystemNotifyParamResetSettings : SystemParam
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::SYSTEM_NTFY_PARAM,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
    };

    ResetResult resetResult; // 0x2

    SystemNotifyParamResetSettings(CommandType ct, ResetResult resetResult)
        : SystemParam(ct, SystemInquiredType::RESET_SETTINGS)
        , resetResult(resetResult)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return SystemParam::isValid(buf, ct)
            && buf.size() == sizeof(SystemNotifyParamResetSettings)
            && buf[offsetof(SystemNotifyParamResetSettings, type)] == static_cast<uint8_t>(SystemInquiredType::RESET_SETTINGS)
            && ResetResult_isValidByteCode(buf[offsetof(SystemNotifyParamResetSettings, resetResult)]);
    }
};

// - FACE_TAP_TEST_MODE

struct SystemNotifyParamFaceTapTestMode : SystemParam
{
    static constexpr Command COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::SYSTEM_NTFY_PARAM,
    };
    static constexpr Command RESPONSE_COMMAND_IDS[] = {
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
        Command::UNKNOWN,
    };

    FaceTapKey key; // 0x2
    FaceTapAction action; // 0x3

    SystemNotifyParamFaceTapTestMode(CommandType ct, FaceTapKey key, FaceTapAction action)
        : SystemParam(ct, SystemInquiredType::FACE_TAP_TEST_MODE)
        , key(key)
        , action(action)
    {}

    static bool isValid(const std::span<const uint8_t>& buf, CommandType ct)
    {
        return SystemParam::isValid(buf, ct)
            && buf.size() == sizeof(SystemNotifyParamFaceTapTestMode)
            && buf[offsetof(SystemNotifyParamFaceTapTestMode, type)] == static_cast<uint8_t>(SystemInquiredType::FACE_TAP_TEST_MODE)
            && FaceTapKey_isValidByteCode(buf[offsetof(SystemNotifyParamFaceTapTestMode, key)])
            && FaceTapAction_isValidByteCode(buf[offsetof(SystemNotifyParamFaceTapTestMode, action)]);
    }
};

// endregion SYSTEM_NTFY_PARAM

// Not implemented

// endregion SYSTEM_*_PARAM

// region SYSTEM_*_EXT_PARAM

// region SYSTEM_GET_EXT_PARAM

struct SystemGetExtParam : Payload
{
    static constexpr Command RESPONSE_COMMAND_ID = Command::SYSTEM_RET_EXT_PARAM;

    SystemInquiredType type; // 0x1

    SystemGetExtParam(SystemInquiredType type)
        : Payload(Command::SYSTEM_GET_EXT_PARAM)
        , type(type)
    {}

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf.size() == sizeof(SystemGetExtParam)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::SYSTEM_GET_EXT_PARAM)
            && SystemInquiredType_isValidByteCode(buf[offsetof(SystemGetExtParam, type)]);
    }
};

// endregion SYSTEM_GET_EXT_PARAM

// Not implemented

// endregion SYSTEM_*_EXT_PARAM

// endregion SYSTEM

} // namespace THMSGV2T1

#pragma pack(pop)
