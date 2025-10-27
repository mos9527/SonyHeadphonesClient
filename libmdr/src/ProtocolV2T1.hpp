#pragma once
#include "ProtocolV2.hpp"
#pragma pack(push, 1)

// Extracted from Sound Connect iOS 11.0.1
namespace mdr::v2::t1
{
#pragma region Enums
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
        GENERAL_SETTING_NTFY_PARAM = 0xD9,
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

    enum class ConnectInquiredType : UInt8
    {
        FIXED_VALUE = 0,
    };

    enum class DeviceInfoType : UInt8
    {
        MODEL_NAME = 1,
        FW_VERSION = 2,
        SERIES_AND_COLOR_INFO = 3,
        INSTRUCTION_GUIDE = 4,
    };

    enum class ModelColor : UInt8
    {
        DEFAULT = 0,
        BLACK = 1,
        WHITE = 2,
        SILVER = 3,
        RED = 4,
        BLUE = 5,
        PINK = 6,
        YELLOW = 7,
        GREEN = 8,
        GRAY = 9,
        GOLD = 10,
        CREAM = 11,
        ORANGE = 12,
        BROWN = 13,
        VIOLET = 14,
    };

    enum class ModelSeriesType : UInt8
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

    enum class CommonInquiredType : UInt8
    {
        CONCIERGE = 0x00,
        CONNECTION_STATUS = 0x01,
        // STATUS: [R N] CommonStatusAudioCodec
        AUDIO_CODEC = 0x02,
        UPSCALING_EFFECT = 0x03,
        BLE_SETUP = 0x04,
        CONNECTION_ESTABLISHED_TIME = 0x05,
        DEVICE_SPECIAL_MODE = 0x06,
        SMART_PHONE_AND_CONNECTED_DEVICE_INFORMATION_FOR_CLASSIC = 0x07,
        TANDEM_RECONNECTION_REQUEST = 0x08,
        DISPLAY_FW_VERSION = 0x09,
    };

    enum class AudioCodec : UInt8
    {
        UNSETTLED = 0x00,
        SBC = 0x01,
        AAC = 0x02,
        LDAC = 0x10,
        APT_X = 0x20,
        APT_X_HD = 0x21,
        LC3 = 0x30,
        OTHER = 0xFF,
    };

    enum class PowerInquiredType : UInt8
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

    enum class BatteryChargingStatus : UInt8
    {
        NOT_CHARGING = 0,
        CHARGING = 1,
        UNKNOWN = 2,
        CHARGED = 3,
    };

    enum class PowerOffSettingValue : UInt8
    {
        USER_POWER_OFF = 0x01,
        FACTORY_POWER_OFF = 0xFF,
    };

    enum class AutoPowerOffElements : UInt8
    {
        POWER_OFF_IN_5_MIN = 0x00,
        POWER_OFF_IN_30_MIN = 0x01,
        POWER_OFF_IN_60_MIN = 0x02,
        POWER_OFF_IN_180_MIN = 0x03,
        POWER_OFF_IN_15_MIN = 0x04,
        POWER_OFF_DISABLE = 0x11,
    };

    enum class AutoPowerOffWearingDetectionElements : UInt8
    {
        POWER_OFF_IN_5_MIN = 0x00,
        POWER_OFF_IN_30_MIN = 0x01,
        POWER_OFF_IN_60_MIN = 0x02,
        POWER_OFF_IN_180_MIN = 0x03,
        POWER_OFF_IN_15_MIN = 0x04,
        POWER_OFF_WHEN_REMOVED_FROM_EARS = 0x10,
        POWER_OFF_DISABLE = 0x11,
    };

    enum class EqEbbInquiredType : UInt8
    {
        // PARAM: [RSN] EqEbbParamEq
        PRESET_EQ = 0x00,
        // PARAM: [RSN] EqEbbParamEbb
        EBB = 0x01,
        // PARAM: [RSN] EqEbbParamEq
        PRESET_EQ_NONCUSTOMIZABLE = 0x02,
        // PARAM: [RSN] EqEbbParamEqAndUltMode
        PRESET_EQ_AND_ULT_MODE = 0x03,
        // PARAM: [RSN] EqEbbParamEq
        PRESET_EQ_AND_ERRORCODE = 0x04,
        // PARAM: [RSN] EqEbbParamSoundEffect
        SOUND_EFFECT = 0x30,
        // PARAM: [RSN] EqEbbParamCustomEq
        CUSTOM_EQ = 0x31,
        // PARAM: [ SN] EqEbbParamTurnKeyEq
        TURN_KEY_EQ = 0x32,
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
        GAMING_EQ = 0x20,
        FPS_1 = 0x21,
        FPS_2 = 0x22,
        FPS_3 = 0x23,
        HEAVY = 0x30,
        CLEAR = 0x31,
        HARD = 0x32,
        SOFT = 0x33,
        CUSTOM = 0xA0,
        USER_SETTING1 = 0xA1,
        USER_SETTING2 = 0xA2,
        USER_SETTING3 = 0xA3,
        USER_SETTING4 = 0xA4,
        USER_SETTING5 = 0xA5,
        ARTIST_COLLAB1 = 0xB0,
        ARTIST_COLLAB2 = 0xB1,
        ARTIST_COLLAB3 = 0xB2,
        ARTIST_COLLAB4 = 0xB3,
        ARTIST_COLLAB5 = 0xB4,
        ARTIST_COLLAB6 = 0xB5,
        ARTIST_COLLAB7 = 0xB6,
        ARTIST_COLLAB8 = 0xB7,
        ARTIST_COLLAB9 = 0xB8,
        ARTIST_COLLAB10 = 0xB9,
        ARTIST_COLLAB11 = 0xBA,
        ARTIST_COLLAB12 = 0xBB,
        UNSPECIFIED = 0xFF,
    };

    enum class EqUltMode : UInt8
    {
        OFF = 0x00,
        ULT_1 = 0x01,
        ULT_2 = 0x02,
    };

    enum class SoundEffectType : UInt8
    {
        SOUND_EFFECT_OFF = 0x00,
        SOUND_EFFECT_ULT = 0x01,
        SOUND_EFFECT_ULT1 = 0x02,
        SOUND_EFFECT_ULT2 = 0x03,
        SOUND_EFFECT_CUSTOM = 0x04,
        SOUND_EFFECT_NONE = 0xFF,
    };

    enum class PresetEqErrorCodeType : UInt8
    {
        CALLING = 0x00,
        DEMO_MODE = 0x01,
        LISTENING_MODE = 0x02,
        OTHER = 0xFE,
    };

    enum class NcAsmInquiredType : UInt8
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

    enum class ValueChangeStatus : UInt8
    {
        UNDER_CHANGING = 0,
        CHANGED = 1,
    };

    enum class NcAsmOnOffValue : UInt8
    {
        OFF = 0,
        ON = 1,
    };

    enum class NcAsmMode : UInt8
    {
        NC = 0,
        ASM = 1,
    };

    enum class AmbientSoundMode : UInt8
    {
        NORMAL = 0,
        VOICE = 1,
    };

    enum class NoiseAdaptiveSensitivity : UInt8
    {
        STANDARD = 0,
        HIGH = 1,
        LOW = 2,
    };

    enum class Function : UInt8
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

    enum class AlertInquiredType : UInt8
    {
        // STATUS: [ S ] AlertSetStatusFixedMessage
        // PARAM:  [ S ] AlertSetParamFixedMessage
        //         [  N] AlertNotifyParamFixedMessage
        FIXED_MESSAGE = 0x00,
        // STATUS: [   ] None
        // PARAM:  [ S ] AlertSetParamVibrator
        VIBRATOR_ALERT_NOTIFICATION = 0x01,
        // STATUS: [   ] None
        // PARAM:  [ S ] AlertSetParamFixedMessageWithLeftRightSelection
        //         [  N] AlertNotifyParamFixedMessageWithLeftRightSelection
        FIXED_MESSAGE_WITH_LEFT_RIGHT_SELECTION = 0x02,
        // STATUS: [R  ] AlertRetStatusVoiceAssistant
        // PARAM:  [   ] None
        VOICE_ASSISTANT_ALERT_NOTIFICATION = 0x03,
        // STATUS: [ S ] AlertSetStatusAppBecomesForeground
        // PARAM:  [ S ] AlertSetParamAppBecomesForeground
        //         [  N] AlertNotifyParamAppBecomesForeground
        APP_BECOMES_FOREGROUND = 0x04,
        // STATUS: [R N] AlertStatusLEAudioAlertNotification
        //         [ S ] AlertSetStatusLEAudioAlertNotification
        // PARAM:  [   ] None
        LE_AUDIO_ALERT_NOTIFICATION = 0x05,
        // STATUS: [   ] None
        // PARAM:  [ S ] AlertSetParamFlexibleMessage
        //         [  N] AlertNotifyParamFlexibleMessage
        FLEXIBLE_MESSAGE = 0x06,
    };

