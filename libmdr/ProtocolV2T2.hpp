#pragma once

#include "ProtocolV2.hpp"
#pragma pack(push, 1)

namespace mdr::v2::t2
{

    // Extracted from Sound Connect iOS 11.0.1
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

    // region Connect

    enum class ConnectInquiredType : UInt8
    {
        FIXED_VALUE = 0,
    };

    struct ConnectGetSupportFunction
    {
        static constexpr Command kResponseCommand = Command::CONNECT_RET_SUPPORT_FUNCTION;
        Command command{Command::CONNECT_GET_SUPPORT_FUNCTION}; // 0x0
        ConnectInquiredType inquiredType{ConnectInquiredType::FIXED_VALUE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(ConnectGetSupportFunction);
    };

    static_assert(MDRIsSerializable<ConnectGetSupportFunction>);

    struct ConnectRetSupportFunction
    {
        Command command{Command::CONNECT_RET_SUPPORT_FUNCTION}; // 0x0
        ConnectInquiredType inquiredType{ConnectInquiredType::FIXED_VALUE}; // 0x1
        MDRPodArray<MessageMdrV2SupportFunction> supportFunctions; // 0x2-

        static size_t Serialize(const ConnectRetSupportFunction& data, UInt8* out);
        static void Deserialize(UInt8* data, ConnectRetSupportFunction& out);
    };

    static_assert(MDRIsSerializable<ConnectRetSupportFunction>);
    // endregion Connect

    // region Peripheral

    enum class PeripheralInquiredType : UInt8
    {
        PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT = 0x00,
        SOURCE_SWITCH_CONTROL = 0x01,
        PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE = 0x02,
        MUSIC_HAND_OVER_SETTING = 0x03,
    };

    enum class PeripheralBluetoothMode : UInt8
    {
        NORMAL_MODE = 0x00,
        INQUIRY_SCAN_MODE = 0x01,
    };

    // region PERI_*_STATUS

    // region PERI_GET_STATUS
    struct PeripheralBase
    {
        Command command{Command::PERI_GET_STATUS};
        PeripheralInquiredType inquiredType; // 0x1
    };

    struct PeripheralGetStatus
    {
        static constexpr Command kResponseCommand = Command::PERI_RET_STATUS;
        PeripheralBase base{Command::PERI_GET_STATUS};

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralGetStatus);
    };

    static_assert(MDRIsSerializable<PeripheralGetStatus>);
    // endregion PERI_GET_STATUS

    // region PERI_RET_STATUS, PERI_SET_STATUS, PERI_NTFY_STATUS


    // - PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT, PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE

    struct PeripheralStatusPairingDeviceManagementCommon
    {
        PeripheralBase base{Command::PERI_GET_STATUS};
        PeripheralBluetoothMode btMode; // 0x2
        MessageMdrV2EnableDisable enableDisableStatus; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralStatusPairingDeviceManagementCommon);
    };
    static_assert(MDRIsSerializable<PeripheralStatusPairingDeviceManagementCommon>);

    // endregion PERI_RET_STATUS, PERI_SET_STATUS, PERI_NTFY_STATUS

    // endregion PERI_*_STATUS

    // region PERI_*_PARAM

    struct PeripheralDeviceInfo
    {
        static constexpr size_t kMacAddressLength = 17;
        Array<char, kMacAddressLength> btDeviceAddress;
        // MAC address "XX:XX:XX:XX:XX:XX" (17 bytes, no null terminator)
        UInt8 connectedStatus;
        MDRPrefixedString btFriendlyName;

        static void Read(UInt8** ppSrcBuffer, PeripheralDeviceInfo &out, size_t maxSize = ~0LL);
        static size_t Write(const PeripheralDeviceInfo &data, UInt8** ppDstBuffer);
    };

    struct PeripheralDeviceInfoWithBluetoothClassOfDevice
    {
        static constexpr size_t kMacAddressLength = 17;
        static constexpr uint32_t kUnknownClassOfDevice = 0xFFFFFF;

        Array<char, kMacAddressLength> btDeviceAddress;
        // MAC address "XX:XX:XX:XX:XX:XX" (17 bytes, no null terminator)
        UInt8 connectedStatus;
        Int32BE bluetoothClassOfDevice;
        MDRPrefixedString btFriendlyName;

        static void Read(UInt8** ppSrcBuffer, PeripheralDeviceInfoWithBluetoothClassOfDevice &out, size_t maxSize = ~0LL);
        static size_t Write(const PeripheralDeviceInfoWithBluetoothClassOfDevice &data, UInt8** ppDstBuffer);
    };

