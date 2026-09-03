#pragma once
#include "ProtocolV1.hpp"
#pragma pack(push, 1)

// Generated from Sound Connect iOS J2ObjC metadata. Do not edit by hand.
namespace mdr::v1::t1
{
#pragma region Enums
    enum class AlertAction : UInt8
    {
        NEGATIVE = 0x00,
        POSITIVE = 0x01,
    };

    enum class AlertActionType : UInt8
    {
        CONFIRMATION_ONLY = 0x00,
        POSITIVE_NEGATIVE = 0x01,
    };

    enum class AlertInquiredType : UInt8
    {
        NO_USE = 0x00,
        FIXED_MESSAGE = 0x01,
        VIBRATOR_ALERT_NOTIFICATION = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class AlertMessageType : UInt8
    {
        NO_USE = 0x00,
        DISCONNECT_CAUSED_BY_CONNECTION_MODE_CHANGE = 0x01,
        DISCONNECT_CAUSED_BY_CHANGING_KEY_ASSIGN = 0x02,
        NEED_DISCONNECTION_FOR_UPDATING_FIRMWARE = 0x03,
        GOOGLE_ASSISTANT_IS_NOW_AVAILABLE = 0x04,
        DUAL_ASSIGN_OF_VOICE_ASSISTANT_IS_UNAVAILABLE = 0x05,
        DISCONNECT_CAUSED_BY_CHANGING_MULTIPOINT_TO_ON = 0x06,
        DISCONNECT_CAUSED_BY_CHANGING_MULTIPOINT_TO_OFF = 0x07,
        BATTERY_CONSUMPTION_INCREASE_DUE_TO_EQ_AND_UPSCALING = 0x08,
    };

