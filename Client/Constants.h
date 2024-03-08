#pragma once

#include <vector>

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
inline unsigned char SERVICE_UUID_IN_BYTES[] = { // this is the SERVICE_UUID but in bytes
        0x95, 0x6C, 0x7B, 0x26, 0xD4, 0x9A, 0x4B, 0xA8, 0xB0, 0x3F, 0xB1, 0x7D, 0x39, 0x3C, 0xB6, 0xE2
};

#define APP_NAME "Sony Headphones App v" __HEADPHONES_APP_VERSION__
#define APP_NAME_W (L"" APP_NAME)

using Buffer = std::vector<char>;

enum class DATA_TYPE : signed char
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
    UNKNOWN = -1
};


enum class NC_ASM_INQUIRED_TYPE : signed char
{
	NO_USE = 0,
	NOISE_CANCELLING = 1,
	NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE = 2,
	AMBIENT_SOUND_MODE = 3
};

enum class NC_ASM_EFFECT : signed char
{
	OFF = 0,
	ON = 1
};

enum class NC_ASM_SETTING_TYPE : signed char
{
	NOISE_CANCELLING = 0,
	AMBIENT_SOUND = 1
};

enum class ASM_ID : signed char
{
	NORMAL = 0,
	VOICE = 1
};

enum class NC_DUAL_SINGLE_VALUE : signed char
{
	OFF = 0,
	SINGLE = 1,
	DUAL = 2
};

// https://github.com/Freeyourgadget/Gadgetbridge/blob/master/app/src/main/java/nodomain/freeyourgadget/gadgetbridge/service/devices/sony/headphones/protocol/impl/v1/PayloadTypeV1.java
enum class COMMAND_TYPE : unsigned char
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

	MULTIPOINT_ENABLE_GET = 0xd6,
	MULTIPOINT_ENABLE_RET = 0xd7,
	MULTIPOINT_ENABLE_SET = 0xd8,
	MULTIPOINT_ENABLE_NOITIFY = 0xd9,

	MULTIPOINT_ENABLE_GET_2 = 0x96,
	MULTIPOINT_ENABLE_RET_2 = 0x97,
	MULTIPOINT_ENABLE_SET_2 = 0x98,
	MULTIPOINT_ENABLE_NOITIFY_2 = 0x99,


	PLAYBACK_STATUS_GET = 0xa6,
	PLAYBACK_STATUS_RET = 0xa7,
	PLAYBACK_STATUS_SET = 0xa8,
	PLAYBACK_STATUS_NOTIFY = 0xa9,

	UNKNOWN = 0xFF
};

enum class VPT_PRESET_ID : signed char
{
	OFF = 0,
	OUTDOOR_FESTIVAL = 1,
	ARENA = 2,
	CONCERT_HALL = 3,
	CLUB = 4
	//Note: Sony reserved 5~15 "for future"
};

enum class SOUND_POSITION_PRESET : signed char
{
	OFF = 0,
	FRONT_LEFT = 1,
	FRONT_RIGHT = 2,
	FRONT = 3,
	REAR_LEFT = 17,
	REAR_RIGHT = 18,
	OUT_OF_RANGE = -1
};

//Needed for converting the ImGui Combo index into the VPT index.
inline const SOUND_POSITION_PRESET SOUND_POSITION_PRESET_ARRAY[] = {
	SOUND_POSITION_PRESET::OFF,
	SOUND_POSITION_PRESET::FRONT_LEFT,
	SOUND_POSITION_PRESET::FRONT_RIGHT,
	SOUND_POSITION_PRESET::FRONT,
	SOUND_POSITION_PRESET::REAR_LEFT,
	SOUND_POSITION_PRESET::REAR_RIGHT,
	SOUND_POSITION_PRESET::OUT_OF_RANGE
};

enum class VPT_INQUIRED_TYPE : signed char
{
	NO_USE = 0,
	VPT = 1,
	SOUND_POSITION = 2,
	OUT_OF_RANGE = -1
};

enum class PLAYBACK_CONTROL : signed char
{
	NONE = 0,
	PAUSE = 1,
	NEXT = 2,
	PREV = 3,
	PLAY = 7
};

enum class PLAYBACK_CONTROL_RESPONSE : signed char
{
	PLAY = 1,
	PAUSE = 2
};