#pragma once
#include "ProtocolV2.hpp"
#include "ProtocolV2T1.hpp"
#pragma pack(push, 1)

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
    struct ConnectGetSupportFunction
    {
        // CODEGEN EnumRange Command::CONNECT_GET_SUPPORT_FUNCTION
        Command command{Command::CONNECT_GET_SUPPORT_FUNCTION}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ConnectInquiredType type{ConnectInquiredType::FIXED_VALUE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(ConnectGetSupportFunction);
    };

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
    struct LEAGetCapability
    {
        // CODEGEN EnumRange Command::LEA_GET_CAPABILITY
        Command command{Command::LEA_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEAGetCapability);
    };

    // THMSGV2T2LEAGetParam
    struct LEAGetParam
    {
        // CODEGEN EnumRange Command::LEA_GET_PARAM
        Command command{Command::LEA_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType type{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEAGetParam);
    };

    // THMSGV2T2LEAGetStatus
    struct LEAGetStatus
    {
        // CODEGEN EnumRange Command::LEA_GET_STATUS
        Command command{Command::LEA_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType type{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEAGetStatus);
    };

    // THMSGV2T2LEANotifyParam
    struct LEANotifyParam
    {
        // CODEGEN EnumRange Command::LEA_NTFY_PARAM
        Command command{Command::LEA_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEANotifyParam);
    };

    // THMSGV2T2LEANotifyParamLEAudioConnectionModeWithBTReconnection
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

    // THMSGV2T2LEANotifyStatus
    struct LEANotifyStatus
    {
        // CODEGEN EnumRange Command::LEA_NTFY_STATUS
        Command command{Command::LEA_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType type{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEANotifyStatus);
    };

    // THMSGV2T2LEANotifyStatusCommon
    struct LEANotifyStatusCommon
    {
        // CODEGEN EnumRange Command::LEA_NTFY_STATUS
        Command command{Command::LEA_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType type{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EnableDisable enableDisable{EnableDisable::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEANotifyStatusCommon);
    };

    // THMSGV2T2LEANotifyStatusCommonUnavailableReason
    struct LEANotifyStatusCommonUnavailableReason
    {
        // CODEGEN EnumRange Command::LEA_NTFY_STATUS
        Command command{Command::LEA_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType type{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UnavailableReason unavailableReason{UnavailableReason::UNAVAILABLE_BY_LE_AUDIO_PRIOR}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEANotifyStatusCommonUnavailableReason);
    };

    // THMSGV2T2LEANotifyStatusPasSupportsA2dpLeaUniLeaBroad
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

    // THMSGV2T2LEARetCapability
    struct LEARetCapability
    {
        // CODEGEN EnumRange Command::LEA_RET_CAPABILITY
        Command command{Command::LEA_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEARetCapability);
    };

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
    struct LEARetCapabilityLEAudioSwitchSupportedCompatibility
    {
        // CODEGEN EnumRange Command::LEA_RET_CAPABILITY
        Command command{Command::LEA_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1
        UInt8 compatibilityVersion{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEARetCapabilityLEAudioSwitchSupportedCompatibility);
    };

    // THMSGV2T2LEARetCapabilityPasSupportsA2dpLeaUniLeaBroad
    struct LEARetCapabilityPasSupportsA2dpLeaUniLeaBroad
    {
        // CODEGEN EnumRange Command::LEA_RET_CAPABILITY
        Command command{Command::LEA_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1
        Array<UInt8, 17> deviceUniqueId{}; // 0x2
        MDRPrefixedString bdAddressLE;

        MDR_DEFINE_EXTERN_SERIALIZATION(LEARetCapabilityPasSupportsA2dpLeaUniLeaBroad);
    };

    // THMSGV2T2LEARetParam
    struct LEARetParam
    {
        // CODEGEN EnumRange Command::LEA_RET_PARAM
        Command command{Command::LEA_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEARetParam);
    };

    // THMSGV2T2LEARetParamGetIdentityResolvingKey
    struct LEARetParamGetIdentityResolvingKey
    {
        // CODEGEN EnumRange Command::LEA_RET_PARAM
        Command command{Command::LEA_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1
        MDRPodArray<UInt8> iRK; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(LEARetParamGetIdentityResolvingKey);
    };

    // THMSGV2T2LEARetParamLEAudioConnectionModeWithBTReconnection
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

    // THMSGV2T2LEARetParamPasSupportsA2dpLeaUniLeaBroad
    struct LEARetParamPasSupportsA2dpLeaUniLeaBroad
    {
        // CODEGEN EnumRange Command::LEA_RET_PARAM
        Command command{Command::LEA_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PairedHistory pairedHistory{PairedHistory::BOTH_CLASSIC_BT_BLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEARetParamPasSupportsA2dpLeaUniLeaBroad);
    };

    // THMSGV2T2LEARetStatus
    struct LEARetStatus
    {
        // CODEGEN EnumRange Command::LEA_RET_STATUS
        Command command{Command::LEA_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType type{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEARetStatus);
    };

    // THMSGV2T2LEARetStatusCommonUnavailableReason
    struct LEARetStatusCommonUnavailableReason
    {
        // CODEGEN EnumRange Command::LEA_RET_STATUS
        Command command{Command::LEA_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType type{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UnavailableReason unavailableReason{UnavailableReason::UNAVAILABLE_BY_LE_AUDIO_PRIOR}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEARetStatusCommonUnavailableReason);
    };

    // THMSGV2T2LEARetStatusLEAudioConnectionModeWithBTReconnection
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

    // THMSGV2T2LEARetStatusPasSupportsA2dpLeaUniLeaBroad
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

    // THMSGV2T2LEASetParam
    struct LEASetParam
    {
        // CODEGEN EnumRange Command::LEA_SET_PARAM
        Command command{Command::LEA_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LEAInquiredType inquiredType{LEAInquiredType::LE_AUDIO_CONNECTION_STATE_NOTIFICATION}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(LEASetParam);
    };

    // THMSGV2T2LEASetParamLEAudioConnectionModeWithBTReconnection
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
    struct PartyGetCapability
    {
        // CODEGEN EnumRange Command::PARTY_GET_CAPABILITY
        Command command{Command::PARTY_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyGetCapability);
    };

    // THMSGV2T2PartyGetParam
    struct PartyGetParam
    {
        // CODEGEN EnumRange Command::PARTY_GET_PARAM
        Command command{Command::PARTY_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyGetParam);
    };

    // THMSGV2T2PartyGetStatus
    struct PartyGetStatus
    {
        // CODEGEN EnumRange Command::PARTY_GET_STATUS
        Command command{Command::PARTY_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyGetStatus);
    };

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
    struct PartyNotifyParam
    {
        // CODEGEN EnumRange Command::PARTY_NTFY_PARAM
        Command command{Command::PARTY_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyNotifyParam);
    };

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

    // THMSGV2T2PartyNotifyStatus
    struct PartyNotifyStatus
    {
        // CODEGEN EnumRange Command::PARTY_NTFY_STATUS
        Command command{Command::PARTY_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyNotifyStatus);
    };

    // THMSGV2T2PartyNotifyStatusCommon
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

    // THMSGV2T2PartyNotifyStatusDjControl
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
    struct PartyRetCapability
    {
        // CODEGEN EnumRange Command::PARTY_RET_CAPABILITY
        Command command{Command::PARTY_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyRetCapability);
    };

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
    struct PartyRetParam
    {
        // CODEGEN EnumRange Command::PARTY_RET_PARAM
        Command command{Command::PARTY_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyRetParam);
    };

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

    // THMSGV2T2PartyRetStatus
    struct PartyRetStatus
    {
        // CODEGEN EnumRange Command::PARTY_RET_STATUS
        Command command{Command::PARTY_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartyRetStatus);
    };

    // THMSGV2T2PartyRetStatusCommon
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

    // THMSGV2T2PartyRetStatusDjControl
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
    struct PartySetExtParam
    {
        // CODEGEN EnumRange Command::PARTY_SET_PARAM
        Command command{Command::PARTY_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartySetExtParam);
    };

    // THMSGV2T2PartySetExtParamDjControl
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

    // THMSGV2T2PartySetExtParamIllumination
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

    // THMSGV2T2PartySetExtParamKaraoke
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

    // THMSGV2T2PartySetParam
    struct PartySetParam
    {
        // CODEGEN EnumRange Command::PARTY_SET_PARAM
        Command command{Command::PARTY_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartySetParam);
    };

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

    // THMSGV2T2PartySetStatus
    struct PartySetStatus
    {
        // CODEGEN EnumRange Command::PARTY_SET_STATUS
        Command command{Command::PARTY_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PartyInquiredType inquiredType{PartyInquiredType::DJ_CONTROL}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PartySetStatus);
    };

    // THMSGV2T2PartySetStatusKaraoke
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
    struct PeripheralGetCapability
    {
        // CODEGEN EnumRange Command::PERI_GET_CAPABILITY
        Command command{Command::PERI_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralGetCapability);
    };

    // THMSGV2T2PeripheralGetParam
    struct PeripheralGetParam
    {
        // CODEGEN EnumRange Command::PERI_GET_PARAM
        Command command{Command::PERI_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralGetParam);
    };

    // THMSGV2T2PeripheralGetStatus
    struct PeripheralGetStatus
    {
        // CODEGEN EnumRange Command::PERI_GET_STATUS
        Command command{Command::PERI_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralGetStatus);
    };

    // THMSGV2T2PeripheralNotifyExtendedParam
    struct PeripheralNotifyExtendedParam
    {
        // CODEGEN EnumRange Command::PERI_NTFY_EXTENDED_PARAM
        Command command{Command::PERI_NTFY_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralNotifyExtendedParam);
    };

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
    struct PeripheralNotifyParam
    {
        // CODEGEN EnumRange Command::PERI_NTFY_PARAM
        Command command{Command::PERI_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralNotifyParam);
    };

    // THMSGV2T2PeripheralNotifyParamMusicHandOverSetting
    struct PeripheralNotifyParamMusicHandOverSetting
    {
        // CODEGEN EnumRange Command::PERI_NTFY_PARAM
        Command command{Command::PERI_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::MUSIC_HAND_OVER_SETTING}; // 0x1
        UInt8 on{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralNotifyParamMusicHandOverSetting);
    };

    // THMSGV2T2PeripheralNotifyParamSourceSwitchControl
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

    // THMSGV2T2PeripheralNotifyStatus
    struct PeripheralNotifyStatus
    {
        // CODEGEN EnumRange Command::PERI_NTFY_STATUS
        Command command{Command::PERI_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralNotifyStatus);
    };

    // THMSGV2T2PeripheralNotifyStatusParingDeviceManagementCommon
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

    // THMSGV2T2PeripheralRetCapability
    struct PeripheralRetCapability
    {
        // CODEGEN EnumRange Command::PERI_RET_CAPABILITY
        Command command{Command::PERI_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralRetCapability);
    };

    // THMSGV2T2PeripheralRetCapabilityParingDeviceManagementCommon
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

    // THMSGV2T2PeripheralRetParam
    struct PeripheralRetParam
    {
        // CODEGEN EnumRange Command::PERI_RET_PARAM
        Command command{Command::PERI_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralRetParam);
    };

    // THMSGV2T2PeripheralRetParamMusicHandOverSetting
    struct PeripheralRetParamMusicHandOverSetting
    {
        // CODEGEN EnumRange Command::PERI_RET_PARAM
        Command command{Command::PERI_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::MUSIC_HAND_OVER_SETTING}; // 0x1
        UInt8 on{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralRetParamMusicHandOverSetting);
    };

    // THMSGV2T2PeripheralRetParamSourceSwitchControl
    struct PeripheralRetParamSourceSwitchControl
    {
        // CODEGEN EnumRange Command::PERI_RET_PARAM
        Command command{Command::PERI_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::SOURCE_SWITCH_CONTROL}; // 0x1
        UInt8 value{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralRetParamSourceSwitchControl);
    };

    // THMSGV2T2PeripheralRetStatus
    struct PeripheralRetStatus
    {
        // CODEGEN EnumRange Command::PERI_RET_STATUS
        Command command{Command::PERI_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralRetStatus);
    };

    // THMSGV2T2PeripheralRetStatusPairingDeviceManagementCommon
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

    // THMSGV2T2PeripheralSetExtendedParam
    struct PeripheralSetExtendedParam
    {
        // CODEGEN EnumRange Command::PERI_SET_EXTENDED_PARAM
        Command command{Command::PERI_SET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralSetExtendedParam);
    };

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
    struct PeripheralSetParam
    {
        // CODEGEN EnumRange Command::PERI_SET_PARAM
        Command command{Command::PERI_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralSetParam);
    };

    // THMSGV2T2PeripheralSetParamMusicHandOverSetting
    struct PeripheralSetParamMusicHandOverSetting
    {
        // CODEGEN EnumRange Command::PERI_SET_PARAM
        Command command{Command::PERI_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::MUSIC_HAND_OVER_SETTING}; // 0x1
        UInt8 on{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralSetParamMusicHandOverSetting);
    };

    // THMSGV2T2PeripheralSetParamSourceSwitchControl
    struct PeripheralSetParamSourceSwitchControl
    {
        // CODEGEN EnumRange Command::PERI_SET_PARAM
        Command command{Command::PERI_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::SOURCE_SWITCH_CONTROL}; // 0x1
        UInt8 value{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralSetParamSourceSwitchControl);
    };

    // THMSGV2T2PeripheralSetStatus
    struct PeripheralSetStatus
    {
        // CODEGEN EnumRange Command::PERI_SET_STATUS
        Command command{Command::PERI_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralSetStatus);
    };

    // THMSGV2T2PeripheralSetStatusParingDeviceManagementCommon
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

    // THMSGV2T2PowerGetCapability
    struct PowerGetCapability
    {
        // CODEGEN EnumRange Command::POWER_GET_CAPABILITY
        Command command{Command::POWER_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerGetCapability);
    };

    // THMSGV2T2PowerGetParam
    struct PowerGetParam
    {
        // CODEGEN EnumRange Command::POWER_GET_PARAM
        Command command{Command::POWER_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerGetParam);
    };

    // THMSGV2T2PowerGetStatus
    struct PowerGetStatus
    {
        // CODEGEN EnumRange Command::POWER_GET_STATUS
        Command command{Command::POWER_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerGetStatus);
    };

    // THMSGV2T2PowerNotifyParam
    struct PowerNotifyParam
    {
        // CODEGEN EnumRange Command::POWER_NTFY_PARAM
        Command command{Command::POWER_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerNotifyParam);
    };

    // THMSGV2T2PowerNotifyParamSettingOnOff
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

    // THMSGV2T2PowerNotifyParamUsbSubmersion
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

    // THMSGV2T2PowerNotifyStatus
    struct PowerNotifyStatus
    {
        // CODEGEN EnumRange Command::POWER_NTFY_STATUS
        Command command{Command::POWER_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerNotifyStatus);
    };

    // THMSGV2T2PowerNotifyStatusCaringChargeWithThreshold
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

    // THMSGV2T2PowerNotifyStatusCommon
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

    // THMSGV2T2PowerNotifyStatusUsbSubmersion
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

    // THMSGV2T2PowerRetCapability
    struct PowerRetCapability
    {
        // CODEGEN EnumRange Command::POWER_RET_CAPABILITY
        Command command{Command::POWER_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType inquiredType{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetCapability);
    };

    // THMSGV2T2PowerRetCapabilityAutoStandby
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

    // THMSGV2T2PowerRetCapabilityCaringChargeWithThreshold
    struct PowerRetCapabilityCaringChargeWithThreshold
    {
        // CODEGEN EnumRange Command::POWER_RET_CAPABILITY
        Command command{Command::POWER_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType inquiredType{PowerInquiredType::CARING_CHARGE_WITH_THRESHOLD}; // 0x1
        UInt8 value{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetCapabilityCaringChargeWithThreshold);
    };

    // THMSGV2T2PowerRetCapabilityUsbSubmersion
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

    // THMSGV2T2PowerRetParam
    struct PowerRetParam
    {
        // CODEGEN EnumRange Command::POWER_RET_PARAM
        Command command{Command::POWER_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetParam);
    };

    // THMSGV2T2PowerRetParamSettingOnOff
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

    // THMSGV2T2PowerRetStatus
    struct PowerRetStatus
    {
        // CODEGEN EnumRange Command::POWER_RET_STATUS
        Command command{Command::POWER_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetStatus);
    };

    // THMSGV2T2PowerRetStatusCaringChargeWithThreshold
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

    // THMSGV2T2PowerRetStatusCommon
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

    // THMSGV2T2PowerRetStatusUsbSubmersion
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

    // THMSGV2T2PowerSetParam
    struct PowerSetParam
    {
        // CODEGEN EnumRange Command::POWER_SET_PARAM
        Command command{Command::POWER_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerInquiredType type{PowerInquiredType::AUTO_STANDBY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerSetParam);
    };

    // THMSGV2T2PowerSetParamSettingOnOff
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
    struct SafeListeningGetExtendedParam
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_GET_EXTENDED_PARAM
        Command command{Command::SAFE_LISTENING_GET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningGetExtendedParam);
    };

    // THMSGV2T2SafeListeningGetParam
    struct SafeListeningGetParam
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_GET_PARAM
        Command command{Command::SAFE_LISTENING_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningGetParam);
    };

    // THMSGV2T2SafeListeningGetStatus
    struct SafeListeningGetStatus
    {
        Command command2{Command::CONNECT_GET_SUPPORT_FUNCTION}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(SafeListeningGetStatus);
    };

    // THMSGV2T2SafeListeningNotifyParam
    struct SafeListeningNotifyParam
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_PARAM
        Command command{Command::SAFE_LISTENING_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyParam);
    };

    // THMSGV2T2SafeListeningNotifyParamMaxVolLvLimit
    struct SafeListeningNotifyParamMaxVolLvLimit
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_PARAM
        Command command{Command::SAFE_LISTENING_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        UInt8 idxOfMaxVolLv{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyParamMaxVolLvLimit);
    };

    // THMSGV2T2SafeListeningNotifyParamSL
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

    // THMSGV2T2SafeListeningNotifyParamSVC
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

    // THMSGV2T2SafeListeningNotifyStatus
    struct SafeListeningNotifyStatus
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_STATUS
        Command command{Command::SAFE_LISTENING_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyStatus);
    };

    // THMSGV2T2SafeListeningNotifyStatusMaxVolLvLimit
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

    // THMSGV2T2SafeListeningNotifyStatusSL
    struct SafeListeningNotifyStatusSL
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_NTFY_STATUS
        Command command{Command::SAFE_LISTENING_NTFY_STATUS}; // 0x0

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyStatusSL);
    };

    // THMSGV2T2SafeListeningNotifyStatusSVC
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

    // THMSGV2T2SafeListeningRetParam
    struct SafeListeningRetParam
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_RET_PARAM
        Command command{Command::SAFE_LISTENING_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetParam);
    };

    // THMSGV2T2SafeListeningRetParamMaxVolLvLimit
    struct SafeListeningRetParamMaxVolLvLimit
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_RET_PARAM
        Command command{Command::SAFE_LISTENING_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        UInt8 idxOfMaxVolLv{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetParamMaxVolLvLimit);
    };

    // THMSGV2T2SafeListeningRetParamSL
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

    // THMSGV2T2SafeListeningRetStatus
    struct SafeListeningRetStatus
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_RET_STATUS
        Command command{Command::SAFE_LISTENING_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetStatus);
    };

    // THMSGV2T2SafeListeningRetStatusMaxVolLvLimit
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

    // THMSGV2T2SafeListeningRetStatusSL
    struct SafeListeningRetStatusSL
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_RET_STATUS
        Command command{Command::SAFE_LISTENING_RET_STATUS}; // 0x0

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetStatusSL);
    };

    // THMSGV2T2SafeListeningSetParam
    struct SafeListeningSetParam
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_SET_PARAM
        Command command{Command::SAFE_LISTENING_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetParam);
    };

    // THMSGV2T2SafeListeningSetParamMaxVolLvLimit
    struct SafeListeningSetParamMaxVolLvLimit
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_SET_PARAM
        Command command{Command::SAFE_LISTENING_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1
        UInt8 idxOfMaxVolLv{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetParamMaxVolLvLimit);
    };

    // THMSGV2T2SafeListeningSetParamSL
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

    // THMSGV2T2SafeListeningSetParamSVC
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

    // THMSGV2T2SafeListeningSetStatus
    struct SafeListeningSetStatus
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_SET_STATUS
        Command command{Command::SAFE_LISTENING_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SafeListeningInquiredType inquiredType{SafeListeningInquiredType::SAFE_LISTENING_HBS_1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetStatus);
    };

    // THMSGV2T2SafeListeningSetStatusMaxVolLvLimit
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

    // THMSGV2T2SafeListeningSetStatusSL
    struct SafeListeningSetStatusSL
    {
        // CODEGEN EnumRange Command::SAFE_LISTENING_SET_STATUS
        Command command{Command::SAFE_LISTENING_SET_STATUS}; // 0x0

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetStatusSL);
    };

    // THMSGV2T2SafeListeningSetStatusSVC
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
    struct SystemGetCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_CAPABILITY
        Command command{Command::SYSTEM_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemGetCapability);
    };

    // THMSGV2T2SystemGetCapabilityCommon
    struct SystemGetCapabilityCommon
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_CAPABILITY
        Command command{Command::SYSTEM_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemGetCapabilityCommon);
    };

    // THMSGV2T2SystemGetCapabilitySVACommand
    struct SystemGetCapabilitySVACommand
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_CAPABILITY
        Command command{Command::SYSTEM_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SONY_VOICE_ASSISTANT_COMMAND}; // 0x1
        Int16BE receivedDataLength{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemGetCapabilitySVACommand);
    };

    // THMSGV2T2SystemGetExtendedParam
    struct SystemGetExtendedParam
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_EXTENDED_PARAM
        Command command{Command::SYSTEM_GET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemGetExtendedParam);
    };

    // THMSGV2T2SystemGetExtendedParamCommon
    struct SystemGetExtendedParamCommon
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_EXTENDED_PARAM
        Command command{Command::SYSTEM_GET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemGetExtendedParamCommon);
    };

    // THMSGV2T2SystemGetExtendedParamUsbBrowser
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

    // THMSGV2T2SystemGetParam
    struct SystemGetParam
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_PARAM
        Command command{Command::SYSTEM_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemGetParam);
    };

    // THMSGV2T2SystemGetStatus
    struct SystemGetStatus
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_STATUS
        Command command{Command::SYSTEM_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemGetStatus);
    };

    // THMSGV2T2SystemNotifyExtendedParam
    struct SystemNotifyExtendedParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_EXTENDED_PARAM
        Command command{Command::SYSTEM_NTFY_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyExtendedParam);
    };

    // THMSGV2T2SystemNotifyExtendedParamUsbBrowser
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

    // THMSGV2T2SystemNotifyExtendedParamUsbBrowserForDirectoryStatus
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

    // THMSGV2T2SystemNotifyExtendedParamUsbBrowserForFileStatus
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

    // THMSGV2T2SystemNotifyParam
    struct SystemNotifyParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyParam);
    };

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

    // THMSGV2T2SystemNotifyParamFunctionChange
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

    // THMSGV2T2SystemNotifyParamLightingDefaultColorColorType
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

    // THMSGV2T2SystemNotifyParamLightingDefaultColorCustomColor
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

    // THMSGV2T2SystemNotifyParamLightingMode
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

    // THMSGV2T2SystemNotifyParamQuickAccessEasySetting
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

    // THMSGV2T2SystemNotifyParamRepeatTapTrainingMode
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

    // THMSGV2T2SystemNotifyParamSVACommand
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

    // THMSGV2T2SystemNotifyParamUsbBrowser
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

    // THMSGV2T2SystemNotifyParamUsbBrowserWithIndex
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

    // THMSGV2T2SystemNotifyParamUsbBrowserWithoutIndex
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

    // THMSGV2T2SystemNotifyStatus
    struct SystemNotifyStatus
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyStatus);
    };

    // THMSGV2T2SystemNotifyStatusCommon
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

    // THMSGV2T2SystemNotifyStatusLinkAutoSwitchForSpeaker
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

    // THMSGV2T2SystemNotifyStatusRepeatTapTrainingMode
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

    // THMSGV2T2SystemNotifyStatusWearingStatusChecker
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

    // THMSGV2T2SystemRetCapability
    struct SystemRetCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetCapability);
    };

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
    struct SystemRetCapabilityLinkAutoSwitchForSpeaker
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::LINK_AUTO_SWITCH_FOR_SPEAKER}; // 0x1
        Int16BE speakerIdentifier{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetCapabilityLinkAutoSwitchForSpeaker);
    };

    // THMSGV2T2SystemRetCapabilityMicOnOffByHeadphoneOperation
    struct SystemRetCapabilityMicOnOffByHeadphoneOperation
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::MIC_ON_OFF_BY_HEADPHONE_OPERATION}; // 0x1
        mdr::v2::t1::Type keyType{mdr::v2::t1::Type::TOUCH_SENSOR}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetCapabilityMicOnOffByHeadphoneOperation);
    };

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
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1
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
    struct SystemRetCapabilityUsbBrowser
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::USB_BROWSER}; // 0x1
        Int16BE numberOfItemsInOneTransaction{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetCapabilityUsbBrowser);
    };

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

    // THMSGV2T2SystemRetExtendedParam
    struct SystemRetExtendedParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_EXTENDED_PARAM
        Command command{Command::SYSTEM_RET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetExtendedParam);
    };

    // THMSGV2T2SystemRetExtendedParamSVASettingMtkTransferSupportLangSwitch_ServiceInformation
    struct SystemRetExtendedParamSVASettingMtkTransferSupportLangSwitch_ServiceInformation
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SonyVoiceAssistantLanguage language{SonyVoiceAssistantLanguage::UNDEFINED_LANGUAGE}; // 0x0
        MDRPrefixedString serviceId; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(SystemRetExtendedParamSVASettingMtkTransferSupportLangSwitch_ServiceInformation);
    };

    // THMSGV2T2SystemRetExtendedParamUsbBrowser
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

    // THMSGV2T2SystemRetExtendedParamUsbBrowserForDirectoryStatus
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

    // THMSGV2T2SystemRetExtendedParamUsbBrowserForFileStatus
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

    // THMSGV2T2SystemRetExtendedParamUsbBrowserForSize
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

    // THMSGV2T2SystemRetParam
    struct SystemRetParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetParam);
    };

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

    // THMSGV2T2SystemRetParamFunctionChange
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

    // THMSGV2T2SystemRetParamLightingDefaultColorColorType
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

    // THMSGV2T2SystemRetParamLightingDefaultColorCustomColor
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

    // THMSGV2T2SystemRetParamLightingMode
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

    // THMSGV2T2SystemRetParamSVASettingMtkTransferSupportLanguageSwitch
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

    // THMSGV2T2SystemRetStatus
    struct SystemRetStatus
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemRetStatus);
    };

    // THMSGV2T2SystemRetStatusCommon
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

    // THMSGV2T2SystemRetStatusLinkAutoSwitchForSpeaker
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

    // THMSGV2T2SystemRetStatusRepeatTapTrainingMode
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

    // THMSGV2T2SystemRetStatusWearingStatusChecker
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

    // THMSGV2T2SystemSetExtendedParam
    struct SystemSetExtendedParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_EXTENDED_PARAM
        Command command{Command::SYSTEM_SET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetExtendedParam);
    };

    // THMSGV2T2SystemSetExtendedParamUsbBrowser
    struct SystemSetExtendedParamUsbBrowser
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_EXTENDED_PARAM
        Command command{Command::SYSTEM_SET_EXTENDED_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::USB_BROWSER}; // 0x1
        Int16BE startIndex{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetExtendedParamUsbBrowser);
    };

    // THMSGV2T2SystemSetParam
    struct SystemSetParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParam);
    };

    // THMSGV2T2SystemSetParamCommon
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

    // THMSGV2T2SystemSetParamFunctionChange
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

    // THMSGV2T2SystemSetParamLightingDefaultColorColorType
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

    // THMSGV2T2SystemSetParamLightingDefaultColorCustomColor
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

    // THMSGV2T2SystemSetParamLightingMode
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

    // THMSGV2T2SystemSetParamLinkAutoSwitchForHeadsets
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

    // THMSGV2T2SystemSetParamLinkAutoSwitchForSpeaker
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

    // THMSGV2T2SystemSetParamMicOnOffByHeadphoneOperation
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
    struct SystemSetParamSVACommand
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue trainingModeOnOffValue{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParamSVACommand);
    };

    // THMSGV2T2SystemSetParamUsbBrowser
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

    // THMSGV2T2SystemSetParamUsbBrowserWithIndex
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

    // THMSGV2T2SystemSetParamUsbBrowserWithoutIndex
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

    // THMSGV2T2SystemSetParamWearingPositionJudgmentBySensor
    struct SystemSetParamWearingPositionJudgmentBySensor
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1
        JudgmentModeOperation judgmentModeOperation{JudgmentModeOperation::JUDGMENT_START}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParamWearingPositionJudgmentBySensor);
    };

    // THMSGV2T2SystemSetStatus
    struct SystemSetStatus
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_STATUS
        Command command{Command::SYSTEM_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetStatus);
    };

    // THMSGV2T2SystemSetStatusRepeatTapTrainingMode
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

    // THMSGV2T2SystemSetStatusSVASettingMtkSupportLanguageSwitch
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
    struct VoiceGuidanceGetCapability
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_GET_CAPABILITY
        Command command{Command::VOICE_GUIDANCE_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceGetCapability);
    };

    // THMSGV2T2VoiceGuidanceGetExtendedParam
    struct VoiceGuidanceGetExtendedParam
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_GET_PARAM
        Command command{Command::VOICE_GUIDANCE_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceGetExtendedParam);
    };

    // THMSGV2T2VoiceGuidanceGetParam
    struct VoiceGuidanceGetParam
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_GET_PARAM
        Command command{Command::VOICE_GUIDANCE_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceGetParam);
    };

    // THMSGV2T2VoiceGuidanceGetStatus
    struct VoiceGuidanceGetStatus
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_GET_STATUS
        Command command{Command::VOICE_GUIDANCE_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceGetStatus);
    };

    // THMSGV2T2VoiceGuidanceGetStatusCommon
    struct VoiceGuidanceGetStatusCommon
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_GET_STATUS
        Command command{Command::VOICE_GUIDANCE_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceGetStatusCommon);
    };

    // THMSGV2T2VoiceGuidanceGetStatusCommonWithStatusType
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

    // THMSGV2T2VoiceGuidanceNotifyParam
    struct VoiceGuidanceNotifyParam
    {
        Command command2{Command::CONNECT_GET_SUPPORT_FUNCTION}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

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
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x0
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
    struct VoiceGuidanceNotifyStatus
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_NTFY_STATUS
        Command command{Command::VOICE_GUIDANCE_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceNotifyStatus);
    };

    // THMSGV2T2VoiceGuidanceNotifyStatusCommon
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

    // THMSGV2T2VoiceGuidanceNotifyStatusSettingMtkNotSupportLangSwitch
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

    // THMSGV2T2VoiceGuidanceNotifyStatusSettingMtkSupportLangSwitch
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

    // THMSGV2T2VoiceGuidanceNotifyStatusSettingSupportLangSwitch
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

    // THMSGV2T2VoiceGuidanceRetCapability
    struct VoiceGuidanceRetCapability
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_CAPABILITY
        Command command{Command::VOICE_GUIDANCE_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetCapability);
    };

    // THMSGV2T2VoiceGuidanceRetCapabilitySettingMtk
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

    // THMSGV2T2VoiceGuidanceRetCapabilitySettingMtkNotSupportLangSwitch
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
    struct VoiceGuidanceRetExtendedParam
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetExtendedParam);
    };

    // THMSGV2T2VoiceGuidanceRetExtendedParamSettingMtkSupportLangSwitch_ServiceInformation
    struct VoiceGuidanceRetExtendedParamSettingMtkSupportLangSwitch_ServiceInformation
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceLanguage language{VoiceGuidanceLanguage::UNDEFINED_LANGUAGE}; // 0x0
        MDRPrefixedString serviceId; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(VoiceGuidanceRetExtendedParamSettingMtkSupportLangSwitch_ServiceInformation);
    };

    // THMSGV2T2VoiceGuidanceRetParam
    struct VoiceGuidanceRetParam
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetParam);
    };

    // THMSGV2T2VoiceGuidanceRetParamSettingMtk
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

    // THMSGV2T2VoiceGuidanceRetParamSettingMtkSupportLangSwitchAndSupportLangSwitch
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

    // THMSGV2T2VoiceGuidanceRetParamSettingOnOff
    struct VoiceGuidanceRetParamSettingOnOff
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue settingValue{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetParamSettingOnOff);
    };

    // THMSGV2T2VoiceGuidanceRetParamVolume
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

    // THMSGV2T2VoiceGuidanceRetStatus
    struct VoiceGuidanceRetStatus
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_STATUS
        Command command{Command::VOICE_GUIDANCE_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceRetStatus);
    };

    // THMSGV2T2VoiceGuidanceRetStatusCommon
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

    // THMSGV2T2VoiceGuidanceRetStatusSettingMtkNotSupportLangSwitch
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

    // THMSGV2T2VoiceGuidanceRetStatusSettingMtkSupportLangSwitch
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

    // THMSGV2T2VoiceGuidanceRetStatusSettingSupportLangSwitch
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

    // THMSGV2T2VoiceGuidanceSetParam
    struct VoiceGuidanceSetParam
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_SET_PARAM
        Command command{Command::VOICE_GUIDANCE_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceSetParam);
    };

    // THMSGV2T2VoiceGuidanceSetParamSettingMtk
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

    // THMSGV2T2VoiceGuidanceSetParamSettingOnOff
    struct VoiceGuidanceSetParamSettingOnOff
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_SET_PARAM
        Command command{Command::VOICE_GUIDANCE_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OnOffSettingValue settingValue{OnOffSettingValue::ON}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceSetParamSettingOnOff);
    };

    // THMSGV2T2VoiceGuidanceSetParamSettingSupportLangSwitch
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

    // THMSGV2T2VoiceGuidanceSetParamVolume
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

    // THMSGV2T2VoiceGuidanceSetStatus
    struct VoiceGuidanceSetStatus
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_SET_STATUS
        Command command{Command::VOICE_GUIDANCE_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceSetStatus);
    };

    // THMSGV2T2VoiceGuidanceSetStatusSettingMtkNotSupportLangSwitch
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

    // THMSGV2T2VoiceGuidanceSetStatusSettingMtkSupportLangSwitch
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
        SystemInquiredType inquiredType{SystemInquiredType::WEARING_STATUS_CHECKER}; // 0x1
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