    enum class AlertMessageType : UInt8
    {
        DISCONNECT_CAUSED_BY_CONNECTION_MODE_CHANGE = 0x00,
        DISCONNECT_CAUSED_BY_CHANGING_KEY_ASSIGN = 0x01,
        NEED_DISCONNECTION_FOR_UPDATING_FIRMWARE = 0x02,
        GOOGLE_ASSISTANT_IS_NOW_AVAILABLE = 0x03,
        DUAL_ASSIGN_OF_VOICE_ASSISTANT_IS_UNAVAILABLE = 0x05,
        DISCONNECT_CAUSED_BY_CHANGING_MULTIPOINT_LDAC_DISABLE = 0x06,
        DISCONNECT_CAUSED_BY_CHANGING_MULTIPOINT = 0x07,
        BATTERY_CONSUMPTION_INCREASE_DUE_TO_EQ_AND_UPSCALING = 0x08,
        CAUTION_FOR_DUAL_ASSIGNMENT_OF_PLAYBACK_CONTROL = 0x09,
        CAUTION_FOR_DISABLE_TOUCH_SENSOR_PANEL = 0x0A,
        CAUTION_FOR_DISABLE_TOUCH_SENSOR_PANEL_AND_RECONNECTION = 0x0B,
        CAUTION_FOR_ABLE_TOUCH_SENSOR_PANEL = 0x0C,
        CAUTION_FOR_ABLE_TOUCH_SENSOR_PANEL_AND_RECONNECTION = 0x0D,
        CAUTION_FOR_DISABLE_VOICE_ASSISTANT_ASSIGNABLE_BUTTON = 0x0E,
        CAUTION_FOR_DISABLE_VOICE_ASSISTANT_ASSIGNABLE_SENSOR = 0x0F,
        CAUTION_FOR_DISABLE_VOICE_ASSISTANT_AND_RECONNECTION_ASSIGNABLE_BUTTON = 0x10,
        CAUTION_FOR_DISABLE_VOICE_ASSISTANT_AND_RECONNECTION_ASSIGNABLE_SENSOR = 0x11,
        CAUTION_FOR_ABLE_VOICE_ASSISTANT_ASSIGNABLE_BUTTON = 0x12,
        CAUTION_FOR_ABLE_VOICE_ASSISTANT_ASSIGNABLE_SENSOR = 0x13,
        CAUTION_FOR_ABLE_VOICE_ASSISTANT_AND_RECONNECTION_ASSIGNABLE_BUTTON = 0x14,
        CAUTION_FOR_ABLE_VOICE_ASSISTANT_AND_RECONNECTION_ASSIGNABLE_SENSOR = 0x15,
        CAUTION_FOR_CHANGE_VOICE_ASSISTANT = 0x16,
        CAUTION_FOR_CHANGE_VOICE_ASSISTANT_TOUCH_SENSOR_PANEL = 0x17,
        CAUTION_FOR_CHANGE_VOICE_ASSISTANT_TOUCH_SENSOR_PANEL_MOBILE_OR_SIRI = 0x18,
        FOREGROUND_CAUTION_NEED_DISCONNECTION_FOR_ENABLING_WAKE_WORD = 0x19,
        FOREGROUND_CAUTION_WAKE_WORD_IS_AVAILABLE_ONLY_IN_USE_OF_ALEXA = 0x1A,
        CAUTION_FOR_NOT_CONNECTED_COMPASS_MOUNTING_SIDE_RIGHT = 0x1B,
        CAUTION_FOR_CONNECTION_MODE_SOUND_QUALITY_PRIOR = 0x1C,
        CAUTION_FOR_FW_UPDATE_IN_PROGRESS = 0x1D,
        CAUTION_FOR_DISABLE_SAR_GM1 = 0x1E,
        CAUTION_FOR_GATT_DISCONNECTION_FOR_SAR = 0x1F,
        CAUTION_FOR_GATT_TO_ON = 0x20,
        CANT_ASSIGN_MS_AND_GOOGLE_ASSISTANT_AT_THE_SAME_TIME = 0x21,
        CAUTION_FOR_GATT_FOR_MS = 0x22,
        CAUTION_FOR_GATT_FOR_QUICK_ACCESS_SERVICE = 0x23,
        DISCONNECT_CAUSED_BY_GATT_ON = 0x24,
        DISCONNECT_AND_CHANGE_KEY_ASSIGN_CAUSED_BY_GATT_ON = 0x25,
        DISCONNECT_CAUSED_BY_CHANGING_KEY_ASSIGN_AND_CHANGE_GATT_TO_OFF = 0x26,
        DISCONNECT_CAUSED_BY_CHANGING_VOICE_ASSISTANT_AND_CHANGE_GATT_TO_OFF = 0x27,
        DISCONNECT_AND_CHANGE_VOICE_ASSISTANT_TO_SIRI_CAUSED_BY_GATT_ON = 0x28,
        CANT_USE_LDAC_WHILE_GATT_IS_ON = 0x29,
        CANT_USE_LDAC_IN_SOUND_QUALITY_PRIOR = 0x2A,
        BATTERY_CONSUMPTION_INCREASE_DUE_TO_SOUND_FUNCTION_1 = 0x2B,
        DISCONNECT_CAUSED_BY_CHANGING_CONNECTION_STANDBY_MODE_CLASSIC_ONLY = 0x2C,
        DISCONNECT_CAUSED_BY_CHANGING_CONNECTION_STANDBY_MODE_LE_AUDIO_CLASSIC = 0x2D,
        DISCONNECT_CAUSED_BY_ENTER_PAIRING_MODE_CLASSIC = 0x2E,
        DISCONNECT_CAUSED_BY_ENTER_PAIRING_MODE_WITH_FOLLOWING_FOR_LE_AUDIO_LIMITATION_VAS_CONNECTION_MODE = 0x2F,
        DISCONNECT_CAUSED_BY_CHANGE_CONNECTION_CLASSIC = 0x30,
        DISCONNECT_CAUSED_BY_CHANGE_CONNECTION_WITH_FOLLOWING_FOR_LE_AUDIO_LIMITATION_VAS_CONNECTION_MODE = 0x31,
        BISTO_LCH_ASSIGNMENT_AND_GATT_ACTIVATION_IS_IMPOSSIBLE = 0x32,
        TEAMS_MUST_BE_ASSIGNED_TO_EITHER_LEFT_OR_RIGHT = 0x33,
        DISCONNECT_CAUSED_BY_ENTER_PAIRING_MODE_WITH_FOLLOWING_FOR_LE_AUDIO_LIMITATION_ASSIGNABLE_SETTINGS_VA_QA_CONNECTION_MODE_QA
        = 0x34,
        DISCONNECT_CAUSED_BY_CHANGE_CONNECTION_WITH_FOLLOWING_FOR_LE_AUDIO_LIMITATION_ASSIGNABLE_SETTINGS_VA_QA_CONNECTION_MODE_QA
        = 0x35,
        DISCONNECT_CAUSED_BY_ENTER_PAIRING_MODE_WITH_FOLLOWING_FOR_LE_AUDIO_LIMITATION_VAS_CONNECTION_MODE_PDM = 0x36,
        DISCONNECT_CAUSED_BY_CHANGE_CONNECTION_WITH_FOLLOWING_FOR_LE_AUDIO_LIMITATION_VAS_CONNECTION_MODE_PDM = 0x37,
        DISCONNECT_CAUSED_BY_ENTER_PAIRING_MODE_WITH_FOLLOWING_FOR_LE_AUDIO_LIMITATION_VAS_CONNECTION_MODE_QA = 0x38,
        DISCONNECT_CAUSED_BY_CHANGE_CONNECTION_WITH_FOLLOWING_FOR_LE_AUDIO_LIMITATION_VAS_CONNECTION_MODE_QA = 0x39,
        DISCONNECT_CAUSED_BY_ENTER_PAIRING_MODE_WITH_FOLLOWING_FOR_LE_AUDIO_LIMITATION_VAS_CONNECTION_MODE_QA_PDM =
        0x40,
        DISCONNECT_CAUSED_BY_CHANGE_CONNECTION_WITH_FOLLOWING_FOR_LE_AUDIO_LIMITATION_VAS_CONNECTION_MODE_QA_PDM = 0x41,
        CANT_USE_LDAC_WHILE_GATT_IS_ON_NO_RIGHT_SIDE_LIMITATION = 0x50,
        CAUTION_FOR_CHANGING_VOICE_ASSISTANT_TO_SIRI = 0x51,
        CAUTION_FOR_GATT_TO_OFF_CAUSED_BY_CHANGING_VOICE_ASSISTANT = 0x52,
        CAUTION_FOR_LDAC_990 = 0x53,
        CAUTION_FOR_GATT_TO_ON_FOR_QA_EASY_SETTING = 0x54,
        CAUTION_FOR_FW_UPDATE_IN_PROGRESS_FOR_QA_EASY_SETTING = 0x55,
        DISCONNECT_CAUSED_BY_GATT_ON_FOR_EASY_SETTING = 0x56,
        DISCONNECT_AND_CHANGE_KEY_ASSIGN_CAUSED_BY_GATT_ON_FOR_QA_EASY_SETTING = 0x57,
        DISCONNECT_AND_CHANGE_VOICE_ASSISTANT_TO_SIRI_CAUSED_BY_GATT_ON_FOR_QA_EASY_SETTING = 0x58,
        CAUTION_FOR_AUTO_VOLUME_OPTIMIZER_TO_ON = 0x59,
        CAUTION_FOR_AUTO_VOLUME_TO_ON_SVC_IS_ON = 0x5A,
        CAUTION_FOR_AUTO_VOLUME_TO_ON_SVC_VOLUME_LIMITATION_IS_ON = 0x5B,
        CAUTION_FOR_EXCLUSIVE_EQ_CHANGING_AND_BGM_MODE = 0x5C,
        CAUTION_FOR_CHANGE_KEY_ASSIGN_TO_OTHER_VOICE_ASSISTANT = 0x5D,
        CAUTION_FOR_CHANGE_KEY_ASSIGN_TO_PLAYBACK_CONTROL_L = 0x5E,
        CAUTION_FOR_CHANGE_KEY_ASSIGN_TO_PLAYBACK_CONTROL_R = 0x5F,
        CAUTION_FOR_CHANGE_KEY_ASSIGN_TO_NC_ASM_QUICK_ACCESS_L = 0x60,
        CAUTION_FOR_CHANGE_KEY_ASSIGN_TO_NC_ASM_QUICK_ACCESS_R = 0x61,
        CAUTION_FOR_CHANGE_KEY_ASSIGN_TO_VOICE_ASSISTANT_TO_NOT_SVA = 0x62,
        CAUTION_FOR_CAN_USE_GOOGLE_HOT_WORD = 0x63,
        CAUTION_FOR_CALLING_WHEN_SVA_TRAINING = 0x64,
        CAUTION_FOR_CHANGE_KEY_ASSIGN_TO_BE_ENABLE_REPEAT_TAP = 0x65,
        CAUTION_FOR_CHANGE_KEY_ASSIGN_TO_BE_DISABLE_REPEAT_TAP = 0x66,
        CAUTION_FOR_ENABLING_2_DEVICES_CONNECTION_WITH_LDAC = 0x70,
        CAUTION_FOR_CHANGING_TO_QUALITY_PRIOR_CONNECTION_MODE_WITH_2_DEVICES_CONNECTION = 0x71,
        CAUTION_FOR_CONNECTED_2_DEVICES_IN_BACKGROUND_WITH_LDAC = 0x72,
        WARNING_FOR_CHANGING_TO_LDAC_990_WITH_2_DEVICES_CONNECTION = 0x73,
        DISCONNECT_CAUSED_BY_CHANGE_CLASSIC_AUDIO_SOUND_QUALITY_PRIOR_FROM_LE_AUDIO = 0x74,
        DISCONNECT_CAUSED_BY_CHANGE_CLASSIC_AUDIO_CONNECTION_QUALITY_PRIOR_FROM_LE_AUDIO = 0x75,
        DISCONNECT_CAUSED_BY_CHANGE_CLASSIC_AUDIO_SOUND_QUALITY_PRIOR = 0x76,
        DISCONNECT_CAUSED_BY_CHANGE_CLASSIC_AUDIO_CONNECTION_QUALITY_PRIOR = 0x77,
        CAUTION_FOR_TV_SOUND_BOOSTER_SETTING_ON = 0x80,
        CAUTION_FOR_TV_SOUND_BOOSTER_SETTING_OFF = 0x81,
        CAUTION_FOR_SETTINGS_STAMINA_FROM_ON_TO_OFF = 0x82,
        CAUTION_FOR_SETTINGS_STAMINA_FROM_ON_TO_OFF_NO_LIGHTING_MODE = 0x83,
        CAUTION_FOR_SETTINGS_STAMINA_FROM_OFF_TO_ON = 0x84,
        CAUTION_FOR_SETTINGS_STAMINA_FROM_OFF_TO_ON_NO_LIGHTING_MODE = 0x85,
        CAUTION_FOR_SETTINGS_SOUND_EFFECT_ON_STAMINA_ON = 0x86,
        CAUTION_FOR_SETTINGS_SOUND_EFFECT_ON_STAMINA_ON_NO_LIGHTING = 0x87,
        CAUTION_FOR_SETTINGS_LIGHTING_MODE_ON_WHEN_STAMINA_ON = 0x88,
        CAUTION_FOR_DIABLING_BGM_MODE = 0x90,
        CAUTION_FOR_USB_SUBMERSION_MONITOR_DURING_NOT_CHARGING = 0x91,
        CAUTION_FOR_USB_SUBMERSION_MONITOR_DURING_CASE_IN = 0x92,
        TURN_KEY_EQ_SUCCESS = 0x93,
    };

    enum class AlertFlexibleMessageType : UInt8
    {
        BATTERY_CONSUMPTION_INCREASE_DUE_TO_SIMULTANEOUS_3_SETTINGS = 0x00,
        CAUTION_FOR_FEATURES_EXCLUSIVE_TO_MULTI_POINT = 0x01,
        CAUTION_FOR_FEATURES_EXCLUSIVE_TO_VOICE_ASSISTANT = 0x02,
        CAUTION_FOR_FEATURES_EXCLUSIVE_TO_GATT = 0x03,
        CAUTION_FOR_FEATURES_EXCLUSIVE_TO_GATT_WITHOUT_REBOOT = 0x04,
        CAUTION_FOR_FEATURES_EXCLUSIVE_TO_GATT_NO_RIGHT_SIDE_LIMITATION = 0x05,
        CAUTION_FOR_FEATURES_EXCLUSIVE_TO_GATT_WITHOUT_REBOOT_NO_RIGHT_SIDE_LIMITATION = 0x06,
        BATTERY_CONSUMPTION_INCREASE_DUE_TO_SIMULTANEOUS_3_SETTINGS_REPLY_CONFIMATION = 0x07,
        CAUTION_FOR_FEATURES_EXCLUSIVE_TO_BGM_MODE = 0x08,
        CAUTION_FOR_FEATURES_EXCLUSIVE_TO_EQ = 0x09,
        CAUTION_FOR_FUNCTIONS_THAT_ARE_EXCLUSIVE_WITH_BT_STANDBY_OFF = 0x0A,
        BATTERY_CONSUMPTION_INCREASE_DUE_TO_ALL_SETTING_ACTIVATE = 0x0B,
        DISCONNECT_CAUSED_BY_ENTER_PAIRING_MODE_WITH_FOLLOWING_FOR_LE_AUDIO_LIMITATION = 0x0C,
        DISCONNECT_CAUSED_BY_CHANGE_CONNECTION_WITH_FOLLOWING_FOR_LE_AUDIO_LIMITATION = 0x0D,
        DISCONNECT_CAUSED_BY_CHANGING_CONNECTION_STANDBY_MODE_CLASSIC_ONLY = 0x0E,
        DISCONNECT_CAUSED_BY_CHANGING_CONNECTION_STANDBY_MODE_LE_AUDIO_CLASSIC = 0x0F,
        DISCONNECT_CAUSED_BY_ENTER_PAIRING_MODE_WITH_FOLLOWING_FOR_LE_AUDIO_LIMITATION_SETTING_ON_CONNECTION_MODE =
        0x10,
        DISCONNECT_CAUSED_BY_CHANGE_CONNECTION_TO_LE_AUDIO_WITH_FOLLOWING_FOR_LE_AUDIO_LIMITATION = 0x11,
    };