    enum class AlertVibrationPattern : UInt8
    {
        NO_USE = 0x00,
        ONE_PATTERN_ONLY = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class AsmId : UInt8
    {
        NORMAL = 0x00,
        VOICE = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class AsmOnOffValue : UInt8
    {
        OFF = 0x00,
        ON = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class AsmSettingType : UInt8
    {
        ON_OFF = 0x00,
        LEVEL_ADJUSTMENT = 0x01,
    };

    enum class AssignableSettingsAction : UInt8
    {
        SINGLE_TAP = 0x00,
        DOUBLE_TAP = 0x01,
        TRIPLE_TAP = 0x02,
        SINGLE_TAP_AND_HOLD = 0x10,
        DOUBLE_TAP_AND_HOLD = 0x11,
        LONG_PRESS_THEN_ACTIVATE = 0x21,
        LONG_PRESS_DURING_ACTIVATION = 0x22,
        OUT_OF_RANGE = 0xFF,
    };

    enum class AssignableSettingsFunction : UInt8
    {
        NO_FUNCTION = 0x00,
        NC_ASM_OFF = 0x01,
        NC_OPTIMIZER = 0x02,
        QUICK_ATTENTION = 0x10,
        VOLUME_UP = 0x11,
        VOLUME_DOWN = 0x12,
        PLAY_PAUSE = 0x20,
        NEXT_TRACK = 0x21,
        PREVIOUS_TRACK = 0x22,
        VOICE_RECOGNITION = 0x30,
        GET_YOUR_NOTIFICATION = 0x31,
        TALK_TO_GA = 0x32,
        STOP_GA = 0x33,
        VOICE_INPUT_CANCEL_AA = 0x34,
        TALK_TO_TENCENT_XIAOWEI = 0x35,
        CANCEL_VOICE_RECOGNITION = 0x36,
        OUT_OF_RANGE = 0xFF,
    };

    enum class AssignableSettingsKey : UInt8
    {
        LEFT_SIDE_KEY = 0x00,
        RIGHT_SIDE_KEY = 0x01,
        CUSTOM_KEY = 0x02,
        C_KEY = 0x03,
        OUT_OF_RANGE = 0xFF,
    };

    enum class AssignableSettingsKeyType : UInt8
    {
        TOUCH_SENSOR = 0x00,
        BUTTON = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class AssignableSettingsPreset : UInt8
    {
        AMBIENT_SOUND_CONTROL = 0x00,
        VOLUME_CONTROL = 0x10,
        PLAYBACK_CONTROL = 0x20,
        VOICE_RECOGNITION = 0x30,
        GOOGLE_ASSISTANT = 0x31,
        AMAZON_ALEXA = 0x32,
        TENCENT_XIAOWEI = 0x33,
        NO_FUNCTION = 0xFF,
        OUT_OF_RANGE = 0xFE,
    };

    enum class AtCommandMessageType : UInt8
    {
        REQUEST = 0x01,
        RESPONSE = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class AudioCodec : UInt8
    {
        UNSETTLED = 0x00,
        SBC = 0x01,
        AAC = 0x02,
        LDAC = 0x10,
        APT_X = 0x20,
        APT_X_HD = 0x21,
        OTHER = 0xFF,
    };

    enum class AudioInquiredType : UInt8
    {
        NO_USE = 0x00,
        CONNECTION_MODE = 0x01,
        UPSCALING = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class AutoPowerOffElementId : UInt8
    {
        POWER_OFF_IN_5_MIN = 0x00,
        POWER_OFF_IN_30_MIN = 0x01,
        POWER_OFF_IN_60_MIN = 0x02,
        POWER_OFF_IN_180_MIN = 0x03,
        POWER_OFF_WHEN_REMOVED_FROM_EARS = 0x10,
        POWER_OFF_DISABLE = 0x11,
    };

    enum class AutoPowerOffParameterType : UInt8
    {
        ACTIVE_AND_SELECTIME_ID = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class BarometricMeasureType : UInt8
    {
        NOT_SUPPORT = 0x00,
        BAROMETRIC_PRESSURE = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class BarometricPressureValue : UInt8
    {
        UNMEASURED = 0x00,
        MEASURED_07 = 0x07,
        MEASURED_08 = 0x08,
        MEASURED_09 = 0x09,
        MEASURED_10 = 0x0A,
        OUT_OF_RANGE = 0xFF,
    };

    enum class BatteryChargingStatus : UInt8
    {
        NOT_CHARGING = 0x00,
        CHARGING = 0x01,
        UNKNOWN = 0xF0,
    };

    enum class BatteryInquiredType : UInt8
    {
        BATTERY = 0x00,
        LEFT_RIGHT_BATTERY = 0x01,
        CRADLE_BATTERY = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class BluetoothDeviceInfoType : UInt8
    {
        BLUETOOTH_DEVICE_ADDRESS = 0x00,
        BLE_HASH_VALUE = 0x01,
    };

    enum class Command : UInt8
    {
        CONNECT_GET_PROTOCOL_INFO = 0x00,
        CONNECT_RET_PROTOCOL_INFO = 0x01,
        CONNECT_GET_CAPABILITY_INFO = 0x02,
        CONNECT_RET_CAPABILITY_INFO = 0x03,
        CONNECT_GET_DEVICE_INFO = 0x04,
        CONNECT_RET_DEVICE_INFO = 0x05,
        CONNECT_GET_SUPPORT_FUNCTION = 0x06,
        CONNECT_RET_SUPPORT_FUNCTION = 0x07,
        COMMON_GET_BATTERY_LEVEL = 0x10,
        COMMON_RET_BATTERY_LEVEL = 0x11,
        COMMON_NTFY_BATTERY_LEVEL = 0x13,
        COMMON_GET_UPSCALING_EFFECT = 0x14,
        COMMON_RET_UPSCALING_EFFECT = 0x15,
        COMMON_NTFY_UPSCALING_EFFECT = 0x17,
        COMMON_GET_AUDIO_CODEC = 0x18,
        COMMON_RET_AUDIO_CODEC = 0x19,
        COMMON_NTFY_AUDIO_CODEC = 0x1B,
        COMMON_GET_BLUETOOTH_DEVICE_INFO = 0x1C,
        COMMON_RET_BLUETOOTH_DEVICE_INFO = 0x1D,
        COMMON_SET_POWER_OFF = 0x22,
        COMMON_GET_CONNECTION_STATUS = 0x24,
        COMMON_RET_CONNECTION_STATUS = 0x25,
        COMMON_NTFY_CONNECTION_STATUS = 0x27,
        COMMON_GET_CONCIERGE_DATA = 0x28,
        COMMON_RET_CONCIERGE_DATA = 0x29,
        COMMON_SET_LINK_CONTROL = 0x2E,
        COMMON_NTFY_LINK_CONTROL = 0x2F,
        UPDT_SET_STATUS = 0x34,
        UPDT_NTFY_STATUS = 0x35,
        UPDT_GET_PARAM = 0x36,
        UPDT_RET_PARAM = 0x37,
        VPT_GET_CAPABILITY = 0x40,
        VPT_RET_CAPABILITY = 0x41,
        VPT_GET_STATUS = 0x42,
        VPT_RET_STATUS = 0x43,
        VPT_NTFY_STATUS = 0x45,
        VPT_GET_PARAM = 0x46,
        VPT_RET_PARAM = 0x47,
        VPT_SET_PARAM = 0x48,
        VPT_NTFY_PARAM = 0x49,
        EQEBB_GET_CAPABILITY = 0x50,
        EQEBB_RET_CAPABILITY = 0x51,
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
        NCASM_NTFY_STATUS = 0x65,
        NCASM_GET_PARAM = 0x66,
        NCASM_RET_PARAM = 0x67,
        NCASM_SET_PARAM = 0x68,
        NCASM_NTFY_PARAM = 0x69,
        SENSE_GET_CAPABILITY = 0x70,
        SENSE_RET_CAPABILITY = 0x71,
        SENSE_SET_STATUS = 0x74,
        OPT_GET_CAPABILITY = 0x80,
        OPT_RET_CAPABILITY = 0x81,
        OPT_GET_STATUS = 0x82,
        OPT_RET_STATUS = 0x83,
        OPT_SET_STATUS = 0x84,
        OPT_NTFY_STATUS = 0x85,
        OPT_GET_PARAM = 0x86,
        OPT_RET_PARAM = 0x87,
        OPT_NTFY_PARAM = 0x89,
        ALERT_GET_CAPABILITY = 0x90,
        ALERT_RET_CAPABILITY = 0x91,
        ALERT_SET_STATUS = 0x94,
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
        SPORTS_GET_CAPABILITY = 0xB0,
        SPORTS_RET_CAPABILITY = 0xB1,
        SPORTS_GET_STATUS = 0xB2,
        SPORTS_RET_STATUS = 0xB3,
        SPORTS_NTFY_STATUS = 0xB5,
        SPORTS_GET_PARAM = 0xB6,
        SPORTS_RET_PARAM = 0xB7,
        SPORTS_SET_PARAM = 0xB8,
        SPORTS_NTFY_PARAM = 0xB9,
        SPORTS_GET_EXTENDED_PARAM = 0xBA,
        SPORTS_RET_EXTENDED_PARAM = 0xBB,
        SPORTS_SET_EXTENDED_PARAM = 0xBC,
        SPORTS_NTFY_EXTENDED_PARAM = 0xBD,
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
        SYSTEM_NTFY_STATUS = 0xF5,
        SYSTEM_GET_PARAM = 0xF6,
        SYSTEM_RET_PARAM = 0xF7,
        SYSTEM_SET_PARAM = 0xF8,
        SYSTEM_NTFY_PARAM = 0xF9,
        SYSTEM_GET_EXTENDED_PARAM = 0xFA,
        SYSTEM_RET_EXTENDED_PARAM = 0xFB,
        SYSTEM_SET_EXTENDED_PARAM = 0xFC,
        SYSTEM_NTFY_EXTENDED_PARAM = 0xFD,
        TEST_COMMAND = 0xFF,
    };

    enum class CommonCapabilityInquiredType : UInt8
    {
        FIXED_VALUE = 0x00,
        OUT_OF_RANGE = 0xFF,
    };

    enum class CommonOnOffSettingType : UInt8
    {
        ON_OFF = 0x00,
    };

    enum class CommonOnOffSettingValue : UInt8
    {
        OFF = 0x00,
        ON = 0x01,
    };

    enum class ConnectionModeSettingType : UInt8
    {
        SOUND_CONNECTION = 0x00,
    };

    enum class ConnectionModeSettingValue : UInt8
    {
        SOUND_QUALITY_PRIOR = 0x00,
        CONNECTION_QUALITY_PRIOR = 0x01,
    };

    enum class ConnectionStatus : UInt8
    {
        NOT_CONNECTED = 0x00,
        CONNECTED = 0x01,
    };

    enum class ConnectionStatusInquiredType : UInt8
    {
        LEFT_RIGHT_CONNECTION_STATUS = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class ControlByWearingSettingType : UInt8
    {
        ON_OFF = 0x00,
    };

    enum class ControlByWearingSettingValue : UInt8
    {
        OFF = 0x00,
        ON = 0x01,
    };

    enum class DetectionSensitivity : UInt8
    {
        AUTO = 0x00,
        HIGH = 0x01,
        LOW = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class DeviceInfoInquiredType : UInt8
    {
        NO_USE = 0x00,
        MODEL_NAME = 0x01,
        FW_VERSION = 0x02,
        SERIES_AND_COLOR_INFO = 0x03,
        INSTRUCTION_GUIDE = 0x04,
        OUT_OF_RANGE = 0xFF,
    };

    enum class DisplayLanguage : UInt8
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

    enum class EqBandInformationType : UInt8
    {
        NO_INFORMATION = 0x00,
        HZ = 0x01,
        KHZ = 0x02,
        SPECIFIC_INFORMATION = 0x10,
    };

    enum class EqEbbInquiredType : UInt8
    {
        NO_USE = 0x00,
        PRESET_EQ = 0x01,
        EBB = 0x02,
        PRESET_EQ_NONCUSTOMIZABLE = 0x03,
        OUT_OF_RANGE = 0xFF,
    };

    enum class EqPresetId : UInt8
    {
        OFF = 0x00,
        ROCK = 0x01,
        POP = 0x02,
        JAZZ = 0x03,
        DANCE = 0x04,
        EDM = 0x05,
        R_AND_B_HIP_HOP = 0x06,
        ACOUSTIC = 0x07,
        RESERVED_FOR_FUTURE_NO8 = 0x08,
        RESERVED_FOR_FUTURE_NO9 = 0x09,
        RESERVED_FOR_FUTURE_NO10 = 0x0A,
        RESERVED_FOR_FUTURE_NO11 = 0x0B,
        RESERVED_FOR_FUTURE_NO12 = 0x0C,
        RESERVED_FOR_FUTURE_NO13 = 0x0D,
        RESERVED_FOR_FUTURE_NO14 = 0x0E,
        RESERVED_FOR_FUTURE_NO15 = 0x0F,
        BRIGHT = 0x10,
        EXCITED = 0x11,
        MELLOW = 0x12,
        RELAXED = 0x13,
        VOCAL = 0x14,
        TREBLE = 0x15,
        BASS = 0x16,
        SPEECH = 0x17,
        RESERVED_FOR_FUTURE_NO24 = 0x18,
        RESERVED_FOR_FUTURE_NO25 = 0x19,
        RESERVED_FOR_FUTURE_NO26 = 0x1A,
        RESERVED_FOR_FUTURE_NO27 = 0x1B,
        RESERVED_FOR_FUTURE_NO28 = 0x1C,
        RESERVED_FOR_FUTURE_NO29 = 0x1D,
        RESERVED_FOR_FUTURE_NO30 = 0x1E,
        RESERVED_FOR_FUTURE_NO31 = 0x1F,
        CUSTOM = 0xA0,
        USER_SETTING1 = 0xA1,
        USER_SETTING2 = 0xA2,
        USER_SETTING3 = 0xA3,
        USER_SETTING4 = 0xA4,
        USER_SETTING5 = 0xA5,
        UNSPECIFIED = 0xFF,
    };

    enum class FunctionType : UInt8
    {
        NO_USE = 0x00,
        BATTERY_LEVEL = 0x11,
        UPSCALING_INDICATOR = 0x12,
        CODEC_INDICATOR = 0x13,
        BLE_SETUP = 0x14,
        LEFT_RIGHT_BATTERY_LEVEL = 0x15,
        LEFT_RIGHT_CONNECTION_STATUS = 0x17,
        CRADLE_BATTERY_LEVEL = 0x18,
        POWER_OFF = 0x21,
        CONCIERGE_DATA = 0x22,
        TANDEM_KEEP_ALIVE = 0x23,
        FW_UPDATE = 0x30,
        PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT = 0x38,
        VOICE_GUIDANCE = 0x39,
        VPT = 0x41,
        SOUND_POSITION = 0x42,
        PRESET_EQ = 0x51,
        EBB = 0x52,
        PRESET_EQ_NONCUSTOMIZABLE = 0x53,
        NOISE_CANCELLING = 0x61,
        NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE = 0x62,
        AMBIENT_SOUND_MODE = 0x63,
        AUTO_NC_ASM = 0x71,
        NC_OPTIMIZER = 0x81,
        VIBRATOR_ALERT_NOTIFICATION = 0x92,
        PLAYBACK_CONTROLLER = 0xA1,
        TRAINING_MODE = 0xB1,
        ACTION_LOG_NOTIFIER = 0xC1,
        GENERAL_SETTING1 = 0xD1,
        GENERAL_SETTING2 = 0xD2,
        GENERAL_SETTING3 = 0xD3,
        CONNECTION_MODE = 0xE1,
        UPSCALING = 0xE2,
        VIBRATOR = 0xF1,
        POWER_SAVING_MODE = 0xF2,
        CONTROL_BY_WEARING = 0xF3,
        AUTO_POWER_OFF = 0xF4,
        SMART_TALKING_MODE = 0xF5,
        ASSIGNABLE_SETTINGS = 0xF6,
    };

    enum class GsInquiredType : UInt8
    {
        GENERAL_SETTING1 = 0xD1,
        GENERAL_SETTING2 = 0xD2,
        GENERAL_SETTING3 = 0xD3,
        OUT_OF_RANGE = 0xFF,
    };

    enum class GsSettingType : UInt8
    {
        NO_USE = 0x00,
        BOOLEAN_TYPE = 0x01,
        LIST_TYPE = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class GsStringFormat : UInt8
    {
        NO_USE = 0x00,
        RAW_NAME = 0x01,
        ENUM_NAME = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class GuidanceCategory : UInt8
    {
        CHANGE_EARPIECE = 0x00,
        WEAR_EARPHONE = 0x10,
        PLAY_BUTTON_OPERATION = 0x20,
        TOUCH_PAD_OPERATION = 0x30,
        MAIN_BODY_OPERATION = 0x40,
        QUICK_ATTENTION = 0x50,
        ASSIGNABLE_BUTTON_SETTINGS = 0x60,
        OUT_OF_RANGE = 0xFF,
    };

    enum class LinkControlInquiredType : UInt8
    {
        KEEP_ALIVE = 0x00,
        OUT_OF_RANGE = 0xFF,
    };

    enum class LogInquiredType : UInt8
    {
        NO_USE = 0x00,
        ACTION_LOG_NOTIFIER = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class MetaDataDisplayType : UInt8
    {
        NOT_SUPPORT = 0x00,
        TRACK_ALBUM_ARTIST_GENRE_PLAYER = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class ModeOutTime : UInt8
    {
        FAST = 0x00,
        MID = 0x01,
        SLOW = 0x02,
        NONE = 0x03,
        OUT_OF_RANGE = 0xFF,
    };

    enum class ModelSeries : UInt8
    {
        NO_SERIES = 0x00,
        EXTRA_BASS = 0x10,
        HEAR = 0x20,
        PREMIUM = 0x30,
        SPORTS = 0x40,
        CASUAL = 0x50,
    };

    enum class NcAsmEffect : UInt8
    {
        OFF = 0x00,
        ON = 0x01,
        ADJUSTMENT_IN_PROGRESS = 0x10,
        ADJUSTMENT_COMPLETION = 0x11,
        OUT_OF_RANGE = 0xFF,
    };

    enum class NcAsmInquiredType : UInt8
    {
        NO_USE = 0x00,
        NOISE_CANCELLING = 0x01,
        NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE = 0x02,
        AMBIENT_SOUND_MODE = 0x03,
        OUT_OF_RANGE = 0xFF,
    };

    enum class NcAsmSettingType : UInt8
    {
        ON_OFF = 0x00,
        LEVEL_ADJUSTMENT = 0x01,
        DUAL_SINGLE_OFF = 0x02,
    };

    enum class NcDualSingleValue : UInt8
    {
        OFF = 0x00,
        SINGLE = 0x01,
        DUAL = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class NcOnOffValue : UInt8
    {
        OFF = 0x00,
        ON = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class NcSettingType : UInt8
    {
        ON_OFF = 0x00,
        LEVEL_ADJUSTMENT = 0x01,
    };

    enum class NcSettingValue : UInt8
    {
        OFF = 0x00,
        ON = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class OptimizerControl : UInt8
    {
        CANCEL = 0x00,
        START = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class OptimizerInquiredType : UInt8
    {
        NO_USE = 0x00,
        NC_OPTIMIZER = 0x01,
        NC_MUSIC_OPTIMIZER = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class OptimizerStatus : UInt8
    {
        IDLE = 0x00,
        IN_PROGRESS_OF_PERSONAL = 0x01,
        IN_PROGRESS_OF_BAROMETRIC_PRESSURE = 0x02,
        OPTIMIZING = 0x10,
        OPTIMIZER_END = 0x11,
    };

    enum class PersonalMeasureType : UInt8
    {
        NOT_SUPPORT = 0x00,
        PERSONAL = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class PersonalValue : UInt8
    {
        UNMEASURED = 0x00,
        MEASURED = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class PlayInquiredType : UInt8
    {
        NO_USE = 0x00,
        PLAYBACK_CONTROLLER = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class PlaybackControl : UInt8
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
        OUT_OF_RANGE = 0xFF,
    };

    enum class PlaybackControlType : UInt8
    {
        NOT_SUPPORT = 0x00,
        PLAY_PAUSE_TRACKUP_TRACKDOWN = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class PlaybackDetailedDataType : UInt8
    {
        TRACK_NAME = 0x00,
        ALBUM_NAME = 0x01,
        ARTIST_NAME = 0x02,
        GENRE_NAME = 0x03,
        PLAYER_NAME = 0x10,
        VOLUME = 0x20,
        OUT_OF_RANGE = 0xFF,
    };

    enum class PlaybackNameStatus : UInt8
    {
        UNSETTLED = 0x00,
        NOTHING = 0x01,
        SETTLED = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class PlaybackStatus : UInt8
    {
        UNSETTLED = 0x00,
        PLAY = 0x01,
        PAUSE = 0x02,
        STOP = 0x03,
        OUT_OF_RANGE = 0xFF,
    };

    enum class PowerOffInquiredType : UInt8
    {
        FIXED_VALUE = 0x00,
        OUT_OF_RANGE = 0xFF,
    };

    enum class PowerOffSettingValue : UInt8
    {
        NO_USE = 0x00,
        USER_POWER_OFF = 0x01,
    };

    enum class PowerSavingModeSettingType : UInt8
    {
        ON_OFF = 0x00,
    };

    enum class PowerSavingModeSettingValue : UInt8
    {
        OFF = 0x00,
        ON = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SenseInquiredType : UInt8
    {
        NO_USE = 0x00,
        AUTO_NC_ASM = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SenseSettingControl : UInt8
    {
        NO_USE = 0x00,
        START = 0x01,
    };

    enum class SenseTableType : UInt8
    {
        NO_USE = 0x00,
        TYPE1 = 0x01,
        TYPE2 = 0x02,
        TYPE3 = 0x03,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SmartTalkingModeDetailSettingType : UInt8
    {
        TYPE_1 = 0x00,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SmartTalkingModeDetectionSensitivityType : UInt8
    {
        AUTO_HIGH_LOW = 0x00,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SmartTalkingModeEffectStatus : UInt8
    {
        NOT_ACTIVE = 0x00,
        ACTIVE = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SmartTalkingModeModeOutTimeType : UInt8
    {
        TYPE_1 = 0x00,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SmartTalkingModeParameterType : UInt8
    {
        NO_USE = 0x00,
        MODE_ON_OFF = 0x01,
        PREVIEW_MODE_ON_OFF = 0x02,
    };

    enum class SmartTalkingModePreviewType : UInt8
    {
        NOT_SUPPORT = 0x00,
        SUPPORT = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SmartTalkingModeSettingType : UInt8
    {
        ON_OFF = 0x00,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SmartTalkingModeSettingValue : UInt8
    {
        OFF = 0x00,
        ON = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SmartTalkingModeVoiceFocusType : UInt8
    {
        ON_OFF = 0x00,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SoundPositionPresetId : UInt8
    {
        OFF = 0x00,
        FRONT_LEFT = 0x01,
        FRONT_RIGHT = 0x02,
        FRONT = 0x03,
        REAR_LEFT = 0x11,
        REAR_RIGHT = 0x12,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SoundPositionType : UInt8
    {
        NO_USE = 0x00,
        TYPE1 = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SportsInquiredType : UInt8
    {
        NO_USE = 0x00,
        TRAINING_MODE = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class SystemInquiredType : UInt8
    {
        NO_USE = 0x00,
        VIBRATOR = 0x01,
        POWER_SAVING_MODE = 0x02,
        CONTROL_BY_WEARING = 0x03,
        AUTO_POWER_OFF = 0x04,
        SMART_TALKING_MODE = 0x05,
        ASSIGNABLE_SETTINGS = 0x06,
        OUT_OF_RANGE = 0xFF,
    };

    enum class TargetType : UInt8
    {
        APP = 0x00,
        HEADPHONES_OR_TWS_MASTER = 0x01,
        TWS_SLAVE = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class TestCommandType : UInt8
    {
        ATCOMMAND = 0x05,
        OUT_OF_RANGE = 0xFF,
    };

    enum class TrainingModeAvailableEffectType : UInt8
    {
        NO_USE = 0x00,
        TYPE1 = 0x01,
        TYPE2 = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class TrainingModeExParameterType : UInt8
    {
        NO_USE = 0x00,
        RESET_SETTINGS = 0x01,
        NCASM_SETTINGS = 0x10,
        NCASM_ACTUAL_EFFECTS = 0x11,
        ASM_SETTINGS = 0x12,
        ASM_ACTUAL_EFFECTS = 0x13,
        PRESET_EQ_SETTINGS = 0x20,
        PRESET_EQ_ACTUAL_EFFECTS = 0x21,
        OUT_OF_RANGE = 0xFF,
    };

    enum class UpdateInquiredType : UInt8
    {
        NO_USE = 0x00,
        FW_UPDATE_MODE = 0x01,
        CATEGORY_ID = 0x02,
        SERVICE_ID = 0x03,
        NATION_CODE = 0x04,
        LANGUAGE = 0x05,
        SERIAL_NUMBER = 0x06,
        BLE_TX_POWER = 0x07,
        BATTERY_POWER_THRESHOLD = 0x08,
        UPDATE_METHOD = 0x09,
        BATTERY_POWER_THRESHOLD_FOR_INTERRUPTIONG_FW_UPDATE = 0x0A,
        UNIQUE_ID_FOR_DEVICE_BINDING = 0x0B,
        OUT_OF_RANGE = 0xFF,
    };

    enum class UpscalingEffectStatus : UInt8
    {
        OFF = 0x00,
        VALID = 0x01,
        INVALID = 0x02,
    };

    enum class UpscalingEffectType : UInt8
    {
        DSEE_HX = 0x00,
        DSEE = 0x01,
        DSEE_HX_AI = 0x02,
    };

    enum class UpscalingSettingType : UInt8
    {
        AUTO_OFF = 0x00,
    };

    enum class UpscalingSettingValue : UInt8
    {
        OFF = 0x00,
        AUTO = 0x01,
    };

    enum class UpscalingType : UInt8
    {
        DSEE_HX = 0x00,
        DSEE = 0x01,
        DSEE_HX_AI = 0x02,
    };

    enum class VibrationType : UInt8
    {
        NO_PATTERN_SPECIFIED = 0x00,
    };

    enum class VibratorSettingType : UInt8
    {
        ON_OFF = 0x00,
    };

    enum class VibratorSettingValue : UInt8
    {
        OFF = 0x00,
        ON = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class VptInquiredType : UInt8
    {
        NO_USE = 0x00,
        VPT = 0x01,
        SOUND_POSITION = 0x02,
        OUT_OF_RANGE = 0xFF,
    };

    enum class VptPresetId : UInt8
    {
        OFF = 0x00,
        OUTDOOR_FESTIVAL = 0x01,
        ARENA = 0x02,
        CONCERT_HALL = 0x03,
        CLUB = 0x04,
        RESERVED_FOR_FUTURE_NO5 = 0x05,
        RESERVED_FOR_FUTURE_NO6 = 0x06,
        RESERVED_FOR_FUTURE_NO7 = 0x07,
        RESERVED_FOR_FUTURE_NO8 = 0x08,
        RESERVED_FOR_FUTURE_NO9 = 0x09,
        RESERVED_FOR_FUTURE_NO10 = 0x0A,
        RESERVED_FOR_FUTURE_NO11 = 0x0B,
        RESERVED_FOR_FUTURE_NO12 = 0x0C,
        RESERVED_FOR_FUTURE_NO13 = 0x0D,
        RESERVED_FOR_FUTURE_NO14 = 0x0E,
        RESERVED_FOR_FUTURE_NO15 = 0x0F,
    };
#pragma endregion Enums

#pragma region Declarations

    // THMSGV1T1AsCapabilityAction
    struct AsCapabilityAction
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AssignableSettingsAction action{AssignableSettingsAction::SINGLE_TAP}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AssignableSettingsFunction function{AssignableSettingsFunction::NO_FUNCTION}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(AsCapabilityAction);
    };

    // THMSGV1T1Asm
    struct Asm
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId id{AsmId::NORMAL}; // 0x0
        UInt8 step{}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(Asm);
    };

    // THMSGV1T1AsmParam
    struct AsmParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::AMBIENT_SOUND_MODE}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF}; // 0x1
        AsmSettingType asmType{AsmSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId asmId{AsmId::NORMAL}; // 0x3
        UInt8 asmValue{}; // 0x4

        MDR_DEFINE_EXTERN_READ_WRITE(AsmParam);
    };

    // THMSGV1T1AssignableSettingsParam
    struct AssignableSettingsParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x0
        MDRPodArray<AssignableSettingsPreset> presets; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(AssignableSettingsParam);
    };

    // THMSGV1T1AssignableSettingsStatus
    struct AssignableSettingsStatus
    {
        MDRPodArray<CommonStatus> allStatus; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE};

        MDR_DEFINE_EXTERN_READ_WRITE(AssignableSettingsStatus);
    };

    // THMSGV1T1AtCommandParam
    struct AtCommandParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AtCommandMessageType messageType{AtCommandMessageType::REQUEST}; // 0x0
        Int16BE commandLength{}; // 0x1
        MDRPrefixedString command2; // 0x3

        MDR_DEFINE_EXTERN_READ_WRITE(AtCommandParam);
    };

    // THMSGV1T1AutoPowerOffParam
    struct AutoPowerOffParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AutoPowerOffParameterType autoPowerOffParameterType{AutoPowerOffParameterType::ACTIVE_AND_SELECTIME_ID}; // 0x1
        AutoPowerOffElementId activeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x2
        AutoPowerOffElementId selectTimeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x3

        MDR_DEFINE_EXTERN_READ_WRITE(AutoPowerOffParam);
    };

    // THMSGV1T1BatteryParam
    struct BatteryParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        BatteryInquiredType type{BatteryInquiredType::BATTERY}; // 0x0
        UInt8 level{}; // 0x1
        BatteryChargingStatus chargingStatus{BatteryChargingStatus::NOT_CHARGING}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(BatteryParam);
    };

    // THMSGV1T1ConnectionModeParam
    struct ConnectionModeParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType type{AudioInquiredType::CONNECTION_MODE}; // 0x0
        ConnectionModeSettingType settingType{ConnectionModeSettingType::SOUND_CONNECTION}; // 0x1
        ConnectionModeSettingValue settingValue{ConnectionModeSettingValue::SOUND_QUALITY_PRIOR}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(ConnectionModeParam);
    };

    // THMSGV1T1ControlByWearingParam
    struct ControlByWearingParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x0
        ControlByWearingSettingType settingType{ControlByWearingSettingType::ON_OFF}; // 0x1
        ControlByWearingSettingValue settingValue{ControlByWearingSettingValue::OFF}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(ControlByWearingParam);
    };

    // THMSGV1T1CradleBatteryParam
    struct CradleBatteryParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        BatteryInquiredType type{BatteryInquiredType::CRADLE_BATTERY}; // 0x0
        UInt8 level{}; // 0x1
        BatteryChargingStatus chargingStatus{BatteryChargingStatus::NOT_CHARGING}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(CradleBatteryParam);
    };

    // THMSGV1T1EbbParam
    struct EbbParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::EBB}; // 0x0
        UInt8 level{}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(EbbParam);
    };

    // THMSGV1T1EqParam
    struct EqParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x0
        EqPresetId presetId{EqPresetId::OFF}; // 0x1
        MDRPodArray<UInt8> bandSteps; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(EqParam);
    };

    // THMSGV1T1EqPreset
    struct EqPreset
    {
        EqPresetId presetId{EqPresetId::OFF}; // 0x0
        MDRPrefixedString name; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(EqPreset);
    };

    // THMSGV1T1FixedMessageParam
    struct FixedMessageParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AlertInquiredType type{AlertInquiredType::FIXED_MESSAGE}; // 0x0
        AlertMessageType messageType{AlertMessageType::NO_USE}; // 0x1
        AlertAction action{AlertAction::NEGATIVE}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(FixedMessageParam);
    };

    // THMSGV1T1GetAlertCapability
    struct GetAlertCapability
    {
        // CODEGEN EnumRange Command::ALERT_GET_CAPABILITY
        Command command{Command::ALERT_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AlertInquiredType alertInquiredType{AlertInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetAlertCapability);
    };

    // THMSGV1T1GetAudioCapability
    struct GetAudioCapability
    {
        // CODEGEN EnumRange Command::AUDIO_GET_CAPABILITY
        Command command{Command::AUDIO_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType inquiredType{AudioInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetAudioCapability);
    };

    // THMSGV1T1GetAudioCodec
    struct GetAudioCodec
    {
        // CODEGEN EnumRange Command::COMMON_GET_AUDIO_CODEC
        Command command{Command::COMMON_GET_AUDIO_CODEC}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonCapabilityInquiredType type{}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetAudioCodec);
    };

    // THMSGV1T1GetAudioParam
    struct GetAudioParam
    {
        // CODEGEN EnumRange Command::AUDIO_GET_PARAM
        Command command{Command::AUDIO_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType audioInquiredType{AudioInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetAudioParam);
    };

    // THMSGV1T1GetAudioStatus
    struct GetAudioStatus
    {
        // CODEGEN EnumRange Command::AUDIO_GET_STATUS
        Command command{Command::AUDIO_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType audioInquiredType{AudioInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetAudioStatus);
    };

    // THMSGV1T1GetBatteryLevel
    struct GetBatteryLevel
    {
        // CODEGEN EnumRange Command::COMMON_GET_BATTERY_LEVEL
        Command command{Command::COMMON_GET_BATTERY_LEVEL}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        BatteryInquiredType batteryInquiredType{BatteryInquiredType::BATTERY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetBatteryLevel);
    };

    // THMSGV1T1GetBluetoothDeviceInfo
    struct GetBluetoothDeviceInfo
    {
        // CODEGEN EnumRange Command::COMMON_GET_BLUETOOTH_DEVICE_INFO
        Command command{Command::COMMON_GET_BLUETOOTH_DEVICE_INFO}; // 0x0
        // CODEGEN EnumRange BluetoothDeviceInfoType::BLUETOOTH_DEVICE_ADDRESS
        BluetoothDeviceInfoType type{BluetoothDeviceInfoType::BLUETOOTH_DEVICE_ADDRESS}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetBluetoothDeviceInfo);
    };

    // THMSGV1T1GetCapabilityInfo
    struct GetCapabilityInfo
    {
        // CODEGEN EnumRange Command::CONNECT_GET_CAPABILITY_INFO
        Command command{Command::CONNECT_GET_CAPABILITY_INFO}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonCapabilityInquiredType type{}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetCapabilityInfo);
    };

    // THMSGV1T1GetConciergeData
    struct GetConciergeData
    {
        // CODEGEN EnumRange Command::COMMON_GET_CONCIERGE_DATA
        Command command{Command::COMMON_GET_CONCIERGE_DATA}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonCapabilityInquiredType type{}; // 0x1
        MDRPrefixedString data; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(GetConciergeData);
    };

    // THMSGV1T1GetConnectionStatus
    struct GetConnectionStatus
    {
        // CODEGEN EnumRange Command::COMMON_GET_CONNECTION_STATUS
        Command command{Command::COMMON_GET_CONNECTION_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ConnectionStatusInquiredType connectionStatusInquiredType{ConnectionStatusInquiredType::LEFT_RIGHT_CONNECTION_STATUS}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetConnectionStatus);
    };

    // THMSGV1T1GetDeviceInfo
    struct GetDeviceInfo
    {
        // CODEGEN EnumRange Command::CONNECT_GET_DEVICE_INFO
        Command command{Command::CONNECT_GET_DEVICE_INFO}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        DeviceInfoInquiredType inquiredType{DeviceInfoInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetDeviceInfo);
    };

    // THMSGV1T1GetEqEbbCapability
    struct GetEqEbbCapability
    {
        // CODEGEN EnumRange Command::EQEBB_GET_CAPABILITY
        Command command{Command::EQEBB_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x1
        DisplayLanguage language{DisplayLanguage::UNDEFINED_LANGUAGE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetEqEbbCapability);
    };

    // THMSGV1T1GetEqEbbExtendedInfo
    struct GetEqEbbExtendedInfo
    {
        // CODEGEN EnumRange Command::EQEBB_GET_EXTENDED_INFO
        Command command{Command::EQEBB_GET_EXTENDED_INFO}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetEqEbbExtendedInfo);
    };

    // THMSGV1T1GetEqEbbParam
    struct GetEqEbbParam
    {
        // CODEGEN EnumRange Command::EQEBB_GET_PARAM
        Command command{Command::EQEBB_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetEqEbbParam);
    };

    // THMSGV1T1GetEqEbbStatus
    struct GetEqEbbStatus
    {
        // CODEGEN EnumRange Command::EQEBB_GET_STATUS
        Command command{Command::EQEBB_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetEqEbbStatus);
    };

    // THMSGV1T1GetGsCapability
    struct GetGsCapability
    {
        // CODEGEN EnumRange Command::GENERAL_SETTING_GET_CAPABILITY
        Command command{Command::GENERAL_SETTING_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsInquiredType type{GsInquiredType::GENERAL_SETTING1}; // 0x1
        DisplayLanguage displayLanguage{DisplayLanguage::UNDEFINED_LANGUAGE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetGsCapability);
    };

    // THMSGV1T1GetGsParam
    struct GetGsParam
    {
        // CODEGEN EnumRange Command::GENERAL_SETTING_GET_PARAM
        Command command{Command::GENERAL_SETTING_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsInquiredType type{GsInquiredType::GENERAL_SETTING1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetGsParam);
    };

    // THMSGV1T1GetGsStatus
    struct GetGsStatus
    {
        // CODEGEN EnumRange Command::GENERAL_SETTING_GET_STATUS
        Command command{Command::GENERAL_SETTING_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsInquiredType type{GsInquiredType::GENERAL_SETTING1}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetGsStatus);
    };

    // THMSGV1T1GetNcAsmCapability
    struct GetNcAsmCapability
    {
        // CODEGEN EnumRange Command::NCASM_GET_CAPABILITY
        Command command{Command::NCASM_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetNcAsmCapability);
    };

    // THMSGV1T1GetNcAsmParam
    struct GetNcAsmParam
    {
        // CODEGEN EnumRange Command::NCASM_GET_PARAM
        Command command{Command::NCASM_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetNcAsmParam);
    };

    // THMSGV1T1GetNcAsmStatus
    struct GetNcAsmStatus
    {
        // CODEGEN EnumRange Command::NCASM_GET_STATUS
        Command command{Command::NCASM_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetNcAsmStatus);
    };

    // THMSGV1T1GetOptimizerCapability
    struct GetOptimizerCapability
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OptimizerInquiredType optimizerInquiredType{OptimizerInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetOptimizerCapability);
    };

    // THMSGV1T1GetOptimizerParam
    struct GetOptimizerParam
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OptimizerInquiredType optimizerInquiredType{OptimizerInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetOptimizerParam);
    };

    // THMSGV1T1GetOptimizerStatus
    struct GetOptimizerStatus
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OptimizerInquiredType optimizerInquiredType{OptimizerInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetOptimizerStatus);
    };

    // THMSGV1T1GetPlayCapability
    struct GetPlayCapability
    {
        // CODEGEN EnumRange Command::PLAY_GET_CAPABILITY
        Command command{Command::PLAY_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlayInquiredType type{PlayInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetPlayCapability);
    };

    // THMSGV1T1GetPlayParam
    struct GetPlayParam
    {
        // CODEGEN EnumRange Command::PLAY_GET_PARAM
        Command command{Command::PLAY_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlayInquiredType type{PlayInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackDetailedDataType dataType{PlaybackDetailedDataType::TRACK_NAME}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetPlayParam);
    };

    // THMSGV1T1GetPlayStatus
    struct GetPlayStatus
    {
        // CODEGEN EnumRange Command::PLAY_GET_STATUS
        Command command{Command::PLAY_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlayInquiredType type{PlayInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetPlayStatus);
    };

    // THMSGV1T1GetProtocolInfo
    struct GetProtocolInfo
    {
        // CODEGEN EnumRange Command::CONNECT_GET_PROTOCOL_INFO
        Command command{Command::CONNECT_GET_PROTOCOL_INFO}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonCapabilityInquiredType type{}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetProtocolInfo);
    };

    // THMSGV1T1GetSenseCapability
    struct GetSenseCapability
    {
        // CODEGEN EnumRange Command::SENSE_GET_CAPABILITY
        Command command{Command::SENSE_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SenseInquiredType senseInquiredType{SenseInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetSenseCapability);
    };

    // THMSGV1T1GetSportsCapability
    struct GetSportsCapability
    {
        // CODEGEN EnumRange Command::SPORTS_GET_CAPABILITY
        Command command{Command::SPORTS_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetSportsCapability);
    };

    // THMSGV1T1GetSportsExParam_TrainingModeRequest
    struct GetSportsExParam_TrainingModeRequest
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        TrainingModeExParameterType type{TrainingModeExParameterType::NO_USE}; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(GetSportsExParam_TrainingModeRequest);
    };

    // THMSGV1T1GetSportsParam
    struct GetSportsParam
    {
        // CODEGEN EnumRange Command::SPORTS_GET_PARAM
        Command command{Command::SPORTS_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetSportsParam);
    };

    // THMSGV1T1GetSportsStatus
    struct GetSportsStatus
    {
        // CODEGEN EnumRange Command::SPORTS_GET_STATUS
        Command command{Command::SPORTS_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetSportsStatus);
    };

    // THMSGV1T1GetSupportFunction
    struct GetSupportFunction
    {
        // CODEGEN EnumRange Command::CONNECT_GET_SUPPORT_FUNCTION
        Command command{Command::CONNECT_GET_SUPPORT_FUNCTION}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonCapabilityInquiredType type{}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetSupportFunction);
    };

    // THMSGV1T1GetSystemCapability
    struct GetSystemCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_CAPABILITY
        Command command{Command::SYSTEM_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType inquiredType{SystemInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetSystemCapability);
    };

    // THMSGV1T1GetSystemExParam
    struct GetSystemExParam
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_PARAM
        Command command{Command::SYSTEM_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType systemInquiredType{SystemInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetSystemExParam);
    };

    // THMSGV1T1GetSystemParam
    struct GetSystemParam
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_PARAM
        Command command{Command::SYSTEM_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType systemInquiredType{SystemInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetSystemParam);
    };

    // THMSGV1T1GetSystemStatus
    struct GetSystemStatus
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_STATUS
        Command command{Command::SYSTEM_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType systemInquiredType{SystemInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetSystemStatus);
    };

    // THMSGV1T1GetUpdateParam
    struct GetUpdateParam
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType updateInquiredType{UpdateInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetUpdateParam);
    };

    // THMSGV1T1GetUpscalingEffect
    struct GetUpscalingEffect
    {
        // CODEGEN EnumRange Command::COMMON_GET_UPSCALING_EFFECT
        Command command{Command::COMMON_GET_UPSCALING_EFFECT}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonCapabilityInquiredType type{}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetUpscalingEffect);
    };

    // THMSGV1T1GetVptCapability
    struct GetVptCapability
    {
        // CODEGEN EnumRange Command::VPT_GET_CAPABILITY
        Command command{Command::VPT_GET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType inquiredType{VptInquiredType::VPT}; // 0x1
        DisplayLanguage displayLanguage{DisplayLanguage::UNDEFINED_LANGUAGE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetVptCapability);
    };

    // THMSGV1T1GetVptParam
    struct GetVptParam
    {
        // CODEGEN EnumRange Command::VPT_GET_PARAM
        Command command{Command::VPT_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType vptInquiredType{VptInquiredType::VPT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetVptParam);
    };

    // THMSGV1T1GetVptStatus
    struct GetVptStatus
    {
        // CODEGEN EnumRange Command::VPT_GET_STATUS
        Command command{Command::VPT_GET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType vptInquiredType{VptInquiredType::VPT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetVptStatus);
    };

    // THMSGV1T1GsBooleanTypeValue
    struct GsBooleanTypeValue
    {
        CommonOnOffSettingValue settingValue{CommonOnOffSettingValue::OFF}; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(GsBooleanTypeValue);
    };

    // THMSGV1T1GsListTypeValue
    struct GsListTypeValue
    {
        UInt8 currentElementIndex{}; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(GsListTypeValue);
    };

    // THMSGV1T1GsSettingInfo
    struct GsSettingInfo
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsStringFormat stringFormat{GsStringFormat::NO_USE}; // 0x0
        MDRPrefixedString subject; // 0x1
        MDRPrefixedString summary;

        MDR_DEFINE_EXTERN_READ_WRITE(GsSettingInfo);
    };

    // THMSGV1T1KeepAliveLinkControlNotifiedParam
    struct KeepAliveLinkControlNotifiedParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LinkControlInquiredType type{LinkControlInquiredType::KEEP_ALIVE}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(KeepAliveLinkControlNotifiedParam);
    };

    // THMSGV1T1KeepAliveLinkControlSettingParam
    struct KeepAliveLinkControlSettingParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LinkControlInquiredType type{LinkControlInquiredType::KEEP_ALIVE}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x1
        UInt8 timeMin{}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(KeepAliveLinkControlSettingParam);
    };

    // THMSGV1T1LeftRightBatteryParam
    struct LeftRightBatteryParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        BatteryInquiredType type{BatteryInquiredType::LEFT_RIGHT_BATTERY}; // 0x0
        UInt8 leftLevel{}; // 0x1
        BatteryChargingStatus leftChargingStatus{BatteryChargingStatus::NOT_CHARGING}; // 0x2
        UInt8 rightLevel{}; // 0x3
        BatteryChargingStatus rightChargingStatus{BatteryChargingStatus::NOT_CHARGING}; // 0x4

        MDR_DEFINE_EXTERN_READ_WRITE(LeftRightBatteryParam);
    };

    // THMSGV1T1LeftRightConnectionStatusParam
    struct LeftRightConnectionStatusParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ConnectionStatusInquiredType type{ConnectionStatusInquiredType::LEFT_RIGHT_CONNECTION_STATUS}; // 0x0
        ConnectionStatus leftConnectionStatus{ConnectionStatus::NOT_CONNECTED}; // 0x1
        ConnectionStatus rightConnectionStatus{ConnectionStatus::NOT_CONNECTED}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(LeftRightConnectionStatusParam);
    };

    // THMSGV1T1NcAsmParam
    struct NcAsmParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF}; // 0x1
        NcAsmSettingType ncType{NcAsmSettingType::ON_OFF}; // 0x2
        UInt8 ncValue{}; // 0x3
        AsmSettingType asmType{AsmSettingType::ON_OFF}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId asmId{AsmId::NORMAL}; // 0x5
        UInt8 asmValue{}; // 0x6

        MDR_DEFINE_EXTERN_READ_WRITE(NcAsmParam);
    };

    // THMSGV1T1NcParam
    struct NcParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NOISE_CANCELLING}; // 0x0
        NcSettingType ncSettingType{NcSettingType::ON_OFF}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcSettingValue ncSettingValue{NcSettingValue::OFF}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(NcParam);
    };

    // THMSGV1T1NotifyAlertParam
    struct NotifyAlertParam
    {
        // CODEGEN EnumRange Command::ALERT_NTFY_PARAM
        Command command{Command::ALERT_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AlertInquiredType type{AlertInquiredType::NO_USE}; // 0x1
        AlertMessageType messageType{}; // 0x2
        AlertActionType actionType{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyAlertParam);
    };

    // THMSGV1T1NotifyAudioCodec
    struct NotifyAudioCodec
    {
        // CODEGEN EnumRange Command::COMMON_NTFY_AUDIO_CODEC
        Command command{Command::COMMON_NTFY_AUDIO_CODEC}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonCapabilityInquiredType commonCapabilityInquiredType{CommonCapabilityInquiredType::FIXED_VALUE}; // 0x1
        AudioCodec audioCodec{AudioCodec::UNSETTLED}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyAudioCodec);
    };

    // THMSGV1T1NotifyAudioParam
    struct NotifyAudioParamConnectionModeParam
    {
        // CODEGEN EnumRange Command::AUDIO_NTFY_PARAM
        Command command{Command::AUDIO_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType type{AudioInquiredType::CONNECTION_MODE}; // 0x1
        ConnectionModeSettingType settingType{ConnectionModeSettingType::SOUND_CONNECTION}; // 0x2
        ConnectionModeSettingValue settingValue{ConnectionModeSettingValue::SOUND_QUALITY_PRIOR}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyAudioParamConnectionModeParam);
    };

    // THMSGV1T1NotifyAudioParam
    struct NotifyAudioParamRetAudioCapability_AudioCapabilityBase
    {
        // CODEGEN EnumRange Command::AUDIO_NTFY_PARAM
        Command command{Command::AUDIO_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType type{AudioInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyAudioParamRetAudioCapability_AudioCapabilityBase);
    };

    // THMSGV1T1NotifyAudioParam
    struct NotifyAudioParamRetAudioCapability_ConnectionModeCapability
    {
        // CODEGEN EnumRange Command::AUDIO_NTFY_PARAM
        Command command{Command::AUDIO_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType type{AudioInquiredType::CONNECTION_MODE}; // 0x1
        ConnectionModeSettingType settingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyAudioParamRetAudioCapability_ConnectionModeCapability);
    };

    // THMSGV1T1NotifyAudioParam
    struct NotifyAudioParamUpscalingParam
    {
        // CODEGEN EnumRange Command::AUDIO_NTFY_PARAM
        Command command{Command::AUDIO_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType type{AudioInquiredType::UPSCALING}; // 0x1
        UpscalingSettingType settingType{UpscalingSettingType::AUTO_OFF}; // 0x2
        UpscalingSettingValue settingValue{UpscalingSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyAudioParamUpscalingParam);
    };

    // THMSGV1T1NotifyAudioStatus
    struct NotifyAudioStatus
    {
        // CODEGEN EnumRange Command::AUDIO_NTFY_STATUS
        Command command{Command::AUDIO_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType type{AudioInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyAudioStatus);
    };

    // THMSGV1T1NotifyBatteryLevel
    struct NotifyBatteryLevelBatteryParam
    {
        // CODEGEN EnumRange Command::COMMON_NTFY_BATTERY_LEVEL
        Command command{Command::COMMON_NTFY_BATTERY_LEVEL}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        BatteryInquiredType type{BatteryInquiredType::BATTERY}; // 0x1
        UInt8 level{}; // 0x2
        BatteryChargingStatus chargingStatus{BatteryChargingStatus::NOT_CHARGING}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyBatteryLevelBatteryParam);
    };

    // THMSGV1T1NotifyBatteryLevel
    struct NotifyBatteryLevelCradleBatteryParam
    {
        // CODEGEN EnumRange Command::COMMON_NTFY_BATTERY_LEVEL
        Command command{Command::COMMON_NTFY_BATTERY_LEVEL}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        BatteryInquiredType type{BatteryInquiredType::CRADLE_BATTERY}; // 0x1
        UInt8 level{}; // 0x2
        BatteryChargingStatus chargingStatus{BatteryChargingStatus::NOT_CHARGING}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyBatteryLevelCradleBatteryParam);
    };

    // THMSGV1T1NotifyBatteryLevel
    struct NotifyBatteryLevelLeftRightBatteryParam
    {
        // CODEGEN EnumRange Command::COMMON_NTFY_BATTERY_LEVEL
        Command command{Command::COMMON_NTFY_BATTERY_LEVEL}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        BatteryInquiredType type{BatteryInquiredType::LEFT_RIGHT_BATTERY}; // 0x1
        UInt8 leftLevel{}; // 0x2
        BatteryChargingStatus leftChargingStatus{BatteryChargingStatus::NOT_CHARGING}; // 0x3
        UInt8 rightLevel{}; // 0x4
        BatteryChargingStatus rightChargingStatus{BatteryChargingStatus::NOT_CHARGING}; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyBatteryLevelLeftRightBatteryParam);
    };

    // THMSGV1T1NotifyBatteryLevel
    struct NotifyBatteryLevelUpdateBatteryPowerThresholdForInterruptiongUpdtParam
    {
        // CODEGEN EnumRange Command::COMMON_NTFY_BATTERY_LEVEL
        Command command{Command::COMMON_NTFY_BATTERY_LEVEL}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        BatteryInquiredType type{BatteryInquiredType::BATTERY}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType updateInquiredType{UpdateInquiredType::BATTERY_POWER_THRESHOLD_FOR_INTERRUPTIONG_FW_UPDATE}; // 0x2
        UInt8 threshold{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyBatteryLevelUpdateBatteryPowerThresholdForInterruptiongUpdtParam);
    };

    // THMSGV1T1NotifyBatteryLevel
    struct NotifyBatteryLevelUpdateBatteryPowerThresholdParam
    {
        // CODEGEN EnumRange Command::COMMON_NTFY_BATTERY_LEVEL
        Command command{Command::COMMON_NTFY_BATTERY_LEVEL}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        BatteryInquiredType type{BatteryInquiredType::BATTERY}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType updateInquiredType{UpdateInquiredType::BATTERY_POWER_THRESHOLD}; // 0x2
        UInt8 threshold{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyBatteryLevelUpdateBatteryPowerThresholdParam);
    };

    // THMSGV1T1NotifyConnectionStatus
    struct NotifyConnectionStatusLeftRightConnectionStatusParam
    {
        // CODEGEN EnumRange Command::COMMON_NTFY_CONNECTION_STATUS
        Command command{Command::COMMON_NTFY_CONNECTION_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ConnectionStatusInquiredType type{ConnectionStatusInquiredType::LEFT_RIGHT_CONNECTION_STATUS}; // 0x1
        ConnectionStatus leftConnectionStatus{ConnectionStatus::NOT_CONNECTED}; // 0x2
        ConnectionStatus rightConnectionStatus{ConnectionStatus::NOT_CONNECTED}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyConnectionStatusLeftRightConnectionStatusParam);
    };

    // THMSGV1T1NotifyEqEbbParam
    struct NotifyEqEbbParamEbbParam
    {
        // CODEGEN EnumRange Command::EQEBB_NTFY_PARAM
        Command command{Command::EQEBB_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::EBB}; // 0x1
        UInt8 level{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyEqEbbParamEbbParam);
    };

    // THMSGV1T1NotifyEqEbbParam
    struct NotifyEqEbbParamEqParam
    {
        // CODEGEN EnumRange Command::EQEBB_NTFY_PARAM
        Command command{Command::EQEBB_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x1
        EqPresetId presetId{EqPresetId::OFF}; // 0x2
        MDRPodArray<UInt8> bandSteps; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifyEqEbbParamEqParam);
    };

    // THMSGV1T1NotifyEqEbbParam
    struct NotifyEqEbbParamRetEqEbbCapability_EbbCapability
    {
        // CODEGEN EnumRange Command::EQEBB_NTFY_PARAM
        Command command{Command::EQEBB_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::EBB}; // 0x1
        UInt8 minValue{}; // 0x2
        UInt8 maxValue{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyEqEbbParamRetEqEbbCapability_EbbCapability);
    };

    // THMSGV1T1NotifyEqEbbParam
    struct NotifyEqEbbParamRetEqEbbCapability_EqEbbCapabilityBase
    {
        // CODEGEN EnumRange Command::EQEBB_NTFY_PARAM
        Command command{Command::EQEBB_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyEqEbbParamRetEqEbbCapability_EqEbbCapabilityBase);
    };

    // THMSGV1T1NotifyEqEbbStatus
    struct NotifyEqEbbStatus
    {
        // CODEGEN EnumRange Command::EQEBB_NTFY_STATUS
        Command command{Command::EQEBB_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus commonStatus{CommonStatus::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyEqEbbStatus);
    };

    // THMSGV1T1NotifyGsParam
    struct NotifyGsParamGsBooleanTypeValue
    {
        // CODEGEN EnumRange Command::GENERAL_SETTING_NTNY_PARAM
        Command command{Command::GENERAL_SETTING_NTNY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsInquiredType type{GsInquiredType::GENERAL_SETTING1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsSettingType settingType{GsSettingType::BOOLEAN_TYPE}; // 0x2
        CommonOnOffSettingValue settingValue{CommonOnOffSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyGsParamGsBooleanTypeValue);
    };

    // THMSGV1T1NotifyGsParam
    struct NotifyGsParamGsListTypeValue
    {
        // CODEGEN EnumRange Command::GENERAL_SETTING_NTNY_PARAM
        Command command{Command::GENERAL_SETTING_NTNY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsInquiredType type{GsInquiredType::GENERAL_SETTING1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsSettingType settingType{GsSettingType::LIST_TYPE}; // 0x2
        UInt8 currentElementIndex{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyGsParamGsListTypeValue);
    };

    // THMSGV1T1NotifyGsStatus
    struct NotifyGsStatus
    {
        // CODEGEN EnumRange Command::GENERAL_SETTING_NTFY_STATUS
        Command command{Command::GENERAL_SETTING_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsInquiredType type{GsInquiredType::GENERAL_SETTING1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyGsStatus);
    };

    // THMSGV1T1NotifyLinkControl
    struct NotifyLinkControlKeepAliveLinkControlNotifiedParam
    {
        // CODEGEN EnumRange Command::COMMON_NTFY_LINK_CONTROL
        Command command{Command::COMMON_NTFY_LINK_CONTROL}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LinkControlInquiredType type{LinkControlInquiredType::KEEP_ALIVE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyLinkControlKeepAliveLinkControlNotifiedParam);
    };

    // THMSGV1T1NotifyLinkControl
    struct NotifyLinkControlKeepAliveLinkControlSettingParam
    {
        // CODEGEN EnumRange Command::COMMON_NTFY_LINK_CONTROL
        Command command{Command::COMMON_NTFY_LINK_CONTROL}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LinkControlInquiredType type{LinkControlInquiredType::KEEP_ALIVE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2
        UInt8 timeMin{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyLinkControlKeepAliveLinkControlSettingParam);
    };

    // THMSGV1T1NotifyLogParam
    struct NotifyLogParam
    {
        // CODEGEN EnumRange Command::LOG_NTFY_PARAM
        Command command{Command::LOG_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LogInquiredType logInquiredType{LogInquiredType::NO_USE}; // 0x1
        MDRPrefixedString data; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifyLogParam);
    };

    // THMSGV1T1NotifyNcAsmParam
    struct NotifyNcAsmParamAsmParam
    {
        // CODEGEN EnumRange Command::NCASM_NTFY_PARAM
        Command command{Command::NCASM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::AMBIENT_SOUND_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF}; // 0x2
        AsmSettingType asmType{AsmSettingType::ON_OFF}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId asmId{AsmId::NORMAL}; // 0x4
        UInt8 asmValue{}; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyNcAsmParamAsmParam);
    };

    // THMSGV1T1NotifyNcAsmParam
    struct NotifyNcAsmParamRetNcAsmCapability_NcAsmCapabilityBase
    {
        // CODEGEN EnumRange Command::NCASM_NTFY_PARAM
        Command command{Command::NCASM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyNcAsmParamRetNcAsmCapability_NcAsmCapabilityBase);
    };

    // THMSGV1T1NotifyNcAsmParam
    struct NotifyNcAsmParamRetNcAsmCapability_NcCapability
    {
        // CODEGEN EnumRange Command::NCASM_NTFY_PARAM
        Command command{Command::NCASM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NOISE_CANCELLING}; // 0x1
        NcSettingType settingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyNcAsmParamRetNcAsmCapability_NcCapability);
    };

    // THMSGV1T1NotifyNcAsmParam
    struct NotifyNcAsmParamcAsmParam
    {
        // CODEGEN EnumRange Command::NCASM_NTFY_PARAM
        Command command{Command::NCASM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF}; // 0x2
        NcAsmSettingType ncType{NcAsmSettingType::ON_OFF}; // 0x3
        UInt8 ncValue{}; // 0x4
        AsmSettingType asmType{AsmSettingType::ON_OFF}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId asmId{AsmId::NORMAL}; // 0x6
        UInt8 asmValue{}; // 0x7

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyNcAsmParamcAsmParam);
    };

    // THMSGV1T1NotifyNcAsmParam
    struct NotifyNcAsmParamcParam
    {
        // CODEGEN EnumRange Command::NCASM_NTFY_PARAM
        Command command{Command::NCASM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NOISE_CANCELLING}; // 0x1
        NcSettingType ncSettingType{NcSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcSettingValue ncSettingValue{NcSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyNcAsmParamcParam);
    };

    // THMSGV1T1NotifyNcAsmStatus
    struct NotifyNcAsmStatus
    {
        // CODEGEN EnumRange Command::NCASM_NTFY_STATUS
        Command command{Command::NCASM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyNcAsmStatus);
    };

    // THMSGV1T1NotifyOptimizerParam
    struct NotifyOptimizerParam
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OptimizerInquiredType type{OptimizerInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PersonalMeasureType personalType{PersonalMeasureType::NOT_SUPPORT}; // 0x2
        UInt8 personalValue{}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        BarometricMeasureType barometricType{BarometricMeasureType::NOT_SUPPORT}; // 0x4
        UInt8 barometricValue{}; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyOptimizerParam);
    };

    // THMSGV1T1NotifyOptimizerStatus
    struct NotifyOptimizerStatus
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OptimizerInquiredType type{OptimizerInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2
        OptimizerStatus optimizerStatus{OptimizerStatus::IDLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyOptimizerStatus);
    };

    // THMSGV1T1NotifyPlayParam
    struct NotifyPlayParamPlaybackControllerVolumeData
    {
        // CODEGEN EnumRange Command::PLAY_NTFY_PARAM
        Command command{Command::PLAY_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlayInquiredType playInquiredType{PlayInquiredType::PLAYBACK_CONTROLLER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackDetailedDataType dataType{PlaybackDetailedDataType::VOLUME}; // 0x2
        UInt8 volumeValue{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyPlayParamPlaybackControllerVolumeData);
    };

    // THMSGV1T1NotifyPlayStatus
    struct NotifyPlayStatus
    {
        // CODEGEN EnumRange Command::PLAY_NTFY_STATUS
        Command command{Command::PLAY_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlayInquiredType playInquiredType{PlayInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackStatus playbackStatus{PlaybackStatus::UNSETTLED}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyPlayStatus);
    };

    // THMSGV1T1NotifySportsExParam
    struct NotifySportsExParamChildPayloadGetSportsExParam_TrainingModeRequest
    {
        // CODEGEN EnumRange Command::SPORTS_NTFY_PARAM
        Command command{Command::SPORTS_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::TRAINING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        TrainingModeExParameterType trainingModeExParameterType{TrainingModeExParameterType::NO_USE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySportsExParamChildPayloadGetSportsExParam_TrainingModeRequest);
    };

    // THMSGV1T1NotifySportsExParam
    struct NotifySportsExParamChildPayloadTrainingModeExAsmParam
    {
        // CODEGEN EnumRange Command::SPORTS_NTFY_PARAM
        Command command{Command::SPORTS_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::TRAINING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF}; // 0x2
        AsmSettingType asmType{AsmSettingType::ON_OFF}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId asmId{AsmId::NORMAL}; // 0x4
        UInt8 asmValue{}; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySportsExParamChildPayloadTrainingModeExAsmParam);
    };

    // THMSGV1T1NotifySportsExParam
    struct NotifySportsExParamChildPayloadTrainingModeExEqParam
    {
        // CODEGEN EnumRange Command::SPORTS_NTFY_PARAM
        Command command{Command::SPORTS_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::TRAINING_MODE}; // 0x1
        EqPresetId presetId{EqPresetId::OFF}; // 0x2
        MDRPodArray<UInt8> bandSteps; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySportsExParamChildPayloadTrainingModeExEqParam);
    };

    // THMSGV1T1NotifySportsExParam
    struct NotifySportsExParamChildPayloadTrainingModeExNcAsmParam
    {
        // CODEGEN EnumRange Command::SPORTS_NTFY_PARAM
        Command command{Command::SPORTS_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::TRAINING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF}; // 0x2
        NcAsmSettingType ncType{NcAsmSettingType::ON_OFF}; // 0x3
        UInt8 ncValue{}; // 0x4
        AsmSettingType asmType{AsmSettingType::ON_OFF}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId asmId{AsmId::NORMAL}; // 0x6
        UInt8 asmValue{}; // 0x7

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySportsExParamChildPayloadTrainingModeExNcAsmParam);
    };

    // THMSGV1T1NotifySportsParam
    struct NotifySportsParamTrainingModeParam
    {
        // CODEGEN EnumRange Command::SPORTS_NTFY_PARAM
        Command command{Command::SPORTS_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::TRAINING_MODE}; // 0x1
        CommonOnOffSettingType settingType{CommonOnOffSettingType::ON_OFF}; // 0x2
        CommonOnOffSettingValue settingValue{CommonOnOffSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySportsParamTrainingModeParam);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamChildPayloadAssignableSettingsParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        MDRPodArray<AssignableSettingsPreset> presets; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemExParamChildPayloadAssignableSettingsParam);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamChildPayloadAutoPowerOffParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AutoPowerOffParameterType autoPowerOffParameterType{AutoPowerOffParameterType::ACTIVE_AND_SELECTIME_ID}; // 0x2
        AutoPowerOffElementId activeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x3
        AutoPowerOffElementId selectTimeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamChildPayloadAutoPowerOffParam);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamChildPayloadControlByWearingParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType settingType{ControlByWearingSettingType::ON_OFF}; // 0x2
        ControlByWearingSettingValue settingValue{ControlByWearingSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamChildPayloadControlByWearingParam);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamChildPayloadPowerSavingModeParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType settingType{PowerSavingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerSavingModeSettingValue settingValue{PowerSavingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamChildPayloadPowerSavingModeParam);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamChildPayloadRetSystemCapability_AutoPowerOffCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        MDRPodArray<AutoPowerOffElementId> candidateElements; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemExParamChildPayloadRetSystemCapability_AutoPowerOffCapability);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamChildPayloadRetSystemCapability_ControlByWearingCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType controlByWearingSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamChildPayloadRetSystemCapability_ControlByWearingCapability);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamChildPayloadRetSystemCapability_PowerSavingModeCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType powerSavingModeSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamChildPayloadRetSystemCapability_PowerSavingModeCapability);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamChildPayloadRetSystemCapability_SmartTalkingModeCapabilityVariant1
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType smartTalkingModeSettingType{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType smartTalkingModePreviewType{}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType smartTalkingModeDetailSettingType{}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType smartTalkingModeDetectionSensitivityType{}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType smartTalkingModeVoceFocusType{}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType smartTalkingModeModeOutTimeType{}; // 0x7
        MDRPodArray<UInt8> smartTalkingModeModeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemExParamChildPayloadRetSystemCapability_SmartTalkingModeCapabilityVariant1);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamChildPayloadRetSystemCapability_SmartTalkingModeCapabilityVariant2
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType previewType{SmartTalkingModePreviewType::NOT_SUPPORT}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType detailSettingType{SmartTalkingModeDetailSettingType::TYPE_1}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType detectionSensitivityType{SmartTalkingModeDetectionSensitivityType::AUTO_HIGH_LOW}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType voiceFocusType{SmartTalkingModeVoiceFocusType::ON_OFF}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType modeOutTimeType{SmartTalkingModeModeOutTimeType::TYPE_1}; // 0x7
        MDRPodArray<UInt8> modeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemExParamChildPayloadRetSystemCapability_SmartTalkingModeCapabilityVariant2);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamChildPayloadRetSystemCapability_SystemCapabilityBase
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamChildPayloadRetSystemCapability_SystemCapabilityBase);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamChildPayloadRetSystemCapability_VibratorCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType vibratorSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamChildPayloadRetSystemCapability_VibratorCapability);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamChildPayloadSmartTalkingModeExType1Param
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        DetectionSensitivity devectionSensitivity{DetectionSensitivity::AUTO}; // 0x2
        CommonOnOffSettingValue voiceFocus{CommonOnOffSettingValue::OFF}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ModeOutTime modeOutTime{ModeOutTime::FAST}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamChildPayloadSmartTalkingModeExType1Param);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamChildPayloadSmartTalkingModeRetParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamChildPayloadSmartTalkingModeRetParam);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamChildPayloadSmartTalkingModeSetNtfyParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        SmartTalkingModeParameterType parameterType{SmartTalkingModeParameterType::NO_USE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamChildPayloadSmartTalkingModeSetNtfyParam);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamChildPayloadVibratorParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType settingType{VibratorSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VibratorSettingValue settingValue{VibratorSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamChildPayloadVibratorParam);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamSmartTalkingModeExParamAssignableSettingsParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        MDRPodArray<AssignableSettingsPreset> presets; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemExParamSmartTalkingModeExParamAssignableSettingsParam);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamSmartTalkingModeExParamAutoPowerOffParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AutoPowerOffParameterType autoPowerOffParameterType{AutoPowerOffParameterType::ACTIVE_AND_SELECTIME_ID}; // 0x2
        AutoPowerOffElementId activeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x3
        AutoPowerOffElementId selectTimeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamSmartTalkingModeExParamAutoPowerOffParam);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamSmartTalkingModeExParamControlByWearingParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType settingType{ControlByWearingSettingType::ON_OFF}; // 0x2
        ControlByWearingSettingValue settingValue{ControlByWearingSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamSmartTalkingModeExParamControlByWearingParam);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamSmartTalkingModeExParamPowerSavingModeParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType settingType{PowerSavingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerSavingModeSettingValue settingValue{PowerSavingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamSmartTalkingModeExParamPowerSavingModeParam);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamSmartTalkingModeExParamRetSystemCapability_AutoPowerOffCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        MDRPodArray<AutoPowerOffElementId> candidateElements; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemExParamSmartTalkingModeExParamRetSystemCapability_AutoPowerOffCapability);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamSmartTalkingModeExParamRetSystemCapability_ControlByWearingCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType controlByWearingSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamSmartTalkingModeExParamRetSystemCapability_ControlByWearingCapability);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamSmartTalkingModeExParamRetSystemCapability_PowerSavingModeCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType powerSavingModeSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamSmartTalkingModeExParamRetSystemCapability_PowerSavingModeCapability);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamSmartTalkingModeExParamRetSystemCapability_SystemCapabilityBase
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamSmartTalkingModeExParamRetSystemCapability_SystemCapabilityBase);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamSmartTalkingModeExParamRetSystemCapability_VibratorCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType vibratorSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamSmartTalkingModeExParamRetSystemCapability_VibratorCapability);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamSmartTalkingModeExParamSmartTalkingModeExParamType1Param
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType detailSettingType{SmartTalkingModeDetailSettingType::TYPE_1}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        DetectionSensitivity devectionSensitivity{DetectionSensitivity::AUTO}; // 0x3
        CommonOnOffSettingValue voiceFocus{CommonOnOffSettingValue::OFF}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ModeOutTime modeOutTime{ModeOutTime::FAST}; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamSmartTalkingModeExParamSmartTalkingModeExParamType1Param);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamSmartTalkingModeExParamVibratorParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType settingType{VibratorSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VibratorSettingValue settingValue{VibratorSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemExParamSmartTalkingModeExParamVibratorParam);
    };

    // THMSGV1T1NotifySystemParam
    struct NotifySystemParamAssignableSettingsParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        MDRPodArray<AssignableSettingsPreset> presets; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemParamAssignableSettingsParam);
    };

    // THMSGV1T1NotifySystemParam
    struct NotifySystemParamAutoPowerOffParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AutoPowerOffParameterType autoPowerOffParameterType{AutoPowerOffParameterType::ACTIVE_AND_SELECTIME_ID}; // 0x2
        AutoPowerOffElementId activeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x3
        AutoPowerOffElementId selectTimeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemParamAutoPowerOffParam);
    };

    // THMSGV1T1NotifySystemParam
    struct NotifySystemParamControlByWearingParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType settingType{ControlByWearingSettingType::ON_OFF}; // 0x2
        ControlByWearingSettingValue settingValue{ControlByWearingSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemParamControlByWearingParam);
    };

    // THMSGV1T1NotifySystemParam
    struct NotifySystemParamPowerSavingModeParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType settingType{PowerSavingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerSavingModeSettingValue settingValue{PowerSavingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemParamPowerSavingModeParam);
    };

    // THMSGV1T1NotifySystemParam
    struct NotifySystemParamRetSystemCapability_AutoPowerOffCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        MDRPodArray<AutoPowerOffElementId> candidateElements; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemParamRetSystemCapability_AutoPowerOffCapability);
    };

    // THMSGV1T1NotifySystemParam
    struct NotifySystemParamRetSystemCapability_ControlByWearingCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType controlByWearingSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemParamRetSystemCapability_ControlByWearingCapability);
    };

