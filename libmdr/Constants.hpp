#pragma once

#include <vector>
#include <ranges>
#include <cstdint>

#define HEADPHONES_DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) \
extern "C++" { \
inline constexpr ENUMTYPE operator | (ENUMTYPE a, ENUMTYPE b) noexcept { return ENUMTYPE(((std::underlying_type_t<ENUMTYPE>)a) | ((std::underlying_type_t<ENUMTYPE>)b)); } \
inline ENUMTYPE &operator |= (ENUMTYPE &a, ENUMTYPE b) noexcept { return (ENUMTYPE &)(((std::underlying_type_t<ENUMTYPE> &)a) |= ((std::underlying_type_t<ENUMTYPE>)b)); } \
inline constexpr ENUMTYPE operator & (ENUMTYPE a, ENUMTYPE b) noexcept { return ENUMTYPE(((std::underlying_type_t<ENUMTYPE>)a) & ((std::underlying_type_t<ENUMTYPE>)b)); } \
inline ENUMTYPE &operator &= (ENUMTYPE &a, ENUMTYPE b) noexcept { return (ENUMTYPE &)(((std::underlying_type_t<ENUMTYPE> &)a) &= ((std::underlying_type_t<ENUMTYPE>)b)); } \
inline constexpr ENUMTYPE operator ~ (ENUMTYPE a) noexcept { return ENUMTYPE(~((std::underlying_type_t<ENUMTYPE>)a)); } \
inline constexpr ENUMTYPE operator ^ (ENUMTYPE a, ENUMTYPE b) noexcept { return ENUMTYPE(((std::underlying_type_t<ENUMTYPE>)a) ^ ((std::underlying_type_t<ENUMTYPE>)b)); } \
inline ENUMTYPE &operator ^= (ENUMTYPE &a, ENUMTYPE b) noexcept { return (ENUMTYPE &)(((std::underlying_type_t<ENUMTYPE> &)a) ^= ((std::underlying_type_t<ENUMTYPE>)b)); } \
}

inline constexpr auto MAX_BLUETOOTH_MESSAGE_SIZE = 2048;
inline constexpr char START_MARKER{ 62 };
inline constexpr char END_MARKER{ 60 };

inline constexpr auto MAC_ADDR_STR_SIZE = 17;

/*
from `sdptool browse [device mac]`

Service Name: Serial HPC
Service RecHandle: 0x20000000
Service Class ID List:
  UUID 128: 956c7b26-d49a-4ba8-b03f-b17d393cb6e2
Protocol Descriptor List:
  "L2CAP" (0x0100)
  "RFCOMM" (0x0003)
	Channel: 9
Language Base Attr List:
  code_ISO639: 0x656e
  encoding:    0x6a
  base_offset: 0x100
Profile Descriptor List:
  "Serial Port" (0x1101)
	Version: 0x0102
 */

inline constexpr auto SERVICE_UUID = "956C7B26-D49A-4BA8-B03F-B17D393CB6E2";
inline uint8_t SERVICE_UUID_IN_BYTES[] = { // this is the SERVICE_UUID but in bytes
		0x95, 0x6C, 0x7B, 0x26, 0xD4, 0x9A, 0x4B, 0xA8, 0xB0, 0x3F, 0xB1, 0x7D, 0x39, 0x3C, 0xB6, 0xE2
};

#define APP_NAME "Sony Headphones Client v" __HEADPHONES_APP_VERSION__
#define APP_NAME_W (L"" APP_NAME)
#define APP_CONFIG_ENV_KEY "SONYHEADPHONESCLIENT_CONFIG_PATH"
#define APP_CONFIG_NAME "sonyheadphonesclient.toml"
using Buffer = std::vector<uint8_t>;
using BufferSpan = std::span<const uint8_t>;

enum CommandType
{
	CT_Get, CT_Ret, CT_Set, CT_Notify
};


enum class DataType : uint8_t
{
    DATA = 0,
    ACK = 1,
    DATA_MC_NO1 = 2,
    DATA_ICD = 9,
    DATA_EV = 10,
    DATA_MDR = 12,
    DATA_COMMON = 13,
    DATA_MDR_NO2 = 14,
    SHOT =  16,
    SHOT_MC_NO1 =  18,
    SHOT_ICD =  25,
    SHOT_EV =  26,
    SHOT_MDR =  28,
    SHOT_COMMON =  29,
    SHOT_MDR_NO2 = 30,
    LARGE_DATA_COMMON =  45,
    UNKNOWN = 0xff
};

enum class ModelColor : uint8_t
{
    Default = 0,
    Black = 1,
    White = 2,
    Silver = 3,
    Red = 4,
    Blue = 5,
    Pink = 6,
    Yellow = 7,
    Green = 8,
    Gray = 9,
    Gold = 10,
    Cream = 11,
    Orange = 12,
    Brown = 13,
    Violet = 14,
};