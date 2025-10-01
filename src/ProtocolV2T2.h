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

    static constexpr bool VARIABLE_SIZE = false;
};

// === CONNECT ===

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
    static constexpr size_t FUNCTIONS_FIRST_INDEX = 3;

    ConnectInquiredType inquiredType; // 0x1
    uint8_t numberOfFunction; // 0x2
    // Support function data follows, each of type MessageMdrV2SupportFunction (2 bytes)

private:
    ConnectRetSupportFunction(
        ConnectInquiredType inquiredType,
        const std::vector<MessageMdrV2SupportFunction>& supportFunctions
    )
        : Payload(Command::CONNECT_RET_SUPPORT_FUNCTION)
        , inquiredType(inquiredType)
    {
        setSupportFunctions(supportFunctions);
    }

public:
    DEFINE_EXTRA_SIZE_NEW_DELETE();

    std::span<const MessageMdrV2SupportFunction> getSupportFunctions() const
    {
        const uint8_t* rawData = reinterpret_cast<const uint8_t*>(this);
        const MessageMdrV2SupportFunction* supportFunctionsBegin =
            reinterpret_cast<const MessageMdrV2SupportFunction*>(&rawData[FUNCTIONS_FIRST_INDEX]);
        return { supportFunctionsBegin, numberOfFunction };
    }

private:
    // Must be called during construction
    void setSupportFunctions(const std::vector<MessageMdrV2SupportFunction>& functions)
    {
        uint8_t* rawData = reinterpret_cast<uint8_t*>(this);
        numberOfFunction = functions.size();
        memcpy(&rawData[FUNCTIONS_FIRST_INDEX], functions.data(), sizeof(MessageMdrV2SupportFunction) * numberOfFunction);
    }

public:
    static std::unique_ptr<ConnectRetSupportFunction> create(
        size_t* outSize, ConnectInquiredType inquiredType, const std::vector<MessageMdrV2SupportFunction>& functions)
    {
        *outSize = calculateSize(functions);
        size_t extra = *outSize - sizeof(ConnectRetSupportFunction);
        return std::unique_ptr<ConnectRetSupportFunction>(
            new(extra) ConnectRetSupportFunction(inquiredType, functions));
    }

    static bool isValid(const std::span<const uint8_t>& buf)
    {
        return Payload::isValid(buf)
            && buf[offsetof(Payload, command)] == static_cast<uint8_t>(Command::CONNECT_RET_SUPPORT_FUNCTION)
            && buf[offsetof(ConnectRetSupportFunction, inquiredType)] == static_cast<uint8_t>(ConnectInquiredType::FIXED_VALUE)
            && buf.size() >= sizeof(ConnectRetSupportFunction)
            && buf.size() == sizeof(ConnectRetSupportFunction) + sizeof(MessageMdrV2SupportFunction) * buf[offsetof(ConnectRetSupportFunction, numberOfFunction)];
    }

    static size_t calculateSize(const std::vector<MessageMdrV2SupportFunction>& functions)
    {
        return sizeof(ConnectRetSupportFunction) + sizeof(MessageMdrV2SupportFunction) * functions.size();
    }

    static constexpr bool VARIABLE_SIZE = true;
};

} // namespace THMSGV2T2

#pragma pack(pop)
