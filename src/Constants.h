#pragma once

#include <vector>
#include <ranges>
#include <cstdint>

inline constexpr auto MAX_BLUETOOTH_MESSAGE_SIZE = 2048;
inline constexpr char START_MARKER{ 62 };
inline constexpr char END_MARKER{ 60 };

inline constexpr auto MAC_ADDR_STR_SIZE = 17;

/*
from `spdtool browse [device mac]`

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
#define APP_CONFIG_NAME "sonyheadphonesclient.toml"
using Buffer = std::vector<uint8_t>;

enum class DATA_TYPE : uint8_t
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

enum class NC_ASM_EFFECT : uint8_t
{
	OFF = 0,
	ON = 1
};

enum class NC_ASM_SETTING_TYPE : uint8_t
{
	NOISE_CANCELLING = 0,
	AMBIENT_SOUND = 1
};

enum class ASM_ID : uint8_t
{
	NORMAL = 0,
	VOICE = 1
};

// https://github.com/Freeyourgadget/Gadgetbridge/blob/master/app/src/main/java/nodomain/freeyourgadget/gadgetbridge/service/devices/sony/headphones/protocol/impl/v1/PayloadTypeV1.java
enum class COMMAND_TYPE : uint8_t
{
	INIT_REQUEST = 0x00,
	INIT_RESPONSE = 0x01,

	BATTERY_LEVEL_GET = 0x22,
	BATTERY_LEVEL_RET = 0x23,

	POWER_OFF = 0x24,

	CONNECTED_DEVIECES_GET = 0x36,
	CONNECTED_DEVIECES_RET = 0x37,
	CONNECTED_DEVIECES_SET = 0x38,
	CONNECTED_DEVIECES_NOTIFY = 0x39,

	MULTIPOINT_DEVICE_GET = 0x3A,
	MULTIPOINT_DEVICE_RET = 0x3B,
	MULTIPOINT_DEVICE_SET = 0x3C,
	MULTIPOINT_DEVICE_NOTIFY = 0x3D,

	VOICEGUIDANCE_PARAM_GET = 0x46,
	VOICEGUIDANCE_PARAM_RET = 0x47,
	VOICEGUIDANCE_PARAM_SET = 0x48,
	VOICEGUIDANCE_PARAM_NOTIFY = 0x49,

	EQUALIZER_GET = 0x56,
	EQUALIZER_RET = 0x57,
	EQUALIZER_SET = 0x58,
	EQUALIZER_NOTIFY = 0x59,

	PLAYBACK_SND_PRESSURE_GET = 0x5a,
	PLAYBACK_SND_PRESSURE_RET = 0x5b,

	NCASM_PARAM_GET = 0x66,
	NCASM_PARAM_RET = 0x67,
	NCASM_PARAM_SET = 0x68,
	NCASM_PARAM_NOTIFY = 0x69,

	PLAYBACK_STATUS_CONTROL_GET = 0xa2,
	PLAYBACK_STATUS_CONTROL_RET = 0xa3,
	PLAYBACK_STATUS_CONTROL_SET = 0xa4,
	PLAYBACK_STATUS_CONTROL_NOTIFY = 0xa5,

	MISC_DATA_GET = 0xc4,
	MISC_DATA_RET = 0xc9,

	MULTIPOINT_ETC_ENABLE_GET = 0xd6,
	MULTIPOINT_ETC_ENABLE_RET = 0xd7,
	MULTIPOINT_ETC_ENABLE_SET = 0xd8,
	MULTIPOINT_ETC_ENABLE_NOITIFY = 0xd9,

	MULTIPOINT_ENABLE_GET_2 = 0x96,
	MULTIPOINT_ENABLE_RET_2 = 0x97,
	MULTIPOINT_ENABLE_SET_2 = 0x98,
	MULTIPOINT_ENABLE_NOITIFY_2 = 0x99,

	PLAYBACK_STATUS_GET = 0xa6,
	PLAYBACK_STATUS_RET = 0xa7,
	PLAYBACK_STATUS_SET = 0xa8,
	PLAYBACK_STATUS_NOTIFY = 0xa9,

	AUTOMATIC_POWER_OFF_BUTTON_MODE_GET = 0xf6,
	AUTOMATIC_POWER_OFF_BUTTON_MODE_RET = 0xf7,
	AUTOMATIC_POWER_OFF_BUTTON_MODE_SET = 0xf8,
	AUTOMATIC_POWER_OFF_BUTTON_MODE_NOTIFY = 0xf9,

	SPEAK_TO_CHAT_GET = 0xfa,
	SPEAK_TO_CHAT_RET = 0xfb,
	SPEAK_TO_CHAT_SET = 0xfc,
	SPEAK_TO_CHAT_NOTIFY = 0xfd,

	UNKNOWN = 0xFF
};

enum class PLAYBACK_CONTROL : uint8_t
{
	NONE = 0,
	PAUSE = 1,
	NEXT = 2,
	PREV = 3,
	PLAY = 7
};

enum class PLAYBACK_CONTROL_RESPONSE : uint8_t
{
	PLAY = 1,
	PAUSE = 2
};

enum class TOUCH_SENSOR_FUNCTION : uint8_t
{
	PLAYBACK_CONTROL = 0x20,
	AMBIENT_NC_CONTROL = 0x35,
	NOT_ASSIGNED = 0xff,
	
	NUM_FUNCTIONS = 3
};

// https://github.com/Plutoberth/SonyHeadphonesClient/commit/66d8e52aad4ffd08aa78e811a23f67a5bad07d9a
enum class EQ_PRESET_ID: uint8_t {
	OFF = 0,
	ROCK = 1,
	POP = 2,
	JAZZ = 3,
	DANCE = 4,
	EDM = 5,
	R_AND_B_HIP_HOP = 6,
	ACOUSTIC = 7,
	/*RESERVED_FOR_FUTURE_NO8 = 8,… */
	/*RESERVED_FOR_FUTURE_NO15 = 15,*/
	BRIGHT = 16,
	EXCITED = 17,
	MELLOW = 18,
	RELAXED = 19,
	VOCAL = 20,
	TREBLE = 21,
	BASS = 22,
	SPEECH = 23,
	/*RESERVED_FOR_FUTURE_NO24 = 24,… */
	/*RESERVED_FOR_FUTURE_NO31 = 31,*/
	CUSTOM = 0xa0,
	USER_SETTING1 = 0xa1,
	USER_SETTING2 = 0xa2,
	USER_SETTING3 = 0xa3,
	USER_SETTING4 = 0xa4,
	USER_SETTING5 = 0xa5,
	UNSPECIFIED = 0xff,
};