    enum class VoiceAssistantType : UInt8
    {
        GOOGLE_ASSISTANT = 0x00,
        AMAZON_ALEXA = 0x01,
        TENCENT_XIAOWEI = 0x02,
    };

    enum class ConfirmationType : UInt8
    {
        CONFIRMED = 0x00,
        CONFIRMED_DONT_SHOW_AGAIN = 0x01,
    };

    enum class AlertAction : UInt8
    {
        NEGATIVE = 0x00,
        POSITIVE = 0x01,
    };

    enum class VibrationType : UInt8
    {
        NO_PATTERN_SPECIFIED = 0x00,
    };

    enum class AlertMessageTypeWithLeftRightSelection : UInt8
    {
        CAUTION_FOR_CHANGE_VOICE_ASSISTANT_ASSIGNABLE_BUTTON = 0x00,
        CAUTION_FOR_CHANGE_VOICE_ASSISTANT_ASSIGNABLE_SENSOR = 0x01,
        CAUTION_FOR_CHANGE_VOICE_ASSISTANT_WITHOUT_REBOOT_ASSIGNABLE_BUTTON = 0x02,
        CAUTION_FOR_CHANGE_VOICE_ASSISTANT_WITHOUT_REBOOT_ASSIGNABLE_SENSOR = 0x03,
        CAUTION_FOR_CHANGE_VOICE_ASSISTANT_WITH_SVA_ASSIGNABLE_BUTTON = 0x04,
        CAUTION_FOR_CHANGE_VOICE_ASSISTANT_WITH_SVA_ASSIGNABLE_SENSOR = 0x05,
    };

    enum class AlertLeftRightAction : UInt8
    {
        NEGATIVE = 0x00,
        LEFT = 0x01,
        RIGHT = 0x02,
    };

    enum class AlertActionType : UInt8
    {
        CONFIRMATION_ONLY = 0x00,
        POSITIVE_NEGATIVE = 0x01,
        POSITIVE_CONFIRMATION_WITH_REPLY = 0x02,
    };

    enum class DefaultSelectedLeftRightValue : UInt8
    {
        LEFT = 0x00,
        RIGHT = 0x01,
    };

    enum class PlayInquiredType : UInt8
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

    enum class PlaybackStatus : UInt8
    {
        UNSETTLED = 0x00,
        PLAY = 0x01,
        PAUSE = 0x02,
        STOP = 0x03,
    };

    enum class MusicCallStatus : UInt8
    {
        MUSIC = 0x0,
        CALL = 0x1,
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
    };

    enum class PlaybackNameStatus
    {
        UNSETTLED = 0,
        NOTHING = 1,
        SETTLED = 2,
    };

    enum class PlayMode : UInt8
    {
        PLAY_MODE_OFF = 0x00,
        PLAY_FOLDER = 0x01,
        REPEAT_ALL = 0x02,
        REPEAT_FOLDER = 0x03,
        REPEAT_TRACK = 0x04,
        SHUFFLE_ALL = 0x05,
    };

    enum class GsInquiredType : UInt8
    {
        GENERAL_SETTING1 = 0xD1, ///< Capture Voice During a Phone Call
        GENERAL_SETTING2 = 0xD2, ///< Multipoint
        GENERAL_SETTING3 = 0xD3, ///< Touch sensor control panel
        GENERAL_SETTING4 = 0xD4,
    };

    enum class GsSettingType : UInt8
    {
        BOOLEAN_TYPE = 0x00,
        LIST_TYPE = 0x01,
    };

    enum class GsSettingValue : UInt8
    {
        ON = 0x00,
        OFF = 0x01,
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

    enum class GsStringFormat
    {
        RAW_NAME = 0x00,
        ENUM_NAME = 0x01,
    };

    enum class AudioInquiredType : UInt8
    {
        // PARAM:      [RSN] AudioParamConnection
        CONNECTION_MODE = 0x00,
        // CAPABILITY: [R  ] AudioRetCapabilityUpscaling
        // STATUS:     [R N] AudioStatusCommon
        // PARAM:      [RSN] AudioParamUpscaling
        UPSCALING = 0x01,
        // PARAM:      [RSN] AudioParamConnectionWithLdacStatus
        CONNECTION_MODE_WITH_LDAC_STATUS = 0x02,
        // STATUS:     [R N] AudioStatusCommon
        // PARAM:      [RSN] AudioParamBGMMode
        BGM_MODE = 0x03,
        // STATUS:     [R N] AudioStatusCommon
        // PARAM:      [RSN] AudioParamUpmixCinema
        UPMIX_CINEMA = 0x04,
        // PARAM:      [R  ] AudioRetParamConnectionModeClassicAudioLeAudio
        //             [ S ] AudioSetParamConnectionModeClassicAudioLeAudio
        //             [  N] AudioNtfyParamConnectionModeClassicAudioLeAudio
        CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO = 0x05,
        // STATUS:     [R N] AudioStatusCommon
        // PARAM:      [RSN] AudioParamVoiceContents
        VOICE_CONTENTS = 0x06,
        // STATUS:     [R N] AudioStatusCommon
        // PARAM:      [RSN] AudioParamSoundLeakageReduction
        SOUND_LEAKAGE_REDUCTION = 0x07,
        // PARAM:      [RSN] AudioParamListeningOptionAssignCustomizable
        LISTENING_OPTION_ASSIGN_CUSTOMIZABLE = 0x08,
        // PARAM:      [RSN] AudioParamBGMMode
        BGM_MODE_AND_ERRORCODE = 0x09,
        // PARAM:      [RSN] AudioParamUpmixSeries
        UPMIX_SERIES = 0x0A,
    };

    enum class PriorMode : UInt8
    {
        SOUND_QUALITY_PRIOR = 0x00,
        CONNECTION_QUALITY_PRIOR = 0x01,
        LOW_LATENCY_PRIOR_BETA = 0x02,
    };

    enum class UpscalingTypeAutoOff : UInt8
    {
        OFF = 0x00,
        AUTO = 0x01,
    };

    enum class RoomSize : UInt8
    {
        SMALL = 0x00,
        MIDDLE = 0x01,
        LARGE = 0x02,
    };

    enum class ListeningOptionAssignCustomizableItem : UInt8
    {
        STANDARD = 0x00,
        BGM = 0x01,
        UPMIX_CINEMA = 0x02,
        UPMIX_GAME = 0x03,
        UPMIX_MUSIC = 0x04,
    };

    enum class UpmixItemId : UInt8
    {
        NONE = 0x00,
        CINEMA = 0x01,
        GAME = 0x02,
        MUSIC = 0x03,
    };

    enum class SwitchingStream : UInt8
    {
        NONE = 0x00,
        LE_AUDIO = 0x01,
        CLASSIC_AUDIO = 0x02,
    };

    enum class UpscalingType : UInt8
    {
        DSEE_HX = 0x00,
        DSEE = 0x01,
        DSEE_HX_AI = 0x02,
        DSEE_ULTIMATE = 0x03,
    };

    enum class SystemInquiredType : UInt8
    {
        // PARAM:     [RSN] SystemParamCommon
        // EXT_PARAM: [   ] None
        VIBRATOR = 0x00,
        // PARAM:     [RSN] SystemParamCommon
        // EXT_PARAM: [   ] None
        PLAYBACK_CONTROL_BY_WEARING = 0x01,
        // PARAM:     [RSN] SystemParamSmartTalking
        // EXT_PARAM: [RSN] SystemExtParamSmartTalkingModeType1
        SMART_TALKING_MODE_TYPE1 = 0x02,
        // PARAM:     [RSN] SystemParamAssignableSettings
        // EXT_PARAM: [RSN] SystemExtParamAssignableSettings
        ASSIGNABLE_SETTINGS = 0x03,
        // PARAM:     [RSN] SystemParamVoiceAssistantSettings
        // EXT_PARAM: [   ] None
        VOICE_ASSISTANT_SETTINGS = 0x04,
        // PARAM:     [RSN] SystemParamCommon
        // EXT_PARAM: [   ] None
        VOICE_ASSISTANT_WAKE_WORD = 0x05,
        // PARAM:     [R N] SystemParamWearingStatusDetector
        //            [ S ] SystemSetParamWearingStatusDetector
        // EXT_PARAM: [R N] SystemNotifyExtParamWearingStatusDetector
        WEARING_STATUS_DETECTOR = 0x06,
        // PARAM:     [RSN] SystemParamEarpieceSelection
        // EXT_PARAM: [   ] None
        EARPIECE_SELECTION = 0x07,
        // PARAM:     [RSN] SystemParamCallSettings
        // EXT_PARAM: [ S ] SystemSetExtParamCallSettings
        //            [  N] SystemNotifyExtParamCallSettings
        CALL_SETTINGS = 0x08,
        // PARAM:     [ S ] SystemSetParamResetSettings
        //            [  N] SystemNotifyParamResetSettings
        // EXT_PARAM: [   ] None
        RESET_SETTINGS = 0x09,
        // PARAM:     [RSN] SystemParamCommon
        // EXT_PARAM: [   ] None
        AUTO_VOLUME = 0x0A,
        // PARAM:     [  N] SystemNotifyParamFaceTapTestMode
        // EXT_PARAM: [   ] None
        FACE_TAP_TEST_MODE = 0x0B,
        // PARAM:     [RSN] SystemParamSmartTalking
        // EXT_PARAM: [RSN] SystemExtParamSmartTalkingModeType2
        SMART_TALKING_MODE_TYPE2 = 0x0C,
        // PARAM:     [R N] SystemParamQuickAccess
        //            [ S ] SystemSetParamQuickAccess
        // EXT_PARAM: [   ] None
        QUICK_ACCESS = 0x0D,
        // PARAM:     [RSN] SystemParamAssignableSettingsWithLimit
        // EXT_PARAM: [   ] None
        ASSIGNABLE_SETTINGS_WITH_LIMITATION = 0x0E,
        // PARAM:     [RSN] SystemParamCommon
        // EXT_PARAM: [RSN] SystemExtParamAssignableSettingsWithLimit
        HEAD_GESTURE_ON_OFF = 0x0F,
        // PARAM:     [RSN] SystemParamHeadGestureTraining
        // EXT_PARAM: [   ] None
        HEAD_GESTURE_TRAINING = 0x10,
    };

    enum class Preset : UInt8
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

    enum class VoiceAssistant : UInt8
    {
        VOICE_RECOGNITION = 0x30,
        GOOGLE_ASSISTANT = 0x31,
        AMAZON_ALEXA = 0x32,
        TENCENT_XIAOWEI = 0x33,
        SONY_VOICE_ASSISTANT = 0x34,
        VOICE_ASSISTANT_ENABLED_IN_OTHER_DEVICE = 0x3F,
        NO_FUNCTION = 0xFF,
    };

    enum class EarpieceFittingDetectionOperationStatus : UInt8
    {
        DETECTION_IS_NOT_STARTED = 0x00,
        DETECTION_STARTED = 0x01,
        DETECTION_COMPLETED_SUCCESSFULLY = 0x02,
        DETECTION_COMPLETED_UNSUCCESSFULLY = 0x03,
    };

    enum class EarpieceFittingDetectionOperationErrorCode : UInt8
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

    enum class EarpieceSeries : UInt8
    {
        OTHER = 0x00,
        POLYURETHANE = 0x01,
        HYBRID = 0x02,
        SOFT_FITTING_FOR_LINKBUDS_FIT = 0x03,
        NOT_DETERMINED = 0xFF,
    };

    enum class EarpieceSize : UInt8
    {
        SS = 0x00,
        S = 0x01,
        M = 0x02,
        L = 0x03,
        LL = 0x04,
        NOT_DETERMINED = 0xFF,
    };

    enum class EarpieceFittingDetectionOperation : UInt8
    {
        DETECTION_START = 0x00,
        DETECTION_CANCEL = 0x01,
        DETECTION_START_FORCEFUL = 0x02,
    };

    enum class ResetType : UInt8
    {
        SETTINGS_ONLY = 0x00,
        FACTORY_RESET = 0x01,
    };

    enum class ResetResult : UInt8
    {
        SUCCESS = 0x00,
        ERROR_CONNECTION_LEFT = 0x01,
        ERROR_CONNECTION_RIGHT = 0x02,
    };

    enum class FaceTapKey : UInt8
    {
        LEFT_SIDE_KEY = 0x00,
        RIGHT_SIDE_KEY = 0x01,
    };

    enum class FaceTapAction : UInt8
    {
        DOUBLE_TAP = 0x00,
        TRIPLE_TAP = 0x01,
    };

    enum class HeadGestureAction : UInt8
    {
        NOD = 0x00,
        SWING = 0x01,
    };