#pragma pack(push, 1)

    // region PERI_GET_PARAM

    struct PeripheralGetParam
    {
        static constexpr Command kResponseCommand = Command::PERI_RET_PARAM;
        PeripheralBase base{Command::PERI_GET_PARAM};

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralGetParam);
    };
    static_assert(MDRIsSerializable<PeripheralGetParam>);
    // endregion PERI_GET_PARAM

    // region PERI_RET_PARAM, PERI_SET_PARAM, PERI_NTFY_PARAM
    // - PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT

#pragma pack(pop)

    struct PeripheralParamPairingDeviceManagementClassicBt
    {
        static constexpr size_t kIndexNumOfPairedDevice = 2;
        static constexpr size_t kBluetoothDeviceAddressLength = 17;
        // App has this set to 1, however the firmware allows the friendly name to be empty.
        // See https://github.com/mos9527/SonyHeadphonesClient/issues/21
        static constexpr size_t kMinBtFriendlyNameLength = 0;
        static constexpr size_t kMaxBtFriendlyNameLength = 128;

        PeripheralBase base{Command::PERI_GET_PARAM,
            PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT};

        MDRArray<PeripheralDeviceInfo> deviceList;
        UInt8 playbackDevice;

        static size_t Serialize(const PeripheralParamPairingDeviceManagementClassicBt &data, UInt8* out);
        static void Deserialize(UInt8* data, PeripheralParamPairingDeviceManagementClassicBt &out);
    };

    // - SOURCE_SWITCH_CONTROL

    struct PeripheralParamSourceSwitchControl
    {
        PeripheralBase base{Command::PERI_GET_PARAM, PeripheralInquiredType::SOURCE_SWITCH_CONTROL};
        MessageMdrV2OnOffSettingValue sourceKeeping; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralParamSourceSwitchControl);
    };
    static_assert(MDRIsSerializable<PeripheralParamSourceSwitchControl>);
    // - PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE

#pragma pack(pop)

    struct PeripheralParamPairingDeviceManagementWithBluetoothClassOfDevice
    {
        static constexpr size_t kIndexNumOfPairedDevice = 2;
        static constexpr size_t kBluetoothDeviceAddressLength = 17;
        // App has this set to 1, however the firmware allows the friendly name to be empty.
        // See https://github.com/mos9527/SonyHeadphonesClient/issues/21
        static constexpr size_t kMinBtFriendlyNameLength = 0;
        static constexpr size_t kMaxBtFriendlyNameLength = 128;

        PeripheralBase base{Command::PERI_GET_PARAM,
            PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE};

        MDRArray<PeripheralDeviceInfoWithBluetoothClassOfDevice> deviceList;
        UInt8 playbackDevice;

        static size_t Serialize(const PeripheralParamPairingDeviceManagementWithBluetoothClassOfDevice &data, UInt8* out);
        static void Deserialize(UInt8* data, PeripheralParamPairingDeviceManagementWithBluetoothClassOfDevice &out);
    };

