#pragma once

#include <vector>

inline constexpr auto MAX_BLUETOOTH_MESSAGE_SIZE = 2048;
inline constexpr char START_MARKER{ 62 };
inline constexpr char END_MARKER{ 60 };

inline constexpr auto MAC_ADDR_STR_SIZE = 17;

inline constexpr auto SERVICE_UUID = "96CC203E-5068-46ad-B32D-E316F5E069BA";
inline unsigned char SERVICE_UUID_IN_BYTES[] = { // this is the SERVICE_UUID but in bytes
	0x96, 0xcc, 0x20, 0x3e, 0x50, 0x68, 0x46, 0xad,
	0xb3, 0x2d, 0xe3, 0x16, 0xf5, 0xe0, 0x69, 0xba
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

	CONNECTED_DEVIECES_GET = 0x36,
	CONNECTED_DEVIECES_RET = 0x37,

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