    enum class DetectSensitivity : UInt8
    {
        AUTO = 0x00,
        HIGH = 0x01,
        LOW = 0x02,
    };

    enum class ModeOutTime : UInt8
    {
        FAST = 0x00,
        MID = 0x01,
        SLOW = 0x02,
        NONE = 0x03,
    };

    enum class Action : UInt8
    {
        SINGLE_TAP = 0x00,
        DOUBLE_TAP = 0x01,
        TRIPLE_TAP = 0x02,
        REPEAT_TAP = 0x03,
        SINGLE_TAP_AND_HOLD = 0x10,
        DOUBLE_TAP_AND_HOLD = 0x11,
        LONG_PRESS_THEN_ACTIVATE = 0x21,
        LONG_PRESS_DURING_ACTIVATE = 0x22,
    };

    enum class EarpieceFittingDetectionResult : UInt8
    {
        GOOD = 0x00,
        POOR = 0x01,
    };

    enum class CallSettingsTestSoundControl : UInt8
    {
        START = 0x00,
    };

    enum class CallSettingsTestSoundControlAck : UInt8
    {
        ACK = 0x00,
    };
#pragma endregion Enums

    struct CommandBase
    {
        Command command;

        MDR_DEFINE_TRIVIAL_SERIALIZATION(CommandBase);
    };
#pragma region Connect
    struct ConnectGetProtocolInfo
    {
        // CODEGEN EnumRange Command::CONNECT_GET_PROTOCOL_INFO
        Command command{Command::CONNECT_GET_PROTOCOL_INFO};
        ConnectInquiredType inquiredType{ConnectInquiredType::FIXED_VALUE};

        MDR_DEFINE_TRIVIAL_SERIALIZATION(ConnectGetProtocolInfo);
    };


    struct ConnectRetProtocolInfo
    {
        // CODEGEN EnumRange Command::CONNECT_RET_PROTOCOL_INFO
        Command command{Command::CONNECT_RET_PROTOCOL_INFO};
        ConnectInquiredType inquiredType{ConnectInquiredType::FIXED_VALUE};
        Int32BE protocolVersion; // 0x2-0x5
        MessageMdrV2EnableDisable supportTable1Value; // 0x6
        MessageMdrV2EnableDisable supportTable2Value; // 0x7

        MDR_DEFINE_TRIVIAL_SERIALIZATION(ConnectRetProtocolInfo);
    };


    struct ConnectGetCapabilityInfo
    {
        // CODEGEN EnumRange Command::CONNECT_GET_CAPABILITY_INFO
        Command command{Command::CONNECT_GET_CAPABILITY_INFO}; // 0x0
        ConnectInquiredType inquiredType{ConnectInquiredType::FIXED_VALUE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(ConnectGetCapabilityInfo);
    };

    struct ConnectGetDeviceInfo
    {
        // CODEGEN EnumRange Command::CONNECT_GET_DEVICE_INFO
        Command command{Command::CONNECT_GET_DEVICE_INFO}; // 0x0
        DeviceInfoType deviceInfoType; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(ConnectGetDeviceInfo);
    };


    struct ConnectRetCapabilityInfo
    {
        // CODEGEN EnumRange Command::CONNECT_RET_CAPABILITY_INFO
        Command command{Command::CONNECT_RET_CAPABILITY_INFO}; // 0x0
        ConnectInquiredType inquiredType{ConnectInquiredType::FIXED_VALUE}; // 0x1
        UInt8 capabilityCounter; // 0x2
        MDRPrefixedString uniqueID;

        MDR_DEFINE_EXTERN_SERIALIZATION(ConnectRetCapabilityInfo);
    };

    struct ConnectRetDeviceInfoBase
    {
        // CODEGEN EnumRange Command::CONNECT_RET_DEVICE_INFO
        Command command{Command::CONNECT_RET_DEVICE_INFO}; // 0x0
        DeviceInfoType type; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(ConnectRetDeviceInfoBase);
    };

    struct ConnectRetDeviceInfoModelName
    {
        // CODEGEN Field command EnumRange Command::CONNECT_RET_DEVICE_INFO
        ConnectRetDeviceInfoBase base;
        MDRPrefixedString value;

        MDR_DEFINE_EXTERN_SERIALIZATION(ConnectRetDeviceInfoModelName);
    };

    struct ConnectRetDeviceInfoFwVersion
    {
        // CODEGEN Field command EnumRange Command::CONNECT_RET_DEVICE_INFO
        ConnectRetDeviceInfoBase base;
        MDRPrefixedString value;

        MDR_DEFINE_EXTERN_SERIALIZATION(ConnectRetDeviceInfoFwVersion);
    };

    struct ConnectRetDeviceInfoSeriesAndColor
    {
        // CODEGEN Field command EnumRange Command::CONNECT_RET_DEVICE_INFO
        ConnectRetDeviceInfoBase base;
        ModelSeriesType series;
        ModelColor color;

        MDR_DEFINE_EXTERN_SERIALIZATION(ConnectRetDeviceInfoSeriesAndColor);
    };

    struct ConnectGetSupportFunction
    {
        // CODEGEN EnumRange Command::CONNECT_GET_SUPPORT_FUNCTION
        Command command{Command::CONNECT_GET_SUPPORT_FUNCTION}; // 0x0
        ConnectInquiredType inquiredType{ConnectInquiredType::FIXED_VALUE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(ConnectGetSupportFunction);
    };


    struct ConnectRetSupportFunction
    {
        // CODEGEN EnumRange Command::CONNECT_RET_SUPPORT_FUNCTION
        Command command{Command::CONNECT_RET_SUPPORT_FUNCTION}; // 0x0
        ConnectInquiredType inquiredType{ConnectInquiredType::FIXED_VALUE}; // 0x1
        MDRPodArray<MessageMdrV2SupportFunction> supportFunctions; // 0x2-

        MDR_DEFINE_EXTERN_SERIALIZATION(ConnectRetSupportFunction);
    };

#pragma endregion Connect
#pragma region Common
    struct CommonGetStatus
    {
        // CODEGEN EnumRange Command::COMMON_GET_STATUS
        Command command{Command::COMMON_GET_STATUS}; // 0x0
        CommonInquiredType type; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(CommonGetStatus);
    };

    struct CommonBase
    {
        Command command{Command::COMMON_GET_STATUS}; // 0x0
        CommonInquiredType type; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(CommonBase);
    };


    // - AUDIO_CODEC
    struct CommonStatusAudioCodec
    {
        // CODEGEN Field command EnumRange Command::COMMON_RET_STATUS Command::COMMON_NTFY_STATUS
        // CODEGEN Field type EnumRange CommonInquiredType::AUDIO_CODEC
        CommonBase base{Command::COMMON_RET_STATUS, CommonInquiredType::AUDIO_CODEC};
        AudioCodec audioCodec{AudioCodec::UNSETTLED}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(CommonStatusAudioCodec);
    };


#pragma endregion
#pragma region Power
    struct PowerGetStatus
    {
        // CODEGEN EnumRange Command::POWER_GET_STATUS
        Command command{Command::POWER_GET_STATUS}; // 0x0
        PowerInquiredType type{PowerInquiredType::BATTERY}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerGetStatus);
    };


    struct PowerBase
    {
        Command command{Command::POWER_GET_STATUS}; // 0x0
        PowerInquiredType type; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerBase);
    };


    struct PowerBatteryStatus
    {
        UInt8 batteryLevel{0}; // 0x0, 0-100
        BatteryChargingStatus chargingStatus{BatteryChargingStatus::UNKNOWN}; // 0x1
    };

    struct PowerLeftRightBatteryStatus
    {
        UInt8 leftBatteryLevel{0}; // 0x0, 0-100
        BatteryChargingStatus leftChargingStatus{BatteryChargingStatus::UNKNOWN}; // 0x1
        UInt8 rightBatteryLevel{0}; // 0x2, 0-100
        BatteryChargingStatus rightChargingStatus{BatteryChargingStatus::UNKNOWN}; // 0x3
    };

    struct PowerBatteryThresholdStatus
    {
        PowerBatteryStatus batteryStatus{}; // 0x0-0x1
        UInt8 batteryThreshold{0}; // 0x2, 0-100
    };

    // - BATTERY
    struct PowerRetStatusBattery
    {
        static constexpr PowerInquiredType kPowerInquiredType = PowerInquiredType::BATTERY;

        Command command{Command::POWER_RET_STATUS}; // 0x0
        PowerInquiredType type{kPowerInquiredType}; // 0x1
        // CODEGEN Field batteryLevel Range 0 100
        PowerBatteryStatus batteryStatus{}; // 0x2-0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetStatusBattery);
    };


    // - LEFT_RIGHT_BATTERY
    struct PowerRetStatusLeftRightBattery
    {
        // CODEGEN Field command EnumRange Command::POWER_RET_STATUS
        // CODEGEN Field type EnumRange PowerInquiredType::LEFT_RIGHT_BATTERY
        PowerBase base{Command::POWER_RET_STATUS, PowerInquiredType::LEFT_RIGHT_BATTERY};
        // CODEGEN Field leftBatteryLevel Range 0 100
        // CODEGEN Field rightBatteryLevel Range 0 100
        PowerLeftRightBatteryStatus batteryStatus{}; // 0x2-0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetStatusLeftRightBattery);
    };


    // - CRADLE_BATTERY
    struct PowerRetStatusCradleBattery
    {
        // CODEGEN Field command EnumRange Command::POWER_RET_STATUS
        // CODEGEN Field type EnumRange PowerInquiredType::CRADLE_BATTERY
        PowerBase base{Command::POWER_RET_STATUS, PowerInquiredType::CRADLE_BATTERY};
        PowerBatteryStatus batteryStatus{}; // 0x2-0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetStatusCradleBattery);
    };


    // - BATTERY_WITH_THRESHOLD
    struct PowerRetStatusBatteryThreshold
    {
        // CODEGEN Field command EnumRange Command::POWER_RET_STATUS
        // CODEGEN Field type EnumRange PowerInquiredType::BATTERY_WITH_THRESHOLD
        PowerBase base{Command::POWER_RET_STATUS, PowerInquiredType::BATTERY_WITH_THRESHOLD};
        // CODEGEN Field batteryStatus.batteryLevel Range 0 100
        // CODEGEN Field batteryThreshold Range 0 100
        PowerBatteryThresholdStatus batteryStatus{}; // 0x2-0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetStatusBatteryThreshold);
    };


    // - LR_BATTERY_WITH_THRESHOLD
    struct PowerRetStatusLeftRightBatteryThreshold
    {
        // CODEGEN Field command EnumRange Command::POWER_RET_STATUS
        // CODEGEN Field type EnumRange PowerInquiredType::LR_BATTERY_WITH_THRESHOLD
        PowerBase base{Command::POWER_RET_STATUS, PowerInquiredType::LR_BATTERY_WITH_THRESHOLD};
        // CODEGEN Field leftBatteryLevel Range 0 100
        // CODEGEN Field rightBatteryLevel Range 0 100
        PowerLeftRightBatteryStatus batteryStatus{}; // 0x2-0x5
        // CODEGEN Range 0 100
        UInt8 leftBatteryThreshold{0}; // 0x6, 0-100
        // CODEGEN Range 0 100
        UInt8 rightBatteryThreshold{0}; // 0x7, 0-100

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetStatusLeftRightBatteryThreshold);
    };


    // - CRADLE_BATTERY_WITH_THRESHOLD
    struct PowerRetStatusCradleBatteryThreshold
    {
        // CODEGEN Field command EnumRange Command::POWER_RET_STATUS
        // CODEGEN Field type EnumRange PowerInquiredType::CRADLE_BATTERY_WITH_THRESHOLD
        PowerBase base{Command::POWER_RET_STATUS, PowerInquiredType::CRADLE_BATTERY_WITH_THRESHOLD};
        // CODEGEN Field batteryStatus.batteryLevel Range 0 100
        PowerBatteryThresholdStatus batteryStatus{}; // 0x2-0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerRetStatusCradleBatteryThreshold);
    };