#pragma pack(push, 1)

    // - MUSIC_HAND_OVER_SETTING

    struct PeripheralParamMusicHandOverSetting
    {
        PeripheralBase base{Command::PERI_GET_PARAM, PeripheralInquiredType::MUSIC_HAND_OVER_SETTING};
        MessageMdrV2OnOffSettingValue isOn; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralParamMusicHandOverSetting);
    };
    static_assert(MDRIsSerializable<PeripheralParamMusicHandOverSetting>);
    // endregion PERI_RET_PARAM, PERI_SET_PARAM, PERI_NTFY_PARAM

    // endregion PERI_*_PARAM

    // region PERI_*_EXTENDED_PARAM

    enum class ConnectivityActionType : UInt8
    {
        DISCONNECT = 0x00,
        CONNECT = 0x01,
        UNPAIR = 0x02,
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
    };

    enum class SourceSwitchControlResult : UInt8
    {
        SUCCESS = 0x00,
        FAIL = 0x01,
        FAIL_CALLING = 0x02,
        FAIL_A2DP_NOT_CONNECT = 0x03,
        FAIL_GIVE_PRIORITY_TO_VOICE_ASSISTANT = 0x04,
    };

    // region PERI_SET_EXTENDED_PARAM

    struct PeripheralSetExtendedParam
    {
        static constexpr Command kResponseCommand = Command::PERI_NTFY_EXTENDED_PARAM;
        PeripheralBase base{Command::PERI_SET_EXTENDED_PARAM};

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralSetExtendedParam);
    };
    static_assert(MDRIsSerializable<PeripheralSetExtendedParam>);

    // - PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT, PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE

    struct PeripheralSetExtendedParamParingDeviceManagementCommon
    {
        static constexpr size_t kBluetoothDeviceAddressLength = 17;
        PeripheralBase base{Command::PERI_SET_EXTENDED_PARAM};
        ConnectivityActionType connectivityActionType; // 0x2
        Array<char, kBluetoothDeviceAddressLength> targetBdAddress;
        // MAC address "XX:XX:XX:XX:XX:XX" (17 bytes, no null terminator)

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralStatusPairingDeviceManagementCommon);
    };
    static_assert(MDRIsSerializable<PeripheralStatusPairingDeviceManagementCommon>);

    // - SOURCE_SWITCH_CONTROL

    struct PeripheralSetExtendedParamSourceSwitchControl
    {
        static constexpr size_t kBluetoothDeviceAddressLength = 17;
        PeripheralBase base{Command::PERI_SET_EXTENDED_PARAM, PeripheralInquiredType::SOURCE_SWITCH_CONTROL};
        Array<char, kBluetoothDeviceAddressLength> targetBdAddress;
        // MAC address "XX:XX:XX:XX:XX:XX" (17 bytes, no null terminator)

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralSetExtendedParamSourceSwitchControl);
    };
    static_assert(MDRIsSerializable<PeripheralSetExtendedParamSourceSwitchControl>);

    // endregion PERI_SET_EXTENDED_PARAM

    // region PERI_NTFY_EXTENDED_PARAM
    // - PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT, PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE

    struct PeripheralNotifyExtendedParamParingDeviceManagementCommon
    {
        static constexpr size_t kBluetoothDeviceAddressLength = 17;
        PeripheralBase base{Command::PERI_NTFY_EXTENDED_PARAM};
        ConnectivityActionType connectivityActionType; // 0x2
        PeripheralResult peripheralResult; // 0x3
        Array<char, kBluetoothDeviceAddressLength> btDeviceAddress;
        // MAC address "XX:XX:XX:XX:XX:XX" (17 bytes, no null terminator)

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralNotifyExtendedParamParingDeviceManagementCommon);
    };
    static_assert(MDRIsSerializable<PeripheralNotifyExtendedParamParingDeviceManagementCommon>);
    // - SOURCE_SWITCH_CONTROL

    struct PeripheralNotifyExtendedParamSourceSwitchControl
    {
        static constexpr size_t kBluetoothDeviceAddressLength = 17;
        PeripheralBase base{Command::PERI_NTFY_EXTENDED_PARAM, PeripheralInquiredType::SOURCE_SWITCH_CONTROL};
        SourceSwitchControlResult result; // 0x2
        Array<char, kBluetoothDeviceAddressLength>  targetBdAddress;
        // MAC address "XX:XX:XX:XX:XX:XX" (17 bytes, no null terminator)

        MDR_DEFINE_TRIVIAL_SERIALIZATION(PeripheralNotifyExtendedParamSourceSwitchControl);
    };
    static_assert(MDRIsSerializable<PeripheralNotifyExtendedParamSourceSwitchControl>);

    // endregion PERI_NTFY_EXTENDED_PARAM

    // endregion PERI_*_EXTENDED_PARAM

    // endregion PERI

    // region VOICE_GUIDANCE

    enum class VoiceGuidanceInquiredType : UInt8
    {
        // PARAM:  [RSN] VoiceGuidanceParamSettingMtk
        MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH = 0x0,
        // PARAM:  [R  ] VoiceGuidanceParamSettingSupportLangSwitch
        //         [ SN] VoiceGuidanceParamSettingMtk
        MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH = 0x1,
        // PARAM:  [RSN] VoiceGuidanceParamSettingSupportLangSwitch
        SUPPORT_LANGUAGE_SWITCH = 0x2,
        // PARAM:  [RSN] VoiceGuidanceParamSettingMtk
        ONLY_ON_OFF_SETTING = 0x3,
        // PARAM:  [R N] VoiceGuidanceParamVolume
        //         [ S ] VoiceGuidanceSetParamVolume
        VOLUME = 0x20,
        // PARAM:  [R N] VoiceGuidanceParamVolume
        //         [ S ] VoiceGuidanceSetParamVolume
        VOLUME_SETTING_FIXED_TO_5_STEPS = 0x21,
        // PARAM:  [RSN] VoiceGuidanceParamSettingOnOff
        BATTERY_LV_VOICE = 0x30,
        // PARAM:  [RSN] VoiceGuidanceParamSettingOnOff
        POWER_ONOFF_SOUND = 0x31,
        // PARAM:  [RSN] VoiceGuidanceParamSettingOnOff
        SOUNDEFFECT_ULT_BEEP_ONOFF = 0x32,
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
    };

    // region VOICE_GUIDANCE_*_CAPABILITY

    // Not implemented

    // endregion VOICE_GUIDANCE_*_CAPABILITY

    // region VOICE_GUIDANCE_*_STATUS

    // Not implemented

    // endregion VOICE_GUIDANCE_*_STATUS

    // region VOICE_GUIDANCE_*_PARAM

    // region VOICE_GUIDANCE_GET_PARAM

    struct VoiceGuidanceBase
    {
        Command command{Command::VOICE_GUIDANCE_GET_PARAM};
        VoiceGuidanceInquiredType inquiredType; // 0x1
    };
    struct VoiceGuidanceGetParam
    {
        static constexpr Command kResponseCommand = Command::VOICE_GUIDANCE_RET_PARAM;
        VoiceGuidanceBase base{Command::VOICE_GUIDANCE_GET_PARAM};

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceGetParam);
    };
    static_assert(MDRIsSerializable<VoiceGuidanceGetParam>);
    // endregion VOICE_GUIDANCE_GET_PARAM

    // region VOICE_GUIDANCE_RET_PARAM, VOICE_GUIDANCE_SET_PARAM, VOICE_GUIDANCE_NTFY_PARAM


    // - MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH, ONLY_ON_OFF_SETTING

    struct VoiceGuidanceParamSettingMtk
    {
        VoiceGuidanceBase base{Command::VOICE_GUIDANCE_GET_PARAM};
        MessageMdrV2OnOffSettingValue settingValue; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceParamSettingMtk);
    };
    static_assert(MDRIsSerializable<VoiceGuidanceParamSettingMtk>);
    // - MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH, SUPPORT_LANGUAGE_SWITCH

    struct VoiceGuidanceParamSettingSupportLangSwitch
    {
        VoiceGuidanceBase base{Command::VOICE_GUIDANCE_GET_PARAM};
        MessageMdrV2OnOffSettingValue settingValue; // 0x2
        VoiceGuidanceLanguage languageValue; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceParamSettingSupportLangSwitch);
    };
    static_assert(MDRIsSerializable<VoiceGuidanceParamSettingSupportLangSwitch>);
    // - VOLUME, VOLUME_SETTING_FIXED_TO_5_STEPS

    struct VoiceGuidanceParamVolume
    {
        VoiceGuidanceBase base{Command::VOICE_GUIDANCE_RET_PARAM};
        int8_t volumeValue; // 0x2, -2 ~ +2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceParamVolume);
    };
    static_assert(MDRIsSerializable<VoiceGuidanceParamVolume>);


    // - BATTERY_LV_VOICE, POWER_ONOFF_SOUND, SOUNDEFFECT_ULT_BEEP_ONOFF

    struct VoiceGuidanceParamSettingOnOff
    {
        VoiceGuidanceBase base{Command::VOICE_GUIDANCE_RET_PARAM};
        MessageMdrV2OnOffSettingValue settingValue; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceParamSettingOnOff);
    };
    static_assert(MDRIsSerializable<VoiceGuidanceParamSettingOnOff>);

    // endregion VOICE_GUIDANCE_RET_PARAM, VOICE_GUIDANCE_SET_PARAM, VOICE_GUIDANCE_NTFY_PARAM

    // region VOICE_GUIDANCE_SET_PARAM

    // - VOLUME, VOLUME_SETTING_FIXED_TO_5_STEPS

    struct VoiceGuidanceSetParamVolume
    {
        PeripheralBase base{Command::VOICE_GUIDANCE_SET_PARAM};
        int8_t volumeValue; // 0x2, -2 ~ +2
        MessageMdrV2OnOffSettingValue feedbackSound; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VoiceGuidanceSetParamVolume);
    };
    static_assert(MDRIsSerializable<VoiceGuidanceSetParamVolume>);
    // endregion VOICE_GUIDANCE_SET_PARAM

    // endregion VOICE_GUIDANCE_*_PARAM

    // endregion VOICE_GUIDANCE

    // region SAFE_LISTENING

    // region SAFE_LISTENING Common Enums

    enum class SafeListeningInquiredType : UInt8
    {
        SAFE_LISTENING_HBS_1 = 0x0,
        SAFE_LISTENING_TWS_1 = 0x1,
        SAFE_LISTENING_HBS_2 = 0x2,
        SAFE_LISTENING_TWS_2 = 0x3,
        SAFE_VOLUME_CONTROL = 0x4,
    };

    enum class SafeListeningErrorCause : UInt8
    {
        NOT_PLAYING = 0x0,
        IN_CALL = 0x1,
        DETACHED = 0x2,
    };
    // endregion SAFE_LISTENING Common Enums

    // region SAFE_LISTENING_*_CAPABILITY

    // region SAFE_LISTENING_GET_CAPABILITY

    struct SafeListeningGetCapability
    {
        static constexpr Command kResponseCommand = Command::SAFE_LISTENING_RET_CAPABILITY;
        Command command{Command::SAFE_LISTENING_GET_CAPABILITY}; // 0x0
        SafeListeningInquiredType inquiredType; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningGetCapability);
    };
    static_assert(MDRIsSerializable<SafeListeningGetCapability>);
    // endregion SAFE_LISTENING_GET_CAPABILITY

    // region SAFE_LISTENING_RET_CAPABILITY

    struct SafeListeningRetCapability
    {
        Command command{Command::SAFE_LISTENING_RET_CAPABILITY};
        SafeListeningInquiredType inquiredType; // 0x1
        UInt8 roundBase; // 0x2
        Int32BE timestampBase; // 0x3-0x6
        UInt8 minimumInterval; // 0x7
        UInt8 logCapacity; // 0x8

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetCapability);
    };
    static_assert(MDRIsSerializable<SafeListeningRetCapability>);
    // endregion SAFE_LISTENING_RET_CAPABILITY

    // endregion SAFE_LISTENING_*_CAPABILITY

    // region SAFE_LISTENING_*_STATUS

    // region SAFE_LISTENING_*_STATUS Common Structs and Enums

    enum class SafeListeningTargetType : UInt8
    {
        HBS = 0x00,
        TWS_L = 0x01,
        TWS_R = 0x02,
    };

    struct SafeListeningData
    {
        SafeListeningTargetType targetType; // 0x0
        Int32BE timestamp; // 0x1-0x4
        Int16BE rtcRC; // 0x5-0x6
        UInt8 viewTime; // 0x7
        Int32BE soundPressure; // 0x8-0xB
    };

    struct SafeListeningData1
    {
        SafeListeningData data;
    };

    struct SafeListeningData2
    {
        SafeListeningData data;
        UInt8 ambientTime; // 0xC
    };

    struct SafeListeningStatus
    {
        Int32BE timestamp; // 0x0-0x3
        Int16BE rtcRC; // 0x4-0x5
    };

    enum class SafeListeningLogDataStatus : UInt8
    {
        DISCONNECTED = 0x00,
        SENDING = 0x01,
        COMPLETED = 0x02,
        NOT_SENDING = 0x03,
        ERROR = 0x04,
    };

    enum class SafeListeningWHOStandardLevel : UInt8
    {
        NORMAL = 0x0,
        SENSITIVE = 0x1,
    };

    // endregion SAFE_LISTENING_*_STATUS Common Structs and Enums

    // region SAFE_LISTENING_GET_STATUS

    struct SafeListeningGetStatus
    {
        static constexpr Command kResponseCommand = Command::SAFE_LISTENING_RET_STATUS;
        Command command{Command::SAFE_LISTENING_GET_STATUS};
        SafeListeningInquiredType inquiredType; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningGetStatus);
    };
    static_assert(MDRIsSerializable<SafeListeningGetStatus>);
    // endregion SAFE_LISTENING_GET_STATUS

    // region SAFE_LISTENING_RET_STATUS

    struct SafeListeningRetStatus
    {
        Command command{Command::SAFE_LISTENING_RET_STATUS};
        SafeListeningInquiredType inquiredType; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetStatus);
    };
    static_assert(MDRIsSerializable<SafeListeningRetStatus>);
    // - SAFE_LISTENING_HBS_*, SAFE_LISTENING_TWS_*

    struct SafeListeningRetStatusSL
    {
        SafeListeningRetStatus base;

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetStatusSL);
    };
    static_assert(MDRIsSerializable<SafeListeningRetStatusSL>);
    // - SAFE_LISTENING_HBS_*

    struct SafeListeningRetStatusHbs
    {
        SafeListeningRetStatusSL base;
        SafeListeningLogDataStatus logDataStatus; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetStatusHbs);
    };
    static_assert(MDRIsSerializable<SafeListeningRetStatusHbs>);
    // - SAFE_LISTENING_HBS_1

    struct SafeListeningRetStatusHbs1
    {
        SafeListeningRetStatusHbs base;
        SafeListeningData1 currentData; // 0x3-0xF

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetStatusHbs1);
    };
    static_assert(MDRIsSerializable<SafeListeningRetStatusHbs1>);
    // - SAFE_LISTENING_HBS_2

    struct SafeListeningRetStatusHbs2
    {
        SafeListeningRetStatusHbs base;
        SafeListeningData2 currentData; // 0x3-0x10

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetStatusHbs2);
    };
    static_assert(MDRIsSerializable<SafeListeningRetStatusHbs2>);
    // - SAFE_LISTENING_TWS_*

    struct SafeListeningRetStatusTws
    {
        SafeListeningRetStatus base;
        SafeListeningLogDataStatus logDataStatusLeft; // 0x2
        SafeListeningLogDataStatus logDataStatusRight; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetStatusTws);
    };
    static_assert(MDRIsSerializable<SafeListeningRetStatusTws>);
    // - SAFE_LISTENING_TWS_1

    struct SafeListeningRetStatusTws1
    {
        SafeListeningRetStatusTws base;
        SafeListeningData1 currentDataLeft; // 0x4-0xF
        SafeListeningData1 currentDataRight; // 0x10-0x1B

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetStatusTws1);
    };
    static_assert(MDRIsSerializable<SafeListeningRetStatusTws1>);
    // - SAFE_LISTENING_TWS_2

    struct SafeListeningRetStatusTws2 : SafeListeningRetStatusTws
    {
        SafeListeningRetStatusTws base;
        SafeListeningData2 currentDataLeft; // 0x4-0x10
        SafeListeningData2 currentDataRight; // 0x11-0x1D

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetStatusTws);
    };
    static_assert(MDRIsSerializable<SafeListeningRetStatusTws2>);
    // endregion SAFE_LISTENING_RET_STATUS

    // region SAFE_LISTENING_SET_STATUS

    struct SafeListeningSetStatus
    {
        static constexpr Command kResponseCommand = Command::SAFE_LISTENING_NTFY_STATUS;
        Command command{Command::SAFE_LISTENING_SET_STATUS}; // 0x0
        SafeListeningInquiredType inquiredType; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetStatus);
    };
    static_assert(MDRIsSerializable<SafeListeningSetStatus>);

    // - SAFE_LISTENING_HBS_*, SAFE_LISTENING_TWS_*

    struct SafeListeningSetStatusSL
    {
        SafeListeningSetStatus base;

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetStatusSL);
    };
    static_assert(MDRIsSerializable<SafeListeningSetStatusSL>);
    // - SAFE_LISTENING_HBS_1, SAFE_LISTENING_HBS_2

    struct SafeListeningSetStatusHbs
    {
        SafeListeningSetStatusSL base;
        SafeListeningLogDataStatus logDataStatus; // 0x2
        SafeListeningStatus status; // 0x3-0x8

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetStatusHbs);
    };
    static_assert(MDRIsSerializable<SafeListeningSetStatusHbs>);
    // - SAFE_LISTENING_TWS_1, SAFE_LISTENING_TWS_2

    struct SafeListeningSetStatusTws
    {
        SafeListeningSetStatusSL base;
        SafeListeningLogDataStatus logDataStatusLeft; // 0x2
        SafeListeningLogDataStatus logDataStatusRight; // 0x3
        SafeListeningStatus statusLeft; // 0x4-0x9
        SafeListeningStatus statusRight; // 0xA-0xF

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetStatusTws);
    };
    static_assert(MDRIsSerializable<SafeListeningSetStatusTws>);
    // - SAFE_VOLUME_CONTROL

    struct SafeListeningSetStatusSVC
    {
        SafeListeningSetStatus base;
        SafeListeningWHOStandardLevel whoStandardLevel; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetStatusSVC);
    };
    static_assert(MDRIsSerializable<SafeListeningSetStatusSVC>);
    // endregion SAFE_LISTENING_SET_STATUS

    // region SAFE_LISTENING_NTFY_STATUS

    struct SafeListeningNotifyStatus
    {
        Command command{Command::SAFE_LISTENING_NTFY_STATUS};
        SafeListeningInquiredType inquiredType; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyStatus);
    };
    static_assert(MDRIsSerializable<SafeListeningNotifyStatus>);
    // - SAFE_LISTENING_HBS_*, SAFE_LISTENING_TWS_*

    struct SafeListeningNotifyStatusSL
    {
        SafeListeningNotifyStatus base;

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyStatusSL);
    };
    static_assert(MDRIsSerializable<SafeListeningNotifyStatusSL>);
    // - SAFE_LISTENING_HBS_1, SAFE_LISTENING_HBS_2

    struct SafeListeningNotifyStatusHbs
    {
        SafeListeningNotifyStatusSL base;
        SafeListeningLogDataStatus logDataStatus; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyStatusHbs);
    };
    static_assert(MDRIsSerializable<SafeListeningNotifyStatusHbs>);
    // - SAFE_LISTENING_HBS_1

    struct SafeListeningNotifyStatusHbs1
    {
        SafeListeningNotifyStatusHbs base;
        MDRPodArray<SafeListeningData1> data;

        static size_t Serialize(const SafeListeningNotifyStatusHbs1 &data, UInt8* out);
        static void Deserialize(UInt8* data, SafeListeningNotifyStatusHbs1 &out);
    };
    static_assert(MDRIsSerializable<SafeListeningNotifyStatusHbs1>);
    // - SAFE_LISTENING_HBS_2

    struct SafeListeningNotifyStatusHbs2
    {
        SafeListeningNotifyStatusHbs base;
        MDRPodArray<SafeListeningData2> data;

        static size_t Serialize(const SafeListeningNotifyStatusHbs2 &data, UInt8* out);
        static void Deserialize(UInt8* data, SafeListeningNotifyStatusHbs2 &out);
    };
    static_assert(MDRIsSerializable<SafeListeningNotifyStatusHbs2>);
    // - SAFE_LISTENING_TWS_1, SAFE_LISTENING_TWS_2

    struct SafeListeningNotifyStatusTws
    {
        SafeListeningNotifyStatusSL base;
        SafeListeningLogDataStatus logDataStatusLeft; // 0x2
        SafeListeningLogDataStatus logDataStatusRight; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyStatusTws);
    };
    static_assert(MDRIsSerializable<SafeListeningNotifyStatusTws>);
    // - SAFE_LISTENING_TWS_1

    struct SafeListeningNotifyStatusTws1
    {
        SafeListeningNotifyStatusTws base;
        MDRPodArray<SafeListeningData1> data;

        static size_t Serialize(const SafeListeningNotifyStatusTws1 &data, UInt8* out);
        static void Deserialize(UInt8* data, SafeListeningNotifyStatusTws1 &out);
    };
    static_assert(MDRIsSerializable<SafeListeningNotifyStatusTws1>);
    // - SAFE_LISTENING_TWS_2

    struct SafeListeningNotifyStatusTws2
    {
        SafeListeningNotifyStatusTws base;
        MDRPodArray<SafeListeningData2> data;

        static size_t Serialize(const SafeListeningNotifyStatusTws2 &data, UInt8* out);
        static void Deserialize(UInt8* data, SafeListeningNotifyStatusTws2 &out);
    };
    static_assert(MDRIsSerializable<SafeListeningNotifyStatusTws2>);

    // - SAFE_VOLUME_CONTROL

    struct SafeListeningNotifyStatusSVC
    {
        SafeListeningNotifyStatus base;
        SafeListeningWHOStandardLevel whoStandardLevel; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyStatusSVC);
    };

    // endregion SAFE_LISTENING_NTFY_STATUS

    // endregion SAFE_LISTENING_*_STATUS

    // region SAFE_LISTENING_*_PARAM

    // region SAFE_LISTENING_GET_PARAM

    struct SafeListeningGetParam
    {
        static constexpr Command kResponseCommand = Command::SAFE_LISTENING_RET_PARAM;
        Command command{Command::SAFE_LISTENING_GET_PARAM};
        SafeListeningInquiredType inquiredType; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningGetParam);
    };
    static_assert(MDRIsSerializable<SafeListeningGetParam>);

    // endregion SAFE_LISTENING_GET_PARAM

    // region SAFE_LISTENING_RET_PARAM

    struct SafeListeningRetParam
    {
        Command command{Command::SAFE_LISTENING_RET_PARAM};
        SafeListeningInquiredType inquiredType; // 0x1
        MessageMdrV2EnableDisable availability; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetParam);
    };
    static_assert(MDRIsSerializable<SafeListeningRetParam>);

    // endregion SAFE_LISTENING_RET_PARAM

    // region SAFE_LISTENING_SET_PARAM

    struct SafeListeningSetParam
    {
        static constexpr Command kResponseCommand = Command::SAFE_LISTENING_NTFY_PARAM;
        Command command{Command::SAFE_LISTENING_SET_PARAM};
        SafeListeningInquiredType inquiredType; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetParam);
    };
    static_assert(MDRIsSerializable<SafeListeningSetParam>);

    // - SAFE_LISTENING_*

    struct SafeListeningSetParamSL
    {
        SafeListeningSetParam base;
        MessageMdrV2EnableDisable safeListeningMode; // 0x2
        MessageMdrV2EnableDisable previewMode; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetParamSL);
    };
    static_assert(MDRIsSerializable<SafeListeningSetParamSL>);

    // - SAFE_VOLUME_CONTROL

    struct SafeListeningSetParamSVC
    {
        SafeListeningSetParam base;
        MessageMdrV2EnableDisable volumeLimitationMode; // 0x2
        MessageMdrV2EnableDisable safeVolumeControlMode; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningSetParamSVC);
    };
    static_assert(MDRIsSerializable<SafeListeningSetParamSVC>);

    // endregion SAFE_LISTENING_SET_PARAM

    // region SAFE_LISTENING_NTFY_PARAM

    struct SafeListeningNotifyParam
    {
        Command command{Command::SAFE_LISTENING_NTFY_PARAM};
        SafeListeningInquiredType inquiredType; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyParam);
    };
    static_assert(MDRIsSerializable<SafeListeningNotifyParam>);
    // - SAFE_LISTENING_*

    struct SafeListeningNotifyParamSL
    {
        SafeListeningNotifyParam base;
        MessageMdrV2EnableDisable safeListeningMode; // 0x2
        MessageMdrV2EnableDisable previewMode; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyParamSL);
    };
    static_assert(MDRIsSerializable<SafeListeningNotifyParamSL>);
    // - SAFE_VOLUME_CONTROL

    struct SafeListeningNotifyParamSVC
    {
        SafeListeningNotifyParam base;
        MessageMdrV2EnableDisable volumeLimitationMode; // 0x2
        MessageMdrV2EnableDisable safeVolumeControlMode; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningNotifyParamSVC);
    };
    static_assert(MDRIsSerializable<SafeListeningNotifyParamSVC>);
    // endregion SAFE_LISTENING_NTFY_PARAM

    // endregion SAFE_LISTENING_*_PARAM

    // region SAFE_LISTENING_*_EXTENDED_PARAM

    // region SAFE_LISTENING_GET_EXTENDED_PARAM

    struct SafeListeningGetExtendedParam
    {
        static constexpr Command kResponseCommand = Command::SAFE_LISTENING_RET_EXTENDED_PARAM;
        Command command{Command::SAFE_LISTENING_GET_EXTENDED_PARAM};
        SafeListeningInquiredType inquiredType; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningGetExtendedParam);
    };
    static_assert(MDRIsSerializable<SafeListeningGetExtendedParam>);
    // endregion SAFE_LISTENING_GET_EXTENDED_PARAM

    // region SAFE_LISTENING_RET_EXTENDED_PARAM

    struct SafeListeningRetExtendedParam
    {
        Command command{Command::SAFE_LISTENING_RET_EXTENDED_PARAM};
        SafeListeningInquiredType inquiredType; // 0x1
        UInt8 levelPerPeriod; // 0x2
        SafeListeningErrorCause errorCause; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SafeListeningRetExtendedParam);
    };
    static_assert(MDRIsSerializable<SafeListeningRetExtendedParam>);

    // endregion SAFE_LISTENING_RET_EXTENDED_PARAM

    // endregion SAFE_LISTENING_*_EXTENDED_PARAM

    // endregion SAFE_LISTENING

} // namespace mdr::v2::t2

#pragma pack(pop)
