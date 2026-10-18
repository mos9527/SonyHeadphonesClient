#pragma once
#include "ProtocolV2.hpp"
#include "ProtocolV2T1.hpp"
#pragma pack(push)
#pragma pack()

// Generated from Sound Connect iOS J2ObjC metadata. Do not edit by hand.
namespace mdr::v2::t2
{
#pragma region Enums
    enum class AutoStandbyCandidateElement : UInt8
    {
        AUTO_STANDBY_IN_15_MIN = 0x00,
        OUT_OF_RANGE = 0xFF,
    };

    enum class ColorElement : UInt8
    {
        RGB_FULL_COLOR = 0x00,
        OUT_OF_RANGE = 0xFF,
    };

    enum class Command : UInt8
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
        UNKNOWN = 0xFF,
    };

    enum class ConnectInquiredType : UInt8
    {
        FIXED_VALUE = 0x00,
        OUT_OF_RANGE = 0xFF,
    };

    enum class ConnectionMode : UInt8
    {
        HIGH_RELIABILITY = 0x00,
        LOW_LATENCY = 0x01,
        LOW_LATENCY_WITH_EQ_RESTRICTION = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class ConnectionResult : UInt8
    {
        SUCCESS = 0x00,
        ERROR_CONNECTION_LEFT = 0x01,
        ERROR_CONNECTION_RIGHT = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class ConnectionState : UInt8
    {
        CONNECTING = 0x00,
        CONNECTED = 0x01,
        DISCONNECTING = 0x02,
        DISCONNECTED = 0x03,
        OUT_OF_RANGE = 0xFF,
    };

    enum class ConnectivityActionType : UInt8
    {
        DISCONNECT = 0x00,
        CONNECT = 0x01,
        UNPAIR = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class DataContinue : UInt8
    {
        NOT_CONTINUE_DATA = 0x00,
        HAS_CONTINUE_DATA = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class DjControlEffectItem : UInt8
    {
        DJ_OFF = 0x00,
        ISOLATOR = 0x01,
        FLANGER = 0x02,
        SAMPLER_DRUMS = 0x03,
        SAMPLER_AUDIENCE = 0x04,
        SAMPLER_PHASER = 0x05,
        SAMPLER_RHYZM = 0x06,
        SAMPLER_SCRATCH = 0x07,
        SAMPLER_VOICE = 0x08,
        SAMPLER_REGGAEHORN = 0x09,
        SAMPLER_ROBOT = 0x0A,
        CUSTOM_EQ = 0x0B,
        SAMPLER_DRUM_ROLL = 0x0C,
        SAMPLER_EXPLOSION = 0x0D,
        SAMPLER_CLAP = 0x0E,
        SAMPLER_DRUMS_7_SOUNDS = 0x0F,
        OUT_OF_RANGE = 0xFF,
    };

    enum class EarphoneShape : UInt8
    {
        CANAL_TYPE = 0x00,
        OPEN_RING_DRIVER = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class ExclusiveFunctionId : UInt8
    {
        STAMINA = 0x00,
        OUT_OF_RANGE = 0xFF,
    };

    enum class FileTransferInMultiConnection : UInt8
    {
        POSSIBLE = 0x00,
        IMPOSSIBLE = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class IlluminationItem : UInt8
    {
        RESET = 0x00,
        PARTY_FLASH = 0x01,
        CUSTOM_COLOR = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class JudgmentModeOperation : UInt8
    {
        JUDGMENT_START = 0x00,
    };

    enum class KaraokeItem : UInt8
    {
        ECHO_SLIDER_4_STEPS = 0x00,
        KEY_CONTROL_SLIDER_13_STEPS = 0x01,
        VOCAL_FADER_OFF_GUIDE_VOCAL_FADER_ON = 0x02,
        VOICE_CHANGER_OFF_DOUBLE_TRACKING_MUNCHKIN_MOUSE_RADIO = 0x03,
        SAMPLER = 0x04,
        SCORING = 0x05,
        BASS_SLIDER_11_STEPS = 0x06,
        HIGH_SLIDER_11_STEPS = 0x07,
        OUT_OF_RANGE = 0xFF,
    };

    enum class KaraokeItemsSamplersStatus : UInt8
    {
        ENABLE = 0x00,
        DISABLE = 0x01,
        ENABLE_EXCEPT_TO_VOICE = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class LEAInquiredType : UInt8
    {
        LE_AUDIO_CONNECTION_STATE_NOTIFICATION = 0x00,
        LE_AUDIO_SWITCH_SUPPORTED_COMPATIBILITY = 0x01,
        LE_AUDIO_CONNECTION_MODE_WITH_BT_RECONNECTION = 0x02,
        GET_IDENTITY_RESOLVING_KEY = 0x03,
        PAS_SUPPORTS_A2DP_LEA_UNI_LEA_BROAD_WITH_CTKD = 0x04,
        LINK_AUTO_SWITCH_CANT_BE_USED_WITH_LEA_CONNECTION = 0xFE,
        OUT_OF_RANGE = 0xFF,
    };

    enum class LightingColorType : UInt8
    {
        MULTI_COLOR = 0x00,
        WHITE = 0x01,
        CUSTOM_COLOR = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class LightingMode : UInt8
    {
        LIGHT_OFF = 0x00,
        DELIGHTFUL = 0x01,
        RAVE = 0x02,
        CHILL = 0x03,
        STROBE = 0x04,
        GRADATION = 0x05,
        PARTY_ALL_FLASHING = 0x07,
        PARTY_RANDOM = 0x08,
        PARTY_CIRCLE = 0x09,
        BEAT = 0x0A,
        WAVE = 0x0B,
        MELLOW = 0x0C,
        HOT = 0x0D,
        COOL = 0x0E,
        CUTE = 0x0F,
        FRESH = 0x10,
        CALM_DAYLIGHT = 0x11,
        OUT_OF_RANGE = 0xFF,
    };

    enum class LimitPattern : UInt8
    {
        CLOSED_MDR = 0x00,
        OPEN_AIR_MDR = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class LimitationType : UInt8
    {
        IN_SAFE_VOLUME_LIMITATION = 0x00,
        OUT_OF_RANGE = 0xFF,
    };

    enum class LinkAutoSwitchAction : UInt8
    {
        REGISTER = 0x00,
        UNREGISTER = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class MicPluggedStatus : UInt8
    {
        PLUG_IN = 0x00,
        UNPLUG = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class MonitoringCapability : UInt8
    {
        MONITOR_DURING_NOT_CHARGING = 0x00,
        MONITOR_DURING_CASE_IN = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class NoticeForBatteryNecessity : UInt8
    {
        NECESSARY = 0x00,
        UNNECESSARY = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class PalettePattern : UInt8
    {
        COLOR_SELECTABLE_FROM_ALL_DIRECTIONS = 0x00,
        COLOR_SELECTABLE_ON_HORIZONTALLY_PARAMETER_ADJUSTABLE_ON_VERTICALLY_WITH_ALL_COLOR_SELECTION = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class PartyInquiredType : UInt8
    {
        DJ_CONTROL = 0x00,
        ILLUMINATION = 0x01,
        KARAOKE = 0x02,
        DJ_CONTROL_WITH_STATUS_DISABLE_REASON = 0x03,
        KARAOKE_WITH_STATUS_DISABLE_REASON = 0x04,
        LIVE_KARAOKE = 0x05,
        OUT_OF_RANGE = 0xFF,
    };

    enum class PeripheralBluetoothMode : UInt8
    {
        NORMAL_MODE = 0x00,
        INQUIRY_SCAN_MODE = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class PeripheralInquiredType : UInt8
    {
        PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT = 0x00,
        SOURCE_SWITCH_CONTROL = 0x01,
        PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE = 0x02,
        MUSIC_HAND_OVER_SETTING = 0x03,
        OUT_OF_RANGE = 0xFF,
    };

    enum class PeripheralResult : UInt8
    {
        DISCONNECTION_SUCCESS = 0x00,
        DISCONNECTION_ERROR = 0x01,
        DISCONNECTION_IN_PROGRESS = 0x02,
        DISCONNECTION_BUSY = 0x03,
        CONNECTION_SUCCESS = 0x10,
        CONNECTION_ERROR = 0x11,
        CONNECTION_IN_PROGRESS = 0x12,
        CONNECTION_BUSY = 0x13,
        UNPAIRING_SUCCESS = 0x20,
        UNPAIRING_ERROR = 0x21,
        UNPAIRING_IN_PROGRESS = 0x22,
        UNPAIRING_BUSY = 0x23,
        PAIRING_SUCCESS = 0x30,
        PAIRING_ERROR = 0x31,
        PAIRING_IN_PROGRESS = 0x32,
        PAIRING_BUSY = 0x33,
        OUT_OF_RANGE = 0xFF,
    };

    enum class PlaybackFunction : UInt8
    {
        AUDIO_IN = 0x00,
        USB = 0x01,
        BT_AUDIO = 0x02,
        OPTICAL = 0x03,
        RCA = 0x04,
        XLR = 0x05,
        USB_C = 0x06,
        OUT_OF_RANGE = 0xFF,
    };

    enum class PowerInquiredType : UInt8
    {
        AUTO_STANDBY = 0x00,
        CARING_CHARGE_WITH_THRESHOLD = 0x01,
        USB_SUBMERSION = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class QuickAccessEasySettingResult : UInt8
    {
        OK = 0x00,
        CANCELED = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class RepeatTapAction : UInt8
    {
        MODE_IN = 0x00,
        MODE_OUT = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class RepeatTapTrainingModeOperation : UInt8
    {
        TRAINING_MODE_START = 0x00,
        TRAINING_MODE_FINISH = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class RepeatTapTrainingModeStatus : UInt8
    {
        IN_TRAINING_MODE = 0x00,
        OUT_OF_TRAINING_MODE = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class ResultCodeValue : UInt8
    {
        OK = 0x00,
        NG = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SVADetectType : UInt8
    {
        NONE = 0x00,
        WAKE_WORD = 0x01,
        COMMAND = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SafeListeningErrorCause : UInt8
    {
        NOT_PLAYING = 0x00,
        IN_CALL = 0x01,
        DETACHED = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SafeListeningInquiredType : UInt8
    {
        SAFE_LISTENING_HBS_1 = 0x00,
        SAFE_LISTENING_TWS_1 = 0x01,
        SAFE_LISTENING_HBS_2 = 0x02,
        SAFE_LISTENING_TWS_2 = 0x03,
        SAFE_VOLUME_CONTROL = 0x04,
        MAX_VOL_LV_LIMIT = 0x05,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SafeListeningLogDataStatus : UInt8
    {
        DISCONNECTED = 0x00,
        SENDING = 0x01,
        COMPLETED = 0x02,
        NOT_SENDING = 0x03,
        ERROR = 0x04,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SafeListeningTargetType : UInt8
    {
        HBS = 0x00,
        TWS_L = 0x01,
        TWS_R = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SafeListeningWHOStandardLevel : UInt8
    {
        NORMAL = 0x00,
        SENSITIVE = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SamplerItem : UInt8
    {
        KARAOKE_HANDCLAP = 0x00,
        VOICE_LETS_GO = 0x01,
        REGGAEHORN = 0x02,
        VOICE_COME_ON = 0x03,
        OUT_OF_RANGE = 0xFF,
    };

    enum class ScoringStatus : UInt8
    {
        STOP = 0x00,
        START = 0x01,
        WAIT = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SonyVoiceAssistantLanguage : UInt8
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
        BRAZILIAN_PORTUGUESE = 0x0C,
        KOREAN = 0x0D,
        TURKISH = 0x0E,
        CHINESE = 0x0F,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SourceSwitchControlResult : UInt8
    {
        SUCCESS = 0x00,
        FAIL = 0x01,
        FAIL_CALLING = 0x02,
        FAIL_A2DP_NOT_CONNECT = 0x03,
        FAIL_GIVE_PRIORITY_TO_VOICE_ASSISTANT = 0x04,
        OUT_OF_RANGE = 0xFF,
    };

    enum class StatusDisableReason : UInt8
    {
        TV_SOUND_BOOSTER = 0x00,
        DEMO_MODE = 0x01,
        SPEAKING = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SystemInquiredType : UInt8
    {
        WEARING_STATUS_CHECKER = 0x00,
        REPEAT_TAP_TRAINING_MODE = 0x01,
        QUICK_ACCESS_EASY_SETTING = 0x02,
        AUTO_VOLUME_OPTIMIZER = 0x03,
        AUTO_VOLUME_WITH_LIMITATION = 0x04,
        SONY_VOICE_ASSISTANT_SETTING_MTK_TRANSFER_SUPPORT_LANGUAGE_SWITCH = 0x05,
        SONY_VOICE_ASSISTANT_COMMAND = 0x06,
        WEARING_DEVICE_INFORMATION = 0x07,
        WEARING_POSITION_JUDGMENT_BY_SENSOR = 0x08,
        LINK_AUTO_SWITCH_FOR_SPEAKER = 0x09,
        LINK_AUTO_SWITCH_FOR_HEADSETS = 0x0A,
        MIC_ON_OFF_BY_HEADPHONE_OPERATION = 0x0B,
        FUNCTION_CHANGE = 0x0C,
        USB_BROWSER = 0x0D,
        LIGHTING_MODE = 0x0E,
        VOICE_ASSISTANT_WITH_SPECIFIC_SETUP_LINK_SUPPORT = 0x0F,
        LIGHTING_DEFAULT_COLOR_COLOR_TYPE = 0x10,
        LIGHTING_DEFAULT_COLOR_CUSTOM_COLOR = 0x11,
        OUT_OF_RANGE = 0xFF,
    };

    enum class TouchPadOperation : UInt8
    {
        DOWN_CONTROL_START = 0x00,
        UP_CONTROL_STOP = 0x01,
        MOVE = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class UsbBrowserCommand : UInt8
    {
        BROWSE_DIRECTORIES = 0x00,
        BROWSE_FILES = 0x01,
        PLAY_ITEM = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class UsbInformationType : UInt8
    {
        SIZE = 0x00,
        DIRECTORY = 0x01,
        FILE = 0x02,
        DIRECTORY_STATUS = 0x03,
        CURRENT_DIRECTORY_STATUS = 0x04,
        OUT_OF_RANGE = 0xFF,
    };

    enum class UsbLayerStatus : UInt8
    {
        SOME_ITEM_IS_PLAYING = 0x00,
        SOME_ITEM_IS_SELECTED = 0x01,
        NOT_SELECTED = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class UsbSubmersionStatus : UInt8
    {
        USB_IS_SUBMERGED = 0x00,
        USB_IS_NOT_SUBMERGED = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class VocalSetting : UInt8
    {
        FADER_OFF = 0x00,
        GUIDE_VOCAL = 0x01,
        FADER_ON = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class VoiceChangerItem : UInt8
    {
        OFF = 0x00,
        DOUBLE_TRACKING = 0x01,
        MUNCHKIN = 0x02,
        MOUSE = 0x03,
        RADIO = 0x04,
        OUT_OF_RANGE = 0xFF,
    };

    enum class VoiceGuidanceInquiredType : UInt8
    {
        MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH = 0x00,
        MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH = 0x01,
        SUPPORT_LANGUAGE_SWITCH = 0x02,
        ONLY_ON_OFF_SETTING = 0x03,
        VOLUME = 0x20,
        VOLUME_SETTING_FIXED_TO_5_STEPS = 0x21,
        BATTERY_LV_VOICE = 0x30,
        POWER_ONOFF_SOUND = 0x31,
        SOUNDEFFECT_ULT_BEEP_ONOFF = 0x32,
        OUT_OF_RANGE = 0xFF,
    };

    enum class VoiceGuidanceLanguage : UInt8
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
        BRAZILIAN_PORTUGUESE = 0x0D,
        KOREAN = 0x0F,
        TURKISH = 0x10,
        CHINESE = 0xF0,
        OUT_OF_RANGE = 0xFF,
    };

    enum class VoiceGuidanceStatusType : UInt8
    {
        ON_OFF = 0x00,
        LANGUAGE = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class VoiceGuidanceSupportsSwitch : UInt8
    {
        NOT_SUPPORT = 0x00,
        SUPPORT = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class WearingPositionOperationStatus : UInt8
    {
        JUDGMENT_COMPLETED_SUCCESSFULLY = 0x00,
        JUDGMENT_COMPLETED_UNSUCCESSFULLY = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class WearingPositionResultCode : UInt8
    {
        NONE = 0x00,
        UNKNOWN = 0x01,
        FRONT_AND_BACK = 0x02,
        UP_AND_DOWN = 0x03,
        LEFT_AND_RIGHT = 0x04,
        RIGHT_ANGLE_TILT = 0x05,
        OUT_OF_RANGE = 0xFF,
    };

    enum class WearingStatusCode : UInt8
    {
        NORMAL = 0x00,
        ILLEGAL = 0x01,
        LEFT_SIDE_NOT_WEAR = 0x02,
        RIGHT_SIDE_NOT_WEAR = 0x03,
        BOTH_NOT_WEAR = 0x04,
        OUT_OF_RANGE = 0xFF,
    };
#pragma endregion Enums

#pragma region Declarations

    // THMSGV2T2ConnectGetSupportFunction
#pragma pack(push, 1)
    struct ConnectGetSupportFunction
    {
        // CODEGEN EnumRange Command::CONNECT_GET_SUPPORT_FUNCTION
        Command command{Command::CONNECT_GET_SUPPORT_FUNCTION}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ConnectInquiredType type{ConnectInquiredType::FIXED_VALUE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(ConnectGetSupportFunction);
    };
#pragma pack(pop)

    // THMSGV2T2ConnectRetSupportFunction
    struct ConnectRetSupportFunction
    {
        // CODEGEN EnumRange Command::CONNECT_RET_SUPPORT_FUNCTION
        Command command{Command::CONNECT_RET_SUPPORT_FUNCTION}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ConnectInquiredType type{ConnectInquiredType::FIXED_VALUE}; // 0x1
        MDRPodArray<SupportFunction> supportFunctions; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(ConnectRetSupportFunction);
    };

    // THMSGV2T2LEAGetCapability
#pragma pack(push, 1)
    struct LEAGetCapability
    {
        // CODEGEN EnumRange Command::LEA_GET_CAPABILITY
        Command command{Command::LEA_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEAGetCapability);
    };
#pragma pack(pop)

    // THMSGV2T2LEAGetParam
#pragma pack(push, 1)
    struct LEAGetParam
    {
        // CODEGEN EnumRange Command::LEA_GET_PARAM
        Command command{Command::LEA_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType type{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEAGetParam);
    };
#pragma pack(pop)

    // THMSGV2T2LEAGetStatus
#pragma pack(push, 1)
    struct LEAGetStatus
    {
        // CODEGEN EnumRange Command::LEA_GET_STATUS
        Command command{Command::LEA_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType type{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEAGetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2LEANotifyParam
#pragma pack(push, 1)
    struct LEANotifyParam
    {
        // CODEGEN EnumRange Command::LEA_NTFY_PARAM
        Command command{Command::LEA_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEANotifyParam);
    };
#pragma pack(pop)

    // THMSGV2T2LEANotifyParamLEAudioConnectionModeWithBTReconnection
#pragma pack(push, 1)
    struct LEANotifyParamLEAudioConnectionModeWithBTReconnection
    {
        // CODEGEN EnumRange Command::LEA_NTFY_PARAM
        Command command{Command::LEA_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ConnectionMode connectionMode{ConnectionMode::HIGH_RELIABILITY}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ConnectionResult connectionResult{ConnectionResult::SUCCESS}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEANotifyParamLEAudioConnectionModeWithBTReconnection);
    };
#pragma pack(pop)

    // THMSGV2T2LEANotifyStatus
#pragma pack(push, 1)
    struct LEANotifyStatus
    {
        // CODEGEN EnumRange Command::LEA_NTFY_STATUS
        Command command{Command::LEA_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType type{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEANotifyStatus);
    };
#pragma pack(pop)

    // THMSGV2T2LEANotifyStatusCommon
#pragma pack(push, 1)
    struct LEANotifyStatusCommon
    {
        // CODEGEN EnumRange Command::LEA_NTFY_STATUS
        Command command{Command::LEA_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType type{LEAInquiredType::LINK_AUTO_SWITCH_CANT_BE_USED_WITH_LEA_CONNECTION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable enableDisable{EnableDisable::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEANotifyStatusCommon);
    };
#pragma pack(pop)

    // THMSGV2T2LEANotifyStatusCommonUnavailableReason
#pragma pack(push, 1)
    struct LEANotifyStatusCommonUnavailableReason
    {
        // CODEGEN EnumRange Command::LEA_NTFY_STATUS
        Command command{Command::LEA_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType type{LEAInquiredType::LINK_AUTO_SWITCH_CANT_BE_USED_WITH_LEA_CONNECTION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UnavailableReason unavailableReason{UnavailableReason::UNAVAILABLE_BY_LE_AUDIO_PRIOR}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEANotifyStatusCommonUnavailableReason);
    };
#pragma pack(pop)

    // THMSGV2T2LEANotifyStatusPasSupportsA2dpLeaUniLeaBroad
#pragma pack(push, 1)
    struct LEANotifyStatusPasSupportsA2dpLeaUniLeaBroad
    {
        // CODEGEN EnumRange Command::LEA_NTFY_STATUS
        Command command{Command::LEA_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType type{LEAInquiredType::PAS_SUPPORTS_A2DP_LEA_UNI_LEA_BROAD_WITH_CTKD}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable status{EnableDisable::ENABLE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        StreamingStatus streamingStatus{StreamingStatus::POWER_OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEANotifyStatusPasSupportsA2dpLeaUniLeaBroad);
    };
#pragma pack(pop)

    // THMSGV2T2LEARetCapability
#pragma pack(push, 1)
    struct LEARetCapability
    {
        // CODEGEN EnumRange Command::LEA_RET_CAPABILITY
        Command command{Command::LEA_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEARetCapability);
    };
#pragma pack(pop)

    // THMSGV2T2LEARetCapabilityLEAudioConnectionModeWithBTReconnection
    struct LEARetCapabilityLEAudioConnectionModeWithBTReconnection
    {
        // CODEGEN EnumRange Command::LEA_RET_CAPABILITY
        Command command{Command::LEA_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1
        MDRPodArray<ConnectionMode> connectionModes; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(LEARetCapabilityLEAudioConnectionModeWithBTReconnection);
    };

    // THMSGV2T2LEARetCapabilityLEAudioSwitchSupportedCompatibility
#pragma pack(push, 1)
    struct LEARetCapabilityLEAudioSwitchSupportedCompatibility
    {
        // CODEGEN EnumRange Command::LEA_RET_CAPABILITY
        Command command{Command::LEA_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1
        UInt8 compatibilityVersion{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEARetCapabilityLEAudioSwitchSupportedCompatibility);
    };
#pragma pack(pop)

    // THMSGV2T2LEARetCapabilityPasSupportsA2dpLeaUniLeaBroad
    struct LEARetCapabilityPasSupportsA2dpLeaUniLeaBroad
    {
        // CODEGEN EnumRange Command::LEA_RET_CAPABILITY
        Command command{Command::LEA_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::PAS_SUPPORTS_A2DP_LEA_UNI_LEA_BROAD_WITH_CTKD}; // 0x1
        Array<UInt8, 17> deviceUniqueId{}; // 0x2
        MDRPrefixedString bdAddressLE;

        MDR_DEFINE_EXTERN_SERIALIZATION(LEARetCapabilityPasSupportsA2dpLeaUniLeaBroad);
    };

    // THMSGV2T2LEARetParam
#pragma pack(push, 1)
    struct LEARetParam
    {
        // CODEGEN EnumRange Command::LEA_RET_PARAM
        Command command{Command::LEA_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEARetParam);
    };
#pragma pack(pop)

    // THMSGV2T2LEARetParamGetIdentityResolvingKey
    struct LEARetParamGetIdentityResolvingKey
    {
        // CODEGEN EnumRange Command::LEA_RET_PARAM
        Command command{Command::LEA_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::GET_IDENTITY_RESOLVING_KEY}; // 0x1
        MDRPodArray<UInt8> iRK; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(LEARetParamGetIdentityResolvingKey);
    };

    // THMSGV2T2LEARetParamLEAudioConnectionModeWithBTReconnection
#pragma pack(push, 1)
    struct LEARetParamLEAudioConnectionModeWithBTReconnection
    {
        // CODEGEN EnumRange Command::LEA_RET_PARAM
        Command command{Command::LEA_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ConnectionMode connectionMode{ConnectionMode::HIGH_RELIABILITY}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEARetParamLEAudioConnectionModeWithBTReconnection);
    };
#pragma pack(pop)

    // THMSGV2T2LEARetParamPasSupportsA2dpLeaUniLeaBroad
#pragma pack(push, 1)
    struct LEARetParamPasSupportsA2dpLeaUniLeaBroad
    {
        // CODEGEN EnumRange Command::LEA_RET_PARAM
        Command command{Command::LEA_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::PAS_SUPPORTS_A2DP_LEA_UNI_LEA_BROAD_WITH_CTKD}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PairedHistory pairedHistory{PairedHistory::BOTH_CLASSIC_BT_BLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEARetParamPasSupportsA2dpLeaUniLeaBroad);
    };
#pragma pack(pop)

    // THMSGV2T2LEARetStatus
#pragma pack(push, 1)
    struct LEARetStatus
    {
        // CODEGEN EnumRange Command::LEA_RET_STATUS
        Command command{Command::LEA_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType type{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEARetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2LEARetStatusCommonUnavailableReason
#pragma pack(push, 1)
    struct LEARetStatusCommonUnavailableReason
    {
        // CODEGEN EnumRange Command::LEA_RET_STATUS
        Command command{Command::LEA_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType type{LEAInquiredType::LINK_AUTO_SWITCH_CANT_BE_USED_WITH_LEA_CONNECTION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UnavailableReason unavailableReason{UnavailableReason::UNAVAILABLE_BY_LE_AUDIO_PRIOR}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEARetStatusCommonUnavailableReason);
    };
#pragma pack(pop)

    // THMSGV2T2LEARetStatusLEAudioConnectionModeWithBTReconnection
#pragma pack(push, 1)
    struct LEARetStatusLEAudioConnectionModeWithBTReconnection
    {
        // CODEGEN EnumRange Command::LEA_RET_STATUS
        Command command{Command::LEA_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType type{LEAInquiredType::LE_AUDIO_CONNECTION_MODE_WITH_BT_RECONNECTION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable enableDisable{EnableDisable::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEARetStatusLEAudioConnectionModeWithBTReconnection);
    };
#pragma pack(pop)

    // THMSGV2T2LEARetStatusPasSupportsA2dpLeaUniLeaBroad
#pragma pack(push, 1)
    struct LEARetStatusPasSupportsA2dpLeaUniLeaBroad
    {
        // CODEGEN EnumRange Command::LEA_RET_STATUS
        Command command{Command::LEA_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType type{LEAInquiredType::PAS_SUPPORTS_A2DP_LEA_UNI_LEA_BROAD_WITH_CTKD}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable status{EnableDisable::ENABLE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        StreamingStatus streamingStatus{StreamingStatus::POWER_OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEARetStatusPasSupportsA2dpLeaUniLeaBroad);
    };
#pragma pack(pop)

    // THMSGV2T2LEASetParam
#pragma pack(push, 1)
    struct LEASetParam
    {
        // CODEGEN EnumRange Command::LEA_SET_PARAM
        Command command{Command::LEA_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEASetParam);
    };
#pragma pack(pop)

    // THMSGV2T2LEASetParamLEAudioConnectionModeWithBTReconnection
#pragma pack(push, 1)
    struct LEASetParamLEAudioConnectionModeWithBTReconnection
    {
        // CODEGEN EnumRange Command::LEA_SET_PARAM
        Command command{Command::LEA_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ConnectionMode connectionMode{ConnectionMode::HIGH_RELIABILITY}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEASetParamLEAudioConnectionModeWithBTReconnection);
    };
#pragma pack(pop)

    // THMSGV2T2LEASetParamLeAudioConnectionStateNotification
    struct LEASetParamLeAudioConnectionStateNotification
    {
        // CODEGEN EnumRange Command::LEA_SET_PARAM
        Command command{Command::LEA_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ConnectionState connectionState{ConnectionState::CONNECTING}; // 0x2
        Array<UInt8, 17> targetDeviceBdAddressOfAccessory{}; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(LEASetParamLeAudioConnectionStateNotification);
    };

    // THMSGV2T2PartyGetCapability
#pragma pack(push, 1)
    struct PartyGetCapability
    {
        // CODEGEN EnumRange Command::PARTY_GET_CAPABILITY
        Command command{Command::PARTY_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyGetCapability);
    };
#pragma pack(pop)

    // THMSGV2T2PartyGetParam
#pragma pack(push, 1)
    struct PartyGetParam
    {
        // CODEGEN EnumRange Command::PARTY_GET_PARAM
        Command command{Command::PARTY_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyGetParam);
    };
#pragma pack(pop)

    // THMSGV2T2PartyGetStatus
#pragma pack(push, 1)
    struct PartyGetStatus
    {
        // CODEGEN EnumRange Command::PARTY_GET_STATUS
        Command command{Command::PARTY_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyGetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2PartyIlluminationInfo
    struct PartyIlluminationInfo
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        IlluminationItem illuminationItem{IlluminationItem::RESET}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ColorElement colorElement{ColorElement::RGB_FULL_COLOR}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PalettePattern palettePattern{PalettePattern::COLOR_SELECTABLE_FROM_ALL_DIRECTIONS}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(PartyIlluminationInfo);
    };

    // THMSGV2T2PartyNotifyParam
#pragma pack(push, 1)
    struct PartyNotifyParam
    {
        // CODEGEN EnumRange Command::PARTY_NTFY_PARAM
        Command command{Command::PARTY_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyNotifyParam);
    };
#pragma pack(pop)

    // THMSGV2T2PartyNotifyParamDjControl
    struct PartyNotifyParamDjControl
    {
        // CODEGEN EnumRange Command::PARTY_NTFY_PARAM
        Command command{Command::PARTY_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        DjControlEffectItem effectItem{DjControlEffectItem::DJ_OFF}; // 0x2
        MDRPodArray<UInt8> customEQBandStepLevel; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(PartyNotifyParamDjControl);
    };

    // THMSGV2T2PartyNotifyParamIllumination
#pragma pack(push, 1)
    struct PartyNotifyParamIllumination
    {
        // CODEGEN EnumRange Command::PARTY_NTFY_PARAM
        Command command{Command::PARTY_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::ILLUMINATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        IlluminationItem illuminationItem{IlluminationItem::RESET}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyNotifyParamIllumination);
    };
#pragma pack(pop)

    // THMSGV2T2PartyNotifyParamKaraoke
    struct PartyNotifyParamKaraoke
    {
        // CODEGEN EnumRange Command::PARTY_NTFY_PARAM
        Command command{Command::PARTY_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::KARAOKE}; // 0x1
        MDRPodArray<UInt8> settings; // 0x2
        UInt8 score{};

        MDR_DEFINE_EXTERN_SERIALIZATION(PartyNotifyParamKaraoke);
    };

    // THMSGV2T2PartyNotifyParamLiveKaraoke
#pragma pack(push, 1)
    struct PartyNotifyParamLiveKaraoke
    {
        // CODEGEN EnumRange Command::PARTY_NTFY_PARAM
        Command command{Command::PARTY_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::LIVE_KARAOKE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue modeStatus{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyNotifyParamLiveKaraoke);
    };
#pragma pack(pop)

    // THMSGV2T2PartyNotifyStatus
#pragma pack(push, 1)
    struct PartyNotifyStatus
    {
        // CODEGEN EnumRange Command::PARTY_NTFY_STATUS
        Command command{Command::PARTY_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyNotifyStatus);
    };
#pragma pack(pop)

    // THMSGV2T2PartyNotifyStatusCommon
#pragma pack(push, 1)
    struct PartyNotifyStatusCommon
    {
        // CODEGEN EnumRange Command::PARTY_NTFY_STATUS
        Command command{Command::PARTY_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable value2{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyNotifyStatusCommon);
    };
#pragma pack(pop)

    // THMSGV2T2PartyNotifyStatusDjControl
#pragma pack(push, 1)
    struct PartyNotifyStatusDjControl
    {
        // CODEGEN EnumRange Command::PARTY_NTFY_STATUS
        Command command{Command::PARTY_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1
        UInt8 value1{}; // 0x2
        UInt8 value2{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyNotifyStatusDjControl);
    };
#pragma pack(pop)

    // THMSGV2T2PartyNotifyStatusDjControlWithStatusDisableReason
    struct PartyNotifyStatusDjControlWithStatusDisableReason
    {
        // CODEGEN EnumRange Command::PARTY_NTFY_STATUS
        Command command{Command::PARTY_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1
        UInt8 value1{}; // 0x2
        UInt8 value2{}; // 0x3
        MDRPodArray<StatusDisableReason> disableReasonList; // 0x4

        MDR_DEFINE_EXTERN_SERIALIZATION(PartyNotifyStatusDjControlWithStatusDisableReason);
    };

    // THMSGV2T2PartyNotifyStatusKaraoke
#pragma pack(push, 1)
    struct PartyNotifyStatusKaraoke
    {
        // CODEGEN EnumRange Command::PARTY_NTFY_STATUS
        Command command{Command::PARTY_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::KARAOKE}; // 0x1
        UInt8 value1{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        MicPluggedStatus pluggedStatus{MicPluggedStatus::PLUG_IN}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ScoringStatus scoringStatus{ScoringStatus::STOP}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyNotifyStatusKaraoke);
    };
#pragma pack(pop)

    // THMSGV2T2PartyNotifyStatusKaraokeWithStatusDisableReason
    struct PartyNotifyStatusKaraokeWithStatusDisableReason
    {
        // CODEGEN EnumRange Command::PARTY_NTFY_STATUS
        Command command{Command::PARTY_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::KARAOKE_WITH_STATUS_DISABLE_REASON}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        KaraokeItemsSamplersStatus status{KaraokeItemsSamplersStatus::ENABLE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        MicPluggedStatus pluggedStatus{MicPluggedStatus::PLUG_IN}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ScoringStatus scoringStatus{ScoringStatus::STOP}; // 0x4
        MDRPodArray<StatusDisableReason> disableReasonList; // 0x5

        MDR_DEFINE_EXTERN_SERIALIZATION(PartyNotifyStatusKaraokeWithStatusDisableReason);
    };

    // THMSGV2T2PartyRetCapability
#pragma pack(push, 1)
    struct PartyRetCapability
    {
        // CODEGEN EnumRange Command::PARTY_RET_CAPABILITY
        Command command{Command::PARTY_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyRetCapability);
    };
#pragma pack(pop)

    // THMSGV2T2PartyRetCapabilityDjControl
    struct PartyRetCapabilityDjControl
    {
        // CODEGEN EnumRange Command::PARTY_RET_CAPABILITY
        Command command{Command::PARTY_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1
        MDRPodArray<DjControlEffectItem> effectItemList; // 0x2
        UInt8 numOfCustomEqBand{};
        UInt8 customEqBandStep{};

        MDR_DEFINE_EXTERN_SERIALIZATION(PartyRetCapabilityDjControl);
    };

    // THMSGV2T2PartyRetCapabilityKaraoke
    struct PartyRetCapabilityKaraoke
    {
        // CODEGEN EnumRange Command::PARTY_RET_CAPABILITY
        Command command{Command::PARTY_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::KARAOKE}; // 0x1
        MDRPodArray<KaraokeItem> karaokeItemList; // 0x2
        MDRPodArray<SamplerItem> samplerItemList;

        MDR_DEFINE_EXTERN_SERIALIZATION(PartyRetCapabilityKaraoke);
    };

    // THMSGV2T2PartyRetParam
#pragma pack(push, 1)
    struct PartyRetParam
    {
        // CODEGEN EnumRange Command::PARTY_RET_PARAM
        Command command{Command::PARTY_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyRetParam);
    };
#pragma pack(pop)

    // THMSGV2T2PartyRetParamDjControl
    struct PartyRetParamDjControl
    {
        // CODEGEN EnumRange Command::PARTY_RET_PARAM
        Command command{Command::PARTY_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        DjControlEffectItem effectItem{DjControlEffectItem::DJ_OFF}; // 0x2
        MDRPodArray<UInt8> customEQBandStepLevel; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(PartyRetParamDjControl);
    };

    // THMSGV2T2PartyRetParamIllumination
#pragma pack(push, 1)
    struct PartyRetParamIllumination
    {
        // CODEGEN EnumRange Command::PARTY_RET_PARAM
        Command command{Command::PARTY_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::ILLUMINATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        IlluminationItem illuminationItem{IlluminationItem::RESET}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyRetParamIllumination);
    };
#pragma pack(pop)

    // THMSGV2T2PartyRetParamKaraoke
    struct PartyRetParamKaraoke
    {
        // CODEGEN EnumRange Command::PARTY_RET_PARAM
        Command command{Command::PARTY_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::KARAOKE}; // 0x1
        MDRPodArray<UInt8> settings; // 0x2
        UInt8 score{};

        MDR_DEFINE_EXTERN_SERIALIZATION(PartyRetParamKaraoke);
    };

    // THMSGV2T2PartyRetParamLiveKaraoke
#pragma pack(push, 1)
    struct PartyRetParamLiveKaraoke
    {
        // CODEGEN EnumRange Command::PARTY_RET_PARAM
        Command command{Command::PARTY_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::LIVE_KARAOKE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue modeStatus{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyRetParamLiveKaraoke);
    };
#pragma pack(pop)

    // THMSGV2T2PartyRetStatus
#pragma pack(push, 1)
    struct PartyRetStatus
    {
        // CODEGEN EnumRange Command::PARTY_RET_STATUS
        Command command{Command::PARTY_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyRetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2PartyRetStatusCommon
#pragma pack(push, 1)
    struct PartyRetStatusCommon
    {
        // CODEGEN EnumRange Command::PARTY_RET_STATUS
        Command command{Command::PARTY_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable value2{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyRetStatusCommon);
    };
#pragma pack(pop)

    // THMSGV2T2PartyRetStatusDjControl
#pragma pack(push, 1)
    struct PartyRetStatusDjControl
    {
        // CODEGEN EnumRange Command::PARTY_RET_STATUS
        Command command{Command::PARTY_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1
        UInt8 value1{}; // 0x2
        UInt8 value2{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyRetStatusDjControl);
    };
#pragma pack(pop)

    // THMSGV2T2PartyRetStatusDjControlWithStatusDisableReason
    struct PartyRetStatusDjControlWithStatusDisableReason
    {
        // CODEGEN EnumRange Command::PARTY_RET_STATUS
        Command command{Command::PARTY_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1
        UInt8 value1{}; // 0x2
        UInt8 value2{}; // 0x3
        MDRPodArray<StatusDisableReason> disableReasonList; // 0x4

        MDR_DEFINE_EXTERN_SERIALIZATION(PartyRetStatusDjControlWithStatusDisableReason);
    };

    // THMSGV2T2PartyRetStatusKaraoke
#pragma pack(push, 1)
    struct PartyRetStatusKaraoke
    {
        // CODEGEN EnumRange Command::PARTY_RET_STATUS
        Command command{Command::PARTY_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::KARAOKE}; // 0x1
        UInt8 value1{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        MicPluggedStatus pluggedStatus{MicPluggedStatus::PLUG_IN}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ScoringStatus scoringStatus{ScoringStatus::STOP}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyRetStatusKaraoke);
    };
#pragma pack(pop)

    // THMSGV2T2PartyRetStatusKaraokeWithStatusDisableReason
    struct PartyRetStatusKaraokeWithStatusDisableReason
    {
        // CODEGEN EnumRange Command::PARTY_RET_STATUS
        Command command{Command::PARTY_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::KARAOKE_WITH_STATUS_DISABLE_REASON}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        KaraokeItemsSamplersStatus status{KaraokeItemsSamplersStatus::ENABLE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        MicPluggedStatus pluggedStatus{MicPluggedStatus::PLUG_IN}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ScoringStatus scoringStatus{ScoringStatus::STOP}; // 0x4
        MDRPodArray<StatusDisableReason> disableReasonList; // 0x5

        MDR_DEFINE_EXTERN_SERIALIZATION(PartyRetStatusKaraokeWithStatusDisableReason);
    };

    // THMSGV2T2PartySetExtParam
#pragma pack(push, 1)
    struct PartySetExtParam
    {
        // CODEGEN EnumRange Command::PARTY_SET_PARAM
        Command command{Command::PARTY_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartySetExtParam);
    };
#pragma pack(pop)

    // THMSGV2T2PartySetExtParamDjControl
#pragma pack(push, 1)
    struct PartySetExtParamDjControl
    {
        // CODEGEN EnumRange Command::PARTY_SET_PARAM
        Command command{Command::PARTY_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        TouchPadOperation touchPadOperation{TouchPadOperation::DOWN_CONTROL_START}; // 0x2
        UInt8 horizontalCoordinate{}; // 0x3
        UInt8 verticalCoordinate{}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartySetExtParamDjControl);
    };
#pragma pack(pop)

    // THMSGV2T2PartySetExtParamIllumination
#pragma pack(push, 1)
    struct PartySetExtParamIllumination
    {
        // CODEGEN EnumRange Command::PARTY_SET_PARAM
        Command command{Command::PARTY_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::ILLUMINATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        TouchPadOperation touchPadOperation{TouchPadOperation::DOWN_CONTROL_START}; // 0x2
        UInt8 horizontalCoordinate{}; // 0x3
        UInt8 verticalCoordinate{}; // 0x4
        UInt8 red{}; // 0x5
        UInt8 green{}; // 0x6
        UInt8 blue{}; // 0x7

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartySetExtParamIllumination);
    };
#pragma pack(pop)

    // THMSGV2T2PartySetExtParamKaraoke
#pragma pack(push, 1)
    struct PartySetExtParamKaraoke
    {
        // CODEGEN EnumRange Command::PARTY_SET_PARAM
        Command command{Command::PARTY_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::KARAOKE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SamplerItem samplerItem{SamplerItem::KARAOKE_HANDCLAP}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartySetExtParamKaraoke);
    };
#pragma pack(pop)

    // THMSGV2T2PartySetParam
#pragma pack(push, 1)
    struct PartySetParam
    {
        // CODEGEN EnumRange Command::PARTY_SET_PARAM
        Command command{Command::PARTY_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartySetParam);
    };
#pragma pack(pop)

    // THMSGV2T2PartySetParamDjControl
    struct PartySetParamDjControl
    {
        // CODEGEN EnumRange Command::PARTY_SET_PARAM
        Command command{Command::PARTY_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        DjControlEffectItem effectItem{DjControlEffectItem::DJ_OFF}; // 0x2
        MDRPodArray<UInt8> customEQBandStepLevel; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(PartySetParamDjControl);
    };

    // THMSGV2T2PartySetParamIllumination
#pragma pack(push, 1)
    struct PartySetParamIllumination
    {
        // CODEGEN EnumRange Command::PARTY_SET_PARAM
        Command command{Command::PARTY_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::ILLUMINATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        IlluminationItem illumination{IlluminationItem::RESET}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartySetParamIllumination);
    };
#pragma pack(pop)

    // THMSGV2T2PartySetParamKaraoke
    struct PartySetParamKaraoke
    {
        // CODEGEN EnumRange Command::PARTY_SET_PARAM
        Command command{Command::PARTY_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::KARAOKE}; // 0x1
        MDRPodArray<UInt8> settings; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(PartySetParamKaraoke);
    };

    // THMSGV2T2PartySetParamLiveKaraoke
#pragma pack(push, 1)
    struct PartySetParamLiveKaraoke
    {
        // CODEGEN EnumRange Command::PARTY_SET_PARAM
        Command command{Command::PARTY_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::LIVE_KARAOKE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue modeStatus{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartySetParamLiveKaraoke);
    };
#pragma pack(pop)

    // THMSGV2T2PartySetStatus
#pragma pack(push, 1)
    struct PartySetStatus
    {
        // CODEGEN EnumRange Command::PARTY_SET_STATUS
        Command command{Command::PARTY_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartySetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2PartySetStatusKaraoke
#pragma pack(push, 1)
    struct PartySetStatusKaraoke
    {
        // CODEGEN EnumRange Command::PARTY_SET_STATUS
        Command command{Command::PARTY_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::KARAOKE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ScoringStatus scoringStatus{ScoringStatus::STOP}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartySetStatusKaraoke);
    };
#pragma pack(pop)

    // THMSGV2T2Payload
    struct Payload
    {
        Command command2{Command::CONNECT_GET_SUPPORT_FUNCTION}; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(Payload);
    };

    // THMSGV2T2PeripheralDeviceInfo
    struct PeripheralDeviceInfo
    {
        Array<UInt8, 17> btDeviceAddress; // 0x0
        UInt8 connectedStatus{};
        Int24BE bluetoothClassOfDevice{};
        MDRPrefixedString btFriendlyName;

        MDR_DEFINE_EXTERN_READ_WRITE(PeripheralDeviceInfo);
    };

    // THMSGV2T2PeripheralDeviceInfo
    struct PeripheralDeviceInfoWithoutBluetoothClassOfDevice
    {
        Array<UInt8, 17> btDeviceAddress; // 0x0
        UInt8 connectedStatus{};
        MDRPrefixedString btFriendlyName;

        MDR_DEFINE_EXTERN_READ_WRITE(PeripheralDeviceInfoWithoutBluetoothClassOfDevice);
    };

    // THMSGV2T2PeripheralGetCapability
#pragma pack(push, 1)
    struct PeripheralGetCapability
    {
        // CODEGEN EnumRange Command::PERI_GET_CAPABILITY
        Command command{Command::PERI_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralGetCapability);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralGetParam
#pragma pack(push, 1)
    struct PeripheralGetParam
    {
        // CODEGEN EnumRange Command::PERI_GET_PARAM
        Command command{Command::PERI_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralGetParam);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralGetStatus
#pragma pack(push, 1)
    struct PeripheralGetStatus
    {
        // CODEGEN EnumRange Command::PERI_GET_STATUS
        Command command{Command::PERI_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralGetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralNotifyExtendedParam
#pragma pack(push, 1)
    struct PeripheralNotifyExtendedParam
    {
        // CODEGEN EnumRange Command::PERI_NTFY_EXTENDED_PARAM
        Command command{Command::PERI_NTFY_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralNotifyExtendedParam);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralNotifyExtendedParamParingDeviceManagementCommon
    struct PeripheralNotifyExtendedParamParingDeviceManagementCommon
    {
        // CODEGEN EnumRange Command::PERI_NTFY_EXTENDED_PARAM
        Command command{Command::PERI_NTFY_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ConnectivityActionType connectivityActionType{ConnectivityActionType::DISCONNECT}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralResult peripheralResult{PeripheralResult::DISCONNECTION_SUCCESS}; // 0x3
        Array<UInt8, 17> btDeviceAddress{}; // 0x4

        MDR_DEFINE_EXTERN_SERIALIZATION(PeripheralNotifyExtendedParamParingDeviceManagementCommon);
    };

    // THMSGV2T2PeripheralNotifyExtendedParamSourceSwitchControl
    struct PeripheralNotifyExtendedParamSourceSwitchControl
    {
        // CODEGEN EnumRange Command::PERI_NTFY_EXTENDED_PARAM
        Command command{Command::PERI_NTFY_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::SOURCE_SWITCH_CONTROL}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SourceSwitchControlResult result{SourceSwitchControlResult::SUCCESS}; // 0x2
        Array<UInt8, 17> targetBdAddress{}; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(PeripheralNotifyExtendedParamSourceSwitchControl);
    };

    // THMSGV2T2PeripheralNotifyParam
#pragma pack(push, 1)
    struct PeripheralNotifyParam
    {
        // CODEGEN EnumRange Command::PERI_NTFY_PARAM
        Command command{Command::PERI_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralNotifyParam);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralNotifyParamMusicHandOverSetting
#pragma pack(push, 1)
    struct PeripheralNotifyParamMusicHandOverSetting
    {
        // CODEGEN EnumRange Command::PERI_NTFY_PARAM
        Command command{Command::PERI_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::MUSIC_HAND_OVER_SETTING}; // 0x1
        UInt8 on{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralNotifyParamMusicHandOverSetting);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralNotifyParamSourceSwitchControl
#pragma pack(push, 1)
    struct PeripheralNotifyParamSourceSwitchControl
    {
        // CODEGEN EnumRange Command::PERI_NTFY_PARAM
        Command command{Command::PERI_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::SOURCE_SWITCH_CONTROL}; // 0x1
        UInt8 value1{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SourceSwitchControlResult result{SourceSwitchControlResult::SUCCESS}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralNotifyParamSourceSwitchControl);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralNotifyStatus
#pragma pack(push, 1)
    struct PeripheralNotifyStatus
    {
        // CODEGEN EnumRange Command::PERI_NTFY_STATUS
        Command command{Command::PERI_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralNotifyStatus);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralNotifyStatusParingDeviceManagementCommon
#pragma pack(push, 1)
    struct PeripheralNotifyStatusParingDeviceManagementCommon
    {
        // CODEGEN EnumRange Command::PERI_NTFY_STATUS
        Command command{Command::PERI_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralBluetoothMode btMode{PeripheralBluetoothMode::NORMAL_MODE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable enableDisableStatus{EnableDisable::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralNotifyStatusParingDeviceManagementCommon);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralRetCapability
#pragma pack(push, 1)
    struct PeripheralRetCapability
    {
        // CODEGEN EnumRange Command::PERI_RET_CAPABILITY
        Command command{Command::PERI_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralRetCapability);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralRetCapabilityParingDeviceManagementCommon
#pragma pack(push, 1)
    struct PeripheralRetCapabilityParingDeviceManagementCommon
    {
        // CODEGEN EnumRange Command::PERI_RET_CAPABILITY
        Command command{Command::PERI_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1
        // CODEGEN Range 0 255
        UInt8 maxOfPairedDevice{}; // 0x2
        // CODEGEN Range 0 255
        UInt8 maxOfConnectedDevice{}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        FileTransferInMultiConnection fileTransferInMultiConnection{FileTransferInMultiConnection::POSSIBLE}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralRetCapabilityParingDeviceManagementCommon);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralRetParam
#pragma pack(push, 1)
    struct PeripheralRetParam
    {
        // CODEGEN EnumRange Command::PERI_RET_PARAM
        Command command{Command::PERI_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralRetParam);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralRetParamMusicHandOverSetting
#pragma pack(push, 1)
    struct PeripheralRetParamMusicHandOverSetting
    {
        // CODEGEN EnumRange Command::PERI_RET_PARAM
        Command command{Command::PERI_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::MUSIC_HAND_OVER_SETTING}; // 0x1
        UInt8 on{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralRetParamMusicHandOverSetting);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralRetParamSourceSwitchControl
#pragma pack(push, 1)
    struct PeripheralRetParamSourceSwitchControl
    {
        // CODEGEN EnumRange Command::PERI_RET_PARAM
        Command command{Command::PERI_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::SOURCE_SWITCH_CONTROL}; // 0x1
        UInt8 value{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralRetParamSourceSwitchControl);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralRetStatus
#pragma pack(push, 1)
    struct PeripheralRetStatus
    {
        // CODEGEN EnumRange Command::PERI_RET_STATUS
        Command command{Command::PERI_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralRetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralRetStatusPairingDeviceManagementCommon
#pragma pack(push, 1)
    struct PeripheralRetStatusPairingDeviceManagementCommon
    {
        // CODEGEN EnumRange Command::PERI_RET_STATUS
        Command command{Command::PERI_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralBluetoothMode btMode{PeripheralBluetoothMode::NORMAL_MODE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable enableDisableStatus{EnableDisable::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralRetStatusPairingDeviceManagementCommon);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralSetExtendedParam
#pragma pack(push, 1)
    struct PeripheralSetExtendedParam
    {
        // CODEGEN EnumRange Command::PERI_SET_EXTENDED_PARAM
        Command command{Command::PERI_SET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralSetExtendedParam);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralSetExtendedParamParingDeviceManagementCommon
    struct PeripheralSetExtendedParamParingDeviceManagementCommon
    {
        // CODEGEN EnumRange Command::PERI_SET_EXTENDED_PARAM
        Command command{Command::PERI_SET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ConnectivityActionType connectivityActionType{ConnectivityActionType::DISCONNECT}; // 0x2
        Array<UInt8, 17> btDeviceAddress{}; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(PeripheralSetExtendedParamParingDeviceManagementCommon);
    };

    // THMSGV2T2PeripheralSetExtendedParamSourceSwitchControl
    struct PeripheralSetExtendedParamSourceSwitchControl
    {
        // CODEGEN EnumRange Command::PERI_SET_EXTENDED_PARAM
        Command command{Command::PERI_SET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::SOURCE_SWITCH_CONTROL}; // 0x1
        Array<UInt8, 17> targetBdAddress{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(PeripheralSetExtendedParamSourceSwitchControl);
    };

    // THMSGV2T2PeripheralSetParam
#pragma pack(push, 1)
    struct PeripheralSetParam
    {
        // CODEGEN EnumRange Command::PERI_SET_PARAM
        Command command{Command::PERI_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralSetParam);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralSetParamMusicHandOverSetting
#pragma pack(push, 1)
    struct PeripheralSetParamMusicHandOverSetting
    {
        // CODEGEN EnumRange Command::PERI_SET_PARAM
        Command command{Command::PERI_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::MUSIC_HAND_OVER_SETTING}; // 0x1
        UInt8 on{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralSetParamMusicHandOverSetting);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralSetParamSourceSwitchControl
#pragma pack(push, 1)
    struct PeripheralSetParamSourceSwitchControl
    {
        // CODEGEN EnumRange Command::PERI_SET_PARAM
        Command command{Command::PERI_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::SOURCE_SWITCH_CONTROL}; // 0x1
        UInt8 value{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralSetParamSourceSwitchControl);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralSetStatus
#pragma pack(push, 1)
    struct PeripheralSetStatus
    {
        // CODEGEN EnumRange Command::PERI_SET_STATUS
        Command command{Command::PERI_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralSetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2PeripheralSetStatusParingDeviceManagementCommon
#pragma pack(push, 1)
    struct PeripheralSetStatusParingDeviceManagementCommon
    {
        // CODEGEN EnumRange Command::PERI_SET_STATUS
        Command command{Command::PERI_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralBluetoothMode btMode{PeripheralBluetoothMode::NORMAL_MODE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable enableDisableStatus{EnableDisable::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralSetStatusParingDeviceManagementCommon);
    };
#pragma pack(pop)

    // THMSGV2T2PowerGetCapability
#pragma pack(push, 1)
    struct PowerGetCapability
    {
        // CODEGEN EnumRange Command::POWER_GET_CAPABILITY
        Command command{Command::POWER_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerGetCapability);
    };
#pragma pack(pop)

    // THMSGV2T2PowerGetParam
#pragma pack(push, 1)
    struct PowerGetParam
    {
        // CODEGEN EnumRange Command::POWER_GET_PARAM
        Command command{Command::POWER_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerGetParam);
    };
#pragma pack(pop)

    // THMSGV2T2PowerGetStatus
#pragma pack(push, 1)
    struct PowerGetStatus
    {
        // CODEGEN EnumRange Command::POWER_GET_STATUS
        Command command{Command::POWER_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerGetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2PowerNotifyParam
#pragma pack(push, 1)
    struct PowerNotifyParam
    {
        // CODEGEN EnumRange Command::POWER_NTFY_PARAM
        Command command{Command::POWER_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerNotifyParam);
    };
#pragma pack(pop)

    // THMSGV2T2PowerNotifyParamSettingOnOff
#pragma pack(push, 1)
    struct PowerNotifyParamSettingOnOff
    {
        // CODEGEN EnumRange Command::POWER_NTFY_PARAM
        Command command{Command::POWER_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue onOffSettingValue{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerNotifyParamSettingOnOff);
    };
#pragma pack(pop)

    // THMSGV2T2PowerNotifyParamUsbSubmersion
#pragma pack(push, 1)
    struct PowerNotifyParamUsbSubmersion
    {
        // CODEGEN EnumRange Command::POWER_NTFY_PARAM
        Command command{Command::POWER_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::USB_SUBMERSION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        RequestResult userConfirmationStatus{RequestResult::ACCEPTED}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerNotifyParamUsbSubmersion);
    };
#pragma pack(pop)

    // THMSGV2T2PowerNotifyStatus
#pragma pack(push, 1)
    struct PowerNotifyStatus
    {
        // CODEGEN EnumRange Command::POWER_NTFY_STATUS
        Command command{Command::POWER_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerNotifyStatus);
    };
#pragma pack(pop)

    // THMSGV2T2PowerNotifyStatusCaringChargeWithThreshold
#pragma pack(push, 1)
    struct PowerNotifyStatusCaringChargeWithThreshold
    {
        // CODEGEN EnumRange Command::POWER_NTFY_STATUS
        Command command{Command::POWER_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::CARING_CHARGE_WITH_THRESHOLD}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable enableDisable{EnableDisable::ENABLE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NoticeForBatteryNecessity noticeForBatteryNecessity{NoticeForBatteryNecessity::NECESSARY}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerNotifyStatusCaringChargeWithThreshold);
    };
#pragma pack(pop)

    // THMSGV2T2PowerNotifyStatusCommon
#pragma pack(push, 1)
    struct PowerNotifyStatusCommon
    {
        // CODEGEN EnumRange Command::POWER_NTFY_STATUS
        Command command{Command::POWER_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable enableDisable{EnableDisable::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerNotifyStatusCommon);
    };
#pragma pack(pop)

    // THMSGV2T2PowerNotifyStatusUsbSubmersion
#pragma pack(push, 1)
    struct PowerNotifyStatusUsbSubmersion
    {
        // CODEGEN EnumRange Command::POWER_NTFY_STATUS
        Command command{Command::POWER_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::USB_SUBMERSION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbSubmersionStatus usbSubmersionStatus{UsbSubmersionStatus::USB_IS_SUBMERGED}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerNotifyStatusUsbSubmersion);
    };
#pragma pack(pop)

    // THMSGV2T2PowerRetCapability
#pragma pack(push, 1)
    struct PowerRetCapability
    {
        // CODEGEN EnumRange Command::POWER_RET_CAPABILITY
        Command command{Command::POWER_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType inquiredType{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetCapability);
    };
#pragma pack(pop)

    // THMSGV2T2PowerRetCapabilityAutoStandby
#pragma pack(push, 1)
    struct PowerRetCapabilityAutoStandby
    {
        // CODEGEN EnumRange Command::POWER_RET_CAPABILITY
        Command command{Command::POWER_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType inquiredType{PowerInquiredType::AUTO_STANDBY}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AutoStandbyCandidateElement candidateElement{AutoStandbyCandidateElement::AUTO_STANDBY_IN_15_MIN}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetCapabilityAutoStandby);
    };
#pragma pack(pop)

    // THMSGV2T2PowerRetCapabilityCaringChargeWithThreshold
#pragma pack(push, 1)
    struct PowerRetCapabilityCaringChargeWithThreshold
    {
        // CODEGEN EnumRange Command::POWER_RET_CAPABILITY
        Command command{Command::POWER_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType inquiredType{PowerInquiredType::CARING_CHARGE_WITH_THRESHOLD}; // 0x1
        UInt8 value{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetCapabilityCaringChargeWithThreshold);
    };
#pragma pack(pop)

    // THMSGV2T2PowerRetCapabilityUsbSubmersion
#pragma pack(push, 1)
    struct PowerRetCapabilityUsbSubmersion
    {
        // CODEGEN EnumRange Command::POWER_RET_CAPABILITY
        Command command{Command::POWER_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType inquiredType{PowerInquiredType::USB_SUBMERSION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        MonitoringCapability monitoringCapability{MonitoringCapability::MONITOR_DURING_NOT_CHARGING}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetCapabilityUsbSubmersion);
    };
#pragma pack(pop)

    // THMSGV2T2PowerRetParam
#pragma pack(push, 1)
    struct PowerRetParam
    {
        // CODEGEN EnumRange Command::POWER_RET_PARAM
        Command command{Command::POWER_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetParam);
    };
#pragma pack(pop)

    // THMSGV2T2PowerRetParamSettingOnOff
#pragma pack(push, 1)
    struct PowerRetParamSettingOnOff
    {
        // CODEGEN EnumRange Command::POWER_RET_PARAM
        Command command{Command::POWER_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue onOffSettingValue{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetParamSettingOnOff);
    };
#pragma pack(pop)

    // THMSGV2T2PowerRetStatus
#pragma pack(push, 1)
    struct PowerRetStatus
    {
        // CODEGEN EnumRange Command::POWER_RET_STATUS
        Command command{Command::POWER_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2PowerRetStatusCaringChargeWithThreshold
#pragma pack(push, 1)
    struct PowerRetStatusCaringChargeWithThreshold
    {
        // CODEGEN EnumRange Command::POWER_RET_STATUS
        Command command{Command::POWER_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::CARING_CHARGE_WITH_THRESHOLD}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable enableDisable{EnableDisable::ENABLE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NoticeForBatteryNecessity noticeForBatteryNecessity{NoticeForBatteryNecessity::NECESSARY}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetStatusCaringChargeWithThreshold);
    };
#pragma pack(pop)

    // THMSGV2T2PowerRetStatusCommon
#pragma pack(push, 1)
    struct PowerRetStatusCommon
    {
        // CODEGEN EnumRange Command::POWER_RET_STATUS
        Command command{Command::POWER_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable enableDisable{EnableDisable::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetStatusCommon);
    };
#pragma pack(pop)

    // THMSGV2T2PowerRetStatusUsbSubmersion
#pragma pack(push, 1)
    struct PowerRetStatusUsbSubmersion
    {
        // CODEGEN EnumRange Command::POWER_RET_STATUS
        Command command{Command::POWER_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::USB_SUBMERSION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbSubmersionStatus usbSubmersionStatus{UsbSubmersionStatus::USB_IS_SUBMERGED}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetStatusUsbSubmersion);
    };
#pragma pack(pop)

    // THMSGV2T2PowerSetParam
#pragma pack(push, 1)
    struct PowerSetParam
    {
        // CODEGEN EnumRange Command::POWER_SET_PARAM
        Command command{Command::POWER_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerSetParam);
    };
#pragma pack(pop)

    // THMSGV2T2PowerSetParamSettingOnOff
#pragma pack(push, 1)
    struct PowerSetParamSettingOnOff
    {
        // CODEGEN EnumRange Command::POWER_SET_PARAM
        Command command{Command::POWER_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue onOffSettingValue{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerSetParamSettingOnOff);
    };
#pragma pack(pop)

    // THMSGV2T2RepeatTapKey
    struct RepeatTapKey
    {
        mdr::v2::t1::Key key{mdr::v2::t1::Key::LEFT_SIDE}; // 0x0
        mdr::v2::t1::Type type{mdr::v2::t1::Type::TOUCH_SENSOR}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(RepeatTapKey);
    };

    // THMSGV2T2SafeListeningData
    struct SafeListeningData
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningTargetType targetType{SafeListeningTargetType::HBS}; // 0x0
        // CODEGEN Range 0 4294967295
        Int32BE timestamp{}; // 0x1
        // CODEGEN Range 0 65535
        Int16BE rtcRc{}; // 0x5
        // CODEGEN Range 0 255
        UInt8 viewTime{}; // 0x7
        Int32BE soundPressure{}; // 0x8

        MDR_DEFINE_EXTERN_READ_WRITE(SafeListeningData);
    };

    // THMSGV2T2SafeListeningData1
    struct SafeListeningData1
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningTargetType targetType{SafeListeningTargetType::HBS}; // 0x0
        // CODEGEN Range 0 4294967295
        Int32BE timestamp{}; // 0x1
        // CODEGEN Range 0 65535
        Int16BE rtcRc{}; // 0x5
        // CODEGEN Range 0 255
        UInt8 viewTime{}; // 0x7
        Int32BE soundPressure{}; // 0x8

        MDR_DEFINE_EXTERN_READ_WRITE(SafeListeningData1);
    };

    // THMSGV2T2SafeListeningData2
    struct SafeListeningData2
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningTargetType targetType{SafeListeningTargetType::HBS}; // 0x0
        // CODEGEN Range 0 4294967295
        Int32BE timestamp{}; // 0x1
        // CODEGEN Range 0 65535
        Int16BE rtcRc{}; // 0x5
        // CODEGEN Range 0 255
        UInt8 viewTime{}; // 0x7
        Int32BE soundPressure{}; // 0x8
        // CODEGEN Range 0 255
        UInt8 value6{}; // 0xC

        MDR_DEFINE_EXTERN_READ_WRITE(SafeListeningData2);
    };

    // THMSGV2T2SafeListeningGetCapability
    struct SafeListeningGetCapability
    {
        Command command2{Command::CONNECT_GET_SUPPORT_FUNCTION}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(SafeListeningGetCapability);
    };

    // THMSGV2T2SafeListeningGetExtendedParam
#pragma pack(push, 1)
    struct SafeListeningGetExtendedParam
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_GET_EXTENDED_PARAM
        Command command{Command::SAFE_LISTENING_GET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningGetExtendedParam);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningGetParam
#pragma pack(push, 1)
    struct SafeListeningGetParam
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_GET_PARAM
        Command command{Command::SAFE_LISTENING_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningGetParam);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningGetStatus
    struct SafeListeningGetStatus
    {
        Command command2{Command::CONNECT_GET_SUPPORT_FUNCTION}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(SafeListeningGetStatus);
    };

    // THMSGV2T2SafeListeningNotifyParam
#pragma pack(push, 1)
    struct SafeListeningNotifyParam
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_PARAM
        Command command{Command::SAFE_LISTENING_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyParam);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningNotifyParamMaxVolLvLimit
#pragma pack(push, 1)
    struct SafeListeningNotifyParamMaxVolLvLimit
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_PARAM
        Command command{Command::SAFE_LISTENING_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        UInt8 idxOfMaxVolLv{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyParamMaxVolLvLimit);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningNotifyParamSL
#pragma pack(push, 1)
    struct SafeListeningNotifyParamSL
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_PARAM
        Command command{Command::SAFE_LISTENING_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue safeListeningMode{OnOffSettingValue::ON}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue previewMode{OnOffSettingValue::ON}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyParamSL);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningNotifyParamSVC
#pragma pack(push, 1)
    struct SafeListeningNotifyParamSVC
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_PARAM
        Command command{Command::SAFE_LISTENING_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue volumeLimitationMode{OnOffSettingValue::ON}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue safeVolumeControlMode{OnOffSettingValue::ON}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyParamSVC);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningNotifyStatus
#pragma pack(push, 1)
    struct SafeListeningNotifyStatus
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_STATUS
        Command command{Command::SAFE_LISTENING_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyStatus);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningNotifyStatusMaxVolLvLimit
#pragma pack(push, 1)
    struct SafeListeningNotifyStatusMaxVolLvLimit
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_STATUS
        Command command{Command::SAFE_LISTENING_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue onOff{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyStatusMaxVolLvLimit);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningNotifyStatusSL
#pragma pack(push, 1)
    struct SafeListeningNotifyStatusSL
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_STATUS
        Command command{Command::SAFE_LISTENING_NTFY_STATUS}; // 0x0

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyStatusSL);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningNotifyStatusSVC
#pragma pack(push, 1)
    struct SafeListeningNotifyStatusSVC
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_STATUS
        Command command{Command::SAFE_LISTENING_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningWHOStandardLevel wHOStandardLevel{SafeListeningWHOStandardLevel::NORMAL}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyStatusSVC);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningRetCapability
    struct SafeListeningRetCapability
    {
        Command command2{Command::CONNECT_GET_SUPPORT_FUNCTION}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(SafeListeningRetCapability);
    };

    // THMSGV2T2SafeListeningRetCapabilityMaxVolLvLimit
    struct SafeListeningRetCapabilityMaxVolLvLimit
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LimitPattern limitPattern{LimitPattern::CLOSED_MDR}; // 0x1
        // CODEGEN Range 0 255
        UInt8 minimumInterval{}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(SafeListeningRetCapabilityMaxVolLvLimit);
    };

    // THMSGV2T2SafeListeningRetCapabilitySL
    struct SafeListeningRetCapabilitySL
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x0
        // CODEGEN Range 0 255
        UInt8 roundBase{}; // 0x1
        // CODEGEN Range 0 4294967295
        Int32BE timestampBase{}; // 0x2
        // CODEGEN Range 0 255
        UInt8 minimumInterval{}; // 0x6
        // CODEGEN Range 0 255
        UInt8 logCapacity{}; // 0x7

        MDR_DEFINE_EXTERN_READ_WRITE(SafeListeningRetCapabilitySL);
    };

    // THMSGV2T2SafeListeningRetExtendedParam
#pragma pack(push, 1)
    struct SafeListeningRetExtendedParam
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_RET_EXTENDED_PARAM
        Command command{Command::SAFE_LISTENING_RET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Range 0 255
        UInt8 levelPerPeriod{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningErrorCause errorCause{SafeListeningErrorCause::NOT_PLAYING}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetExtendedParam);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningRetParam
#pragma pack(push, 1)
    struct SafeListeningRetParam
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_RET_PARAM
        Command command{Command::SAFE_LISTENING_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetParam);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningRetParamMaxVolLvLimit
#pragma pack(push, 1)
    struct SafeListeningRetParamMaxVolLvLimit
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_RET_PARAM
        Command command{Command::SAFE_LISTENING_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        UInt8 idxOfMaxVolLv{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetParamMaxVolLvLimit);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningRetParamSL
#pragma pack(push, 1)
    struct SafeListeningRetParamSL
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_RET_PARAM
        Command command{Command::SAFE_LISTENING_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable availability{EnableDisable::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetParamSL);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningRetStatus
#pragma pack(push, 1)
    struct SafeListeningRetStatus
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_RET_STATUS
        Command command{Command::SAFE_LISTENING_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningRetStatusMaxVolLvLimit
#pragma pack(push, 1)
    struct SafeListeningRetStatusMaxVolLvLimit
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_RET_STATUS
        Command command{Command::SAFE_LISTENING_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue onOff{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetStatusMaxVolLvLimit);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningRetStatusSL
#pragma pack(push, 1)
    struct SafeListeningRetStatusSL
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_RET_STATUS
        Command command{Command::SAFE_LISTENING_RET_STATUS}; // 0x0

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetStatusSL);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningSetParam
#pragma pack(push, 1)
    struct SafeListeningSetParam
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_SET_PARAM
        Command command{Command::SAFE_LISTENING_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetParam);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningSetParamMaxVolLvLimit
#pragma pack(push, 1)
    struct SafeListeningSetParamMaxVolLvLimit
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_SET_PARAM
        Command command{Command::SAFE_LISTENING_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        UInt8 idxOfMaxVolLv{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetParamMaxVolLvLimit);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningSetParamSL
#pragma pack(push, 1)
    struct SafeListeningSetParamSL
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_SET_PARAM
        Command command{Command::SAFE_LISTENING_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue safeListeningMode{OnOffSettingValue::ON}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue previewMode{OnOffSettingValue::ON}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetParamSL);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningSetParamSVC
#pragma pack(push, 1)
    struct SafeListeningSetParamSVC
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_SET_PARAM
        Command command{Command::SAFE_LISTENING_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue volumeLimitationMode{OnOffSettingValue::ON}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue safeVolumeControlMode{OnOffSettingValue::ON}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetParamSVC);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningSetStatus
#pragma pack(push, 1)
    struct SafeListeningSetStatus
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_SET_STATUS
        Command command{Command::SAFE_LISTENING_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningSetStatusMaxVolLvLimit
#pragma pack(push, 1)
    struct SafeListeningSetStatusMaxVolLvLimit
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_SET_STATUS
        Command command{Command::SAFE_LISTENING_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue onOff{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetStatusMaxVolLvLimit);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningSetStatusSL
#pragma pack(push, 1)
    struct SafeListeningSetStatusSL
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_SET_STATUS
        Command command{Command::SAFE_LISTENING_SET_STATUS}; // 0x0

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetStatusSL);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningSetStatusSVC
#pragma pack(push, 1)
    struct SafeListeningSetStatusSVC
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_SET_STATUS
        Command command{Command::SAFE_LISTENING_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningWHOStandardLevel wHOStandardLevel{SafeListeningWHOStandardLevel::NORMAL}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetStatusSVC);
    };
#pragma pack(pop)

    // THMSGV2T2SafeListeningStatus
    struct SafeListeningStatus
    {
        // CODEGEN Range 0 4294967295
        Int32BE timestamp{}; // 0x0
        // CODEGEN Range 0 65535
        Int16BE rtcRc{}; // 0x4

        MDR_DEFINE_EXTERN_READ_WRITE(SafeListeningStatus);
    };

    // THMSGV2T2SystemGetCapability
#pragma pack(push, 1)
    struct SystemGetCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_CAPABILITY
        Command command{Command::SYSTEM_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemGetCapability);
    };
#pragma pack(pop)

    // THMSGV2T2SystemGetCapabilityCommon
#pragma pack(push, 1)
    struct SystemGetCapabilityCommon
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_CAPABILITY
        Command command{Command::SYSTEM_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemGetCapabilityCommon);
    };
#pragma pack(pop)

    // THMSGV2T2SystemGetCapabilitySVACommand
#pragma pack(push, 1)
    struct SystemGetCapabilitySVACommand
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_CAPABILITY
        Command command{Command::SYSTEM_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SONY_VOICE_ASSISTANT_COMMAND}; // 0x1
        Int16BE receivedDataLength{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemGetCapabilitySVACommand);
    };
#pragma pack(pop)

    // THMSGV2T2SystemGetExtendedParam
#pragma pack(push, 1)
    struct SystemGetExtendedParam
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_EXTENDED_PARAM
        Command command{Command::SYSTEM_GET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemGetExtendedParam);
    };
#pragma pack(pop)

    // THMSGV2T2SystemGetExtendedParamCommon
#pragma pack(push, 1)
    struct SystemGetExtendedParamCommon
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_EXTENDED_PARAM
        Command command{Command::SYSTEM_GET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemGetExtendedParamCommon);
    };
#pragma pack(pop)

    // THMSGV2T2SystemGetExtendedParamUsbBrowser
#pragma pack(push, 1)
    struct SystemGetExtendedParamUsbBrowser
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_EXTENDED_PARAM
        Command command{Command::SYSTEM_GET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::USB_BROWSER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbInformationType informationType{UsbInformationType::SIZE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemGetExtendedParamUsbBrowser);
    };
#pragma pack(pop)

    // THMSGV2T2SystemGetParam
#pragma pack(push, 1)
    struct SystemGetParam
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_PARAM
        Command command{Command::SYSTEM_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemGetParam);
    };
#pragma pack(pop)

    // THMSGV2T2SystemGetStatus
#pragma pack(push, 1)
    struct SystemGetStatus
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_STATUS
        Command command{Command::SYSTEM_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemGetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyExtendedParam
#pragma pack(push, 1)
    struct SystemNotifyExtendedParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_EXTENDED_PARAM
        Command command{Command::SYSTEM_NTFY_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyExtendedParam);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyExtendedParamUsbBrowser
#pragma pack(push, 1)
    struct SystemNotifyExtendedParamUsbBrowser
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_EXTENDED_PARAM
        Command command{Command::SYSTEM_NTFY_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::USB_BROWSER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbInformationType informationType{UsbInformationType::SIZE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyExtendedParamUsbBrowser);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyExtendedParamUsbBrowserForDirectoryStatus
#pragma pack(push, 1)
    struct SystemNotifyExtendedParamUsbBrowserForDirectoryStatus
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_EXTENDED_PARAM
        Command command{Command::SYSTEM_NTFY_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::USB_BROWSER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbInformationType informationType{UsbInformationType::DIRECTORY}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbLayerStatus currentLayerStatus{UsbLayerStatus::SOME_ITEM_IS_PLAYING}; // 0x3
        Int16BE directoryIndex{}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyExtendedParamUsbBrowserForDirectoryStatus);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyExtendedParamUsbBrowserForFileStatus
#pragma pack(push, 1)
    struct SystemNotifyExtendedParamUsbBrowserForFileStatus
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_EXTENDED_PARAM
        Command command{Command::SYSTEM_NTFY_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::USB_BROWSER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbInformationType informationType{UsbInformationType::FILE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbLayerStatus currentLayerStatus{UsbLayerStatus::SOME_ITEM_IS_PLAYING}; // 0x3
        Int16BE currentDirectoryIndex{}; // 0x4
        Int16BE fileIndex{}; // 0x6

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyExtendedParamUsbBrowserForFileStatus);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyParam
#pragma pack(push, 1)
    struct SystemNotifyParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyParam);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyParamAutoVolumeWithLimitation
    struct SystemNotifyParamAutoVolumeWithLimitation
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_VOLUME_WITH_LIMITATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue settingValue{OnOffSettingValue::ON}; // 0x2
        MDRPodArray<LimitationType> limitationTypeList; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemNotifyParamAutoVolumeWithLimitation);
    };

    // THMSGV2T2SystemNotifyParamCommon
#pragma pack(push, 1)
    struct SystemNotifyParamCommon
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue settingValue{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyParamCommon);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyParamFunctionChange
#pragma pack(push, 1)
    struct SystemNotifyParamFunctionChange
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::FUNCTION_CHANGE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackFunction function{PlaybackFunction::AUDIO_IN}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyParamFunctionChange);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyParamLightingDefaultColorColorType
#pragma pack(push, 1)
    struct SystemNotifyParamLightingDefaultColorColorType
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::LIGHTING_DEFAULT_COLOR_COLOR_TYPE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LightingColorType colorType{LightingColorType::MULTI_COLOR}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyParamLightingDefaultColorColorType);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyParamLightingDefaultColorCustomColor
#pragma pack(push, 1)
    struct SystemNotifyParamLightingDefaultColorCustomColor
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::LIGHTING_DEFAULT_COLOR_CUSTOM_COLOR}; // 0x1
        // CODEGEN Range 0 255
        UInt8 red{}; // 0x2
        // CODEGEN Range 0 255
        UInt8 green{}; // 0x3
        // CODEGEN Range 0 255
        UInt8 blue{}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyParamLightingDefaultColorCustomColor);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyParamLightingMode
#pragma pack(push, 1)
    struct SystemNotifyParamLightingMode
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::LIGHTING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LightingMode lightingMode{LightingMode::LIGHT_OFF}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyParamLightingMode);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyParamLinkAutoSwitchForHeadsets
    struct SystemNotifyParamLinkAutoSwitchForHeadsets
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::LINK_AUTO_SWITCH_FOR_HEADSETS}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue linkAutoSwitchStatus{OnOffSettingValue::ON}; // 0x2
        Int16BE speakerIdentifier{}; // 0x3
        Array<UInt8, 17> smartPhoneBtAddress{}; // 0x5

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemNotifyParamLinkAutoSwitchForHeadsets);
    };

    // THMSGV2T2SystemNotifyParamLinkAutoSwitchForSpeaker
    struct SystemNotifyParamLinkAutoSwitchForSpeaker
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::LINK_AUTO_SWITCH_FOR_SPEAKER}; // 0x1
        MDRArray<MDRPrefixedString> smartPhoneBDAddressList; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemNotifyParamLinkAutoSwitchForSpeaker);
    };

    // THMSGV2T2SystemNotifyParamMicOnOffByHeadphoneOperation
#pragma pack(push, 1)
    struct SystemNotifyParamMicOnOffByHeadphoneOperation
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::MIC_ON_OFF_BY_HEADPHONE_OPERATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue micOnOffByHeadphoneOperation{OnOffSettingValue::ON}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue micOffSoundEffect{OnOffSettingValue::ON}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyParamMicOnOffByHeadphoneOperation);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyParamQuickAccessEasySetting
#pragma pack(push, 1)
    struct SystemNotifyParamQuickAccessEasySetting
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::QUICK_ACCESS_EASY_SETTING}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        QuickAccessEasySettingResult result{QuickAccessEasySettingResult::OK}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyParamQuickAccessEasySetting);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyParamRepeatTapTrainingMode
#pragma pack(push, 1)
    struct SystemNotifyParamRepeatTapTrainingMode
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::REPEAT_TAP_TRAINING_MODE}; // 0x1
        mdr::v2::t1::Key key{mdr::v2::t1::Key::LEFT_SIDE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        RepeatTapAction action{RepeatTapAction::MODE_IN}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyParamRepeatTapTrainingMode);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyParamSVACommand
#pragma pack(push, 1)
    struct SystemNotifyParamSVACommand
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SONY_VOICE_ASSISTANT_COMMAND}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue trainingModeOnOffValue{OnOffSettingValue::ON}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SVADetectType detectType{SVADetectType::COMMAND}; // 0x3
        // CODEGEN Range 0 255
        UInt8 detectId{}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyParamSVACommand);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyParamUsbBrowser
#pragma pack(push, 1)
    struct SystemNotifyParamUsbBrowser
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::USB_BROWSER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbBrowserCommand usbBrowserCommand{UsbBrowserCommand::BROWSE_DIRECTORIES}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        RequestResult requestResult{RequestResult::ACCEPTED}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyParamUsbBrowser);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyParamUsbBrowserWithIndex
#pragma pack(push, 1)
    struct SystemNotifyParamUsbBrowserWithIndex
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::USB_BROWSER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbBrowserCommand usbBrowserCommand{UsbBrowserCommand::BROWSE_DIRECTORIES}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        RequestResult requestResult{RequestResult::ACCEPTED}; // 0x3
        Int16BE index{}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyParamUsbBrowserWithIndex);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyParamUsbBrowserWithoutIndex
#pragma pack(push, 1)
    struct SystemNotifyParamUsbBrowserWithoutIndex
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::USB_BROWSER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbBrowserCommand usbBrowserCommand{UsbBrowserCommand::BROWSE_DIRECTORIES}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        RequestResult requestResult{RequestResult::ACCEPTED}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyParamUsbBrowserWithoutIndex);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyStatus
#pragma pack(push, 1)
    struct SystemNotifyStatus
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyStatus);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyStatusCommon
#pragma pack(push, 1)
    struct SystemNotifyStatusCommon
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable status{EnableDisable::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyStatusCommon);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyStatusLinkAutoSwitchForSpeaker
#pragma pack(push, 1)
    struct SystemNotifyStatusLinkAutoSwitchForSpeaker
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::LINK_AUTO_SWITCH_FOR_SPEAKER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable status{EnableDisable::ENABLE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue stereoPairStatus{OnOffSettingValue::ON}; // 0x3
        Int16BE partnerSpeakerIdentifier{}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyStatusLinkAutoSwitchForSpeaker);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyStatusRepeatTapTrainingMode
#pragma pack(push, 1)
    struct SystemNotifyStatusRepeatTapTrainingMode
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::REPEAT_TAP_TRAINING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        RepeatTapTrainingModeStatus status{RepeatTapTrainingModeStatus::IN_TRAINING_MODE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyStatusRepeatTapTrainingMode);
    };
#pragma pack(pop)

    // THMSGV2T2SystemNotifyStatusWearingStatusChecker
#pragma pack(push, 1)
    struct SystemNotifyStatusWearingStatusChecker
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        WearingStatusCode status{WearingStatusCode::NORMAL}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyStatusWearingStatusChecker);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetCapability
#pragma pack(push, 1)
    struct SystemRetCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetCapability);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetCapabilityFunctionChange
    struct SystemRetCapabilityFunctionChange
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::FUNCTION_CHANGE}; // 0x1
        MDRPodArray<PlaybackFunction> functions; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemRetCapabilityFunctionChange);
    };

    // THMSGV2T2SystemRetCapabilityLightingDefaultColorColorType
    struct SystemRetCapabilityLightingDefaultColorColorType
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::LIGHTING_DEFAULT_COLOR_COLOR_TYPE}; // 0x1
        MDRPodArray<LightingColorType> colorTypeList; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemRetCapabilityLightingDefaultColorColorType);
    };

    // THMSGV2T2SystemRetCapabilityLightingMode
    struct SystemRetCapabilityLightingMode
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::LIGHTING_MODE}; // 0x1
        MDRPodArray<LightingMode> lightingModeList; // 0x2
        MDRPodArray<ExclusiveFunctionId> exclusiveFunctionIdList;

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemRetCapabilityLightingMode);
    };

    // THMSGV2T2SystemRetCapabilityLinkAutoSwitchForSpeaker
#pragma pack(push, 1)
    struct SystemRetCapabilityLinkAutoSwitchForSpeaker
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::LINK_AUTO_SWITCH_FOR_SPEAKER}; // 0x1
        Int16BE speakerIdentifier{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetCapabilityLinkAutoSwitchForSpeaker);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetCapabilityMicOnOffByHeadphoneOperation
#pragma pack(push, 1)
    struct SystemRetCapabilityMicOnOffByHeadphoneOperation
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::MIC_ON_OFF_BY_HEADPHONE_OPERATION}; // 0x1
        mdr::v2::t1::Type keyType{mdr::v2::t1::Type::TOUCH_SENSOR}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetCapabilityMicOnOffByHeadphoneOperation);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetCapabilitySVACommand
    struct SystemRetCapabilitySVACommand
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SONY_VOICE_ASSISTANT_COMMAND}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        DataContinue dataContinue{DataContinue::NOT_CONTINUE_DATA}; // 0x2
        // CODEGEN Range 0 255
        UInt8 dataLength{}; // 0x3
        MDRPodArray<UInt8> data; // 0x4

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemRetCapabilitySVACommand);
    };

    // THMSGV2T2SystemRetCapabilitySVASettingMtkTransferSupportLanguageSwitch
    struct SystemRetCapabilitySVASettingMtkTransferSupportLanguageSwitch
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SONY_VOICE_ASSISTANT_SETTING_MTK_TRANSFER_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Range 0 255
        UInt8 value2{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable resumableStatus{EnableDisable::ENABLE}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable trueWirelessStatus{EnableDisable::ENABLE}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable bGTransferStatus{EnableDisable::ENABLE}; // 0x5
        MDRPodArray<SonyVoiceAssistantLanguage> supportLanguages; // 0x6

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemRetCapabilitySVASettingMtkTransferSupportLanguageSwitch);
    };

    // THMSGV2T2SystemRetCapabilityUsbBrowser
#pragma pack(push, 1)
    struct SystemRetCapabilityUsbBrowser
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::USB_BROWSER}; // 0x1
        Int16BE numberOfItemsInOneTransaction{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetCapabilityUsbBrowser);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetCapabilityVoiceAssistantWithSpecificLinkSupport
    struct SystemRetCapabilityVoiceAssistantWithSpecificLinkSupport
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VOICE_ASSISTANT_WITH_SPECIFIC_SETUP_LINK_SUPPORT}; // 0x1
        Array<UInt8, 17> bluetoothAddressForVASetupLink{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemRetCapabilityVoiceAssistantWithSpecificLinkSupport);
    };

    // THMSGV2T2SystemRetCapabilityWearingDeviceInformation
#pragma pack(push, 1)
    struct SystemRetCapabilityWearingDeviceInformation
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_DEVICE_INFORMATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EarphoneShape earphoneShape{EarphoneShape::CANAL_TYPE}; // 0x2
        // CODEGEN Range 1 255
        UInt8 numOfDeviceSupporter{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetCapabilityWearingDeviceInformation);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetExtendedParam
#pragma pack(push, 1)
    struct SystemRetExtendedParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_EXTENDED_PARAM
        Command command{Command::SYSTEM_RET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetExtendedParam);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetExtendedParamSVASettingMtkTransferSupportLangSwitch_ServiceInformation
    struct SystemRetExtendedParamSVASettingMtkTransferSupportLangSwitch_ServiceInformation
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SonyVoiceAssistantLanguage language{SonyVoiceAssistantLanguage::UNDEFINED_LANGUAGE}; // 0x0
        MDRPrefixedString serviceId; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(SystemRetExtendedParamSVASettingMtkTransferSupportLangSwitch_ServiceInformation);
    };

    // THMSGV2T2SystemRetExtendedParamUsbBrowser
#pragma pack(push, 1)
    struct SystemRetExtendedParamUsbBrowser
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_EXTENDED_PARAM
        Command command{Command::SYSTEM_RET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::USB_BROWSER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbInformationType informationType{UsbInformationType::SIZE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetExtendedParamUsbBrowser);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetExtendedParamUsbBrowserForDirectoryStatus
#pragma pack(push, 1)
    struct SystemRetExtendedParamUsbBrowserForDirectoryStatus
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_EXTENDED_PARAM
        Command command{Command::SYSTEM_RET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::USB_BROWSER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbInformationType informationType{UsbInformationType::DIRECTORY}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbLayerStatus currentLayerStatus{UsbLayerStatus::SOME_ITEM_IS_PLAYING}; // 0x3
        Int16BE directoryIndex{}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetExtendedParamUsbBrowserForDirectoryStatus);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetExtendedParamUsbBrowserForFileStatus
#pragma pack(push, 1)
    struct SystemRetExtendedParamUsbBrowserForFileStatus
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_EXTENDED_PARAM
        Command command{Command::SYSTEM_RET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::USB_BROWSER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbInformationType informationType{UsbInformationType::FILE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbLayerStatus currentLayerStatus{UsbLayerStatus::SOME_ITEM_IS_PLAYING}; // 0x3
        Int16BE currentDirectoryIndex{}; // 0x4
        Int16BE fileIndex{}; // 0x6

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetExtendedParamUsbBrowserForFileStatus);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetExtendedParamUsbBrowserForSize
#pragma pack(push, 1)
    struct SystemRetExtendedParamUsbBrowserForSize
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_EXTENDED_PARAM
        Command command{Command::SYSTEM_RET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::USB_BROWSER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbInformationType informationType{UsbInformationType::SIZE}; // 0x2
        Int16BE size{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetExtendedParamUsbBrowserForSize);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetParam
#pragma pack(push, 1)
    struct SystemRetParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetParam);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetParamAutoVolumeWithLimitation
    struct SystemRetParamAutoVolumeWithLimitation
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_VOLUME_WITH_LIMITATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue settingValue{OnOffSettingValue::ON}; // 0x2
        MDRPodArray<LimitationType> limitationTypeList; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemRetParamAutoVolumeWithLimitation);
    };

    // THMSGV2T2SystemRetParamCommon
#pragma pack(push, 1)
    struct SystemRetParamCommon
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue settingValue{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetParamCommon);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetParamFunctionChange
#pragma pack(push, 1)
    struct SystemRetParamFunctionChange
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::FUNCTION_CHANGE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackFunction function{PlaybackFunction::AUDIO_IN}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetParamFunctionChange);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetParamLightingDefaultColorColorType
#pragma pack(push, 1)
    struct SystemRetParamLightingDefaultColorColorType
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::LIGHTING_DEFAULT_COLOR_COLOR_TYPE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LightingColorType colorType{LightingColorType::MULTI_COLOR}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetParamLightingDefaultColorColorType);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetParamLightingDefaultColorCustomColor
#pragma pack(push, 1)
    struct SystemRetParamLightingDefaultColorCustomColor
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::LIGHTING_DEFAULT_COLOR_CUSTOM_COLOR}; // 0x1
        // CODEGEN Range 0 255
        UInt8 red{}; // 0x2
        // CODEGEN Range 0 255
        UInt8 green{}; // 0x3
        // CODEGEN Range 0 255
        UInt8 blue{}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetParamLightingDefaultColorCustomColor);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetParamLightingMode
#pragma pack(push, 1)
    struct SystemRetParamLightingMode
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::LIGHTING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LightingMode lightingMode{LightingMode::LIGHT_OFF}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetParamLightingMode);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetParamLinkAutoSwitchForHeadsets
    struct SystemRetParamLinkAutoSwitchForHeadsets
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::LINK_AUTO_SWITCH_FOR_HEADSETS}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue linkAutoSwitchStatus{OnOffSettingValue::ON}; // 0x2
        Int16BE speakerIdentifier{}; // 0x3
        Array<UInt8, 17> smartPhoneBtAddress{}; // 0x5

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemRetParamLinkAutoSwitchForHeadsets);
    };

    // THMSGV2T2SystemRetParamLinkAutoSwitchForSpeaker
    struct SystemRetParamLinkAutoSwitchForSpeaker
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::LINK_AUTO_SWITCH_FOR_SPEAKER}; // 0x1
        MDRArray<MDRPrefixedString> smartPhoneBDAddressList; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemRetParamLinkAutoSwitchForSpeaker);
    };

    // THMSGV2T2SystemRetParamMicOnOffByHeadphoneOperation
#pragma pack(push, 1)
    struct SystemRetParamMicOnOffByHeadphoneOperation
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::MIC_ON_OFF_BY_HEADPHONE_OPERATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue micOnOffByHeadphoneOperation{OnOffSettingValue::ON}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue micOffSoundEffect{OnOffSettingValue::ON}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetParamMicOnOffByHeadphoneOperation);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetParamSVASettingMtkTransferSupportLanguageSwitch
#pragma pack(push, 1)
    struct SystemRetParamSVASettingMtkTransferSupportLanguageSwitch
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SONY_VOICE_ASSISTANT_SETTING_MTK_TRANSFER_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SonyVoiceAssistantLanguage currentLanguage{SonyVoiceAssistantLanguage::UNDEFINED_LANGUAGE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetParamSVASettingMtkTransferSupportLanguageSwitch);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetStatus
#pragma pack(push, 1)
    struct SystemRetStatus
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetStatusCommon
#pragma pack(push, 1)
    struct SystemRetStatusCommon
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable status{EnableDisable::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetStatusCommon);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetStatusLinkAutoSwitchForSpeaker
#pragma pack(push, 1)
    struct SystemRetStatusLinkAutoSwitchForSpeaker
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::LINK_AUTO_SWITCH_FOR_SPEAKER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable status{EnableDisable::ENABLE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue stereoPairStatus{OnOffSettingValue::ON}; // 0x3
        Int16BE partnerSpeakerIdentifier{}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetStatusLinkAutoSwitchForSpeaker);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetStatusRepeatTapTrainingMode
#pragma pack(push, 1)
    struct SystemRetStatusRepeatTapTrainingMode
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::REPEAT_TAP_TRAINING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        RepeatTapTrainingModeStatus status{RepeatTapTrainingModeStatus::IN_TRAINING_MODE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetStatusRepeatTapTrainingMode);
    };
#pragma pack(pop)

    // THMSGV2T2SystemRetStatusWearingStatusChecker
#pragma pack(push, 1)
    struct SystemRetStatusWearingStatusChecker
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        WearingStatusCode status{WearingStatusCode::NORMAL}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetStatusWearingStatusChecker);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetExtendedParam
#pragma pack(push, 1)
    struct SystemSetExtendedParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_EXTENDED_PARAM
        Command command{Command::SYSTEM_SET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetExtendedParam);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetExtendedParamUsbBrowser
#pragma pack(push, 1)
    struct SystemSetExtendedParamUsbBrowser
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_EXTENDED_PARAM
        Command command{Command::SYSTEM_SET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::USB_BROWSER}; // 0x1
        Int16BE startIndex{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetExtendedParamUsbBrowser);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetParam
#pragma pack(push, 1)
    struct SystemSetParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParam);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetParamCommon
#pragma pack(push, 1)
    struct SystemSetParamCommon
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue settingValue{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParamCommon);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetParamFunctionChange
#pragma pack(push, 1)
    struct SystemSetParamFunctionChange
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::FUNCTION_CHANGE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackFunction function{PlaybackFunction::AUDIO_IN}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParamFunctionChange);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetParamLightingDefaultColorColorType
#pragma pack(push, 1)
    struct SystemSetParamLightingDefaultColorColorType
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::LIGHTING_DEFAULT_COLOR_COLOR_TYPE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LightingColorType colorType{LightingColorType::MULTI_COLOR}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParamLightingDefaultColorColorType);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetParamLightingDefaultColorCustomColor
#pragma pack(push, 1)
    struct SystemSetParamLightingDefaultColorCustomColor
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::LIGHTING_DEFAULT_COLOR_CUSTOM_COLOR}; // 0x1
        UInt8 notifyRequired{}; // 0x2
        // CODEGEN Range 0 255
        UInt8 red{}; // 0x3
        // CODEGEN Range 0 255
        UInt8 green{}; // 0x4
        // CODEGEN Range 0 255
        UInt8 blue{}; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParamLightingDefaultColorCustomColor);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetParamLightingMode
#pragma pack(push, 1)
    struct SystemSetParamLightingMode
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::LIGHTING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LightingMode function{LightingMode::LIGHT_OFF}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParamLightingMode);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetParamLinkAutoSwitchForHeadsets
#pragma pack(push, 1)
    struct SystemSetParamLinkAutoSwitchForHeadsets
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::LINK_AUTO_SWITCH_FOR_HEADSETS}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue linkAutoSwitchStatus{OnOffSettingValue::ON}; // 0x2
        Int16BE speakerIdentifier{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParamLinkAutoSwitchForHeadsets);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetParamLinkAutoSwitchForSpeaker
#pragma pack(push, 1)
    struct SystemSetParamLinkAutoSwitchForSpeaker
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::LINK_AUTO_SWITCH_FOR_SPEAKER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LinkAutoSwitchAction linkAutoSwitchAction{LinkAutoSwitchAction::REGISTER}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParamLinkAutoSwitchForSpeaker);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetParamMicOnOffByHeadphoneOperation
#pragma pack(push, 1)
    struct SystemSetParamMicOnOffByHeadphoneOperation
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::MIC_ON_OFF_BY_HEADPHONE_OPERATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue micOnOffByHeadphoneOperation{OnOffSettingValue::ON}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue micOffSoundEffect{OnOffSettingValue::ON}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParamMicOnOffByHeadphoneOperation);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetParamQuickAccessEasySetting
    struct SystemSetParamQuickAccessEasySetting
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::QUICK_ACCESS_EASY_SETTING}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue gattSetting{OnOffSettingValue::ON}; // 0x2
        MDRPodArray<mdr::v2::t1::Preset> presetList; // 0x3
        MDRPodArray<UInt8> quickAccessFunctionList;

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemSetParamQuickAccessEasySetting);
    };

    // THMSGV2T2SystemSetParamSVACommand
#pragma pack(push, 1)
    struct SystemSetParamSVACommand
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::SONY_VOICE_ASSISTANT_COMMAND}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue trainingModeOnOffValue{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParamSVACommand);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetParamUsbBrowser
#pragma pack(push, 1)
    struct SystemSetParamUsbBrowser
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::USB_BROWSER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbBrowserCommand usbBrowserCommand{UsbBrowserCommand::BROWSE_DIRECTORIES}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParamUsbBrowser);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetParamUsbBrowserWithIndex
#pragma pack(push, 1)
    struct SystemSetParamUsbBrowserWithIndex
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::USB_BROWSER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbBrowserCommand usbBrowserCommand{UsbBrowserCommand::BROWSE_DIRECTORIES}; // 0x2
        Int16BE index{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParamUsbBrowserWithIndex);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetParamUsbBrowserWithoutIndex
#pragma pack(push, 1)
    struct SystemSetParamUsbBrowserWithoutIndex
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::USB_BROWSER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UsbBrowserCommand usbBrowserCommand{UsbBrowserCommand::BROWSE_DIRECTORIES}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParamUsbBrowserWithoutIndex);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetParamWearingPositionJudgmentBySensor
#pragma pack(push, 1)
    struct SystemSetParamWearingPositionJudgmentBySensor
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1
        JudgmentModeOperation judgmentModeOperation{JudgmentModeOperation::JUDGMENT_START}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParamWearingPositionJudgmentBySensor);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetStatus
#pragma pack(push, 1)
    struct SystemSetStatus
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_STATUS
        Command command{Command::SYSTEM_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetStatusRepeatTapTrainingMode
#pragma pack(push, 1)
    struct SystemSetStatusRepeatTapTrainingMode
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_STATUS
        Command command{Command::SYSTEM_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::REPEAT_TAP_TRAINING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        RepeatTapTrainingModeOperation modeOperation{RepeatTapTrainingModeOperation::TRAINING_MODE_START}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetStatusRepeatTapTrainingMode);
    };
#pragma pack(pop)

    // THMSGV2T2SystemSetStatusSVASettingMtkSupportLanguageSwitch
#pragma pack(push, 1)
    struct SystemSetStatusSVASettingMtkSupportLanguageSwitch
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_STATUS
        Command command{Command::SYSTEM_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SONY_VOICE_ASSISTANT_SETTING_MTK_TRANSFER_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable status{EnableDisable::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetStatusSVASettingMtkSupportLanguageSwitch);
    };
#pragma pack(pop)

    // THMSGV2T2UsbItemDirectoryInfo
    struct UsbItemDirectoryInfo
    {
        UInt8 directoryIndex{}; // 0x0
        MDRPrefixedString name; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(UsbItemDirectoryInfo);
    };

    // THMSGV2T2UsbItemFileInfo
    struct UsbItemFileInfo
    {
        UInt8 currentDirectoryIndex{}; // 0x0
        UInt8 fileIndex{}; // 0x1
        MDRPrefixedString name; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(UsbItemFileInfo);
    };

    // THMSGV2T2VoiceGuidanceGetCapability
#pragma pack(push, 1)
    struct VoiceGuidanceGetCapability
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_GET_CAPABILITY
        Command command{Command::VOICE_GUIDANCE_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceGetCapability);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceGetExtendedParam
#pragma pack(push, 1)
    struct VoiceGuidanceGetExtendedParam
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_GET_PARAM
        Command command{Command::VOICE_GUIDANCE_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceGetExtendedParam);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceGetParam
#pragma pack(push, 1)
    struct VoiceGuidanceGetParam
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_GET_PARAM
        Command command{Command::VOICE_GUIDANCE_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceGetParam);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceGetStatus
#pragma pack(push, 1)
    struct VoiceGuidanceGetStatus
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_GET_STATUS
        Command command{Command::VOICE_GUIDANCE_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceGetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceGetStatusCommon
#pragma pack(push, 1)
    struct VoiceGuidanceGetStatusCommon
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_GET_STATUS
        Command command{Command::VOICE_GUIDANCE_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceGetStatusCommon);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceGetStatusCommonWithStatusType
#pragma pack(push, 1)
    struct VoiceGuidanceGetStatusCommonWithStatusType
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_GET_STATUS
        Command command{Command::VOICE_GUIDANCE_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceStatusType statusType{VoiceGuidanceStatusType::ON_OFF}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceGetStatusCommonWithStatusType);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceNotifyParam
    struct VoiceGuidanceNotifyParam
    {
        Command command2{Command::CONNECT_GET_SUPPORT_FUNCTION}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::BATTERY_LV_VOICE}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(VoiceGuidanceNotifyParam);
    };

    // THMSGV2T2VoiceGuidanceNotifyParamSettingMtk
    struct VoiceGuidanceNotifyParamSettingMtk
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue settingValue{OnOffSettingValue::ON}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(VoiceGuidanceNotifyParamSettingMtk);
    };

    // THMSGV2T2VoiceGuidanceNotifyParamSettingOnOff
    struct VoiceGuidanceNotifyParamSettingOnOff
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::ONLY_ON_OFF_SETTING}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue settingValue{OnOffSettingValue::ON}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(VoiceGuidanceNotifyParamSettingOnOff);
    };

    // THMSGV2T2VoiceGuidanceNotifyParamSettingSupportLangSwitch
    struct VoiceGuidanceNotifyParamSettingSupportLangSwitch
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue settingValue{OnOffSettingValue::ON}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceLanguage languageValue{VoiceGuidanceLanguage::UNDEFINED_LANGUAGE}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(VoiceGuidanceNotifyParamSettingSupportLangSwitch);
    };

    // THMSGV2T2VoiceGuidanceNotifyParamVolume
    struct VoiceGuidanceNotifyParamVolume
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::VOLUME}; // 0x0
        // CODEGEN Range -2 2
        Int8 volumeValue{}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(VoiceGuidanceNotifyParamVolume);
    };

    // THMSGV2T2VoiceGuidanceNotifyStatus
#pragma pack(push, 1)
    struct VoiceGuidanceNotifyStatus
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_NTFY_STATUS
        Command command{Command::VOICE_GUIDANCE_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceNotifyStatus);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceNotifyStatusCommon
#pragma pack(push, 1)
    struct VoiceGuidanceNotifyStatusCommon
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_NTFY_STATUS
        Command command{Command::VOICE_GUIDANCE_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable status{EnableDisable::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceNotifyStatusCommon);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceNotifyStatusSettingMtkNotSupportLangSwitch
#pragma pack(push, 1)
    struct VoiceGuidanceNotifyStatusSettingMtkNotSupportLangSwitch
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_NTFY_STATUS
        Command command{Command::VOICE_GUIDANCE_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceStatusType statusType{VoiceGuidanceStatusType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable status{EnableDisable::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceNotifyStatusSettingMtkNotSupportLangSwitch);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceNotifyStatusSettingMtkSupportLangSwitch
#pragma pack(push, 1)
    struct VoiceGuidanceNotifyStatusSettingMtkSupportLangSwitch
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_NTFY_STATUS
        Command command{Command::VOICE_GUIDANCE_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceStatusType statusType{VoiceGuidanceStatusType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable status{EnableDisable::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceNotifyStatusSettingMtkSupportLangSwitch);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceNotifyStatusSettingSupportLangSwitch
#pragma pack(push, 1)
    struct VoiceGuidanceNotifyStatusSettingSupportLangSwitch
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_NTFY_STATUS
        Command command{Command::VOICE_GUIDANCE_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceStatusType statusType{VoiceGuidanceStatusType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable status{EnableDisable::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceNotifyStatusSettingSupportLangSwitch);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceRetCapability
#pragma pack(push, 1)
    struct VoiceGuidanceRetCapability
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_CAPABILITY
        Command command{Command::VOICE_GUIDANCE_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetCapability);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceRetCapabilitySettingMtk
#pragma pack(push, 1)
    struct VoiceGuidanceRetCapabilitySettingMtk
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_CAPABILITY
        Command command{Command::VOICE_GUIDANCE_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Range 0 255
        UInt8 value2{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable resumableStatus{EnableDisable::ENABLE}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable trueWirelessStatus{EnableDisable::ENABLE}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable bGTransferStatus{EnableDisable::ENABLE}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceSupportsSwitch supportOnOffSwitch{VoiceGuidanceSupportsSwitch::NOT_SUPPORT}; // 0x6

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetCapabilitySettingMtk);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceRetCapabilitySettingMtkNotSupportLangSwitch
#pragma pack(push, 1)
    struct VoiceGuidanceRetCapabilitySettingMtkNotSupportLangSwitch
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_CAPABILITY
        Command command{Command::VOICE_GUIDANCE_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Range 0 255
        UInt8 value2{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable resumableStatus{EnableDisable::ENABLE}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable trueWirelessStatus{EnableDisable::ENABLE}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable bGTransferStatus{EnableDisable::ENABLE}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceSupportsSwitch supportOnOffSwitch{VoiceGuidanceSupportsSwitch::NOT_SUPPORT}; // 0x6

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetCapabilitySettingMtkNotSupportLangSwitch);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceRetCapabilitySettingMtkSupportLangSwitch
    struct VoiceGuidanceRetCapabilitySettingMtkSupportLangSwitch
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_CAPABILITY
        Command command{Command::VOICE_GUIDANCE_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Range 0 255
        UInt8 value2{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable resumableStatus{EnableDisable::ENABLE}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable trueWirelessStatus{EnableDisable::ENABLE}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable bGTransferStatus{EnableDisable::ENABLE}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceSupportsSwitch supportOnOffSwitch{VoiceGuidanceSupportsSwitch::NOT_SUPPORT}; // 0x6
        MDRPodArray<VoiceGuidanceLanguage> supportLanguages; // 0x7

        MDR_DEFINE_EXTERN_SERIALIZATION(VoiceGuidanceRetCapabilitySettingMtkSupportLangSwitch);
    };

    // THMSGV2T2VoiceGuidanceRetCapabilitySettingSupportLangSwitch
    struct VoiceGuidanceRetCapabilitySettingSupportLangSwitch
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_CAPABILITY
        Command command{Command::VOICE_GUIDANCE_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceSupportsSwitch supportOnOffSwitch{VoiceGuidanceSupportsSwitch::NOT_SUPPORT}; // 0x2
        MDRPodArray<VoiceGuidanceLanguage> supportLanguages; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(VoiceGuidanceRetCapabilitySettingSupportLangSwitch);
    };

    // THMSGV2T2VoiceGuidanceRetExtendedParam
#pragma pack(push, 1)
    struct VoiceGuidanceRetExtendedParam
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetExtendedParam);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceRetExtendedParamSettingMtkSupportLangSwitch_ServiceInformation
    struct VoiceGuidanceRetExtendedParamSettingMtkSupportLangSwitch_ServiceInformation
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceLanguage language{VoiceGuidanceLanguage::UNDEFINED_LANGUAGE}; // 0x0
        MDRPrefixedString serviceId; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(VoiceGuidanceRetExtendedParamSettingMtkSupportLangSwitch_ServiceInformation);
    };

    // THMSGV2T2VoiceGuidanceRetParam
#pragma pack(push, 1)
    struct VoiceGuidanceRetParam
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetParam);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceRetParamSettingMtk
#pragma pack(push, 1)
    struct VoiceGuidanceRetParamSettingMtk
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue settingValue{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetParamSettingMtk);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceRetParamSettingMtkSupportLangSwitchAndSupportLangSwitch
#pragma pack(push, 1)
    struct VoiceGuidanceRetParamSettingMtkSupportLangSwitchAndSupportLangSwitch
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue settingValue{OnOffSettingValue::ON}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceLanguage currentLanguage{VoiceGuidanceLanguage::UNDEFINED_LANGUAGE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetParamSettingMtkSupportLangSwitchAndSupportLangSwitch);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceRetParamSettingOnOff
#pragma pack(push, 1)
    struct VoiceGuidanceRetParamSettingOnOff
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::ONLY_ON_OFF_SETTING}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue settingValue{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetParamSettingOnOff);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceRetParamVolume
#pragma pack(push, 1)
    struct VoiceGuidanceRetParamVolume
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::VOLUME}; // 0x1
        // CODEGEN Range -2 2
        Int8 volumeValue{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetParamVolume);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceRetStatus
#pragma pack(push, 1)
    struct VoiceGuidanceRetStatus
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_STATUS
        Command command{Command::VOICE_GUIDANCE_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceRetStatusCommon
#pragma pack(push, 1)
    struct VoiceGuidanceRetStatusCommon
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_STATUS
        Command command{Command::VOICE_GUIDANCE_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable status{EnableDisable::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetStatusCommon);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceRetStatusSettingMtkNotSupportLangSwitch
#pragma pack(push, 1)
    struct VoiceGuidanceRetStatusSettingMtkNotSupportLangSwitch
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_STATUS
        Command command{Command::VOICE_GUIDANCE_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceStatusType statusType{VoiceGuidanceStatusType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable status{EnableDisable::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetStatusSettingMtkNotSupportLangSwitch);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceRetStatusSettingMtkSupportLangSwitch
#pragma pack(push, 1)
    struct VoiceGuidanceRetStatusSettingMtkSupportLangSwitch
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_STATUS
        Command command{Command::VOICE_GUIDANCE_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceStatusType statusType{VoiceGuidanceStatusType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable status{EnableDisable::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetStatusSettingMtkSupportLangSwitch);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceRetStatusSettingSupportLangSwitch
#pragma pack(push, 1)
    struct VoiceGuidanceRetStatusSettingSupportLangSwitch
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_STATUS
        Command command{Command::VOICE_GUIDANCE_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceStatusType statusType{VoiceGuidanceStatusType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable status{EnableDisable::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetStatusSettingSupportLangSwitch);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceSetParam
#pragma pack(push, 1)
    struct VoiceGuidanceSetParam
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_SET_PARAM
        Command command{Command::VOICE_GUIDANCE_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceSetParam);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceSetParamSettingMtk
#pragma pack(push, 1)
    struct VoiceGuidanceSetParamSettingMtk
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_SET_PARAM
        Command command{Command::VOICE_GUIDANCE_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue settingValue{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceSetParamSettingMtk);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceSetParamSettingOnOff
#pragma pack(push, 1)
    struct VoiceGuidanceSetParamSettingOnOff
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_SET_PARAM
        Command command{Command::VOICE_GUIDANCE_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::ONLY_ON_OFF_SETTING}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue settingValue{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceSetParamSettingOnOff);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceSetParamSettingSupportLangSwitch
#pragma pack(push, 1)
    struct VoiceGuidanceSetParamSettingSupportLangSwitch
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_SET_PARAM
        Command command{Command::VOICE_GUIDANCE_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue settingValue{OnOffSettingValue::ON}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceLanguage languageValue{VoiceGuidanceLanguage::UNDEFINED_LANGUAGE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceSetParamSettingSupportLangSwitch);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceSetParamVolume
#pragma pack(push, 1)
    struct VoiceGuidanceSetParamVolume
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_SET_PARAM
        Command command{Command::VOICE_GUIDANCE_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::VOLUME}; // 0x1
        // CODEGEN Range -2 2
        Int8 volumeValue{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue feedbackSound{OnOffSettingValue::ON}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceSetParamVolume);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceSetStatus
#pragma pack(push, 1)
    struct VoiceGuidanceSetStatus
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_SET_STATUS
        Command command{Command::VOICE_GUIDANCE_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceSetStatus);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceSetStatusSettingMtkNotSupportLangSwitch
#pragma pack(push, 1)
    struct VoiceGuidanceSetStatusSettingMtkNotSupportLangSwitch
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_SET_STATUS
        Command command{Command::VOICE_GUIDANCE_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceStatusType statusType{VoiceGuidanceStatusType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable status{EnableDisable::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceSetStatusSettingMtkNotSupportLangSwitch);
    };
#pragma pack(pop)

    // THMSGV2T2VoiceGuidanceSetStatusSettingMtkSupportLangSwitch
#pragma pack(push, 1)
    struct VoiceGuidanceSetStatusSettingMtkSupportLangSwitch
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_SET_STATUS
        Command command{Command::VOICE_GUIDANCE_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceStatusType statusType{VoiceGuidanceStatusType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable status{EnableDisable::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceSetStatusSettingMtkSupportLangSwitch);
    };
#pragma pack(pop)

    // THMSGV2T2WearingPositionAccelerometerData
    struct WearingPositionAccelerometerData
    {
        UInt8 axisX{}; // 0x0
        UInt8 axisY{}; // 0x1
        UInt8 axisZ{}; // 0x2
        MDRPodArray<UInt8> axisXBytes; // 0x3
        MDRPodArray<UInt8> axisYBytes;
        MDRPodArray<UInt8> axisZBytes;

        MDR_DEFINE_EXTERN_READ_WRITE(WearingPositionAccelerometerData);
    };

    // THMSGV2T2WearingPositionJudgmentBySensorResultSet
    struct WearingPositionJudgmentBySensorResultSet
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        WearingPositionResultCode wearingPositionResultCode{WearingPositionResultCode::NONE}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ResultCodeValue resultCodeValue{ResultCodeValue::OK}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(WearingPositionJudgmentBySensorResultSet);
    };

    // THMSGV2T2PartyRetCapabilityIllumination
    struct PartyRetCapabilityIllumination
    {
        // CODEGEN EnumRange Command::PARTY_RET_CAPABILITY
        Command command{Command::PARTY_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::ILLUMINATION}; // 0x1
        MDRPodArray<PartyIlluminationInfo> partyIlluminationInfoList; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(PartyRetCapabilityIllumination);
    };

    // THMSGV2T2PeripheralNotifyParamPairingDeviceManagementClassicBt
    struct PeripheralNotifyParamPairingDeviceManagementClassicBt
    {
        // CODEGEN EnumRange Command::PERI_NTFY_PARAM
        Command command{Command::PERI_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1
        MDRArray<PeripheralDeviceInfoWithoutBluetoothClassOfDevice> deviceInfo; // 0x2
        // CODEGEN Range 0 255
        UInt8 playbackrightDevice{};

        MDR_DEFINE_EXTERN_SERIALIZATION(PeripheralNotifyParamPairingDeviceManagementClassicBt);
    };

    // THMSGV2T2PeripheralNotifyParamPairingDeviceManagementWithBluetoothClassOfDevice
    struct PeripheralNotifyParamPairingDeviceManagementWithBluetoothClassOfDevice
    {
        // CODEGEN EnumRange Command::PERI_NTFY_PARAM
        Command command{Command::PERI_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE}; // 0x1
        MDRArray<PeripheralDeviceInfo> deviceInfo; // 0x2
        // CODEGEN Range 0 255
        UInt8 playbackrightDevice{};

        MDR_DEFINE_EXTERN_SERIALIZATION(PeripheralNotifyParamPairingDeviceManagementWithBluetoothClassOfDevice);
    };

    // THMSGV2T2PeripheralRetParamPairingDeviceManagementClassicBt
    struct PeripheralRetParamPairingDeviceManagementClassicBt
    {
        // CODEGEN EnumRange Command::PERI_RET_PARAM
        Command command{Command::PERI_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1
        MDRArray<PeripheralDeviceInfoWithoutBluetoothClassOfDevice> deviceInfo; // 0x2
        // CODEGEN Range 0 255
        UInt8 playbackrightDevice{};

        MDR_DEFINE_EXTERN_SERIALIZATION(PeripheralRetParamPairingDeviceManagementClassicBt);
    };

    // THMSGV2T2PeripheralRetParamPairingDeviceManagementWithBluetoothClassOfDevice
    struct PeripheralRetParamPairingDeviceManagementWithBluetoothClassOfDevice
    {
        // CODEGEN EnumRange Command::PERI_RET_PARAM
        Command command{Command::PERI_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE}; // 0x1
        MDRArray<PeripheralDeviceInfo> deviceInfo; // 0x2
        // CODEGEN Range 0 255
        UInt8 playbackrightDevice{};

        MDR_DEFINE_EXTERN_SERIALIZATION(PeripheralRetParamPairingDeviceManagementWithBluetoothClassOfDevice);
    };

    // THMSGV2T2SafeListeningNotifyStatusHbs
    struct SafeListeningNotifyStatusHbs
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_STATUS
        Command command{Command::SAFE_LISTENING_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatus{SafeListeningLogDataStatus::DISCONNECTED}; // 0x2
        MDRPodArray<SafeListeningData> safeListeningDatas; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(SafeListeningNotifyStatusHbs);
    };

    // THMSGV2T2SafeListeningNotifyStatusHbs1
    struct SafeListeningNotifyStatusHbs1
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_STATUS
        Command command{Command::SAFE_LISTENING_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatus{SafeListeningLogDataStatus::DISCONNECTED}; // 0x2
        MDRPodArray<SafeListeningData1> logDataList; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(SafeListeningNotifyStatusHbs1);
    };

    // THMSGV2T2SafeListeningNotifyStatusHbs2
    struct SafeListeningNotifyStatusHbs2
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_STATUS
        Command command{Command::SAFE_LISTENING_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_2}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatus{SafeListeningLogDataStatus::DISCONNECTED}; // 0x2
        MDRPodArray<SafeListeningData2> logDataList; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(SafeListeningNotifyStatusHbs2);
    };

    // THMSGV2T2SafeListeningNotifyStatusTws
    struct SafeListeningNotifyStatusTws
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_STATUS
        Command command{Command::SAFE_LISTENING_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatusLeft{SafeListeningLogDataStatus::DISCONNECTED}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatusRight{SafeListeningLogDataStatus::DISCONNECTED}; // 0x3
        MDRPodArray<SafeListeningData> safeListeningDatas; // 0x4

        MDR_DEFINE_EXTERN_SERIALIZATION(SafeListeningNotifyStatusTws);
    };

    // THMSGV2T2SafeListeningNotifyStatusTws1
    struct SafeListeningNotifyStatusTws1
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_STATUS
        Command command{Command::SAFE_LISTENING_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_TWS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatusLeft{SafeListeningLogDataStatus::DISCONNECTED}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatusRight{SafeListeningLogDataStatus::DISCONNECTED}; // 0x3
        MDRPodArray<SafeListeningData1> logDataList; // 0x4

        MDR_DEFINE_EXTERN_SERIALIZATION(SafeListeningNotifyStatusTws1);
    };

    // THMSGV2T2SafeListeningNotifyStatusTws2
    struct SafeListeningNotifyStatusTws2
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_STATUS
        Command command{Command::SAFE_LISTENING_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_TWS_2}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatusLeft{SafeListeningLogDataStatus::DISCONNECTED}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatusRight{SafeListeningLogDataStatus::DISCONNECTED}; // 0x3
        MDRPodArray<SafeListeningData2> logDataList; // 0x4

        MDR_DEFINE_EXTERN_SERIALIZATION(SafeListeningNotifyStatusTws2);
    };

    // THMSGV2T2SafeListeningRetStatusHbs
    struct SafeListeningRetStatusHbs
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_RET_STATUS
        Command command{Command::SAFE_LISTENING_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatus{SafeListeningLogDataStatus::DISCONNECTED}; // 0x2
        SafeListeningData safeListeningData{}; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(SafeListeningRetStatusHbs);
    };

    // THMSGV2T2SafeListeningRetStatusHbs1
    struct SafeListeningRetStatusHbs1
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_RET_STATUS
        Command command{Command::SAFE_LISTENING_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatus{SafeListeningLogDataStatus::DISCONNECTED}; // 0x2
        SafeListeningData1 currentData{}; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(SafeListeningRetStatusHbs1);
    };

    // THMSGV2T2SafeListeningRetStatusHbs2
    struct SafeListeningRetStatusHbs2
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_RET_STATUS
        Command command{Command::SAFE_LISTENING_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_2}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatus{SafeListeningLogDataStatus::DISCONNECTED}; // 0x2
        SafeListeningData2 currentData{}; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(SafeListeningRetStatusHbs2);
    };

    // THMSGV2T2SafeListeningRetStatusTws
    struct SafeListeningRetStatusTws
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_RET_STATUS
        Command command{Command::SAFE_LISTENING_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatusLeft{SafeListeningLogDataStatus::DISCONNECTED}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatusRight{SafeListeningLogDataStatus::DISCONNECTED}; // 0x3
        SafeListeningData safeListeningData{}; // 0x4
        SafeListeningData safeListeningData2{};

        MDR_DEFINE_EXTERN_SERIALIZATION(SafeListeningRetStatusTws);
    };

    // THMSGV2T2SafeListeningRetStatusTws1
    struct SafeListeningRetStatusTws1
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_RET_STATUS
        Command command{Command::SAFE_LISTENING_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_TWS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatusLeft{SafeListeningLogDataStatus::DISCONNECTED}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatusRight{SafeListeningLogDataStatus::DISCONNECTED}; // 0x3
        SafeListeningData1 currentDataLeft{}; // 0x4
        SafeListeningData1 currentDataRight{};

        MDR_DEFINE_EXTERN_SERIALIZATION(SafeListeningRetStatusTws1);
    };

    // THMSGV2T2SafeListeningRetStatusTws2
    struct SafeListeningRetStatusTws2
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_RET_STATUS
        Command command{Command::SAFE_LISTENING_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_TWS_2}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatusLeft{SafeListeningLogDataStatus::DISCONNECTED}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatusRight{SafeListeningLogDataStatus::DISCONNECTED}; // 0x3
        SafeListeningData2 currentDataLeft{}; // 0x4
        SafeListeningData2 currentDataRight{};

        MDR_DEFINE_EXTERN_SERIALIZATION(SafeListeningRetStatusTws2);
    };

    // THMSGV2T2SafeListeningSetStatusHbs
    struct SafeListeningSetStatusHbs
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_SET_STATUS
        Command command{Command::SAFE_LISTENING_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatus{SafeListeningLogDataStatus::DISCONNECTED}; // 0x2
        SafeListeningStatus status{}; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(SafeListeningSetStatusHbs);
    };

    // THMSGV2T2SafeListeningSetStatusTws
    struct SafeListeningSetStatusTws
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_SET_STATUS
        Command command{Command::SAFE_LISTENING_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatusLeft{SafeListeningLogDataStatus::DISCONNECTED}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningLogDataStatus logDataStatusRight{SafeListeningLogDataStatus::DISCONNECTED}; // 0x3
        SafeListeningStatus statusLeft{}; // 0x4
        SafeListeningStatus statusRight{};

        MDR_DEFINE_EXTERN_SERIALIZATION(SafeListeningSetStatusTws);
    };

    // THMSGV2T2SystemNotifyParamWearingPositionJudgmentBySensor
    struct SystemNotifyParamWearingPositionJudgmentBySensor
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_POSITION_JUDGMENT_BY_SENSOR}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        WearingPositionOperationStatus wearingPositionOperationStatus{WearingPositionOperationStatus::JUDGMENT_COMPLETED_SUCCESSFULLY}; // 0x2
        MDRPodArray<WearingPositionJudgmentBySensorResultSet> leftSideParameterList; // 0x3
        MDRArray<WearingPositionAccelerometerData> rightSideParameterList;

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemNotifyParamWearingPositionJudgmentBySensor);
    };

    // THMSGV2T2SystemRetCapabilityRepeatTapTrainingMode
    struct SystemRetCapabilityRepeatTapTrainingMode
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::REPEAT_TAP_TRAINING_MODE}; // 0x1
        MDRArray<RepeatTapKey> keys; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemRetCapabilityRepeatTapTrainingMode);
    };

    // THMSGV2T2SystemRetExtendedParamSVASettingMtkTransferSupportLangSwitch
    struct SystemRetExtendedParamSVASettingMtkTransferSupportLangSwitch
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_EXTENDED_PARAM
        Command command{Command::SYSTEM_RET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::SONY_VOICE_ASSISTANT_SETTING_MTK_TRANSFER_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        UInt8 requiredTime{}; // 0x2
        MDRPrefixedString categoryId; // 0x3
        MDRPrefixedString serialNumber;
        UInt8 threshold{};
        UInt8 thresholdForInterrupt{};
        MDRPrefixedString uniqueId;
        MDRArray<SystemRetExtendedParamSVASettingMtkTransferSupportLangSwitch_ServiceInformation> serviceInformationList;

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemRetExtendedParamSVASettingMtkTransferSupportLangSwitch);
    };

    // THMSGV2T2VoiceGuidanceRetExtendedParamSettingMtkSupportLangSwitch
    struct VoiceGuidanceRetExtendedParamSettingMtkSupportLangSwitch
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        UInt8 requiredTime{}; // 0x2
        MDRPrefixedString categoryId; // 0x3
        MDRPrefixedString serialNumber;
        UInt8 threshold{};
        UInt8 thresholdForInterrupt{};
        MDRPrefixedString uniqueId;
        MDRArray<VoiceGuidanceRetExtendedParamSettingMtkSupportLangSwitch_ServiceInformation> serviceInformationList;

        MDR_DEFINE_EXTERN_SERIALIZATION(VoiceGuidanceRetExtendedParamSettingMtkSupportLangSwitch);
    };
#pragma endregion Declarations
} // namespace mdr::v2::t2

#pragma pack(pop)

#include "Generated/ProtocolV2T2Enum.hpp"
#include "Generated/ProtocolV2T2Traits.hpp"