    // - POWER_OFF
    struct PowerSetStatusPowerOff
    {
        // CODEGEN Field command EnumRange Command::POWER_SET_STATUS
        // CODEGEN Field type EnumRange PowerInquiredType::POWER_OFF
        PowerBase base{Command::POWER_SET_STATUS, PowerInquiredType::POWER_OFF};
        PowerOffSettingValue powerOffSettingValue{PowerOffSettingValue::USER_POWER_OFF}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerSetStatusPowerOff);
    };


    struct PowerGetParam
    {
        // CODEGEN EnumRange Command::POWER_GET_PARAM
        Command command{Command::POWER_GET_PARAM}; // 0x0
        // CODEGEN EnumRange PowerInquiredType::AUTO_POWER_OFF PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION PowerInquiredType::POWER_SAVE_MODE PowerInquiredType::BATTERY_SAFE_MODE PowerInquiredType::CARING_CHARGE PowerInquiredType::BT_STANDBY PowerInquiredType::STAMINA PowerInquiredType::AUTOMATIC_TOUCH_PANEL_BACKLIGHT_TURN_OFF
        PowerInquiredType type{PowerInquiredType::AUTO_POWER_OFF}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerGetParam);
    };


    // - AUTO_POWER_OFF
    struct PowerParamAutoPowerOff
    {
        // CODEGEN Field command EnumRange Command::POWER_GET_PARAM Command::POWER_RET_PARAM Command::POWER_NTFY_PARAM
        // CODEGEN Field type EnumRange PowerInquiredType::AUTO_POWER_OFF
        PowerBase base{Command::POWER_GET_PARAM, PowerInquiredType::AUTO_POWER_OFF};
        AutoPowerOffElements currentPowerOffElements{AutoPowerOffElements::POWER_OFF_IN_5_MIN}; // 0x2
        AutoPowerOffElements lastSelectPowerOffElements{AutoPowerOffElements::POWER_OFF_IN_5_MIN}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerParamAutoPowerOff);
    };


    // - AUTO_POWER_OFF_WEARING_DETECTION
    struct PowerParamAutoPowerOffWithWearingDetection
    {
        // CODEGEN Field command EnumRange Command::POWER_GET_PARAM Command::POWER_RET_PARAM Command::POWER_NTFY_PARAM
        // CODEGEN Field type EnumRange PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION
        PowerBase base{Command::POWER_GET_PARAM, PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION};
        AutoPowerOffWearingDetectionElements currentPowerOffElements{
            AutoPowerOffWearingDetectionElements::POWER_OFF_IN_5_MIN}; // 0x2
        AutoPowerOffWearingDetectionElements lastSelectPowerOffElements{
            AutoPowerOffWearingDetectionElements::POWER_OFF_IN_5_MIN}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerParamAutoPowerOffWithWearingDetection);
    };


    // - POWER_SAVE_MODE, CARING_CHARGE, BT_STANDBY, STAMINA, AUTOMATIC_TOUCH_PANEL_BACKLIGHT_TURN_OFF
    struct PowerParamSettingOnOff
    {
        // CODEGEN Field command EnumRange Command::POWER_GET_PARAM Command::POWER_RET_PARAM Command::POWER_NTFY_PARAM
        // CODEGEN Field type EnumRange PowerInquiredType::POWER_SAVE_MODE PowerInquiredType::CARING_CHARGE PowerInquiredType::BT_STANDBY PowerInquiredType::STAMINA PowerInquiredType::AUTOMATIC_TOUCH_PANEL_BACKLIGHT_TURN_OFF
        PowerBase base{Command::POWER_GET_PARAM, PowerInquiredType::POWER_SAVE_MODE};
        MessageMdrV2OnOffSettingValue onOffSetting{MessageMdrV2OnOffSettingValue::OFF}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerParamSettingOnOff);
    };


    // - BATTERY_SAFE_MODE
    struct PowerParamBatterySafeMode
    {
        // CODEGEN Field command EnumRange Command::POWER_GET_PARAM Command::POWER_RET_PARAM Command::POWER_NTFY_PARAM
        // CODEGEN Field type EnumRange PowerInquiredType::BATTERY_SAFE_MODE
        PowerBase base{Command::POWER_GET_PARAM, PowerInquiredType::BATTERY_SAFE_MODE};
        MessageMdrV2OnOffSettingValue onOffSettingValue{MessageMdrV2OnOffSettingValue::OFF}; // 0x2
        MessageMdrV2OnOffSettingValue effectStatus{MessageMdrV2OnOffSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PowerParamBatterySafeMode);
    };

#pragma endregion Power
#pragma region EQ
    // 0x0, 0x1
    struct EqEbbBase
    {
        Command command{Command::EQEBB_RET_STATUS}; // 0x0
        EqEbbInquiredType type{EqEbbInquiredType::PRESET_EQ}; // 0x1
    };

    struct EqEbbGetStatus
    {
        // CODEGEN EnumRange Command::EQEBB_GET_STATUS
        Command command{Command::EQEBB_GET_STATUS}; // 0x0
        EqEbbInquiredType type{EqEbbInquiredType::PRESET_EQ}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(EqEbbGetStatus);
    };


    struct EqEbbStatusErrorCode
    {
        // CODEGEN Field command EnumRange Command::EQEBB_RET_STATUS Command::EQEBB_NTFY_STATUS
        // CODEGEN Field type EnumRange EqEbbInquiredType::PRESET_EQ_AND_ERRORCODE
        EqEbbBase base{Command::EQEBB_RET_STATUS, EqEbbInquiredType::PRESET_EQ_AND_ERRORCODE};
        MessageMdrV2EnableDisable value{MessageMdrV2EnableDisable::DISABLE}; // 0x2
        MDRPodArray<PresetEqErrorCodeType> errors; // 0x3, 0x4-

        MDR_DEFINE_EXTERN_SERIALIZATION(EqEbbStatusErrorCode);
    };


    struct EqEbbGetParam
    {
        // CODEGEN Field command EnumRange Command::EQEBB_GET_PARAM
        EqEbbBase base{Command::EQEBB_GET_PARAM, EqEbbInquiredType::PRESET_EQ};

        MDR_DEFINE_TRIVIAL_SERIALIZATION(EqEbbGetParam);
    };


    // - PRESET_EQ, PRESET_EQ_NONCUSTOMIZABLE, PRESET_EQ_AND_ERRORCODE
    struct EqEbbParamEq
    {
        // CODEGEN Field command EnumRange Command::EQEBB_RET_PARAM Command::EQEBB_SET_PARAM Command::EQEBB_NTFY_PARAM
        // CODEGEN Field type EnumRange EqEbbInquiredType::PRESET_EQ EqEbbInquiredType::PRESET_EQ_NONCUSTOMIZABLE EqEbbInquiredType::PRESET_EQ_AND_ERRORCODE
        EqEbbBase base{Command::EQEBB_RET_PARAM, EqEbbInquiredType::PRESET_EQ};
        EqPresetId presetId{EqPresetId::OFF}; // 0x2
        MDRPodArray<UInt8> bands; // 0x3, 0x4-

        MDR_DEFINE_EXTERN_SERIALIZATION(EqEbbParamEq);
    };


    // - EBB
    struct EqEbbParamEbb
    {
        // CODEGEN Field command EnumRange Command::EQEBB_RET_PARAM Command::EQEBB_SET_PARAM Command::EQEBB_NTFY_PARAM
        // CODEGEN Field type EnumRange EqEbbInquiredType::EBB
        EqEbbBase base{Command::EQEBB_RET_PARAM, EqEbbInquiredType::EBB};
        UInt8 level{0}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(EqEbbParamEbb);
    };


    // - PRESET_EQ_AND_ULT_MODE
    struct EqEbbParamEqAndUltMode
    {
        // CODEGEN Field command EnumRange Command::EQEBB_RET_PARAM Command::EQEBB_SET_PARAM Command::EQEBB_NTFY_PARAM
        // CODEGEN Field type EnumRange EqEbbInquiredType::PRESET_EQ_AND_ULT_MODE
        EqEbbBase base{Command::EQEBB_RET_PARAM, EqEbbInquiredType::PRESET_EQ_AND_ULT_MODE};
        EqPresetId presetId{EqPresetId::OFF}; // 0x2
        EqUltMode eqUltModeStatus{EqUltMode::OFF}; // 0x3
        MDRPodArray<UInt8> bandSteps; // 0x4, 0x5-

        MDR_DEFINE_EXTERN_SERIALIZATION(EqEbbParamEqAndUltMode);
    };


    // - SOUND_EFFECT
    struct EqEbbParamSoundEffect
    {
        // CODEGEN Field command EnumRange Command::EQEBB_RET_PARAM Command::EQEBB_SET_PARAM Command::EQEBB_NTFY_PARAM
        // CODEGEN Field type EnumRange EqEbbInquiredType::SOUND_EFFECT
        EqEbbBase base{Command::EQEBB_RET_PARAM, EqEbbInquiredType::SOUND_EFFECT};
        SoundEffectType soundEffectValue{SoundEffectType::SOUND_EFFECT_OFF}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(EqEbbParamSoundEffect);
    };


    // - CUSTOM_EQ
    struct EqEbbParamCustomEq
    {
        // CODEGEN Field command EnumRange Command::EQEBB_RET_PARAM Command::EQEBB_SET_PARAM Command::EQEBB_NTFY_PARAM
        // CODEGEN Field type EnumRange EqEbbInquiredType::CUSTOM_EQ
        EqEbbBase base{Command::EQEBB_RET_PARAM, EqEbbInquiredType::CUSTOM_EQ};
        MDRPodArray<UInt8> bandSteps; // 0x2, 0x3-

        MDR_DEFINE_EXTERN_SERIALIZATION(EqEbbParamCustomEq);
    };

#pragma endregion EQ
#pragma region NC/ASM
    struct NcAsmGetParam
    {
        // CODEGEN EnumRange Command::NCASM_GET_PARAM
        Command command{Command::NCASM_GET_PARAM};
        NcAsmInquiredType type; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NcAsmGetParam);
    };


    // 0x1-0x3
    struct NcAsmParamBase
    {
        Command command;
        NcAsmInquiredType type; // 0x1
        ValueChangeStatus valueChangeStatus; // 0x2
        NcAsmOnOffValue ncAsmTotalEffect; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NcAsmParamBase);
    };

    // - MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS
    struct NcAsmParamModeNcDualModeSwitchAsmSeamless
    {
        // CODEGEN Field command EnumRange Command::NCASM_RET_PARAM Command::NCASM_SET_PARAM Command::NCASM_NTFY_PARAM
        // CODEGEN Field type EnumRange NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS
        NcAsmParamBase base;
        NcAsmMode ncAsmMode; // 0x4
        AmbientSoundMode ambientSoundMode; // 0x5
        UInt8 ambientSoundLevelValue; // 0x6

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NcAsmParamModeNcDualModeSwitchAsmSeamless);
    };


    // - MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA
    struct NcAsmParamModeNcDualModeSwitchAsmSeamlessNa
    {
        // CODEGEN Field command EnumRange Command::NCASM_RET_PARAM Command::NCASM_SET_PARAM Command::NCASM_NTFY_PARAM
        // CODEGEN Field type EnumRange NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA
        NcAsmParamBase base;
        NcAsmMode ncAsmMode; // 0x4
        AmbientSoundMode ambientSoundMode; // 0x5
        UInt8 ambientSoundLevelValue; // 0x6
        NcAsmOnOffValue noiseAdaptiveOnOffValue; // 0x7
        NoiseAdaptiveSensitivity noiseAdaptiveSensitivitySettings; // 0x8

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NcAsmParamModeNcDualModeSwitchAsmSeamlessNa);
    };


    // - ASM_ON_OFF
    struct NcAsmParamAsmOnOff
    {
        // CODEGEN Field command EnumRange Command::NCASM_RET_PARAM Command::NCASM_SET_PARAM Command::NCASM_NTFY_PARAM
        // CODEGEN Field type EnumRange NcAsmInquiredType::ASM_ON_OFF
        NcAsmParamBase base;
        AmbientSoundMode ambientSoundMode; // 0x4
        NcAsmOnOffValue ambientSoundValue; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NcAsmParamAsmOnOff);
    };


    // - ASM_SEAMLESS
    struct NcAsmParamAsmSeamless
    {
        // CODEGEN Field command EnumRange Command::NCASM_RET_PARAM Command::NCASM_SET_PARAM Command::NCASM_NTFY_PARAM
        // CODEGEN Field type EnumRange NcAsmInquiredType::ASM_SEAMLESS
        NcAsmParamBase base;
        AmbientSoundMode ambientSoundMode; // 0x4
        UInt8 ambientSoundLevelValue; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NcAsmParamAsmSeamless);
    };


    // - NC_AMB_TOGGLE
    struct NcAsmParamNcAmbToggle
    {
        // CODEGEN Field command EnumRange Command::NCASM_RET_PARAM Command::NCASM_SET_PARAM Command::NCASM_NTFY_PARAM
        // CODEGEN Field type EnumRange NcAsmInquiredType::NC_AMB_TOGGLE
        NcAsmParamBase base;
        Function function; // 0x2
        MDR_DEFINE_TRIVIAL_SERIALIZATION(NcAsmParamNcAmbToggle);
    };