    // THMSGV1T1NotifySystemParam
    struct NotifySystemParamRetSystemCapability_PowerSavingModeCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType powerSavingModeSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemParamRetSystemCapability_PowerSavingModeCapability);
    };

    // THMSGV1T1NotifySystemParam
    struct NotifySystemParamRetSystemCapability_SmartTalkingModeCapabilityVariant1
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType smartTalkingModeSettingType{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType smartTalkingModePreviewType{}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType smartTalkingModeDetailSettingType{}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType smartTalkingModeDetectionSensitivityType{}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType smartTalkingModeVoceFocusType{}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType smartTalkingModeModeOutTimeType{}; // 0x7
        MDRPodArray<UInt8> smartTalkingModeModeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemParamRetSystemCapability_SmartTalkingModeCapabilityVariant1);
    };

    // THMSGV1T1NotifySystemParam
    struct NotifySystemParamRetSystemCapability_SmartTalkingModeCapabilityVariant2
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType previewType{SmartTalkingModePreviewType::NOT_SUPPORT}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType detailSettingType{SmartTalkingModeDetailSettingType::TYPE_1}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType detectionSensitivityType{SmartTalkingModeDetectionSensitivityType::AUTO_HIGH_LOW}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType voiceFocusType{SmartTalkingModeVoiceFocusType::ON_OFF}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType modeOutTimeType{SmartTalkingModeModeOutTimeType::TYPE_1}; // 0x7
        MDRPodArray<UInt8> modeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemParamRetSystemCapability_SmartTalkingModeCapabilityVariant2);
    };

    // THMSGV1T1NotifySystemParam
    struct NotifySystemParamRetSystemCapability_SystemCapabilityBase
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemParamRetSystemCapability_SystemCapabilityBase);
    };

    // THMSGV1T1NotifySystemParam
    struct NotifySystemParamRetSystemCapability_VibratorCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType vibratorSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemParamRetSystemCapability_VibratorCapability);
    };

    // THMSGV1T1NotifySystemParam
    struct NotifySystemParamSmartTalkingModeRetParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemParamSmartTalkingModeRetParam);
    };

    // THMSGV1T1NotifySystemParam
    struct NotifySystemParamSmartTalkingModeSetNtfyParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        SmartTalkingModeParameterType parameterType{SmartTalkingModeParameterType::NO_USE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemParamSmartTalkingModeSetNtfyParam);
    };

    // THMSGV1T1NotifySystemParam
    struct NotifySystemParamVibratorAlertNotificationParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AlertInquiredType alertInquiredType{AlertInquiredType::VIBRATOR_ALERT_NOTIFICATION}; // 0x2
        VibrationType vibrationType{VibrationType::NO_PATTERN_SPECIFIED}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemParamVibratorAlertNotificationParam);
    };

    // THMSGV1T1NotifySystemParam
    struct NotifySystemParamVibratorParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType settingType{VibratorSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VibratorSettingValue settingValue{VibratorSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemParamVibratorParam);
    };

    // THMSGV1T1NotifySystemStatus
    struct NotifySystemStatusAssignableSettingsParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        MDRPodArray<AssignableSettingsPreset> presets; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemStatusAssignableSettingsParam);
    };

    // THMSGV1T1NotifySystemStatus
    struct NotifySystemStatusAssignableSettingsStatus
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        MDRPodArray<CommonStatus> allStatus; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE};

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemStatusAssignableSettingsStatus);
    };

    // THMSGV1T1NotifySystemStatus
    struct NotifySystemStatusAutoPowerOffParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AutoPowerOffParameterType autoPowerOffParameterType{AutoPowerOffParameterType::ACTIVE_AND_SELECTIME_ID}; // 0x2
        AutoPowerOffElementId activeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x3
        AutoPowerOffElementId selectTimeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemStatusAutoPowerOffParam);
    };

    // THMSGV1T1NotifySystemStatus
    struct NotifySystemStatusControlByWearingParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType settingType{ControlByWearingSettingType::ON_OFF}; // 0x2
        ControlByWearingSettingValue settingValue{ControlByWearingSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemStatusControlByWearingParam);
    };

    // THMSGV1T1NotifySystemStatus
    struct NotifySystemStatusPowerSavingModeParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType settingType{PowerSavingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerSavingModeSettingValue settingValue{PowerSavingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemStatusPowerSavingModeParam);
    };

    // THMSGV1T1NotifySystemStatus
    struct NotifySystemStatusRetSystemCapability_AutoPowerOffCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        MDRPodArray<AutoPowerOffElementId> candidateElements; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemStatusRetSystemCapability_AutoPowerOffCapability);
    };

    // THMSGV1T1NotifySystemStatus
    struct NotifySystemStatusRetSystemCapability_ControlByWearingCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType controlByWearingSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemStatusRetSystemCapability_ControlByWearingCapability);
    };

    // THMSGV1T1NotifySystemStatus
    struct NotifySystemStatusRetSystemCapability_PowerSavingModeCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType powerSavingModeSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemStatusRetSystemCapability_PowerSavingModeCapability);
    };

    // THMSGV1T1NotifySystemStatus
    struct NotifySystemStatusRetSystemCapability_SmartTalkingModeCapabilityVariant1
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType smartTalkingModeSettingType{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType smartTalkingModePreviewType{}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType smartTalkingModeDetailSettingType{}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType smartTalkingModeDetectionSensitivityType{}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType smartTalkingModeVoceFocusType{}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType smartTalkingModeModeOutTimeType{}; // 0x7
        MDRPodArray<UInt8> smartTalkingModeModeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemStatusRetSystemCapability_SmartTalkingModeCapabilityVariant1);
    };

    // THMSGV1T1NotifySystemStatus
    struct NotifySystemStatusRetSystemCapability_SmartTalkingModeCapabilityVariant2
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType previewType{SmartTalkingModePreviewType::NOT_SUPPORT}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType detailSettingType{SmartTalkingModeDetailSettingType::TYPE_1}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType detectionSensitivityType{SmartTalkingModeDetectionSensitivityType::AUTO_HIGH_LOW}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType voiceFocusType{SmartTalkingModeVoiceFocusType::ON_OFF}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType modeOutTimeType{SmartTalkingModeModeOutTimeType::TYPE_1}; // 0x7
        MDRPodArray<UInt8> modeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemStatusRetSystemCapability_SmartTalkingModeCapabilityVariant2);
    };

    // THMSGV1T1NotifySystemStatus
    struct NotifySystemStatusRetSystemCapability_SystemCapabilityBase
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemStatusRetSystemCapability_SystemCapabilityBase);
    };

    // THMSGV1T1NotifySystemStatus
    struct NotifySystemStatusRetSystemCapability_VibratorCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType vibratorSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemStatusRetSystemCapability_VibratorCapability);
    };

    // THMSGV1T1NotifySystemStatus
    struct NotifySystemStatusSmartTalkingModeRetParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemStatusSmartTalkingModeRetParam);
    };

    // THMSGV1T1NotifySystemStatus
    struct NotifySystemStatusSmartTalkingModeSetNtfyParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        SmartTalkingModeParameterType parameterType{SmartTalkingModeParameterType::NO_USE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemStatusSmartTalkingModeSetNtfyParam);
    };

    // THMSGV1T1NotifySystemStatus
    struct NotifySystemStatusSmartTalkingModeStatus
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeEffectStatus effectStatus{SmartTalkingModeEffectStatus::NOT_ACTIVE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemStatusSmartTalkingModeStatus);
    };

    // THMSGV1T1NotifySystemStatus
    struct NotifySystemStatusVibratorParam
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType settingType{VibratorSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VibratorSettingValue settingValue{VibratorSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifySystemStatusVibratorParam);
    };

    // THMSGV1T1NotifyUpdateStatus
    struct NotifyUpdateStatus
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType type{UpdateInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyUpdateStatus);
    };

    // THMSGV1T1NotifyUpscalingEffect
    struct NotifyUpscalingEffect
    {
        // CODEGEN EnumRange Command::COMMON_NTFY_UPSCALING_EFFECT
        Command command{Command::COMMON_NTFY_UPSCALING_EFFECT}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonCapabilityInquiredType commonCapabilityInquiredType{CommonCapabilityInquiredType::FIXED_VALUE}; // 0x1
        UpscalingEffectType effectType{UpscalingEffectType::DSEE_HX}; // 0x2
        UpscalingEffectStatus effectStatus{UpscalingEffectStatus::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyUpscalingEffect);
    };

    // THMSGV1T1NotifyVptParam
    struct NotifyVptParamRetVptCapability_VptCapabilityBase
    {
        // CODEGEN EnumRange Command::VPT_NTFY_PARAM
        Command command{Command::VPT_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::VPT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyVptParamRetVptCapability_VptCapabilityBase);
    };

    // THMSGV1T1NotifyVptParam
    struct NotifyVptParamSoundPositionParam
    {
        // CODEGEN EnumRange Command::VPT_NTFY_PARAM
        Command command{Command::VPT_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::SOUND_POSITION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SoundPositionPresetId presetId{SoundPositionPresetId::OFF}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyVptParamSoundPositionParam);
    };

    // THMSGV1T1NotifyVptParam
    struct NotifyVptParamVptParam
    {
        // CODEGEN EnumRange Command::VPT_NTFY_PARAM
        Command command{Command::VPT_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::VPT}; // 0x1
        VptPresetId presetId{VptPresetId::OFF}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyVptParamVptParam);
    };

    // THMSGV1T1NotifyVptStatus
    struct NotifyVptStatus
    {
        // CODEGEN EnumRange Command::VPT_NTFY_STATUS
        Command command{Command::VPT_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::VPT}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyVptStatus);
    };

    // THMSGV1T1Payload
    struct Payload
    {
        UInt8 commandType{}; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(Payload);
    };

    // THMSGV1T1PlaybackControllerNotifyNameData
    struct PlaybackControllerNotifyNameData
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackDetailedDataType dataType{PlaybackDetailedDataType::TRACK_NAME}; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(PlaybackControllerNotifyNameData);
    };

    // THMSGV1T1PlaybackControllerVolumeData
    struct PlaybackControllerVolumeData
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackDetailedDataType dataType{PlaybackDetailedDataType::VOLUME}; // 0x0
        UInt8 volumeValue{}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(PlaybackControllerVolumeData);
    };

    // THMSGV1T1PlaybackName
    struct PlaybackName
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackNameStatus nameStatus{PlaybackNameStatus::UNSETTLED}; // 0x0
        MDRPrefixedString name; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(PlaybackName);
    };

    // THMSGV1T1PowerSavingModeParam
    struct PowerSavingModeParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x0
        PowerSavingModeSettingType settingType{PowerSavingModeSettingType::ON_OFF}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerSavingModeSettingValue settingValue{PowerSavingModeSettingValue::OFF}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(PowerSavingModeParam);
    };

    // THMSGV1T1RawPayload
    struct RawPayload
    {
        MDRPodArray<UInt8> bytes; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(RawPayload);
    };

    // THMSGV1T1RetAlertCapability
    struct RetAlertCapability
    {
        // CODEGEN EnumRange Command::ALERT_RET_CAPABILITY
        Command command{Command::ALERT_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AlertInquiredType inquiredType{}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AlertVibrationPattern vibrationPattern{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetAlertCapability);
    };

    // THMSGV1T1RetAudioCapability_AudioCapabilityBase
    struct RetAudioCapability_AudioCapabilityBase
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType type{AudioInquiredType::NO_USE}; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(RetAudioCapability_AudioCapabilityBase);
    };

    // THMSGV1T1RetAudioCodec
    struct RetAudioCodec
    {
        // CODEGEN EnumRange Command::COMMON_RET_AUDIO_CODEC
        Command command{Command::COMMON_RET_AUDIO_CODEC}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonCapabilityInquiredType commonCapabilityInquiredType{CommonCapabilityInquiredType::FIXED_VALUE}; // 0x1
        AudioCodec audioCodec{AudioCodec::UNSETTLED}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetAudioCodec);
    };

    // THMSGV1T1RetAudioParam
    struct RetAudioParamCapability_AudioCapabilityBase
    {
        // CODEGEN EnumRange Command::AUDIO_RET_PARAM
        Command command{Command::AUDIO_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType type{AudioInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetAudioParamCapability_AudioCapabilityBase);
    };

    // THMSGV1T1RetAudioParam
    struct RetAudioParamCapability_ConnectionModeCapability
    {
        // CODEGEN EnumRange Command::AUDIO_RET_PARAM
        Command command{Command::AUDIO_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType type{AudioInquiredType::CONNECTION_MODE}; // 0x1
        ConnectionModeSettingType settingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetAudioParamCapability_ConnectionModeCapability);
    };

    // THMSGV1T1RetAudioParam
    struct RetAudioParamConnectionModeParam
    {
        // CODEGEN EnumRange Command::AUDIO_RET_PARAM
        Command command{Command::AUDIO_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType type{AudioInquiredType::CONNECTION_MODE}; // 0x1
        ConnectionModeSettingType settingType{ConnectionModeSettingType::SOUND_CONNECTION}; // 0x2
        ConnectionModeSettingValue settingValue{ConnectionModeSettingValue::SOUND_QUALITY_PRIOR}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetAudioParamConnectionModeParam);
    };

    // THMSGV1T1RetAudioParam
    struct RetAudioParamUpscalingParam
    {
        // CODEGEN EnumRange Command::AUDIO_RET_PARAM
        Command command{Command::AUDIO_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType type{AudioInquiredType::UPSCALING}; // 0x1
        UpscalingSettingType settingType{UpscalingSettingType::AUTO_OFF}; // 0x2
        UpscalingSettingValue settingValue{UpscalingSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetAudioParamUpscalingParam);
    };

    // THMSGV1T1RetAudioStatus
    struct RetAudioStatus
    {
        // CODEGEN EnumRange Command::AUDIO_RET_STATUS
        Command command{Command::AUDIO_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType type{AudioInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetAudioStatus);
    };

    // THMSGV1T1RetBatteryLevel
    struct RetBatteryLevelBatteryParam
    {
        // CODEGEN EnumRange Command::COMMON_RET_BATTERY_LEVEL
        Command command{Command::COMMON_RET_BATTERY_LEVEL}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        BatteryInquiredType type{BatteryInquiredType::BATTERY}; // 0x1
        UInt8 level{}; // 0x2
        BatteryChargingStatus chargingStatus{BatteryChargingStatus::NOT_CHARGING}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetBatteryLevelBatteryParam);
    };

    // THMSGV1T1RetBatteryLevel
    struct RetBatteryLevelCradleBatteryParam
    {
        // CODEGEN EnumRange Command::COMMON_RET_BATTERY_LEVEL
        Command command{Command::COMMON_RET_BATTERY_LEVEL}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        BatteryInquiredType type{BatteryInquiredType::CRADLE_BATTERY}; // 0x1
        UInt8 level{}; // 0x2
        BatteryChargingStatus chargingStatus{BatteryChargingStatus::NOT_CHARGING}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetBatteryLevelCradleBatteryParam);
    };

    // THMSGV1T1RetBatteryLevel
    struct RetBatteryLevelLeftRightBatteryParam
    {
        // CODEGEN EnumRange Command::COMMON_RET_BATTERY_LEVEL
        Command command{Command::COMMON_RET_BATTERY_LEVEL}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        BatteryInquiredType type{BatteryInquiredType::LEFT_RIGHT_BATTERY}; // 0x1
        UInt8 leftLevel{}; // 0x2
        BatteryChargingStatus leftChargingStatus{BatteryChargingStatus::NOT_CHARGING}; // 0x3
        UInt8 rightLevel{}; // 0x4
        BatteryChargingStatus rightChargingStatus{BatteryChargingStatus::NOT_CHARGING}; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetBatteryLevelLeftRightBatteryParam);
    };

    // THMSGV1T1RetBatteryLevel
    struct RetBatteryLevelUpdateBatteryPowerThresholdForInterruptiongUpdtParam
    {
        // CODEGEN EnumRange Command::COMMON_RET_BATTERY_LEVEL
        Command command{Command::COMMON_RET_BATTERY_LEVEL}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        BatteryInquiredType type{BatteryInquiredType::BATTERY}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType updateInquiredType{UpdateInquiredType::BATTERY_POWER_THRESHOLD_FOR_INTERRUPTIONG_FW_UPDATE}; // 0x2
        UInt8 threshold{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetBatteryLevelUpdateBatteryPowerThresholdForInterruptiongUpdtParam);
    };

    // THMSGV1T1RetBatteryLevel
    struct RetBatteryLevelUpdateBatteryPowerThresholdParam
    {
        // CODEGEN EnumRange Command::COMMON_RET_BATTERY_LEVEL
        Command command{Command::COMMON_RET_BATTERY_LEVEL}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        BatteryInquiredType type{BatteryInquiredType::BATTERY}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType updateInquiredType{UpdateInquiredType::BATTERY_POWER_THRESHOLD}; // 0x2
        UInt8 threshold{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetBatteryLevelUpdateBatteryPowerThresholdParam);
    };

    // THMSGV1T1RetBluetoothDeviceInfo
    struct RetBluetoothDeviceInfo
    {
        // CODEGEN EnumRange Command::COMMON_RET_BLUETOOTH_DEVICE_INFO
        Command command{Command::COMMON_RET_BLUETOOTH_DEVICE_INFO}; // 0x0
        BluetoothDeviceInfoType type{}; // 0x1
        MDRPrefixedString deviceInformation; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetBluetoothDeviceInfo);
    };

    // THMSGV1T1RetCapabilityInfo
    struct RetCapabilityInfo
    {
        // CODEGEN EnumRange Command::CONNECT_RET_CAPABILITY_INFO
        Command command{Command::CONNECT_RET_CAPABILITY_INFO}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonCapabilityInquiredType type{}; // 0x1
        UInt8 capabilityCounter{}; // 0x2
        MDRPrefixedString uniqueId; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(RetCapabilityInfo);
    };

    // THMSGV1T1RetConciergeData
    struct RetConciergeData
    {
        // CODEGEN EnumRange Command::COMMON_RET_CONCIERGE_DATA
        Command command{Command::COMMON_RET_CONCIERGE_DATA}; // 0x0
        MDRPrefixedString data; // 0x1

        MDR_DEFINE_EXTERN_SERIALIZATION(RetConciergeData);
    };

    // THMSGV1T1RetConnectionStatus
    struct RetConnectionStatusLeftRightConnectionStatusParam
    {
        // CODEGEN EnumRange Command::COMMON_RET_CONNECTION_STATUS
        Command command{Command::COMMON_RET_CONNECTION_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ConnectionStatusInquiredType type{ConnectionStatusInquiredType::LEFT_RIGHT_CONNECTION_STATUS}; // 0x1
        ConnectionStatus leftConnectionStatus{ConnectionStatus::NOT_CONNECTED}; // 0x2
        ConnectionStatus rightConnectionStatus{ConnectionStatus::NOT_CONNECTED}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetConnectionStatusLeftRightConnectionStatusParam);
    };

    // THMSGV1T1RetDeviceInfo_DeviceInfoBase
    struct RetDeviceInfo_DeviceInfoBase
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        DeviceInfoInquiredType type{DeviceInfoInquiredType::NO_USE}; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(RetDeviceInfo_DeviceInfoBase);
    };

    // THMSGV1T1RetEqEbbCapability_EqEbbCapabilityBase
    struct RetEqEbbCapability_EqEbbCapabilityBase
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(RetEqEbbCapability_EqEbbCapabilityBase);
    };

    // THMSGV1T1RetEqEbbParam
    struct RetEqEbbParamCapability_EbbCapability
    {
        // CODEGEN EnumRange Command::EQEBB_RET_PARAM
        Command command{Command::EQEBB_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::EBB}; // 0x1
        UInt8 minValue{}; // 0x2
        UInt8 maxValue{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetEqEbbParamCapability_EbbCapability);
    };

    // THMSGV1T1RetEqEbbParam
    struct RetEqEbbParamCapability_EqEbbCapabilityBase
    {
        // CODEGEN EnumRange Command::EQEBB_RET_PARAM
        Command command{Command::EQEBB_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetEqEbbParamCapability_EqEbbCapabilityBase);
    };

    // THMSGV1T1RetEqEbbParam
    struct RetEqEbbParamEbbParam
    {
        // CODEGEN EnumRange Command::EQEBB_RET_PARAM
        Command command{Command::EQEBB_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::EBB}; // 0x1
        UInt8 level{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetEqEbbParamEbbParam);
    };

    // THMSGV1T1RetEqEbbParam
    struct RetEqEbbParamEqParam
    {
        // CODEGEN EnumRange Command::EQEBB_RET_PARAM
        Command command{Command::EQEBB_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x1
        EqPresetId presetId{EqPresetId::OFF}; // 0x2
        MDRPodArray<UInt8> bandSteps; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(RetEqEbbParamEqParam);
    };

    // THMSGV1T1RetEqEbbStatus
    struct RetEqEbbStatus
    {
        // CODEGEN EnumRange Command::EQEBB_RET_STATUS
        Command command{Command::EQEBB_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetEqEbbStatus);
    };

    // THMSGV1T1RetGsParam
    struct RetGsParamGsBooleanTypeValue
    {
        // CODEGEN EnumRange Command::GENERAL_SETTING_RET_PARAM
        Command command{Command::GENERAL_SETTING_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsInquiredType type{GsInquiredType::GENERAL_SETTING1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsSettingType settingType{GsSettingType::BOOLEAN_TYPE}; // 0x2
        CommonOnOffSettingValue settingValue{CommonOnOffSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetGsParamGsBooleanTypeValue);
    };

    // THMSGV1T1RetGsParam
    struct RetGsParamGsListTypeValue
    {
        // CODEGEN EnumRange Command::GENERAL_SETTING_RET_PARAM
        Command command{Command::GENERAL_SETTING_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsInquiredType type{GsInquiredType::GENERAL_SETTING1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsSettingType settingType{GsSettingType::LIST_TYPE}; // 0x2
        UInt8 currentElementIndex{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetGsParamGsListTypeValue);
    };

    // THMSGV1T1RetGsStatus
    struct RetGsStatus
    {
        // CODEGEN EnumRange Command::GENERAL_SETTING_RET_STATUS
        Command command{Command::GENERAL_SETTING_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsInquiredType type{GsInquiredType::GENERAL_SETTING1}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetGsStatus);
    };

    // THMSGV1T1RetNcAsmCapability_NcAsmCapabilityBase
    struct RetNcAsmCapability_NcAsmCapabilityBase
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NO_USE}; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(RetNcAsmCapability_NcAsmCapabilityBase);
    };

    // THMSGV1T1RetNcAsmParam
    struct RetNcAsmParamAsmParam
    {
        // CODEGEN EnumRange Command::NCASM_RET_PARAM
        Command command{Command::NCASM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::AMBIENT_SOUND_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF}; // 0x2
        AsmSettingType asmType{AsmSettingType::ON_OFF}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId asmId{AsmId::NORMAL}; // 0x4
        UInt8 asmValue{}; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetNcAsmParamAsmParam);
    };

    // THMSGV1T1RetNcAsmParam
    struct RetNcAsmParamCapability_NcAsmCapabilityBase
    {
        // CODEGEN EnumRange Command::NCASM_RET_PARAM
        Command command{Command::NCASM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetNcAsmParamCapability_NcAsmCapabilityBase);
    };

    // THMSGV1T1RetNcAsmParam
    struct RetNcAsmParamCapability_NcCapability
    {
        // CODEGEN EnumRange Command::NCASM_RET_PARAM
        Command command{Command::NCASM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NOISE_CANCELLING}; // 0x1
        NcSettingType settingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetNcAsmParamCapability_NcCapability);
    };

    // THMSGV1T1RetNcAsmParam
    struct RetNcAsmParamNcAsmParam
    {
        // CODEGEN EnumRange Command::NCASM_RET_PARAM
        Command command{Command::NCASM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF}; // 0x2
        NcAsmSettingType ncType{NcAsmSettingType::ON_OFF}; // 0x3
        UInt8 ncValue{}; // 0x4
        AsmSettingType asmType{AsmSettingType::ON_OFF}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId asmId{AsmId::NORMAL}; // 0x6
        UInt8 asmValue{}; // 0x7

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetNcAsmParamNcAsmParam);
    };

    // THMSGV1T1RetNcAsmParam
    struct RetNcAsmParamNcParam
    {
        // CODEGEN EnumRange Command::NCASM_RET_PARAM
        Command command{Command::NCASM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NOISE_CANCELLING}; // 0x1
        NcSettingType ncSettingType{NcSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcSettingValue ncSettingValue{NcSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetNcAsmParamNcParam);
    };

    // THMSGV1T1RetNcAsmStatus
    struct RetNcAsmStatus
    {
        // CODEGEN EnumRange Command::NCASM_RET_STATUS
        Command command{Command::NCASM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetNcAsmStatus);
    };

    // THMSGV1T1RetOptimizerCapability
    struct RetOptimizerCapability
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OptimizerInquiredType type{OptimizerInquiredType::NO_USE}; // 0x1
        UInt8 optimizationTime{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PersonalMeasureType personalType{}; // 0x3
        UInt8 personalTime{}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        BarometricMeasureType barometricType{}; // 0x5
        UInt8 barometricTime{}; // 0x6

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetOptimizerCapability);
    };

    // THMSGV1T1RetOptimizerParam
    struct RetOptimizerParam
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OptimizerInquiredType type{OptimizerInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PersonalMeasureType personalType{PersonalMeasureType::NOT_SUPPORT}; // 0x2
        UInt8 personalValue{}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        BarometricMeasureType barometricType{BarometricMeasureType::NOT_SUPPORT}; // 0x4
        UInt8 barometricValue{}; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetOptimizerParam);
    };

    // THMSGV1T1RetOptimizerStatus
    struct RetOptimizerStatus
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OptimizerInquiredType type{OptimizerInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2
        OptimizerStatus optimizerStatus{OptimizerStatus::IDLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetOptimizerStatus);
    };

    // THMSGV1T1RetPlayCapability
    struct RetPlayCapabilityPlaybackControllerVolumeData
    {
        // CODEGEN EnumRange Command::PLAY_RET_CAPABILITY
        Command command{Command::PLAY_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlayInquiredType inquiredType{PlayInquiredType::PLAYBACK_CONTROLLER}; // 0x1
        UInt8 volumeStep{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackControlType playbackControlType{}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackDetailedDataType dataType{PlaybackDetailedDataType::VOLUME}; // 0x4
        UInt8 volumeValue{}; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetPlayCapabilityPlaybackControllerVolumeData);
    };

    // THMSGV1T1RetPlayParam
    struct RetPlayParamPlaybackControllerVolumeData
    {
        // CODEGEN EnumRange Command::PLAY_RET_PARAM
        Command command{Command::PLAY_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlayInquiredType playInquiredType{PlayInquiredType::PLAYBACK_CONTROLLER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackDetailedDataType dataType{PlaybackDetailedDataType::VOLUME}; // 0x2
        UInt8 volumeValue{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetPlayParamPlaybackControllerVolumeData);
    };

    // THMSGV1T1RetPlayStatus
    struct RetPlayStatus
    {
        // CODEGEN EnumRange Command::PLAY_RET_STATUS
        Command command{Command::PLAY_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlayInquiredType playInquiredType{PlayInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackStatus playbackStatus{PlaybackStatus::UNSETTLED}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetPlayStatus);
    };

    // THMSGV1T1RetProtocolInfo
    struct RetProtocolInfo
    {
        // CODEGEN EnumRange Command::CONNECT_RET_PROTOCOL_INFO
        Command command{Command::CONNECT_RET_PROTOCOL_INFO}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonCapabilityInquiredType type{}; // 0x1
        Int16BE protocolVersion{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetProtocolInfo);
    };

    // THMSGV1T1RetSenseCapability
    struct RetSenseCapability
    {
        // CODEGEN EnumRange Command::SENSE_RET_CAPABILITY
        Command command{Command::SENSE_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SenseInquiredType type{}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SenseTableType tableType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSenseCapability);
    };

    // THMSGV1T1RetSportsExParam
    struct RetSportsExParamChildPayloadGetSportsExParam_TrainingModeRequest
    {
        // CODEGEN EnumRange Command::SPORTS_RET_PARAM
        Command command{Command::SPORTS_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::TRAINING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        TrainingModeExParameterType trainingModeExParameterType{TrainingModeExParameterType::NO_USE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSportsExParamChildPayloadGetSportsExParam_TrainingModeRequest);
    };

    // THMSGV1T1RetSportsExParam
    struct RetSportsExParamChildPayloadTrainingModeExAsmParam
    {
        // CODEGEN EnumRange Command::SPORTS_RET_PARAM
        Command command{Command::SPORTS_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::TRAINING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF}; // 0x2
        AsmSettingType asmType{AsmSettingType::ON_OFF}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId asmId{AsmId::NORMAL}; // 0x4
        UInt8 asmValue{}; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSportsExParamChildPayloadTrainingModeExAsmParam);
    };

    // THMSGV1T1RetSportsExParam
    struct RetSportsExParamChildPayloadTrainingModeExEqParam
    {
        // CODEGEN EnumRange Command::SPORTS_RET_PARAM
        Command command{Command::SPORTS_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::TRAINING_MODE}; // 0x1
        EqPresetId presetId{EqPresetId::OFF}; // 0x2
        MDRPodArray<UInt8> bandSteps; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSportsExParamChildPayloadTrainingModeExEqParam);
    };

    // THMSGV1T1RetSportsExParam
    struct RetSportsExParamChildPayloadTrainingModeExNcAsmParam
    {
        // CODEGEN EnumRange Command::SPORTS_RET_PARAM
        Command command{Command::SPORTS_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::TRAINING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF}; // 0x2
        NcAsmSettingType ncType{NcAsmSettingType::ON_OFF}; // 0x3
        UInt8 ncValue{}; // 0x4
        AsmSettingType asmType{AsmSettingType::ON_OFF}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId asmId{AsmId::NORMAL}; // 0x6
        UInt8 asmValue{}; // 0x7

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSportsExParamChildPayloadTrainingModeExNcAsmParam);
    };

    // THMSGV1T1RetSportsParam
    struct RetSportsParamTrainingModeParam
    {
        // CODEGEN EnumRange Command::SPORTS_RET_PARAM
        Command command{Command::SPORTS_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::TRAINING_MODE}; // 0x1
        CommonOnOffSettingType settingType{CommonOnOffSettingType::ON_OFF}; // 0x2
        CommonOnOffSettingValue settingValue{CommonOnOffSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSportsParamTrainingModeParam);
    };

    // THMSGV1T1RetSupportFunction
    struct RetSupportFunction
    {
        // CODEGEN EnumRange Command::CONNECT_RET_SUPPORT_FUNCTION
        Command command{Command::CONNECT_RET_SUPPORT_FUNCTION}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonCapabilityInquiredType type{}; // 0x1
        MDRPodArray<FunctionType> supportFunctions; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSupportFunction);
    };

    // THMSGV1T1RetSystemCapability_SystemCapabilityBase
    struct RetSystemCapability_SystemCapabilityBase
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::NO_USE}; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(RetSystemCapability_SystemCapabilityBase);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamChildPayloadAssignableSettingsParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        MDRPodArray<AssignableSettingsPreset> presets; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemExParamChildPayloadAssignableSettingsParam);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamChildPayloadAutoPowerOffParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AutoPowerOffParameterType autoPowerOffParameterType{AutoPowerOffParameterType::ACTIVE_AND_SELECTIME_ID}; // 0x2
        AutoPowerOffElementId activeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x3
        AutoPowerOffElementId selectTimeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamChildPayloadAutoPowerOffParam);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamChildPayloadCapability_AutoPowerOffCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        MDRPodArray<AutoPowerOffElementId> candidateElements; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemExParamChildPayloadCapability_AutoPowerOffCapability);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamChildPayloadCapability_ControlByWearingCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType controlByWearingSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamChildPayloadCapability_ControlByWearingCapability);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamChildPayloadCapability_PowerSavingModeCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType powerSavingModeSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamChildPayloadCapability_PowerSavingModeCapability);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamChildPayloadCapability_SmartTalkingModeCapabilityVariant1
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType smartTalkingModeSettingType{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType smartTalkingModePreviewType{}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType smartTalkingModeDetailSettingType{}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType smartTalkingModeDetectionSensitivityType{}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType smartTalkingModeVoceFocusType{}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType smartTalkingModeModeOutTimeType{}; // 0x7
        MDRPodArray<UInt8> smartTalkingModeModeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemExParamChildPayloadCapability_SmartTalkingModeCapabilityVariant1);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamChildPayloadCapability_SmartTalkingModeCapabilityVariant2
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType previewType{SmartTalkingModePreviewType::NOT_SUPPORT}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType detailSettingType{SmartTalkingModeDetailSettingType::TYPE_1}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType detectionSensitivityType{SmartTalkingModeDetectionSensitivityType::AUTO_HIGH_LOW}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType voiceFocusType{SmartTalkingModeVoiceFocusType::ON_OFF}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType modeOutTimeType{SmartTalkingModeModeOutTimeType::TYPE_1}; // 0x7
        MDRPodArray<UInt8> modeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemExParamChildPayloadCapability_SmartTalkingModeCapabilityVariant2);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamChildPayloadCapability_SystemCapabilityBase
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamChildPayloadCapability_SystemCapabilityBase);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamChildPayloadCapability_VibratorCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType vibratorSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamChildPayloadCapability_VibratorCapability);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamChildPayloadControlByWearingParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType settingType{ControlByWearingSettingType::ON_OFF}; // 0x2
        ControlByWearingSettingValue settingValue{ControlByWearingSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamChildPayloadControlByWearingParam);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamChildPayloadPowerSavingModeParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType settingType{PowerSavingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerSavingModeSettingValue settingValue{PowerSavingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamChildPayloadPowerSavingModeParam);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamChildPayloadSmartTalkingModeExType1Param
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        DetectionSensitivity devectionSensitivity{DetectionSensitivity::AUTO}; // 0x2
        CommonOnOffSettingValue voiceFocus{CommonOnOffSettingValue::OFF}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ModeOutTime modeOutTime{ModeOutTime::FAST}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamChildPayloadSmartTalkingModeExType1Param);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamChildPayloadSmartTalkingModeRetParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamChildPayloadSmartTalkingModeRetParam);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamChildPayloadSmartTalkingModeSetNtfyParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        SmartTalkingModeParameterType parameterType{SmartTalkingModeParameterType::NO_USE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamChildPayloadSmartTalkingModeSetNtfyParam);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamChildPayloadVibratorParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType settingType{VibratorSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VibratorSettingValue settingValue{VibratorSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamChildPayloadVibratorParam);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamSmartTalkingModeExParamAssignableSettingsParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        MDRPodArray<AssignableSettingsPreset> presets; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemExParamSmartTalkingModeExParamAssignableSettingsParam);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamSmartTalkingModeExParamAutoPowerOffParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AutoPowerOffParameterType autoPowerOffParameterType{AutoPowerOffParameterType::ACTIVE_AND_SELECTIME_ID}; // 0x2
        AutoPowerOffElementId activeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x3
        AutoPowerOffElementId selectTimeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamSmartTalkingModeExParamAutoPowerOffParam);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamSmartTalkingModeExParamCapability_AutoPowerOffCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        MDRPodArray<AutoPowerOffElementId> candidateElements; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemExParamSmartTalkingModeExParamCapability_AutoPowerOffCapability);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamSmartTalkingModeExParamCapability_ControlByWearingCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType controlByWearingSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamSmartTalkingModeExParamCapability_ControlByWearingCapability);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamSmartTalkingModeExParamCapability_PowerSavingModeCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType powerSavingModeSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamSmartTalkingModeExParamCapability_PowerSavingModeCapability);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamSmartTalkingModeExParamCapability_SystemCapabilityBase
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamSmartTalkingModeExParamCapability_SystemCapabilityBase);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamSmartTalkingModeExParamCapability_VibratorCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType vibratorSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamSmartTalkingModeExParamCapability_VibratorCapability);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamSmartTalkingModeExParamControlByWearingParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType settingType{ControlByWearingSettingType::ON_OFF}; // 0x2
        ControlByWearingSettingValue settingValue{ControlByWearingSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamSmartTalkingModeExParamControlByWearingParam);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamSmartTalkingModeExParamPowerSavingModeParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType settingType{PowerSavingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerSavingModeSettingValue settingValue{PowerSavingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamSmartTalkingModeExParamPowerSavingModeParam);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamSmartTalkingModeExParamSmartTalkingModeExParamType1Param
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType detailSettingType{SmartTalkingModeDetailSettingType::TYPE_1}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        DetectionSensitivity devectionSensitivity{DetectionSensitivity::AUTO}; // 0x3
        CommonOnOffSettingValue voiceFocus{CommonOnOffSettingValue::OFF}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ModeOutTime modeOutTime{ModeOutTime::FAST}; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamSmartTalkingModeExParamSmartTalkingModeExParamType1Param);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamSmartTalkingModeExParamVibratorParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType settingType{VibratorSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VibratorSettingValue settingValue{VibratorSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemExParamSmartTalkingModeExParamVibratorParam);
    };

    // THMSGV1T1RetSystemParam
    struct RetSystemParamAssignableSettingsParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        MDRPodArray<AssignableSettingsPreset> presets; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemParamAssignableSettingsParam);
    };

    // THMSGV1T1RetSystemParam
    struct RetSystemParamAutoPowerOffParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AutoPowerOffParameterType autoPowerOffParameterType{AutoPowerOffParameterType::ACTIVE_AND_SELECTIME_ID}; // 0x2
        AutoPowerOffElementId activeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x3
        AutoPowerOffElementId selectTimeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemParamAutoPowerOffParam);
    };

    // THMSGV1T1RetSystemParam
    struct RetSystemParamCapability_AutoPowerOffCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        MDRPodArray<AutoPowerOffElementId> candidateElements; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemParamCapability_AutoPowerOffCapability);
    };

    // THMSGV1T1RetSystemParam
    struct RetSystemParamCapability_ControlByWearingCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType controlByWearingSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemParamCapability_ControlByWearingCapability);
    };

    // THMSGV1T1RetSystemParam
    struct RetSystemParamCapability_PowerSavingModeCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType powerSavingModeSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemParamCapability_PowerSavingModeCapability);
    };

    // THMSGV1T1RetSystemParam
    struct RetSystemParamCapability_SmartTalkingModeCapabilityVariant1
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType smartTalkingModeSettingType{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType smartTalkingModePreviewType{}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType smartTalkingModeDetailSettingType{}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType smartTalkingModeDetectionSensitivityType{}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType smartTalkingModeVoceFocusType{}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType smartTalkingModeModeOutTimeType{}; // 0x7
        MDRPodArray<UInt8> smartTalkingModeModeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemParamCapability_SmartTalkingModeCapabilityVariant1);
    };

    // THMSGV1T1RetSystemParam
    struct RetSystemParamCapability_SmartTalkingModeCapabilityVariant2
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType previewType{SmartTalkingModePreviewType::NOT_SUPPORT}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType detailSettingType{SmartTalkingModeDetailSettingType::TYPE_1}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType detectionSensitivityType{SmartTalkingModeDetectionSensitivityType::AUTO_HIGH_LOW}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType voiceFocusType{SmartTalkingModeVoiceFocusType::ON_OFF}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType modeOutTimeType{SmartTalkingModeModeOutTimeType::TYPE_1}; // 0x7
        MDRPodArray<UInt8> modeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemParamCapability_SmartTalkingModeCapabilityVariant2);
    };

    // THMSGV1T1RetSystemParam
    struct RetSystemParamCapability_SystemCapabilityBase
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemParamCapability_SystemCapabilityBase);
    };

    // THMSGV1T1RetSystemParam
    struct RetSystemParamCapability_VibratorCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType vibratorSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemParamCapability_VibratorCapability);
    };

    // THMSGV1T1RetSystemParam
    struct RetSystemParamControlByWearingParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType settingType{ControlByWearingSettingType::ON_OFF}; // 0x2
        ControlByWearingSettingValue settingValue{ControlByWearingSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemParamControlByWearingParam);
    };

    // THMSGV1T1RetSystemParam
    struct RetSystemParamPowerSavingModeParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType settingType{PowerSavingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerSavingModeSettingValue settingValue{PowerSavingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemParamPowerSavingModeParam);
    };

    // THMSGV1T1RetSystemParam
    struct RetSystemParamSmartTalkingModeRetParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemParamSmartTalkingModeRetParam);
    };

    // THMSGV1T1RetSystemParam
    struct RetSystemParamSmartTalkingModeSetNtfyParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        SmartTalkingModeParameterType parameterType{SmartTalkingModeParameterType::NO_USE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemParamSmartTalkingModeSetNtfyParam);
    };

    // THMSGV1T1RetSystemParam
    struct RetSystemParamVibratorAlertNotificationParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AlertInquiredType alertInquiredType{AlertInquiredType::VIBRATOR_ALERT_NOTIFICATION}; // 0x2
        VibrationType vibrationType{VibrationType::NO_PATTERN_SPECIFIED}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemParamVibratorAlertNotificationParam);
    };

    // THMSGV1T1RetSystemParam
    struct RetSystemParamVibratorParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType settingType{VibratorSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VibratorSettingValue settingValue{VibratorSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemParamVibratorParam);
    };

    // THMSGV1T1RetSystemStatus
    struct RetSystemStatusAssignableSettingsParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        MDRPodArray<AssignableSettingsPreset> presets; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemStatusAssignableSettingsParam);
    };

    // THMSGV1T1RetSystemStatus
    struct RetSystemStatusAssignableSettingsStatus
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        MDRPodArray<CommonStatus> allStatus; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE};

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemStatusAssignableSettingsStatus);
    };

    // THMSGV1T1RetSystemStatus
    struct RetSystemStatusAutoPowerOffParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AutoPowerOffParameterType autoPowerOffParameterType{AutoPowerOffParameterType::ACTIVE_AND_SELECTIME_ID}; // 0x2
        AutoPowerOffElementId activeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x3
        AutoPowerOffElementId selectTimeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemStatusAutoPowerOffParam);
    };

    // THMSGV1T1RetSystemStatus
    struct RetSystemStatusCapability_AutoPowerOffCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        MDRPodArray<AutoPowerOffElementId> candidateElements; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemStatusCapability_AutoPowerOffCapability);
    };

    // THMSGV1T1RetSystemStatus
    struct RetSystemStatusCapability_ControlByWearingCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType controlByWearingSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemStatusCapability_ControlByWearingCapability);
    };

    // THMSGV1T1RetSystemStatus
    struct RetSystemStatusCapability_PowerSavingModeCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType powerSavingModeSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemStatusCapability_PowerSavingModeCapability);
    };

    // THMSGV1T1RetSystemStatus
    struct RetSystemStatusCapability_SmartTalkingModeCapabilityVariant1
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType smartTalkingModeSettingType{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType smartTalkingModePreviewType{}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType smartTalkingModeDetailSettingType{}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType smartTalkingModeDetectionSensitivityType{}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType smartTalkingModeVoceFocusType{}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType smartTalkingModeModeOutTimeType{}; // 0x7
        MDRPodArray<UInt8> smartTalkingModeModeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemStatusCapability_SmartTalkingModeCapabilityVariant1);
    };

    // THMSGV1T1RetSystemStatus
    struct RetSystemStatusCapability_SmartTalkingModeCapabilityVariant2
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType previewType{SmartTalkingModePreviewType::NOT_SUPPORT}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType detailSettingType{SmartTalkingModeDetailSettingType::TYPE_1}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType detectionSensitivityType{SmartTalkingModeDetectionSensitivityType::AUTO_HIGH_LOW}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType voiceFocusType{SmartTalkingModeVoiceFocusType::ON_OFF}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType modeOutTimeType{SmartTalkingModeModeOutTimeType::TYPE_1}; // 0x7
        MDRPodArray<UInt8> modeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemStatusCapability_SmartTalkingModeCapabilityVariant2);
    };

    // THMSGV1T1RetSystemStatus
    struct RetSystemStatusCapability_SystemCapabilityBase
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemStatusCapability_SystemCapabilityBase);
    };

    // THMSGV1T1RetSystemStatus
    struct RetSystemStatusCapability_VibratorCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType vibratorSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemStatusCapability_VibratorCapability);
    };

    // THMSGV1T1RetSystemStatus
    struct RetSystemStatusControlByWearingParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType settingType{ControlByWearingSettingType::ON_OFF}; // 0x2
        ControlByWearingSettingValue settingValue{ControlByWearingSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemStatusControlByWearingParam);
    };

    // THMSGV1T1RetSystemStatus
    struct RetSystemStatusPowerSavingModeParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType settingType{PowerSavingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerSavingModeSettingValue settingValue{PowerSavingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemStatusPowerSavingModeParam);
    };

    // THMSGV1T1RetSystemStatus
    struct RetSystemStatusSmartTalkingModeRetParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemStatusSmartTalkingModeRetParam);
    };

    // THMSGV1T1RetSystemStatus
    struct RetSystemStatusSmartTalkingModeSetNtfyParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        SmartTalkingModeParameterType parameterType{SmartTalkingModeParameterType::NO_USE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemStatusSmartTalkingModeSetNtfyParam);
    };

    // THMSGV1T1RetSystemStatus
    struct RetSystemStatusSmartTalkingModeStatus
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeEffectStatus effectStatus{SmartTalkingModeEffectStatus::NOT_ACTIVE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemStatusSmartTalkingModeStatus);
    };

    // THMSGV1T1RetSystemStatus
    struct RetSystemStatusVibratorParam
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType settingType{VibratorSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VibratorSettingValue settingValue{VibratorSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemStatusVibratorParam);
    };

    // THMSGV1T1RetUpdateParam
    struct RetUpdateParamUpdateBatteryPowerThresholdForInterruptiongUpdtParam
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType type{UpdateInquiredType::BATTERY_POWER_THRESHOLD_FOR_INTERRUPTIONG_FW_UPDATE}; // 0x1
        UInt8 threshold{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetUpdateParamUpdateBatteryPowerThresholdForInterruptiongUpdtParam);
    };

    // THMSGV1T1RetUpdateParam
    struct RetUpdateParamUpdateBatteryPowerThresholdParam
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType type{UpdateInquiredType::BATTERY_POWER_THRESHOLD}; // 0x1
        UInt8 threshold{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetUpdateParamUpdateBatteryPowerThresholdParam);
    };

    // THMSGV1T1RetUpdateParam
    struct RetUpdateParamUpdateBleTxPowerParam
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType type{UpdateInquiredType::BLE_TX_POWER}; // 0x1
        UInt8 bleTxPower{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetUpdateParamUpdateBleTxPowerParam);
    };

    // THMSGV1T1RetUpdateParam
    struct RetUpdateParamUpdateMethodParam
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType type{UpdateInquiredType::UPDATE_METHOD}; // 0x1
        UInt8 value{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetUpdateParamUpdateMethodParam);
    };

    // THMSGV1T1RetUpdateParam
    struct RetUpdateParamUpdateStringParamNSString
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType type{UpdateInquiredType::NO_USE}; // 0x1
        MDRPrefixedString string; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetUpdateParamUpdateStringParamNSString);
    };

    // THMSGV1T1RetUpdateParam
    struct RetUpdateParamUpdateStringParamUpdateInquiredType
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType type{UpdateInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetUpdateParamUpdateStringParamUpdateInquiredType);
    };

    // THMSGV1T1RetUpscalingEffect
    struct RetUpscalingEffect
    {
        // CODEGEN EnumRange Command::COMMON_RET_UPSCALING_EFFECT
        Command command{Command::COMMON_RET_UPSCALING_EFFECT}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonCapabilityInquiredType commonCapabilityInquiredType{CommonCapabilityInquiredType::FIXED_VALUE}; // 0x1
        UpscalingEffectType effectType{UpscalingEffectType::DSEE_HX}; // 0x2
        UpscalingEffectStatus effectStatus{UpscalingEffectStatus::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetUpscalingEffect);
    };

    // THMSGV1T1RetVptCapability_VptCapabilityBase
    struct RetVptCapability_VptCapabilityBase
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::VPT}; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(RetVptCapability_VptCapabilityBase);
    };

    // THMSGV1T1RetVptParam
    struct RetVptParamCapability_VptCapabilityBase
    {
        // CODEGEN EnumRange Command::VPT_RET_PARAM
        Command command{Command::VPT_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::VPT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetVptParamCapability_VptCapabilityBase);
    };

    // THMSGV1T1RetVptParam
    struct RetVptParamSoundPositionParam
    {
        // CODEGEN EnumRange Command::VPT_RET_PARAM
        Command command{Command::VPT_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::SOUND_POSITION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SoundPositionPresetId presetId{SoundPositionPresetId::OFF}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetVptParamSoundPositionParam);
    };

    // THMSGV1T1RetVptParam
    struct RetVptParamVptParam
    {
        // CODEGEN EnumRange Command::VPT_RET_PARAM
        Command command{Command::VPT_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::VPT}; // 0x1
        VptPresetId presetId{VptPresetId::OFF}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetVptParamVptParam);
    };

    // THMSGV1T1RetVptStatus
    struct RetVptStatus
    {
        // CODEGEN EnumRange Command::VPT_RET_STATUS
        Command command{Command::VPT_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::VPT}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetVptStatus);
    };

    // THMSGV1T1SetAlertParam
    struct SetAlertParamFixedMessageParam
    {
        // CODEGEN EnumRange Command::ALERT_SET_PARAM
        Command command{Command::ALERT_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AlertInquiredType type{AlertInquiredType::FIXED_MESSAGE}; // 0x1
        AlertMessageType messageType{AlertMessageType::NO_USE}; // 0x2
        AlertAction action{AlertAction::NEGATIVE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetAlertParamFixedMessageParam);
    };

    // THMSGV1T1SetAlertParam
    struct SetAlertParamVibratorAlertNotificationParam
    {
        // CODEGEN EnumRange Command::ALERT_SET_PARAM
        Command command{Command::ALERT_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AlertInquiredType type{AlertInquiredType::VIBRATOR_ALERT_NOTIFICATION}; // 0x1
        VibrationType vibrationType{VibrationType::NO_PATTERN_SPECIFIED}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetAlertParamVibratorAlertNotificationParam);
    };

    // THMSGV1T1SetAlertStatus
    struct SetAlertStatus
    {
        // CODEGEN EnumRange Command::ALERT_SET_STATUS
        Command command{Command::ALERT_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AlertInquiredType type{AlertInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetAlertStatus);
    };

    // THMSGV1T1SetAudioParam
    struct SetAudioParamConnectionModeParam
    {
        // CODEGEN EnumRange Command::AUDIO_SET_PARAM
        Command command{Command::AUDIO_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType type{AudioInquiredType::CONNECTION_MODE}; // 0x1
        ConnectionModeSettingType settingType{ConnectionModeSettingType::SOUND_CONNECTION}; // 0x2
        ConnectionModeSettingValue settingValue{ConnectionModeSettingValue::SOUND_QUALITY_PRIOR}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetAudioParamConnectionModeParam);
    };

    // THMSGV1T1SetAudioParam
    struct SetAudioParamRetAudioCapability_ConnectionModeCapability
    {
        // CODEGEN EnumRange Command::AUDIO_SET_PARAM
        Command command{Command::AUDIO_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType type{AudioInquiredType::CONNECTION_MODE}; // 0x1
        ConnectionModeSettingType settingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetAudioParamRetAudioCapability_ConnectionModeCapability);
    };

    // THMSGV1T1SetAudioParam
    struct SetAudioParamUpscalingParam
    {
        // CODEGEN EnumRange Command::AUDIO_SET_PARAM
        Command command{Command::AUDIO_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType type{AudioInquiredType::UPSCALING}; // 0x1
        UpscalingSettingType settingType{UpscalingSettingType::AUTO_OFF}; // 0x2
        UpscalingSettingValue settingValue{UpscalingSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetAudioParamUpscalingParam);
    };

    // THMSGV1T1SetEqEbbParam
    struct SetEqEbbParamEbbParam
    {
        // CODEGEN EnumRange Command::EQEBB_SET_PARAM
        Command command{Command::EQEBB_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::EBB}; // 0x1
        UInt8 level{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetEqEbbParamEbbParam);
    };

    // THMSGV1T1SetEqEbbParam
    struct SetEqEbbParamEqParam
    {
        // CODEGEN EnumRange Command::EQEBB_SET_PARAM
        Command command{Command::EQEBB_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x1
        EqPresetId presetId{EqPresetId::OFF}; // 0x2
        MDRPodArray<UInt8> bandSteps; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(SetEqEbbParamEqParam);
    };

    // THMSGV1T1SetEqEbbParam
    struct SetEqEbbParamRetEqEbbCapability_EbbCapability
    {
        // CODEGEN EnumRange Command::EQEBB_SET_PARAM
        Command command{Command::EQEBB_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::EBB}; // 0x1
        UInt8 minValue{}; // 0x2
        UInt8 maxValue{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetEqEbbParamRetEqEbbCapability_EbbCapability);
    };

    // THMSGV1T1SetEqEbbParam
    struct SetEqEbbParamRetEqEbbCapability_EqEbbCapabilityBase
    {
        // CODEGEN EnumRange Command::EQEBB_SET_PARAM
        Command command{Command::EQEBB_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetEqEbbParamRetEqEbbCapability_EqEbbCapabilityBase);
    };

    // THMSGV1T1SetLinkControl
    struct SetLinkControlKeepAliveLinkControlNotifiedParam
    {
        // CODEGEN EnumRange Command::COMMON_SET_LINK_CONTROL
        Command command{Command::COMMON_SET_LINK_CONTROL}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LinkControlInquiredType type{LinkControlInquiredType::KEEP_ALIVE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetLinkControlKeepAliveLinkControlNotifiedParam);
    };

    // THMSGV1T1SetLinkControl
    struct SetLinkControlKeepAliveLinkControlSettingParam
    {
        // CODEGEN EnumRange Command::COMMON_SET_LINK_CONTROL
        Command command{Command::COMMON_SET_LINK_CONTROL}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LinkControlInquiredType type{LinkControlInquiredType::KEEP_ALIVE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x2
        UInt8 timeMin{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetLinkControlKeepAliveLinkControlSettingParam);
    };

    // THMSGV1T1SetLogStatus
    struct SetLogStatus
    {
        // CODEGEN EnumRange Command::LOG_SET_STATUS
        Command command{Command::LOG_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        LogInquiredType type{LogInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetLogStatus);
    };

    // THMSGV1T1SetNcAsmParam
    struct SetNcAsmParamAsmParam
    {
        // CODEGEN EnumRange Command::NCASM_SET_PARAM
        Command command{Command::NCASM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::AMBIENT_SOUND_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF}; // 0x2
        AsmSettingType asmType{AsmSettingType::ON_OFF}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId asmId{AsmId::NORMAL}; // 0x4
        UInt8 asmValue{}; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetNcAsmParamAsmParam);
    };

    // THMSGV1T1SetNcAsmParam
    struct SetNcAsmParamNcAsmParam
    {
        // CODEGEN EnumRange Command::NCASM_SET_PARAM
        Command command{Command::NCASM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF}; // 0x2
        NcAsmSettingType ncType{NcAsmSettingType::ON_OFF}; // 0x3
        UInt8 ncValue{}; // 0x4
        AsmSettingType asmType{AsmSettingType::ON_OFF}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId asmId{AsmId::NORMAL}; // 0x6
        UInt8 asmValue{}; // 0x7

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetNcAsmParamNcAsmParam);
    };

    // THMSGV1T1SetNcAsmParam
    struct SetNcAsmParamNcParam
    {
        // CODEGEN EnumRange Command::NCASM_SET_PARAM
        Command command{Command::NCASM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NOISE_CANCELLING}; // 0x1
        NcSettingType ncSettingType{NcSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcSettingValue ncSettingValue{NcSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetNcAsmParamNcParam);
    };

    // THMSGV1T1SetNcAsmParam
    struct SetNcAsmParamRetNcAsmCapability_NcAsmCapabilityBase
    {
        // CODEGEN EnumRange Command::NCASM_SET_PARAM
        Command command{Command::NCASM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetNcAsmParamRetNcAsmCapability_NcAsmCapabilityBase);
    };

    // THMSGV1T1SetNcAsmParam
    struct SetNcAsmParamRetNcAsmCapability_NcCapability
    {
        // CODEGEN EnumRange Command::NCASM_SET_PARAM
        Command command{Command::NCASM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NOISE_CANCELLING}; // 0x1
        NcSettingType settingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetNcAsmParamRetNcAsmCapability_NcCapability);
    };

    // THMSGV1T1SetOptimizerStatus
    struct SetOptimizerStatus
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OptimizerInquiredType optimizerInquiredType{OptimizerInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        OptimizerControl optimizerControl{OptimizerControl::CANCEL}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetOptimizerStatus);
    };

    // THMSGV1T1SetPlayParam
    struct SetPlayParamPlaybackControllerVolumeData
    {
        // CODEGEN EnumRange Command::PLAY_SET_PARAM
        Command command{Command::PLAY_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlayInquiredType type{PlayInquiredType::PLAYBACK_CONTROLLER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackDetailedDataType dataType{PlaybackDetailedDataType::VOLUME}; // 0x2
        UInt8 volumeValue{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetPlayParamPlaybackControllerVolumeData);
    };

    // THMSGV1T1SetPlayStatus
    struct SetPlayStatus
    {
        // CODEGEN EnumRange Command::PLAY_SET_STATUS
        Command command{Command::PLAY_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlayInquiredType type{PlayInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackControl control{PlaybackControl::KEY_OFF}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetPlayStatus);
    };

    // THMSGV1T1SetPowerOff
    struct SetPowerOff
    {
        // CODEGEN EnumRange Command::COMMON_SET_POWER_OFF
        Command command{Command::COMMON_SET_POWER_OFF}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerOffInquiredType type{PowerOffInquiredType::FIXED_VALUE}; // 0x1
        PowerOffSettingValue settingValue{PowerOffSettingValue::NO_USE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetPowerOff);
    };

    // THMSGV1T1SetSenseStatus
    struct SetSenseStatus
    {
        // CODEGEN EnumRange Command::SENSE_SET_STATUS
        Command command{Command::SENSE_SET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SenseInquiredType type{SenseInquiredType::NO_USE}; // 0x1
        SenseSettingControl senseSettingControl{SenseSettingControl::NO_USE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSenseStatus);
    };

    // THMSGV1T1SetSystemExParam
    struct SetSystemExParamAssignableSettingsParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        MDRPodArray<AssignableSettingsPreset> presets; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SetSystemExParamAssignableSettingsParam);
    };

    // THMSGV1T1SetSystemExParam
    struct SetSystemExParamAutoPowerOffParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AutoPowerOffParameterType autoPowerOffParameterType{AutoPowerOffParameterType::ACTIVE_AND_SELECTIME_ID}; // 0x2
        AutoPowerOffElementId activeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x3
        AutoPowerOffElementId selectTimeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemExParamAutoPowerOffParam);
    };

    // THMSGV1T1SetSystemExParam
    struct SetSystemExParamControlByWearingParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType settingType{ControlByWearingSettingType::ON_OFF}; // 0x2
        ControlByWearingSettingValue settingValue{ControlByWearingSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemExParamControlByWearingParam);
    };

    // THMSGV1T1SetSystemExParam
    struct SetSystemExParamPowerSavingModeParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType settingType{PowerSavingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerSavingModeSettingValue settingValue{PowerSavingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemExParamPowerSavingModeParam);
    };

    // THMSGV1T1SetSystemExParam
    struct SetSystemExParamRetSystemCapability_AutoPowerOffCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        MDRPodArray<AutoPowerOffElementId> candidateElements; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SetSystemExParamRetSystemCapability_AutoPowerOffCapability);
    };

    // THMSGV1T1SetSystemExParam
    struct SetSystemExParamRetSystemCapability_ControlByWearingCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType controlByWearingSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemExParamRetSystemCapability_ControlByWearingCapability);
    };

    // THMSGV1T1SetSystemExParam
    struct SetSystemExParamRetSystemCapability_PowerSavingModeCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType powerSavingModeSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemExParamRetSystemCapability_PowerSavingModeCapability);
    };

    // THMSGV1T1SetSystemExParam
    struct SetSystemExParamRetSystemCapability_SmartTalkingModeCapabilityVariant1
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType smartTalkingModeSettingType{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType smartTalkingModePreviewType{}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType smartTalkingModeDetailSettingType{}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType smartTalkingModeDetectionSensitivityType{}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType smartTalkingModeVoceFocusType{}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType smartTalkingModeModeOutTimeType{}; // 0x7
        MDRPodArray<UInt8> smartTalkingModeModeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(SetSystemExParamRetSystemCapability_SmartTalkingModeCapabilityVariant1);
    };

    // THMSGV1T1SetSystemExParam
    struct SetSystemExParamRetSystemCapability_SmartTalkingModeCapabilityVariant2
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType previewType{SmartTalkingModePreviewType::NOT_SUPPORT}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType detailSettingType{SmartTalkingModeDetailSettingType::TYPE_1}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType detectionSensitivityType{SmartTalkingModeDetectionSensitivityType::AUTO_HIGH_LOW}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType voiceFocusType{SmartTalkingModeVoiceFocusType::ON_OFF}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType modeOutTimeType{SmartTalkingModeModeOutTimeType::TYPE_1}; // 0x7
        MDRPodArray<UInt8> modeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(SetSystemExParamRetSystemCapability_SmartTalkingModeCapabilityVariant2);
    };

    // THMSGV1T1SetSystemExParam
    struct SetSystemExParamRetSystemCapability_SystemCapabilityBase
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemExParamRetSystemCapability_SystemCapabilityBase);
    };

    // THMSGV1T1SetSystemExParam
    struct SetSystemExParamRetSystemCapability_VibratorCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType vibratorSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemExParamRetSystemCapability_VibratorCapability);
    };

    // THMSGV1T1SetSystemExParam
    struct SetSystemExParamVibratorParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType settingType{VibratorSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VibratorSettingValue settingValue{VibratorSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemExParamVibratorParam);
    };

    // THMSGV1T1SetSystemExParam
    struct SetSystemExParammartTalkingModeExParamType1Param
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType detailSettingType{SmartTalkingModeDetailSettingType::TYPE_1}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        DetectionSensitivity devectionSensitivity{DetectionSensitivity::AUTO}; // 0x3
        CommonOnOffSettingValue voiceFocus{CommonOnOffSettingValue::OFF}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ModeOutTime modeOutTime{ModeOutTime::FAST}; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemExParammartTalkingModeExParamType1Param);
    };

    // THMSGV1T1SetSystemExParam
    struct SetSystemExParammartTalkingModeExType1Param
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        DetectionSensitivity devectionSensitivity{DetectionSensitivity::AUTO}; // 0x2
        CommonOnOffSettingValue voiceFocus{CommonOnOffSettingValue::OFF}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ModeOutTime modeOutTime{ModeOutTime::FAST}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemExParammartTalkingModeExType1Param);
    };

    // THMSGV1T1SetSystemExParam
    struct SetSystemExParammartTalkingModeRetParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemExParammartTalkingModeRetParam);
    };

    // THMSGV1T1SetSystemExParam
    struct SetSystemExParammartTalkingModeSetNtfyParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        SmartTalkingModeParameterType parameterType{SmartTalkingModeParameterType::NO_USE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemExParammartTalkingModeSetNtfyParam);
    };

    // THMSGV1T1SetSystemParam
    struct SetSystemParamAssignableSettingsParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        MDRPodArray<AssignableSettingsPreset> presets; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SetSystemParamAssignableSettingsParam);
    };

    // THMSGV1T1SetSystemParam
    struct SetSystemParamAutoPowerOffParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AutoPowerOffParameterType autoPowerOffParameterType{AutoPowerOffParameterType::ACTIVE_AND_SELECTIME_ID}; // 0x2
        AutoPowerOffElementId activeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x3
        AutoPowerOffElementId selectTimeElementId{AutoPowerOffElementId::POWER_OFF_IN_5_MIN}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemParamAutoPowerOffParam);
    };

    // THMSGV1T1SetSystemParam
    struct SetSystemParamControlByWearingParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType settingType{ControlByWearingSettingType::ON_OFF}; // 0x2
        ControlByWearingSettingValue settingValue{ControlByWearingSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemParamControlByWearingParam);
    };

    // THMSGV1T1SetSystemParam
    struct SetSystemParamPowerSavingModeParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType settingType{PowerSavingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PowerSavingModeSettingValue settingValue{PowerSavingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemParamPowerSavingModeParam);
    };

    // THMSGV1T1SetSystemParam
    struct SetSystemParamRetSystemCapability_AutoPowerOffCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::AUTO_POWER_OFF}; // 0x1
        MDRPodArray<AutoPowerOffElementId> candidateElements; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SetSystemParamRetSystemCapability_AutoPowerOffCapability);
    };

    // THMSGV1T1SetSystemParam
    struct SetSystemParamRetSystemCapability_ControlByWearingCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::CONTROL_BY_WEARING}; // 0x1
        ControlByWearingSettingType controlByWearingSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemParamRetSystemCapability_ControlByWearingCapability);
    };

    // THMSGV1T1SetSystemParam
    struct SetSystemParamRetSystemCapability_PowerSavingModeCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::POWER_SAVING_MODE}; // 0x1
        PowerSavingModeSettingType powerSavingModeSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemParamRetSystemCapability_PowerSavingModeCapability);
    };

    // THMSGV1T1SetSystemParam
    struct SetSystemParamRetSystemCapability_SmartTalkingModeCapabilityVariant1
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType smartTalkingModeSettingType{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType smartTalkingModePreviewType{}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType smartTalkingModeDetailSettingType{}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType smartTalkingModeDetectionSensitivityType{}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType smartTalkingModeVoceFocusType{}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType smartTalkingModeModeOutTimeType{}; // 0x7
        MDRPodArray<UInt8> smartTalkingModeModeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(SetSystemParamRetSystemCapability_SmartTalkingModeCapabilityVariant1);
    };

    // THMSGV1T1SetSystemParam
    struct SetSystemParamRetSystemCapability_SmartTalkingModeCapabilityVariant2
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType previewType{SmartTalkingModePreviewType::NOT_SUPPORT}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType detailSettingType{SmartTalkingModeDetailSettingType::TYPE_1}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType detectionSensitivityType{SmartTalkingModeDetectionSensitivityType::AUTO_HIGH_LOW}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType voiceFocusType{SmartTalkingModeVoiceFocusType::ON_OFF}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType modeOutTimeType{SmartTalkingModeModeOutTimeType::TYPE_1}; // 0x7
        MDRPodArray<UInt8> modeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(SetSystemParamRetSystemCapability_SmartTalkingModeCapabilityVariant2);
    };

    // THMSGV1T1SetSystemParam
    struct SetSystemParamRetSystemCapability_VibratorCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType vibratorSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemParamRetSystemCapability_VibratorCapability);
    };

    // THMSGV1T1SetSystemParam
    struct SetSystemParamVibratorParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x1
        VibratorSettingType settingType{VibratorSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VibratorSettingValue settingValue{VibratorSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemParamVibratorParam);
    };

    // THMSGV1T1SetSystemParam
    struct SetSystemParammartTalkingModeRetParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemParammartTalkingModeRetParam);
    };

    // THMSGV1T1SetSystemParam
    struct SetSystemParammartTalkingModeSetNtfyParam
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x1
        SmartTalkingModeParameterType parameterType{SmartTalkingModeParameterType::NO_USE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetSystemParammartTalkingModeSetNtfyParam);
    };

    // THMSGV1T1SetUpdateStatus
    struct SetUpdateStatus
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType updateInquiredType{UpdateInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus commonStatus{CommonStatus::ENABLE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetUpdateStatus);
    };

    // THMSGV1T1SetVptParam
    struct SetVptParamRetVptCapability_VptCapabilityBase
    {
        // CODEGEN EnumRange Command::VPT_SET_PARAM
        Command command{Command::VPT_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::VPT}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetVptParamRetVptCapability_VptCapabilityBase);
    };

    // THMSGV1T1SetVptParam
    struct SetVptParamVptParam
    {
        // CODEGEN EnumRange Command::VPT_SET_PARAM
        Command command{Command::VPT_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::VPT}; // 0x1
        VptPresetId presetId{VptPresetId::OFF}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetVptParamVptParam);
    };

    // THMSGV1T1SetVptParam
    struct SetVptParamVptPreset
    {
        // CODEGEN EnumRange Command::VPT_SET_PARAM
        Command command{Command::VPT_SET_PARAM}; // 0x0
        VptPresetId presetId{VptPresetId::OFF}; // 0x1
        MDRPrefixedString name; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SetVptParamVptPreset);
    };

    // THMSGV1T1SetVptParam
    struct SetVptParamoundPositionParam
    {
        // CODEGEN EnumRange Command::VPT_SET_PARAM
        Command command{Command::VPT_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::SOUND_POSITION}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SoundPositionPresetId presetId{SoundPositionPresetId::OFF}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetVptParamoundPositionParam);
    };

    // THMSGV1T1SmartTalkingModeExParam
    struct SmartTalkingModeExParamType1Param
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType detailSettingType{SmartTalkingModeDetailSettingType::TYPE_1}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        DetectionSensitivity devectionSensitivity{DetectionSensitivity::AUTO}; // 0x1
        CommonOnOffSettingValue voiceFocus{CommonOnOffSettingValue::OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ModeOutTime modeOutTime{ModeOutTime::FAST}; // 0x3

        MDR_DEFINE_EXTERN_READ_WRITE(SmartTalkingModeExParamType1Param);
    };

    // THMSGV1T1SmartTalkingModeExType1Param
    struct SmartTalkingModeExType1Param
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        DetectionSensitivity devectionSensitivity{DetectionSensitivity::AUTO}; // 0x0
        CommonOnOffSettingValue voiceFocus{CommonOnOffSettingValue::OFF}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        ModeOutTime modeOutTime{ModeOutTime::FAST}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(SmartTalkingModeExType1Param);
    };

    // THMSGV1T1SmartTalkingModeRetParam
    struct SmartTalkingModeRetParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(SmartTalkingModeRetParam);
    };

    // THMSGV1T1SmartTalkingModeSetNtfyParam
    struct SmartTalkingModeSetNtfyParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::SMART_TALKING_MODE}; // 0x0
        SmartTalkingModeParameterType parameterType{SmartTalkingModeParameterType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingValue settingValue{SmartTalkingModeSettingValue::OFF}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(SmartTalkingModeSetNtfyParam);
    };

    // THMSGV1T1SmartTalkingModeStatus
    struct SmartTalkingModeStatus
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeEffectStatus effectStatus{SmartTalkingModeEffectStatus::NOT_ACTIVE}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(SmartTalkingModeStatus);
    };

    // THMSGV1T1SoundPositionParam
    struct SoundPositionParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::SOUND_POSITION}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SoundPositionPresetId presetId{SoundPositionPresetId::OFF}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(SoundPositionParam);
    };

    // THMSGV1T1SpecificInformationType
    struct SpecificInformationType
    {
        UInt8 code{}; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(SpecificInformationType);
    };

    // THMSGV1T1SystemStatus
    struct SystemStatus
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(SystemStatus);
    };

    // THMSGV1T1TrainingModeCapability
    struct TrainingModeCapability
    {
        CommonOnOffSettingType settingType{CommonOnOffSettingType::ON_OFF}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        TrainingModeAvailableEffectType availableEffectType{TrainingModeAvailableEffectType::NO_USE}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(TrainingModeCapability);
    };

    // THMSGV1T1TrainingModeExAsmParam
    struct TrainingModeExAsmParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF}; // 0x0
        AsmSettingType asmType{AsmSettingType::ON_OFF}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId asmId{AsmId::NORMAL}; // 0x2
        UInt8 asmValue{}; // 0x3

        MDR_DEFINE_EXTERN_READ_WRITE(TrainingModeExAsmParam);
    };

    // THMSGV1T1TrainingModeExEqParam
    struct TrainingModeExEqParam
    {
        EqPresetId presetId{EqPresetId::OFF}; // 0x0
        MDRPodArray<UInt8> bandSteps; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(TrainingModeExEqParam);
    };

    // THMSGV1T1TrainingModeExNcAsmParam
    struct TrainingModeExNcAsmParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF}; // 0x0
        NcAsmSettingType ncType{NcAsmSettingType::ON_OFF}; // 0x1
        UInt8 ncValue{}; // 0x2
        AsmSettingType asmType{AsmSettingType::ON_OFF}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId asmId{AsmId::NORMAL}; // 0x4
        UInt8 asmValue{}; // 0x5

        MDR_DEFINE_EXTERN_READ_WRITE(TrainingModeExNcAsmParam);
    };

    // THMSGV1T1TrainingModeParam
    struct TrainingModeParam
    {
        CommonOnOffSettingType settingType{CommonOnOffSettingType::ON_OFF}; // 0x0
        CommonOnOffSettingValue settingValue{CommonOnOffSettingValue::OFF}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(TrainingModeParam);
    };

    // THMSGV1T1TrainingModeStatus
    struct TrainingModeStatus
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(TrainingModeStatus);
    };

    // THMSGV1T1UpdateBatteryPowerThresholdForInterruptiongUpdtParam
    struct UpdateBatteryPowerThresholdForInterruptiongUpdtParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType type{UpdateInquiredType::BATTERY_POWER_THRESHOLD_FOR_INTERRUPTIONG_FW_UPDATE}; // 0x0
        UInt8 threshold{}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(UpdateBatteryPowerThresholdForInterruptiongUpdtParam);
    };

    // THMSGV1T1UpdateBatteryPowerThresholdParam
    struct UpdateBatteryPowerThresholdParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType type{UpdateInquiredType::BATTERY_POWER_THRESHOLD}; // 0x0
        UInt8 threshold{}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(UpdateBatteryPowerThresholdParam);
    };

    // THMSGV1T1UpdateBleTxPowerParam
    struct UpdateBleTxPowerParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType type{UpdateInquiredType::BLE_TX_POWER}; // 0x0
        UInt8 bleTxPower{}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(UpdateBleTxPowerParam);
    };

    // THMSGV1T1UpdateMethodParam
    struct UpdateMethodParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType type{UpdateInquiredType::UPDATE_METHOD}; // 0x0
        UInt8 value{}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(UpdateMethodParam);
    };

    // THMSGV1T1UpdateStringParam
    struct UpdateStringParamNSString
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType type{UpdateInquiredType::NO_USE}; // 0x0
        MDRPrefixedString string; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(UpdateStringParamNSString);
    };

    // THMSGV1T1UpdateStringParam
    struct UpdateStringParamUpdateInquiredType
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        UpdateInquiredType type{UpdateInquiredType::NO_USE}; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(UpdateStringParamUpdateInquiredType);
    };

    // THMSGV1T1UpscalingParam
    struct UpscalingParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType type{AudioInquiredType::UPSCALING}; // 0x0
        UpscalingSettingType settingType{UpscalingSettingType::AUTO_OFF}; // 0x1
        UpscalingSettingValue settingValue{UpscalingSettingValue::OFF}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(UpscalingParam);
    };

    // THMSGV1T1VibratorAlertNotificationParam
    struct VibratorAlertNotificationParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AlertInquiredType type{AlertInquiredType::VIBRATOR_ALERT_NOTIFICATION}; // 0x0
        VibrationType vibrationType{VibrationType::NO_PATTERN_SPECIFIED}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(VibratorAlertNotificationParam);
    };

    // THMSGV1T1VibratorParam
    struct VibratorParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::VIBRATOR}; // 0x0
        VibratorSettingType settingType{VibratorSettingType::ON_OFF}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VibratorSettingValue settingValue{VibratorSettingValue::OFF}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(VibratorParam);
    };

    // THMSGV1T1VptParam
    struct VptParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::VPT}; // 0x0
        VptPresetId presetId{VptPresetId::OFF}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(VptParam);
    };

    // THMSGV1T1VptPreset
    struct VptPreset
    {
        VptPresetId presetId{VptPresetId::OFF}; // 0x0
        MDRPrefixedString name; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(VptPreset);
    };

    // THMSGV1T1AsCapabilityPreset
    struct AsCapabilityPreset
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AssignableSettingsPreset preset{AssignableSettingsPreset::AMBIENT_SOUND_CONTROL}; // 0x0
        MDRPodArray<AsCapabilityAction> actions; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(AsCapabilityPreset);
    };

    // THMSGV1T1EqBandInformation
    struct EqBandInformation
    {
        EqBandInformationType infoType{EqBandInformationType::NO_INFORMATION}; // 0x0
        UInt8 valueAsFrequency{}; // 0x1
        SpecificInformationType valueAsSpecificInformation{}; // 0x2

        MDR_DEFINE_EXTERN_READ_WRITE(EqBandInformation);
    };

    // THMSGV1T1GetSportsExParam
    struct GetSportsExParam
    {
        // CODEGEN EnumRange Command::SPORTS_GET_PARAM
        Command command{Command::SPORTS_GET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::NO_USE}; // 0x1
        GetSportsExParam_TrainingModeRequest getSportsExParam_TrainingModeRequest{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(GetSportsExParam);
    };

    // THMSGV1T1GsCandidateElementList
    struct GsCandidateElementList
    {
        MDRArray<GsSettingInfo> elements; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(GsCandidateElementList);
    };

    // THMSGV1T1NotifyEqEbbParam
    struct NotifyEqEbbParamRetEqEbbCapability_EqCapability
    {
        // CODEGEN EnumRange Command::EQEBB_NTFY_PARAM
        Command command{Command::EQEBB_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x1
        UInt8 band{}; // 0x2
        UInt8 step{}; // 0x3
        MDRArray<EqPreset> presetList; // 0x4

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifyEqEbbParamRetEqEbbCapability_EqCapability);
    };

    // THMSGV1T1NotifyNcAsmParam
    struct NotifyNcAsmParamRetNcAsmCapability_AsmCapability
    {
        // CODEGEN EnumRange Command::NCASM_NTFY_PARAM
        Command command{Command::NCASM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::AMBIENT_SOUND_MODE}; // 0x1
        AsmSettingType asmSettingType{}; // 0x2
        MDRPodArray<Asm> asmList; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifyNcAsmParamRetNcAsmCapability_AsmCapability);
    };

    // THMSGV1T1NotifyNcAsmParam
    struct NotifyNcAsmParamRetNcAsmCapability_NcAsmCapability
    {
        // CODEGEN EnumRange Command::NCASM_NTFY_PARAM
        Command command{Command::NCASM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE}; // 0x1
        NcAsmSettingType ncSettingType{}; // 0x2
        UInt8 ncStep{}; // 0x3
        AsmSettingType asmSettingType{}; // 0x4
        MDRPodArray<Asm> asmList; // 0x5

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifyNcAsmParamRetNcAsmCapability_NcAsmCapability);
    };

    // THMSGV1T1NotifyPlayParam
    struct NotifyPlayParamPlaybackControllerNameData
    {
        // CODEGEN EnumRange Command::PLAY_NTFY_PARAM
        Command command{Command::PLAY_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlayInquiredType playInquiredType{PlayInquiredType::PLAYBACK_CONTROLLER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackDetailedDataType dataType{PlaybackDetailedDataType::TRACK_NAME}; // 0x2
        PlaybackName playbackName{}; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifyPlayParamPlaybackControllerNameData);
    };

    // THMSGV1T1NotifySportsExParam
    struct NotifySportsExParamTrainingModeExParam
    {
        // CODEGEN EnumRange Command::SPORTS_NTFY_PARAM
        Command command{Command::SPORTS_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::TRAINING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        TrainingModeExParameterType parameterType{TrainingModeExParameterType::NO_USE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF}; // 0x3
        NcAsmSettingType ncAsmSettingType{NcAsmSettingType::ON_OFF}; // 0x4
        UInt8 value4{}; // 0x5
        AsmSettingType asmSettingType{AsmSettingType::ON_OFF}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId asmId{AsmId::NORMAL}; // 0x7
        UInt8 value7{}; // 0x8
        TrainingModeExNcAsmParam ncAsmParam{}; // 0x9
        TrainingModeExAsmParam asmParam{};
        TrainingModeExEqParam eqParam{};

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySportsExParamTrainingModeExParam);
    };

    // THMSGV1T1NotifySportsStatus
    struct NotifySportsStatusTrainingModeStatus
    {
        // CODEGEN EnumRange Command::SPORTS_NTFY_STATUS
        Command command{Command::SPORTS_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::TRAINING_MODE}; // 0x1
        TrainingModeStatus trainingStatus{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySportsStatusTrainingModeStatus);
    };

    // THMSGV1T1NotifyVptParam
    struct NotifyVptParamRetVptCapability_VptCapability
    {
        // CODEGEN EnumRange Command::VPT_NTFY_PARAM
        Command command{Command::VPT_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::VPT}; // 0x1
        MDRArray<VptPreset> vptPresets; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifyVptParamRetVptCapability_VptCapability);
    };

    // THMSGV1T1PlaybackControllerNameData
    struct PlaybackControllerNameData
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackDetailedDataType dataType{PlaybackDetailedDataType::TRACK_NAME}; // 0x0
        PlaybackName playbackName{}; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(PlaybackControllerNameData);
    };

    // THMSGV1T1RetAudioCapability
    struct RetAudioCapability
    {
        // CODEGEN EnumRange Command::AUDIO_RET_CAPABILITY
        Command command{Command::AUDIO_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AudioInquiredType type{AudioInquiredType::NO_USE}; // 0x1
        RetAudioCapability_AudioCapabilityBase capability{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetAudioCapability);
    };

    // THMSGV1T1RetAudioCapability_ConnectionModeCapability
    struct RetAudioCapability_ConnectionModeCapability
    {
        // CODEGEN EnumRange Command::AUDIO_RET_CAPABILITY
        Command command{Command::AUDIO_RET_CAPABILITY}; // 0x0
        // CODEGEN Field type EnumRange AudioInquiredType::CONNECTION_MODE
        RetAudioCapability_AudioCapabilityBase base{AudioInquiredType::CONNECTION_MODE};
        ConnectionModeSettingType settingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetAudioCapability_ConnectionModeCapability);
    };

    // THMSGV1T1RetAudioCapability_UpscalingCapability
    struct RetAudioCapability_UpscalingCapability
    {
        // CODEGEN EnumRange Command::AUDIO_RET_CAPABILITY
        Command command{Command::AUDIO_RET_CAPABILITY}; // 0x0
        // CODEGEN Field type EnumRange AudioInquiredType::UPSCALING
        RetAudioCapability_AudioCapabilityBase base{AudioInquiredType::UPSCALING};
        UpscalingType upscalingType{}; // 0x2
        UpscalingSettingType settingType{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetAudioCapability_UpscalingCapability);
    };

    // THMSGV1T1RetDeviceInfo
    struct RetDeviceInfo
    {
        // CODEGEN EnumRange Command::CONNECT_RET_DEVICE_INFO
        Command command{Command::CONNECT_RET_DEVICE_INFO}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        DeviceInfoInquiredType type{DeviceInfoInquiredType::NO_USE}; // 0x1
        RetDeviceInfo_DeviceInfoBase deviceInfo{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetDeviceInfo);
    };

    // THMSGV1T1RetDeviceInfo_DeviceInfoFwVersion
    struct RetDeviceInfo_DeviceInfoFwVersion
    {
        // CODEGEN EnumRange Command::CONNECT_RET_DEVICE_INFO
        Command command{Command::CONNECT_RET_DEVICE_INFO}; // 0x0
        // CODEGEN Field type EnumRange DeviceInfoInquiredType::FW_VERSION
        RetDeviceInfo_DeviceInfoBase base{DeviceInfoInquiredType::FW_VERSION};
        MDRPrefixedString fwVersion; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetDeviceInfo_DeviceInfoFwVersion);
    };

    // THMSGV1T1RetDeviceInfo_DeviceInfoInstructionGuide
    struct RetDeviceInfo_DeviceInfoInstructionGuide
    {
        // CODEGEN EnumRange Command::CONNECT_RET_DEVICE_INFO
        Command command{Command::CONNECT_RET_DEVICE_INFO}; // 0x0
        // CODEGEN Field type EnumRange DeviceInfoInquiredType::INSTRUCTION_GUIDE
        RetDeviceInfo_DeviceInfoBase base{DeviceInfoInquiredType::INSTRUCTION_GUIDE};
        MDRPodArray<GuidanceCategory> guidanceCategories; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetDeviceInfo_DeviceInfoInstructionGuide);
    };

    // THMSGV1T1RetDeviceInfo_DeviceInfoModelName
    struct RetDeviceInfo_DeviceInfoModelName
    {
        // CODEGEN EnumRange Command::CONNECT_RET_DEVICE_INFO
        Command command{Command::CONNECT_RET_DEVICE_INFO}; // 0x0
        // CODEGEN Field type EnumRange DeviceInfoInquiredType::MODEL_NAME
        RetDeviceInfo_DeviceInfoBase base{DeviceInfoInquiredType::MODEL_NAME};
        MDRPrefixedString modelName; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetDeviceInfo_DeviceInfoModelName);
    };

    // THMSGV1T1RetDeviceInfo_DeviceInfoSeriesAndColor
    struct RetDeviceInfo_DeviceInfoSeriesAndColor
    {
        // CODEGEN EnumRange Command::CONNECT_RET_DEVICE_INFO
        Command command{Command::CONNECT_RET_DEVICE_INFO}; // 0x0
        // CODEGEN Field type EnumRange DeviceInfoInquiredType::SERIES_AND_COLOR_INFO
        RetDeviceInfo_DeviceInfoBase base{DeviceInfoInquiredType::SERIES_AND_COLOR_INFO};
        ModelSeries series{}; // 0x2
        ModelColor color{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetDeviceInfo_DeviceInfoSeriesAndColor);
    };

    // THMSGV1T1RetEqEbbCapability
    struct RetEqEbbCapability
    {
        // CODEGEN EnumRange Command::EQEBB_RET_CAPABILITY
        Command command{Command::EQEBB_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x1
        RetEqEbbCapability_EqEbbCapabilityBase eqEbbCapability{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetEqEbbCapability);
    };

    // THMSGV1T1RetEqEbbCapability_EbbCapability
    struct RetEqEbbCapability_EbbCapability
    {
        // CODEGEN EnumRange Command::EQEBB_RET_CAPABILITY
        Command command{Command::EQEBB_RET_CAPABILITY}; // 0x0
        // CODEGEN Field type EnumRange EqEbbInquiredType::EBB
        RetEqEbbCapability_EqEbbCapabilityBase base{EqEbbInquiredType::EBB};
        UInt8 minValue{}; // 0x2
        UInt8 maxValue{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetEqEbbCapability_EbbCapability);
    };

    // THMSGV1T1RetEqEbbCapability_EqCapability
    struct RetEqEbbCapability_EqCapability
    {
        // CODEGEN EnumRange Command::EQEBB_RET_CAPABILITY
        Command command{Command::EQEBB_RET_CAPABILITY}; // 0x0
        // CODEGEN Field type EnumRange EqEbbInquiredType::NO_USE
        RetEqEbbCapability_EqEbbCapabilityBase base{EqEbbInquiredType::NO_USE};
        UInt8 band{}; // 0x2
        UInt8 step{}; // 0x3
        MDRArray<EqPreset> presetList; // 0x4

        MDR_DEFINE_EXTERN_SERIALIZATION(RetEqEbbCapability_EqCapability);
    };

    // THMSGV1T1RetEqEbbParam
    struct RetEqEbbParamCapability_EqCapability
    {
        // CODEGEN EnumRange Command::EQEBB_RET_PARAM
        Command command{Command::EQEBB_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x1
        UInt8 band{}; // 0x2
        UInt8 step{}; // 0x3
        MDRArray<EqPreset> presetList; // 0x4

        MDR_DEFINE_EXTERN_SERIALIZATION(RetEqEbbParamCapability_EqCapability);
    };

    // THMSGV1T1RetGsCapability
    struct RetGsCapabilityGsSettingInfo
    {
        // CODEGEN EnumRange Command::GENERAL_SETTING_RET_CAPABILITY
        Command command{Command::GENERAL_SETTING_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsInquiredType type{GsInquiredType::GENERAL_SETTING1}; // 0x1
        GsSettingInfo title{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsSettingType settingType{GsSettingType::BOOLEAN_TYPE};

        MDR_DEFINE_EXTERN_SERIALIZATION(RetGsCapabilityGsSettingInfo);
    };

    // THMSGV1T1RetNcAsmCapability
    struct RetNcAsmCapability
    {
        // CODEGEN EnumRange Command::NCASM_RET_CAPABILITY
        Command command{Command::NCASM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NO_USE}; // 0x1
        RetNcAsmCapability_NcAsmCapabilityBase ncAsmCapability{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetNcAsmCapability);
    };

    // THMSGV1T1RetNcAsmCapability_AsmCapability
    struct RetNcAsmCapability_AsmCapability
    {
        // CODEGEN EnumRange Command::NCASM_RET_CAPABILITY
        Command command{Command::NCASM_RET_CAPABILITY}; // 0x0
        // CODEGEN Field type EnumRange NcAsmInquiredType::AMBIENT_SOUND_MODE
        RetNcAsmCapability_NcAsmCapabilityBase base{NcAsmInquiredType::AMBIENT_SOUND_MODE};
        AsmSettingType asmSettingType{}; // 0x2
        MDRPodArray<Asm> asmList; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(RetNcAsmCapability_AsmCapability);
    };

    // THMSGV1T1RetNcAsmCapability_NcAsmCapability
    struct RetNcAsmCapability_NcAsmCapability
    {
        // CODEGEN EnumRange Command::NCASM_RET_CAPABILITY
        Command command{Command::NCASM_RET_CAPABILITY}; // 0x0
        // CODEGEN Field type EnumRange NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE
        RetNcAsmCapability_NcAsmCapabilityBase base{NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE};
        NcAsmSettingType ncSettingType{}; // 0x2
        UInt8 ncStep{}; // 0x3
        AsmSettingType asmSettingType{}; // 0x4
        MDRPodArray<Asm> asmList; // 0x5

        MDR_DEFINE_EXTERN_SERIALIZATION(RetNcAsmCapability_NcAsmCapability);
    };

    // THMSGV1T1RetNcAsmCapability_NcCapability
    struct RetNcAsmCapability_NcCapability
    {
        // CODEGEN EnumRange Command::NCASM_RET_CAPABILITY
        Command command{Command::NCASM_RET_CAPABILITY}; // 0x0
        // CODEGEN Field type EnumRange NcAsmInquiredType::NOISE_CANCELLING
        RetNcAsmCapability_NcAsmCapabilityBase base{NcAsmInquiredType::NOISE_CANCELLING};
        NcSettingType settingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetNcAsmCapability_NcCapability);
    };

    // THMSGV1T1RetNcAsmParam
    struct RetNcAsmParamCapability_AsmCapability
    {
        // CODEGEN EnumRange Command::NCASM_RET_PARAM
        Command command{Command::NCASM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::AMBIENT_SOUND_MODE}; // 0x1
        AsmSettingType asmSettingType{}; // 0x2
        MDRPodArray<Asm> asmList; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(RetNcAsmParamCapability_AsmCapability);
    };

    // THMSGV1T1RetNcAsmParam
    struct RetNcAsmParamCapability_NcAsmCapability
    {
        // CODEGEN EnumRange Command::NCASM_RET_PARAM
        Command command{Command::NCASM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE}; // 0x1
        NcAsmSettingType ncSettingType{}; // 0x2
        UInt8 ncStep{}; // 0x3
        AsmSettingType asmSettingType{}; // 0x4
        MDRPodArray<Asm> asmList; // 0x5

        MDR_DEFINE_EXTERN_SERIALIZATION(RetNcAsmParamCapability_NcAsmCapability);
    };

    // THMSGV1T1RetPlayCapability
    struct RetPlayCapabilityPlaybackControllerNameData
    {
        // CODEGEN EnumRange Command::PLAY_RET_CAPABILITY
        Command command{Command::PLAY_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlayInquiredType inquiredType{PlayInquiredType::PLAYBACK_CONTROLLER}; // 0x1
        UInt8 volumeStep{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackControlType playbackControlType{}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackDetailedDataType dataType{PlaybackDetailedDataType::TRACK_NAME}; // 0x4
        PlaybackName playbackName{}; // 0x5

        MDR_DEFINE_EXTERN_SERIALIZATION(RetPlayCapabilityPlaybackControllerNameData);
    };

    // THMSGV1T1RetPlayParam
    struct RetPlayParamPlaybackControllerNameData
    {
        // CODEGEN EnumRange Command::PLAY_RET_PARAM
        Command command{Command::PLAY_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlayInquiredType playInquiredType{PlayInquiredType::PLAYBACK_CONTROLLER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackDetailedDataType dataType{PlaybackDetailedDataType::TRACK_NAME}; // 0x2
        PlaybackName playbackName{}; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(RetPlayParamPlaybackControllerNameData);
    };

    // THMSGV1T1RetSportsCapability
    struct RetSportsCapability
    {
        // CODEGEN EnumRange Command::SPORTS_RET_CAPABILITY
        Command command{Command::SPORTS_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::NO_USE}; // 0x1
        TrainingModeCapability trainingCapability{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSportsCapability);
    };

    // THMSGV1T1RetSportsExParam
    struct RetSportsExParamTrainingModeExParam
    {
        // CODEGEN EnumRange Command::SPORTS_RET_PARAM
        Command command{Command::SPORTS_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::TRAINING_MODE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        TrainingModeExParameterType parameterType{TrainingModeExParameterType::NO_USE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF}; // 0x3
        NcAsmSettingType ncAsmSettingType{NcAsmSettingType::ON_OFF}; // 0x4
        UInt8 value4{}; // 0x5
        AsmSettingType asmSettingType{AsmSettingType::ON_OFF}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId asmId{AsmId::NORMAL}; // 0x7
        UInt8 value7{}; // 0x8
        TrainingModeExNcAsmParam ncAsmParam{}; // 0x9
        TrainingModeExAsmParam asmParam{};
        TrainingModeExEqParam eqParam{};

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSportsExParamTrainingModeExParam);
    };

    // THMSGV1T1RetSportsStatus
    struct RetSportsStatusTrainingModeStatus
    {
        // CODEGEN EnumRange Command::SPORTS_RET_STATUS
        Command command{Command::SPORTS_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::TRAINING_MODE}; // 0x1
        TrainingModeStatus trainingStatus{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSportsStatusTrainingModeStatus);
    };

    // THMSGV1T1RetSystemCapability
    struct RetSystemCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::NO_USE}; // 0x1
        RetSystemCapability_SystemCapabilityBase capability{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemCapability);
    };

    // THMSGV1T1RetSystemCapability_AutoPowerOffCapability
    struct RetSystemCapability_AutoPowerOffCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Field type EnumRange SystemInquiredType::AUTO_POWER_OFF
        RetSystemCapability_SystemCapabilityBase base{SystemInquiredType::AUTO_POWER_OFF};
        MDRPodArray<AutoPowerOffElementId> candidateElements; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemCapability_AutoPowerOffCapability);
    };

    // THMSGV1T1RetSystemCapability_ControlByWearingCapability
    struct RetSystemCapability_ControlByWearingCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Field type EnumRange SystemInquiredType::CONTROL_BY_WEARING
        RetSystemCapability_SystemCapabilityBase base{SystemInquiredType::CONTROL_BY_WEARING};
        ControlByWearingSettingType controlByWearingSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemCapability_ControlByWearingCapability);
    };

    // THMSGV1T1RetSystemCapability_PowerSavingModeCapability
    struct RetSystemCapability_PowerSavingModeCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Field type EnumRange SystemInquiredType::POWER_SAVING_MODE
        RetSystemCapability_SystemCapabilityBase base{SystemInquiredType::POWER_SAVING_MODE};
        PowerSavingModeSettingType powerSavingModeSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemCapability_PowerSavingModeCapability);
    };

    // THMSGV1T1RetSystemCapability_SmartTalkingModeCapability
    struct RetSystemCapability_SmartTalkingModeCapabilityVariant1
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Field type EnumRange SystemInquiredType::SMART_TALKING_MODE
        RetSystemCapability_SystemCapabilityBase base{SystemInquiredType::SMART_TALKING_MODE};
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType smartTalkingModeSettingType{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType smartTalkingModePreviewType{}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType smartTalkingModeDetailSettingType{}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType smartTalkingModeDetectionSensitivityType{}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType smartTalkingModeVoceFocusType{}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType smartTalkingModeModeOutTimeType{}; // 0x7
        MDRPodArray<UInt8> smartTalkingModeModeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemCapability_SmartTalkingModeCapabilityVariant1);
    };

    // THMSGV1T1RetSystemCapability_SmartTalkingModeCapability
    struct RetSystemCapability_SmartTalkingModeCapabilityVariant2
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Field type EnumRange SystemInquiredType::SMART_TALKING_MODE
        RetSystemCapability_SystemCapabilityBase base{SystemInquiredType::SMART_TALKING_MODE};
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeSettingType settingType{SmartTalkingModeSettingType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModePreviewType previewType{SmartTalkingModePreviewType::NOT_SUPPORT}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetailSettingType detailSettingType{SmartTalkingModeDetailSettingType::TYPE_1}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeDetectionSensitivityType detectionSensitivityType{SmartTalkingModeDetectionSensitivityType::AUTO_HIGH_LOW}; // 0x5
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeVoiceFocusType voiceFocusType{SmartTalkingModeVoiceFocusType::ON_OFF}; // 0x6
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SmartTalkingModeModeOutTimeType modeOutTimeType{SmartTalkingModeModeOutTimeType::TYPE_1}; // 0x7
        MDRPodArray<UInt8> modeTimeOutValues; // 0x8

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemCapability_SmartTalkingModeCapabilityVariant2);
    };

    // THMSGV1T1RetSystemCapability_VibratorCapability
    struct RetSystemCapability_VibratorCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Field type EnumRange SystemInquiredType::VIBRATOR
        RetSystemCapability_SystemCapabilityBase base{SystemInquiredType::VIBRATOR};
        VibratorSettingType vibratorSettingType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetSystemCapability_VibratorCapability);
    };

    // THMSGV1T1RetVptCapability
    struct RetVptCapability
    {
        // CODEGEN EnumRange Command::VPT_RET_CAPABILITY
        Command command{Command::VPT_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::VPT}; // 0x1
        RetVptCapability_VptCapabilityBase capability{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetVptCapability);
    };

    // THMSGV1T1RetVptCapability_SoundPositionCapability
    struct RetVptCapability_SoundPositionCapability
    {
        // CODEGEN EnumRange Command::VPT_RET_CAPABILITY
        Command command{Command::VPT_RET_CAPABILITY}; // 0x0
        // CODEGEN Field type EnumRange VptInquiredType::SOUND_POSITION
        RetVptCapability_VptCapabilityBase base{VptInquiredType::SOUND_POSITION};
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SoundPositionType soundPositionType{}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetVptCapability_SoundPositionCapability);
    };

    // THMSGV1T1RetVptCapability_VptCapability
    struct RetVptCapability_VptCapability
    {
        // CODEGEN EnumRange Command::VPT_RET_CAPABILITY
        Command command{Command::VPT_RET_CAPABILITY}; // 0x0
        // CODEGEN Field type EnumRange VptInquiredType::VPT
        RetVptCapability_VptCapabilityBase base{VptInquiredType::VPT};
        MDRArray<VptPreset> vptPresets; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetVptCapability_VptCapability);
    };

    // THMSGV1T1RetVptParam
    struct RetVptParamCapability_VptCapability
    {
        // CODEGEN EnumRange Command::VPT_RET_PARAM
        Command command{Command::VPT_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::VPT}; // 0x1
        MDRArray<VptPreset> vptPresets; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetVptParamCapability_VptCapability);
    };

    // THMSGV1T1SetEqEbbParam
    struct SetEqEbbParamRetEqEbbCapability_EqCapability
    {
        // CODEGEN EnumRange Command::EQEBB_SET_PARAM
        Command command{Command::EQEBB_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x1
        UInt8 band{}; // 0x2
        UInt8 step{}; // 0x3
        MDRArray<EqPreset> presetList; // 0x4

        MDR_DEFINE_EXTERN_SERIALIZATION(SetEqEbbParamRetEqEbbCapability_EqCapability);
    };

    // THMSGV1T1SetGsParam
    struct SetGsParamGsBooleanTypeValue
    {
        // CODEGEN EnumRange Command::GENERAL_SETTING_SET_PARAM
        Command command{Command::GENERAL_SETTING_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsInquiredType type{GsInquiredType::GENERAL_SETTING1}; // 0x1
        GsBooleanTypeValue settingValue{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SetGsParamGsBooleanTypeValue);
    };

    // THMSGV1T1SetGsParam
    struct SetGsParamGsListTypeValue
    {
        // CODEGEN EnumRange Command::GENERAL_SETTING_SET_PARAM
        Command command{Command::GENERAL_SETTING_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsInquiredType type{GsInquiredType::GENERAL_SETTING1}; // 0x1
        GsListTypeValue currentElementIndex{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SetGsParamGsListTypeValue);
    };

    // THMSGV1T1SetNcAsmParam
    struct SetNcAsmParamRetNcAsmCapability_AsmCapability
    {
        // CODEGEN EnumRange Command::NCASM_SET_PARAM
        Command command{Command::NCASM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::AMBIENT_SOUND_MODE}; // 0x1
        AsmSettingType asmSettingType{}; // 0x2
        MDRPodArray<Asm> asmList; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(SetNcAsmParamRetNcAsmCapability_AsmCapability);
    };

    // THMSGV1T1SetNcAsmParam
    struct SetNcAsmParamRetNcAsmCapability_NcAsmCapability
    {
        // CODEGEN EnumRange Command::NCASM_SET_PARAM
        Command command{Command::NCASM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmInquiredType type{NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE}; // 0x1
        NcAsmSettingType ncSettingType{}; // 0x2
        UInt8 ncStep{}; // 0x3
        AsmSettingType asmSettingType{}; // 0x4
        MDRPodArray<Asm> asmList; // 0x5

        MDR_DEFINE_EXTERN_SERIALIZATION(SetNcAsmParamRetNcAsmCapability_NcAsmCapability);
    };

    // THMSGV1T1SetPlayParam
    struct SetPlayParamPlaybackControllerNameData
    {
        // CODEGEN EnumRange Command::PLAY_SET_PARAM
        Command command{Command::PLAY_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlayInquiredType type{PlayInquiredType::PLAYBACK_CONTROLLER}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PlaybackDetailedDataType dataType{PlaybackDetailedDataType::TRACK_NAME}; // 0x2
        PlaybackName playbackName{}; // 0x3

        MDR_DEFINE_EXTERN_SERIALIZATION(SetPlayParamPlaybackControllerNameData);
    };

    // THMSGV1T1SetSportsParam
    struct SetSportsParam
    {
        // CODEGEN EnumRange Command::SPORTS_SET_PARAM
        Command command{Command::SPORTS_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::NO_USE}; // 0x1
        TrainingModeParam trainingParam{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SetSportsParam);
    };

    // THMSGV1T1SetVptParam
    struct SetVptParamRetVptCapability_VptCapability
    {
        // CODEGEN EnumRange Command::VPT_SET_PARAM
        Command command{Command::VPT_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VptInquiredType type{VptInquiredType::VPT}; // 0x1
        MDRArray<VptPreset> vptPresets; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SetVptParamRetVptCapability_VptCapability);
    };

    // THMSGV1T1TestCommand
    struct TestCommand
    {
        // CODEGEN EnumRange Command::TEST_COMMAND
        Command command{Command::TEST_COMMAND}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        TestCommandType testCommandType{}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        TargetType sender{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        TargetType receiver{}; // 0x3
        UInt8 majorVersion{}; // 0x4
        UInt8 minorVersion{}; // 0x5
        AtCommandParam atCommandParam{}; // 0x6

        MDR_DEFINE_EXTERN_SERIALIZATION(TestCommand);
    };

    // THMSGV1T1TrainingModeExParam
    struct TrainingModeExParam
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        TrainingModeExParameterType parameterType{TrainingModeExParameterType::NO_USE}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF}; // 0x1
        NcAsmSettingType ncAsmSettingType{NcAsmSettingType::ON_OFF}; // 0x2
        UInt8 value4{}; // 0x3
        AsmSettingType asmSettingType{AsmSettingType::ON_OFF}; // 0x4
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AsmId asmId{AsmId::NORMAL}; // 0x5
        UInt8 value7{}; // 0x6
        TrainingModeExNcAsmParam ncAsmParam{}; // 0x7
        TrainingModeExAsmParam asmParam{};
        TrainingModeExEqParam eqParam{};

        MDR_DEFINE_EXTERN_READ_WRITE(TrainingModeExParam);
    };

    // THMSGV1T1AsCapabilityKey
    struct AsCapabilityKey
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AssignableSettingsKey key{AssignableSettingsKey::LEFT_SIDE_KEY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AssignableSettingsKeyType keyType{AssignableSettingsKeyType::TOUCH_SENSOR}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        AssignableSettingsPreset defaultPreset{AssignableSettingsPreset::AMBIENT_SOUND_CONTROL}; // 0x2
        MDRArray<AsCapabilityPreset> presets; // 0x3

        MDR_DEFINE_EXTERN_READ_WRITE(AsCapabilityKey);
    };

    // THMSGV1T1EqExtendedInfo
    struct EqExtendedInfo
    {
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x0
        MDRArray<EqBandInformation> bandInfos; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(EqExtendedInfo);
    };

    // THMSGV1T1RetEqEbbExtendedInfo
    struct RetEqEbbExtendedInfoEqExtendedInfo
    {
        // CODEGEN EnumRange Command::EQEBB_RET_EXTENDED_INFO
        Command command{Command::EQEBB_RET_EXTENDED_INFO}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        EqEbbInquiredType type{EqEbbInquiredType::NO_USE}; // 0x1
        MDRArray<EqBandInformation> bandInfos; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetEqEbbExtendedInfoEqExtendedInfo);
    };

    // THMSGV1T1RetGsCapability
    struct RetGsCapabilityGsCandidateElementList
    {
        // CODEGEN EnumRange Command::GENERAL_SETTING_RET_CAPABILITY
        Command command{Command::GENERAL_SETTING_RET_CAPABILITY}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsInquiredType type{GsInquiredType::GENERAL_SETTING1}; // 0x1
        GsSettingInfo title{}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        GsSettingType settingType{GsSettingType::LIST_TYPE};
        GsCandidateElementList listTypeCapability{};

        MDR_DEFINE_EXTERN_SERIALIZATION(RetGsCapabilityGsCandidateElementList);
    };

    // THMSGV1T1SetSportsExParam
    struct SetSportsExParam
    {
        // CODEGEN EnumRange Command::SPORTS_SET_PARAM
        Command command{Command::SPORTS_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SportsInquiredType type{SportsInquiredType::NO_USE}; // 0x1
        TrainingModeExParam trainingModeExParam{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SetSportsExParam);
    };

    // THMSGV1T1AsCapabilityKeyList
    struct AsCapabilityKeyList
    {
        MDRArray<AsCapabilityKey> elements; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(AsCapabilityKeyList);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamChildPayloadRetSystemCapability_AssignableSettingsCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        AsCapabilityKeyList assignableSettingKeyList{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemExParamChildPayloadRetSystemCapability_AssignableSettingsCapability);
    };

    // THMSGV1T1NotifySystemExParam
    struct NotifySystemExParamSmartTalkingModeExParamRetSystemCapability_AssignableSettingsCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        AsCapabilityKeyList assignableSettingKeyList{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemExParamSmartTalkingModeExParamRetSystemCapability_AssignableSettingsCapability);
    };

    // THMSGV1T1NotifySystemParam
    struct NotifySystemParamRetSystemCapability_AssignableSettingsCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_PARAM
        Command command{Command::SYSTEM_NTFY_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        AsCapabilityKeyList assignableSettingKeyList{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemParamRetSystemCapability_AssignableSettingsCapability);
    };

    // THMSGV1T1NotifySystemStatus
    struct NotifySystemStatusRetSystemCapability_AssignableSettingsCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_NTFY_STATUS
        Command command{Command::SYSTEM_NTFY_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        AsCapabilityKeyList assignableSettingKeyList{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifySystemStatusRetSystemCapability_AssignableSettingsCapability);
    };

    // THMSGV1T1RetSystemCapability_AssignableSettingsCapability
    struct RetSystemCapability_AssignableSettingsCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_CAPABILITY
        Command command{Command::SYSTEM_RET_CAPABILITY}; // 0x0
        // CODEGEN Field type EnumRange SystemInquiredType::ASSIGNABLE_SETTINGS
        RetSystemCapability_SystemCapabilityBase base{SystemInquiredType::ASSIGNABLE_SETTINGS};
        AsCapabilityKeyList assignableSettingKeyList{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemCapability_AssignableSettingsCapability);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamChildPayloadCapability_AssignableSettingsCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        AsCapabilityKeyList assignableSettingKeyList{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemExParamChildPayloadCapability_AssignableSettingsCapability);
    };

    // THMSGV1T1RetSystemExParam
    struct RetSystemExParamSmartTalkingModeExParamCapability_AssignableSettingsCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        AsCapabilityKeyList assignableSettingKeyList{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemExParamSmartTalkingModeExParamCapability_AssignableSettingsCapability);
    };

    // THMSGV1T1RetSystemParam
    struct RetSystemParamCapability_AssignableSettingsCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_PARAM
        Command command{Command::SYSTEM_RET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        AsCapabilityKeyList assignableSettingKeyList{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemParamCapability_AssignableSettingsCapability);
    };

    // THMSGV1T1RetSystemStatus
    struct RetSystemStatusCapability_AssignableSettingsCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_RET_STATUS
        Command command{Command::SYSTEM_RET_STATUS}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        AsCapabilityKeyList assignableSettingKeyList{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(RetSystemStatusCapability_AssignableSettingsCapability);
    };

    // THMSGV1T1SetSystemExParam
    struct SetSystemExParamRetSystemCapability_AssignableSettingsCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        AsCapabilityKeyList assignableSettingKeyList{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SetSystemExParamRetSystemCapability_AssignableSettingsCapability);
    };

    // THMSGV1T1SetSystemParam
    struct SetSystemParamRetSystemCapability_AssignableSettingsCapability
    {
        // CODEGEN EnumRange Command::SYSTEM_SET_PARAM
        Command command{Command::SYSTEM_SET_PARAM}; // 0x0
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SystemInquiredType type{SystemInquiredType::ASSIGNABLE_SETTINGS}; // 0x1
        AsCapabilityKeyList assignableSettingKeyList{}; // 0x2

        MDR_DEFINE_EXTERN_SERIALIZATION(SetSystemParamRetSystemCapability_AssignableSettingsCapability);
    };
#pragma endregion Declarations
} // namespace mdr::v1::t1

#pragma pack(pop)

#include "Generated/ProtocolV1T1Enum.hpp"
#include "Generated/ProtocolV1T1Traits.hpp"