#pragma endregion NC/ASM
#pragma region Alert

    struct AlertBase
    {
        Command command{Command::ALERT_RET_STATUS};
        AlertInquiredType type; // 0x1
    };

    struct AlertGetStatus
    {
        // CODEGEN EnumRange Command::ALERT_GET_STATUS
        Command command{Command::ALERT_GET_STATUS};
        AlertInquiredType type; // 0x1
        MDR_DEFINE_TRIVIAL_SERIALIZATION(AlertGetStatus);
    };


    // - LE_AUDIO_ALERT_NOTIFICATION
    struct AlertStatusLEAudioAlertNotification
    {
        // CODEGEN Field command EnumRange Command::ALERT_RET_STATUS Command::ALERT_NTFY_STATUS
        // CODEGEN Field type EnumRange AlertInquiredType::LE_AUDIO_ALERT_NOTIFICATION
        AlertBase base{Command::ALERT_RET_STATUS, AlertInquiredType::LE_AUDIO_ALERT_NOTIFICATION};
        MessageMdrV2EnableDisable leAudioAlertStatus; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AlertStatusLEAudioAlertNotification);
    };


    // - VOICE_ASSISTANT_ALERT_NOTIFICATION
    struct AlertRetStatusVoiceAssistant
    {
        // CODEGEN Field command EnumRange Command::ALERT_RET_STATUS
        // CODEGEN Field type EnumRange AlertInquiredType::VOICE_ASSISTANT_ALERT_NOTIFICATION
        AlertBase base{Command::ALERT_RET_STATUS, AlertInquiredType::VOICE_ASSISTANT_ALERT_NOTIFICATION};
        MDRPodArray<VoiceAssistantType> voiceAssistants;

        MDR_DEFINE_EXTERN_SERIALIZATION(AlertRetStatusVoiceAssistant);
    };


    // - FIXED_MESSAGE
    struct AlertSetStatusFixedMessage
    {
        // CODEGEN Field command EnumRange Command::ALERT_SET_STATUS
        // CODEGEN Field type EnumRange AlertInquiredType::FIXED_MESSAGE
        AlertBase base{Command::ALERT_SET_STATUS, AlertInquiredType::FIXED_MESSAGE};
        MessageMdrV2EnableDisable status; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AlertSetStatusFixedMessage);
    };


    // - APP_BECOMES_FOREGROUND
    struct AlertSetStatusAppBecomesForeground
    {
        // CODEGEN Field command EnumRange Command::ALERT_SET_STATUS
        // CODEGEN Field type EnumRange AlertInquiredType::APP_BECOMES_FOREGROUND
        AlertBase base{Command::ALERT_SET_STATUS, AlertInquiredType::APP_BECOMES_FOREGROUND};
        MessageMdrV2EnableDisable status; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AlertSetStatusAppBecomesForeground);
    };

    // - LE_AUDIO_ALERT_NOTIFICATION

    struct AlertSetStatusLEAudioAlertNotification
    {
        // CODEGEN Field command EnumRange Command::ALERT_SET_STATUS
        // CODEGEN Field type EnumRange AlertInquiredType::LE_AUDIO_ALERT_NOTIFICATION
        AlertBase base{Command::ALERT_SET_STATUS, AlertInquiredType::LE_AUDIO_ALERT_NOTIFICATION};
        MessageMdrV2EnableDisable leAudioAlertStatus; // 0x2
        ConfirmationType confirmationType; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AlertSetStatusLEAudioAlertNotification);
    };


    // - FIXED_MESSAGE
    struct AlertSetParamFixedMessage
    {
        // CODEGEN Field command EnumRange Command::ALERT_SET_PARAM
        // CODEGEN Field type EnumRange AlertInquiredType::FIXED_MESSAGE
        AlertBase base{Command::ALERT_SET_PARAM, AlertInquiredType::FIXED_MESSAGE};
        AlertMessageType messageType; // 0x2
        AlertAction actionType; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AlertSetParamFixedMessage);
    };

    // - VIBRATOR_ALERT_NOTIFICATION

    struct AlertSetParamVibrator
    {
        // CODEGEN Field command EnumRange Command::ALERT_SET_PARAM
        // CODEGEN Field type EnumRange AlertInquiredType::VIBRATOR_ALERT_NOTIFICATION
        AlertBase base{Command::ALERT_SET_PARAM, AlertInquiredType::VIBRATOR_ALERT_NOTIFICATION};
        VibrationType vibrationType; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AlertSetParamVibrator);
    };


    // - FIXED_MESSAGE_WITH_LEFT_RIGHT_SELECTION
    struct AlertSetParamFixedMessageWithLeftRightSelection
    {
        // CODEGEN Field command EnumRange Command::ALERT_SET_PARAM
        // CODEGEN Field type EnumRange AlertInquiredType::FIXED_MESSAGE_WITH_LEFT_RIGHT_SELECTION
        AlertBase base{Command::ALERT_SET_PARAM, AlertInquiredType::FIXED_MESSAGE_WITH_LEFT_RIGHT_SELECTION};
        AlertMessageTypeWithLeftRightSelection messageType; // 0x2
        AlertLeftRightAction actionType; // 0x3
        MDR_DEFINE_TRIVIAL_SERIALIZATION(AlertSetParamFixedMessageWithLeftRightSelection);
    };


    // - APP_BECOMES_FOREGROUND
    struct AlertSetParamAppBecomesForeground
    {
        // CODEGEN Field command EnumRange Command::ALERT_SET_PARAM
        // CODEGEN Field type EnumRange AlertInquiredType::APP_BECOMES_FOREGROUND
        AlertBase base{Command::ALERT_SET_PARAM, AlertInquiredType::APP_BECOMES_FOREGROUND};
        AlertMessageType messageType; // 0x2
        AlertAction actionType; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AlertSetParamAppBecomesForeground);
    };


    // - FLEXIBLE_MESSAGE
    struct AlertSetParamFlexibleMessage
    {
        // CODEGEN Field command EnumRange Command::ALERT_SET_PARAM
        // CODEGEN Field type EnumRange AlertInquiredType::FLEXIBLE_MESSAGE
        AlertBase base{Command::ALERT_SET_PARAM, AlertInquiredType::FLEXIBLE_MESSAGE};
        AlertFlexibleMessageType messageType; // 0x2
        AlertAction actionType; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AlertSetParamFlexibleMessage);
    };


    // - FIXED_MESSAGE
    struct AlertNotifyParamFixedMessage
    {
        // CODEGEN Field command EnumRange Command::ALERT_NTFY_PARAM
        // CODEGEN Field type EnumRange AlertInquiredType::FIXED_MESSAGE
        AlertBase base{Command::ALERT_NTFY_PARAM, AlertInquiredType::FIXED_MESSAGE};
        AlertMessageType messageType; // 0x2
        AlertActionType actionType; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AlertNotifyParamFixedMessage);
    };


    // - FIXED_MESSAGE_WITH_LEFT_RIGHT_SELECTION
    struct AlertNotifyParamFixedMessageWithLeftRightSelection
    {
        // CODEGEN Field command EnumRange Command::ALERT_NTFY_PARAM
        // CODEGEN Field type EnumRange AlertInquiredType::FIXED_MESSAGE_WITH_LEFT_RIGHT_SELECTION
        AlertBase base{Command::ALERT_NTFY_PARAM, AlertInquiredType::FIXED_MESSAGE_WITH_LEFT_RIGHT_SELECTION};
        AlertMessageTypeWithLeftRightSelection messageType; // 0x2
        DefaultSelectedLeftRightValue defaultSelectedValue; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AlertNotifyParamFixedMessageWithLeftRightSelection);
    };


    // - APP_BECOMES_FOREGROUND
    struct AlertNotifyParamAppBecomesForeground
    {
        // CODEGEN Field command EnumRange Command::ALERT_NTFY_PARAM
        // CODEGEN Field type EnumRange AlertInquiredType::APP_BECOMES_FOREGROUND
        AlertBase base{Command::ALERT_NTFY_PARAM, AlertInquiredType::APP_BECOMES_FOREGROUND};
        AlertMessageType messageType; // 0x2
        AlertActionType actionType; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AlertNotifyParamAppBecomesForeground);
    };

#pragma endregion Alert
#pragma region Playback

    struct GetPlayParam
    {
        // CODEGEN EnumRange Command::PLAY_GET_STATUS
        Command command{Command::PLAY_GET_PARAM};
        PlayInquiredType type; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetPlayParam);
    };

    struct GetPlayStatus
    {
        // CODEGEN EnumRange Command::PLAY_GET_STATUS
        Command command{Command::PLAY_GET_STATUS};
        PlayInquiredType type; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetPlayStatus);
    };


    struct PlayBase
    {
        Command command{Command::PLAY_RET_STATUS};
        PlayInquiredType type; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PlayBase);
    };


    // - PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT
    struct PlayStatusPlaybackController
    {
        // CODEGEN Field command EnumRange Command::PLAY_RET_STATUS Command::PLAY_NTFY_STATUS
        // CODEGEN Field type EnumRange PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT
        PlayBase base{Command::PLAY_RET_STATUS, PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT};
        MessageMdrV2EnableDisable status; // 0x2
        PlaybackStatus playbackStatus; // 0x3
        MusicCallStatus musicCallStatus; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PlayStatusPlaybackController);
    };


    // - PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE
    struct PlayStatusPlaybackControlWithCallVolumeAdjustmentAndFunctionChange
    {
        // CODEGEN Field command EnumRange Command::PLAY_RET_STATUS Command::PLAY_NTFY_STATUS
        // CODEGEN Field type EnumRange PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE
        PlayBase base{Command::PLAY_RET_STATUS,
                      PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE};
        MessageMdrV2EnableDisable status; // 0x2
        PlaybackStatus playbackStatus; // 0x3
        MusicCallStatus musicCallStatus; // 0x4
        MessageMdrV2EnableDisable playbackControlStatus; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PlayStatusPlaybackControlWithCallVolumeAdjustmentAndFunctionChange);
    };


    // - PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE
    struct PlayStatusPlaybackControlWithFunctionChange
    {
        // CODEGEN Field command EnumRange Command::PLAY_RET_STATUS Command::PLAY_NTFY_STATUS
        // CODEGEN Field type EnumRange PlayInquiredType::PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE
        PlayBase base{Command::PLAY_RET_STATUS, PlayInquiredType::PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE};
        MessageMdrV2EnableDisable status; // 0x2
        PlaybackStatus playbackStatus; // 0x3
        MessageMdrV2EnableDisable playbackControlStatus; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PlayStatusPlaybackControlWithFunctionChange);
    };


    // - PLAY_MODE
    struct PlayStatusCommon
    {
        // CODEGEN Field command EnumRange Command::PLAY_RET_STATUS Command::PLAY_NTFY_STATUS
        PlayBase base{Command::PLAY_RET_STATUS, PlayInquiredType::PLAY_MODE};
        MessageMdrV2EnableDisable status; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PlayStatusCommon);
    };


    // - PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT, PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE, PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE
    struct PlayStatusSetPlaybackController
    {
        // CODEGEN Field command EnumRange Command::PLAY_SET_STATUS
        // CODEGEN Field type EnumRange PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE PlayInquiredType::PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE
        PlayBase base{Command::PLAY_SET_STATUS, PlayInquiredType::PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE};
        MessageMdrV2EnableDisable status; // 0x2
        PlaybackControl control; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PlayStatusSetPlaybackController);
    };


    struct PlayGetParam
    {
        // CODEGEN Field command EnumRange Command::PLAY_GET_PARAM
        PlayBase base{Command::PLAY_GET_PARAM};

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PlayGetParam);
    };


    struct PlayParamBase
    {
        Command command{Command::PLAY_RET_PARAM};
        PlayInquiredType type; // 0x1
    };

    // - PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT, PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE, PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE
    struct PlaybackName
    {
        PlaybackNameStatus playbackNameStatus;
        MDRPrefixedString playbackName;

        MDR_DEFINE_EXTERN_READ_WRITE(PlaybackName);
    };


    struct PlayParamPlaybackControllerName
    {
        // CODEGEN Field command EnumRange Command::PLAY_RET_PARAM Command::PLAY_NTFY_PARAM
        // CODEGEN Field type EnumRange PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE PlayInquiredType::PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE
        PlayParamBase base{Command::PLAY_RET_PARAM, PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT};
        MDRFixedArray<PlaybackName, 4> playbackNames;

        MDR_DEFINE_EXTERN_SERIALIZATION(PlayParamPlaybackControllerName);
    };


    // - MUSIC_VOLUME, CALL_VOLUME
    struct PlayParamPlaybackControllerVolume
    {
        // CODEGEN Field command EnumRange Command::PLAY_SET_PARAM Command::PLAY_RET_PARAM Command::PLAY_NTFY_PARAM
        // CODEGEN Field type EnumRange PlayInquiredType::MUSIC_VOLUME PlayInquiredType::CALL_VOLUME PlayInquiredType::MUSIC_VOLUME_WITH_MUTE PlayInquiredType::CALL_VOLUME_WITH_MUTE
        PlayParamBase base{Command::PLAY_RET_PARAM, PlayInquiredType::MUSIC_VOLUME};
        UInt8 volumeValue; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PlayParamPlaybackControllerVolume);
    };


    // - MUSIC_VOLUME_WITH_MUTE, CALL_VOLUME_WITH_MUTE
    struct PlayParamPlaybackControllerVolumeWithMute
    {
        // CODEGEN Field command EnumRange Command::PLAY_RET_PARAM Command::PLAY_NTFY_PARAM
        // CODEGEN Field type EnumRange PlayInquiredType::MUSIC_VOLUME_WITH_MUTE PlayInquiredType::CALL_VOLUME_WITH_MUTE
        PlayParamBase base{Command::PLAY_SET_PARAM, PlayInquiredType::MUSIC_VOLUME_WITH_MUTE};
        UInt8 volumeValue; // 0x2
        MessageMdrV2EnableDisable muteSetting; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PlayParamPlaybackControllerVolumeWithMute);
    };


    // - PLAY_MODE
    struct PlayParamPlayMode
    {
        // CODEGEN Field command EnumRange Command::PLAY_SET_PARAM Command::PLAY_RET_PARAM Command::PLAY_NTFY_PARAM
        // CODEGEN Field type EnumRange PlayInquiredType::PLAY_MODE
        PlayParamBase base{Command::PLAY_SET_PARAM, PlayInquiredType::PLAY_MODE};
        PlayMode playMode; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PlayParamPlayMode);
    };

#pragma endregion Playback
#pragma region General Setting

    struct GsGetCapability
    {
        // CODEGEN EnumRange Command::GENERAL_SETTING_GET_CAPABILITY
        Command command{Command::GENERAL_SETTING_GET_CAPABILITY};
        GsInquiredType type; // 0x1
        DisplayLanguage displayLanguage; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GsGetCapability);
    };


    struct GsSettingInfo
    {
        GsStringFormat stringFormat;
        MDRPrefixedString subject;
        MDRPrefixedString summary;

        MDR_DEFINE_EXTERN_READ_WRITE(GsSettingInfo);
    };


    struct GsRetCapability
    {
        // CODEGEN EnumRange Command::GENERAL_SETTING_RET_CAPABILITY
        Command command{Command::GENERAL_SETTING_RET_CAPABILITY};
        GsInquiredType type; // 0x1
        GsSettingType settingType; // 0x2
        GsSettingInfo settingInfo; // 0x3-...

        MDR_DEFINE_EXTERN_SERIALIZATION(GsRetCapability);
    };


    struct GsGetParam
    {
        // CODEGEN EnumRange Command::GENERAL_SETTING_GET_PARAM
        Command command{Command::GENERAL_SETTING_GET_PARAM};
        GsInquiredType type; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GsGetParam);
    };


    struct GsParamBase
    {
        Command command{Command::GENERAL_SETTING_RET_PARAM};
        GsInquiredType type; // 0x1
        GsSettingType settingType; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GsParamBase);
    };

    // - BOOLEAN_TYPE
    struct GsParamBoolean
    {
        // CODEGEN Field command EnumRange Command::GENERAL_SETTING_RET_PARAM Command::GENERAL_SETTING_SET_PARAM Command::GENERAL_SETTING_NTFY_PARAM
        // CODEGEN Field settingType EnumRange GsSettingType::BOOLEAN_TYPE
        GsParamBase base{Command::GENERAL_SETTING_RET_PARAM, GsInquiredType::GENERAL_SETTING1,
                         GsSettingType::BOOLEAN_TYPE};
        GsSettingValue settingValue; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GsParamBoolean);
    };


    // - LIST_TYPE
    struct GsParamList
    {
        // CODEGEN Field command EnumRange Command::GENERAL_SETTING_RET_PARAM Command::GENERAL_SETTING_SET_PARAM Command::GENERAL_SETTING_NTFY_PARAM
        // CODEGEN Field settingType EnumRange GsSettingType::LIST_TYPE
        GsParamBase base{Command::GENERAL_SETTING_RET_PARAM, GsInquiredType::GENERAL_SETTING1,
                         GsSettingType::LIST_TYPE};
        UInt8 currentElementIndex; // 0x3, 0-63

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GsParamList);
    };

#pragma endregion General Setting
#pragma region Audio

    struct AudioGetCapability
    {
        // CODEGEN EnumRange Command::AUDIO_GET_CAPABILITY
        Command command{Command::AUDIO_GET_CAPABILITY};
        AudioInquiredType type; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AudioGetCapability);
    };


    struct AudioBase
    {
        Command command{Command::AUDIO_RET_STATUS};
        AudioInquiredType type; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AudioBase);
    };

    // - UPSCALING
    struct AudioRetCapabilityUpscaling
    {
        // CODEGEN Field command EnumRange Command::AUDIO_RET_CAPABILITY
        // CODEGEN Field type EnumRange AudioInquiredType::UPSCALING
        AudioBase base{Command::AUDIO_RET_CAPABILITY, AudioInquiredType::UPSCALING};
        UpscalingType upscalingType; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AudioRetCapabilityUpscaling);
    };


    struct AudioGetStatus
    {
        // CODEGEN EnumRange Command::AUDIO_GET_STATUS
        Command command{Command::AUDIO_GET_STATUS};
        AudioInquiredType type; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AudioGetStatus);
    };


    // - UPSCALING, BGM_MODE, UPMIX_CINEMA, VOICE_CONTENTS, SOUND_LEAKAGE_REDUCTION
    struct AudioGetParam
    {
        // CODEGEN EnumRange Command::AUDIO_GET_PARAM
        Command command{Command::AUDIO_GET_PARAM};
        AudioInquiredType type; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AudioGetParam);
    };

    // - CONNECTION_MODE
    struct AudioParamConnection
    {
        // CODEGEN Field command EnumRange Command::AUDIO_RET_PARAM Command::AUDIO_SET_PARAM Command::AUDIO_NTFY_PARAM
        // CODEGEN Field type EnumRange AudioInquiredType::CONNECTION_MODE
        AudioBase base{Command::AUDIO_RET_PARAM, AudioInquiredType::CONNECTION_MODE};
        PriorMode settingValue; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AudioParamConnection);
    };


    // - UPSCALING
    struct AudioParamUpscaling
    {
        // CODEGEN Field command EnumRange Command::AUDIO_RET_PARAM Command::AUDIO_SET_PARAM Command::AUDIO_NTFY_PARAM
        // CODEGEN Field type EnumRange AudioInquiredType::UPSCALING
        AudioBase base{Command::AUDIO_RET_PARAM, AudioInquiredType::UPSCALING};
        UpscalingTypeAutoOff settingValue; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AudioParamUpscaling);
    };


    struct AudioStatusCommon
    {
        // CODEGEN Field command EnumRange Command::AUDIO_RET_PARAM Command::AUDIO_SET_PARAM Command::AUDIO_NTFY_PARAM
        AudioBase base{Command::AUDIO_RET_PARAM, AudioInquiredType::UPSCALING};
        MessageMdrV2EnableDisable status;

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AudioStatusCommon);
    };

    // - CONNECTION_MODE_WITH_LDAC_STATUS
    struct AudioParamConnectionWithLdacStatus
    {
        // CODEGEN Field command EnumRange Command::AUDIO_RET_PARAM Command::AUDIO_SET_PARAM Command::AUDIO_NTFY_PARAM
        // CODEGEN Field type EnumRange AudioInquiredType::CONNECTION_MODE_WITH_LDAC_STATUS
        AudioBase base{Command::AUDIO_RET_PARAM, AudioInquiredType::CONNECTION_MODE_WITH_LDAC_STATUS};
        PriorMode settingValue; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AudioParamConnectionWithLdacStatus);
    };


    // - CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO
    struct AudioRetParamConnectionModeClassicAudioLeAudio
    {
        // CODEGEN Field command EnumRange Command::AUDIO_RET_PARAM
        // CODEGEN Field type EnumRange AudioInquiredType::CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO
        AudioBase base{Command::AUDIO_RET_PARAM, AudioInquiredType::CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO};
        PriorMode settingValue; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AudioRetParamConnectionModeClassicAudioLeAudio);
    };


    // - BGM_MODE, BGM_MODE_AND_ERRORCODE
    struct AudioParamBGMMode
    {
        // CODEGEN Field command EnumRange Command::AUDIO_RET_PARAM Command::AUDIO_SET_PARAM Command::AUDIO_NTFY_PARAM
        // CODEGEN Field type EnumRange AudioInquiredType::BGM_MODE
        AudioBase base{Command::AUDIO_RET_PARAM, AudioInquiredType::BGM_MODE};
        MessageMdrV2EnableDisable onOffSettingValue; // 0x2
        RoomSize targetRoomSize; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AudioParamBGMMode);
    };


    // - UPMIX_CINEMA
    struct AudioParamUpmixCinema
    {
        // CODEGEN Field command EnumRange Command::AUDIO_RET_PARAM Command::AUDIO_SET_PARAM Command::AUDIO_NTFY_PARAM
        // CODEGEN Field type EnumRange AudioInquiredType::UPMIX_CINEMA
        AudioBase base{Command::AUDIO_RET_PARAM, AudioInquiredType::UPMIX_CINEMA};
        MessageMdrV2EnableDisable onOffSettingValue; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AudioParamUpmixCinema);
    };


    // - VOICE_CONTENTS
    struct AudioParamVoiceContents
    {
        // CODEGEN Field command EnumRange Command::AUDIO_RET_PARAM Command::AUDIO_SET_PARAM Command::AUDIO_NTFY_PARAM
        // CODEGEN Field type EnumRange AudioInquiredType::VOICE_CONTENTS
        AudioBase base{Command::AUDIO_RET_PARAM, AudioInquiredType::VOICE_CONTENTS};
        MessageMdrV2EnableDisable onOffSettingValue; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AudioParamVoiceContents);
    };

    // - SOUND_LEAKAGE_REDUCTION
    struct AudioParamSoundLeakageReduction
    {
        // CODEGEN Field command EnumRange Command::AUDIO_RET_PARAM Command::AUDIO_SET_PARAM Command::AUDIO_NTFY_PARAM
        // CODEGEN Field type EnumRange AudioInquiredType::SOUND_LEAKAGE_REDUCTION
        AudioBase base{Command::AUDIO_RET_PARAM, AudioInquiredType::SOUND_LEAKAGE_REDUCTION};
        MessageMdrV2EnableDisable onOffSettingValue; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AudioParamSoundLeakageReduction);
    };

    // - LISTENING_OPTION_ASSIGN_CUSTOMIZABLE
    struct AudioParamListeningOptionAssignCustomizableItem
    {
        // CODEGEN Field command EnumRange Command::AUDIO_RET_PARAM Command::AUDIO_SET_PARAM Command::AUDIO_NTFY_PARAM
        // CODEGEN Field type EnumRange AudioInquiredType::LISTENING_OPTION_ASSIGN_CUSTOMIZABLE
        AudioBase base{Command::AUDIO_RET_PARAM, AudioInquiredType::LISTENING_OPTION_ASSIGN_CUSTOMIZABLE};
        MDRPodArray<ListeningOptionAssignCustomizableItem> items;

        MDR_DEFINE_EXTERN_SERIALIZATION(AudioParamListeningOptionAssignCustomizableItem);
    };

    // - UPMIX_SERIES
    struct AudioParamUpmixSeries
    {
        // CODEGEN Field command EnumRange Command::AUDIO_RET_PARAM Command::AUDIO_SET_PARAM Command::AUDIO_NTFY_PARAM
        // CODEGEN Field type EnumRange AudioInquiredType::UPMIX_SERIES
        AudioBase base{Command::AUDIO_RET_PARAM, AudioInquiredType::UPMIX_SERIES};
        UpmixItemId upmixItemId; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AudioParamUpmixSeries);
    };


    // - CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO
    struct AudioSetParamConnectionModeClassicAudioLeAudio
    {
        // CODEGEN Field command EnumRange Command::AUDIO_SET_PARAM
        // CODEGEN Field type EnumRange AudioInquiredType::CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO
        AudioBase base{Command::AUDIO_SET_PARAM, AudioInquiredType::CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO};
        PriorMode settingValue; // 0x2
        MessageMdrV2EnableDisable alertConfirmation; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AudioSetParamConnectionModeClassicAudioLeAudio);
    };


    // - CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO
    struct AudioNtfyParamConnectionModeClassicAudioLeAudio
    {
        AudioBase base{Command::AUDIO_NTFY_PARAM, AudioInquiredType::CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO};
        PriorMode settingValue; // 0x2
        SwitchingStream switchingStream; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(AudioNtfyParamConnectionModeClassicAudioLeAudio);
    };

#pragma endregion Audio
#pragma region System

    struct SystemGetParam
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_PARAM
        Command command{Command::SYSTEM_GET_PARAM};
        SystemInquiredType type; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemGetParam);
    };


    struct SystemBase
    {
        Command command{Command::SYSTEM_RET_PARAM};
        SystemInquiredType type; // 0x1
    };

    // - VIBRATOR, PLAYBACK_CONTROL_BY_WEARING, VOICE_ASSISTANT_WAKE_WORD, AUTO_VOLUME, HEAD_GESTURE_ON_OFF
    struct SystemParamCommon
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_RET_PARAM Command::SYSTEM_SET_PARAM Command::SYSTEM_NTFY_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::VIBRATOR SystemInquiredType::PLAYBACK_CONTROL_BY_WEARING SystemInquiredType::VOICE_ASSISTANT_WAKE_WORD SystemInquiredType::AUTO_VOLUME SystemInquiredType::HEAD_GESTURE_ON_OFF
        SystemBase base{Command::SYSTEM_RET_PARAM};
        MessageMdrV2EnableDisable settingValue; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemParamCommon);
    };


    // - SMART_TALKING_MODE_TYPE1, SMART_TALKING_MODE_TYPE2
    struct SystemParamSmartTalking
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_RET_PARAM Command::SYSTEM_SET_PARAM Command::SYSTEM_NTFY_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::SMART_TALKING_MODE_TYPE1 SystemInquiredType::SMART_TALKING_MODE_TYPE2
        SystemBase base{Command::SYSTEM_RET_PARAM, SystemInquiredType::SMART_TALKING_MODE_TYPE1};
        MessageMdrV2EnableDisable onOffValue; // 0x2
        MessageMdrV2EnableDisable previewModeOnOffValue; // 0x3
    };

    // - ASSIGNABLE_SETTINGS
    struct SystemParamAssignableSettings
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_RET_PARAM Command::SYSTEM_SET_PARAM Command::SYSTEM_NTFY_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::ASSIGNABLE_SETTINGS
        SystemBase base{Command::SYSTEM_RET_PARAM, SystemInquiredType::ASSIGNABLE_SETTINGS};
        MDRPodArray<Preset> presets;

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemParamAssignableSettings);
    };


    // - VOICE_ASSISTANT_SETTINGS
    struct SystemParamVoiceAssistantSettings
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_RET_PARAM Command::SYSTEM_SET_PARAM Command::SYSTEM_NTFY_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::VOICE_ASSISTANT_SETTINGS
        SystemBase base{Command::SYSTEM_RET_PARAM, SystemInquiredType::VOICE_ASSISTANT_SETTINGS};
        VoiceAssistant voiceAssistant; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemParamVoiceAssistantSettings);
    };


    // - WEARING_STATUS_DETECTOR
    struct SystemParamWearingStatusDetector
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_RET_PARAM Command::SYSTEM_SET_PARAM Command::SYSTEM_NTFY_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::WEARING_STATUS_DETECTOR
        SystemBase base{Command::SYSTEM_RET_PARAM, SystemInquiredType::WEARING_STATUS_DETECTOR};
        EarpieceFittingDetectionOperationStatus operationStatus; // 0x2
        EarpieceFittingDetectionOperationErrorCode errorCode; // 0x3
        UInt8 numOfSelectedEarpieces; // 0x4
        UInt8 indexOfCurrentDetection; // 0x5
        EarpieceSeries currentDetectingSeries; // 0x6
        EarpieceSize earpieceSize; // 0x7

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemParamWearingStatusDetector);
    };


    // - EARPIECE_SELECTION
    struct SystemParamEarpieceSelection
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_RET_PARAM Command::SYSTEM_SET_PARAM Command::SYSTEM_NTFY_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::EARPIECE_SELECTION
        SystemBase base{Command::SYSTEM_RET_PARAM, SystemInquiredType::EARPIECE_SELECTION};
        EarpieceSeries series; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemParamEarpieceSelection);
    };


    // - CALL_SETTINGS
    struct SystemParamCallSettings
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_RET_PARAM Command::SYSTEM_SET_PARAM Command::SYSTEM_NTFY_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::CALL_SETTINGS
        SystemBase base{Command::SYSTEM_RET_PARAM, SystemInquiredType::CALL_SETTINGS};
        MessageMdrV2EnableDisable selfVoiceOnOff; // 0x2
        UInt8 selfVoiceVolume; // 0x3
        UInt8 callVoiceVolume; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemParamCallSettings);
    };

    // - QUICK_ACCESS (Not implemented, variable size)
    // - ASSIGNABLE_SETTINGS_WITH_LIMITATION
    struct SystemParamAssignableSettingsWithLimit
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_RET_PARAM Command::SYSTEM_SET_PARAM Command::SYSTEM_NTFY_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::ASSIGNABLE_SETTINGS_WITH_LIMITATION
        SystemBase base{Command::SYSTEM_RET_PARAM, SystemInquiredType::ASSIGNABLE_SETTINGS_WITH_LIMITATION};
        MDRPodArray<Preset> presets;

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemParamAssignableSettingsWithLimit);
    };


    // - HEAD_GESTURE_TRAINING
    struct SystemParamHeadGestureTraining
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_RET_PARAM Command::SYSTEM_SET_PARAM Command::SYSTEM_NTFY_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::HEAD_GESTURE_TRAINING
        SystemBase base{Command::SYSTEM_RET_PARAM, SystemInquiredType::HEAD_GESTURE_TRAINING};
        HeadGestureAction headGestureAction; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemParamHeadGestureTraining);
    };


    // - WEARING_STATUS_DETECTOR
    struct SystemSetParamWearingStatusDetector
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_SET_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::WEARING_STATUS_DETECTOR
        SystemBase base{Command::SYSTEM_SET_PARAM, SystemInquiredType::WEARING_STATUS_DETECTOR};
        EarpieceFittingDetectionOperation operation; // 0x2
        UInt8 indexOfCurrentDetection; // 0x3
        EarpieceSeries currentDetectionSeries; // 0x4
        EarpieceSize currentDetectionSize; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParamWearingStatusDetector);
    };


    // - RESET_SETTINGS
    struct SystemSetParamResetSettings
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_SET_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::RESET_SETTINGS
        SystemBase base{Command::SYSTEM_SET_PARAM, SystemInquiredType::RESET_SETTINGS};
        ResetType resetType; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetParamResetSettings);
    };


    // - RESET_SETTINGS
    struct SystemNotifyParamResetSettings
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_NTFY_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::RESET_SETTINGS
        SystemBase base{Command::SYSTEM_NTFY_PARAM, SystemInquiredType::RESET_SETTINGS};
        ResetResult resetResult; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyParamResetSettings);
    };


    // - FACE_TAP_TEST_MODE
    struct SystemNotifyParamFaceTapTestMode
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_NTFY_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::FACE_TAP_TEST_MODE
        SystemBase base{Command::SYSTEM_NTFY_PARAM, SystemInquiredType::FACE_TAP_TEST_MODE};
        FaceTapKey key; // 0x2
        FaceTapAction action; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyParamFaceTapTestMode);
    };


    struct AssignableSettingsAction
    {
        Action action;
        Function function;
    };

    struct AssignableSettingsPreset
    {
        Preset preset;
        MDRPodArray<AssignableSettingsAction> actions;

        MDR_DEFINE_EXTERN_READ_WRITE(AssignableSettingsPreset);
    };


    struct SystemGetExtParam
    {
        // CODEGEN EnumRange Command::SYSTEM_GET_EXT_PARAM
        Command command{Command::SYSTEM_GET_EXT_PARAM};
        SystemInquiredType type; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemGetExtParam);
    };


    struct SystemExtBase
    {
        Command command{Command::SYSTEM_RET_EXT_PARAM};
        SystemInquiredType type; // 0x1
    };

    // - SMART_TALKING_MODE_TYPE1
    struct SystemExtParamSmartTalkingMode1
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_RET_EXT_PARAM Command::SYSTEM_SET_EXT_PARAM Command::SYSTEM_NTFY_EXT_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::SMART_TALKING_MODE_TYPE1
        SystemExtBase base{Command::SYSTEM_RET_EXT_PARAM, SystemInquiredType::SMART_TALKING_MODE_TYPE1};
        DetectSensitivity detectSensitivity; // 0x2
        MessageMdrV2EnableDisable voiceFocus; // 0x3
        ModeOutTime modeOffTime; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemExtParamSmartTalkingMode1);
    };


    // - ASSIGNABLE_SETTINGS
    struct SystemExtParamAssignableSettings
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_RET_EXT_PARAM Command::SYSTEM_SET_EXT_PARAM Command::SYSTEM_NTFY_EXT_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::ASSIGNABLE_SETTINGS
        SystemExtBase base{Command::SYSTEM_RET_EXT_PARAM, SystemInquiredType::ASSIGNABLE_SETTINGS};
        MDRArray<AssignableSettingsPreset> presets;

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemExtParamAssignableSettings);
    };

    // - WEARING_STATUS_DETECTOR
    struct SystemExtParamWearingStatusDetector
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_RET_EXT_PARAM Command::SYSTEM_SET_EXT_PARAM Command::SYSTEM_NTFY_EXT_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::WEARING_STATUS_DETECTOR
        SystemExtBase base{Command::SYSTEM_RET_EXT_PARAM, SystemInquiredType::WEARING_STATUS_DETECTOR};
        EarpieceFittingDetectionResult fittingResultLeft; // 0x2
        EarpieceFittingDetectionResult fittingResultRight; // 0x3
        EarpieceSeries bestEarpieceSeriesLeft; // 0x4
        EarpieceSeries bestEarpieceSeriesRight; // 0x5
        EarpieceSize bestEarpieceSizeLeft; // 0x6
        EarpieceSize bestEarpieceSizeRight; // 0x7

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemExtParamWearingStatusDetector);
    };


    // - SMART_TALKING_MODE_TYPE2
    struct SystemExtParamSmartTalkingMode2
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_RET_EXT_PARAM Command::SYSTEM_SET_EXT_PARAM Command::SYSTEM_NTFY_EXT_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::SMART_TALKING_MODE_TYPE2
        SystemExtBase base{Command::SYSTEM_RET_EXT_PARAM, SystemInquiredType::SMART_TALKING_MODE_TYPE2};
        DetectSensitivity detectSensitivity; // 0x2
        ModeOutTime modeOffTime; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemExtParamSmartTalkingMode2);
    };


    // - ASSIGNABLE_SETTINGS_WITH_LIMITATION
    struct SystemExtParamAssignableSettingsWithLimit
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_RET_EXT_PARAM Command::SYSTEM_SET_EXT_PARAM Command::SYSTEM_NTFY_EXT_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::ASSIGNABLE_SETTINGS_WITH_LIMITATION
        SystemExtBase base{Command::SYSTEM_RET_EXT_PARAM, SystemInquiredType::ASSIGNABLE_SETTINGS_WITH_LIMITATION};
        MDRArray<AssignableSettingsPreset> presets;

        MDR_DEFINE_EXTERN_SERIALIZATION(SystemExtParamAssignableSettingsWithLimit);
    };


    // - CALL_SETTINGS
    struct SystemSetExtParamCallSettings
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_SET_EXT_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::CALL_SETTINGS
        SystemExtBase base{Command::SYSTEM_SET_EXT_PARAM, SystemInquiredType::CALL_SETTINGS};
        CallSettingsTestSoundControl testSoundControl; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemSetExtParamCallSettings);
    };


    // - CALL_SETTINGS
    struct SystemNotifyExtParamCallSettings
    {
        // CODEGEN Field command EnumRange Command::SYSTEM_NTFY_EXT_PARAM
        // CODEGEN Field type EnumRange SystemInquiredType::CALL_SETTINGS
        SystemExtBase base{Command::SYSTEM_NTFY_EXT_PARAM, SystemInquiredType::CALL_SETTINGS};
        CallSettingsTestSoundControlAck testSoundControlAck; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SystemNotifyExtParamCallSettings);
    };

#pragma endregion System
} // namespace mdr::v2::t1

#pragma pack(pop)

#include "Generated/ProtocolV2T1Enum.hpp"
#include "Generated/ProtocolV2T1Traits.hpp"
