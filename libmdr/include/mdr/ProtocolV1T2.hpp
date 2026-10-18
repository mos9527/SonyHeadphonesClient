#pragma once
#include "ProtocolV1.hpp"
#pragma pack(push)
#pragma pack()

// Generated from Sound Connect iOS J2ObjC metadata. Do not edit by hand.
namespace mdr::v1::t2
{
#pragma region Enums
    enum class ActionType : UInt8
    {
        DISCONNECT = 0x00,
        CONNECT = 0x01,
        UNPAIR = 0x02,
    };

    enum class Command : UInt8
    {
        PERIPHERAL_GET_CAPABILITY = 0x30,
        PERIPHERAL_RET_CAPABILITY = 0x31,
        PERIPHERAL_GET_STATUS = 0x32,
        PERIPHERAL_RET_STATUS = 0x33,
        PERIPHERAL_SET_STATUS = 0x34,
        PERIPHERAL_NTFY_STATUS = 0x35,
        PERIPHERAL_GET_PARAM = 0x36,
        PERIPHERAL_RET_PARAM = 0x37,
        PERIPHERAL_NTFY_PARAM = 0x39,
        PERIPHERAL_SET_EX_PARAM = 0x3C,
        PERIPHERAL_NTFY_EX_PARAM = 0x3D,
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
        UNKNOWN = 0xFF,
    };

    enum class DetailedDataType : UInt8
    {
        NO_USE = 0x00,
        ON_OFF = 0x01,
        LANGUAGE = 0x02,
        REQUIRED_TIME = 0x03,
        DOWNLOAD_SERVER_METHOD = 0x04,
        UPDATE_METHOD = 0x05,
    };

    enum class DownloadServerMethod : UInt8
    {
        NO_USE = 0x00,
        AUTOMAGIC = 0x01,
    };

    enum class FileTransferInMultiConnection : UInt8
    {
        POSSIBLE = 0x00,
        IMPOSSIBLE = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class PeripheralBluetoothModeStatus : UInt8
    {
        NORMAL_MODE = 0x00,
        INQUIRY_SCAN_MODE = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class PeripheralDetailDataType : UInt8
    {
        CONNECTION_CONTROL = 0x00,
    };

    enum class PeripheralInquiredType : UInt8
    {
        NO_USE = 0x00,
        PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT = 0x01,
    };

    enum class ResultType : UInt8
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

    enum class StatusType : UInt8
    {
        NO_USE = 0x00,
        ON_OFF = 0x01,
        LANGUAGE = 0x02,
    };

    enum class SupportsSwitch : UInt8
    {
        NOT_SUPPORT = 0x00,
        SUPPORT = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class VoiceGuidanceInquiredType : UInt8
    {
        NO_USE = 0x00,
        VOICE_GUIDANCE_SETTING = 0x01,
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
        BRAZILIAN_PORTUGUESE = 0x0C,
        KOREAN = 0x0D,
        TURKISH = 0x0E,
        CHINESE = 0x0F,
    };

    enum class VoiceGuidanceSettingValue : UInt8
    {
        OFF = 0x00,
        ON = 0x01,
        OUT_OF_RANGE = 0xFF,
    };
#pragma endregion Enums

#pragma region Declarations

    // THMSGV1T2GetPeripheralCapability
#pragma pack(push, 1)
    struct GetPeripheralCapability
    {
        // CODEGEN EnumRange Command::PERIPHERAL_GET_CAPABILITY
        Command command{Command::PERIPHERAL_GET_CAPABILITY}; // 0x0
        PeripheralInquiredType inquiredType{PeripheralInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetPeripheralCapability);
    };
#pragma pack(pop)

    // THMSGV1T2GetPeripheralParam
#pragma pack(push, 1)
    struct GetPeripheralParam
    {
        // CODEGEN EnumRange Command::PERIPHERAL_GET_PARAM
        Command command{Command::PERIPHERAL_GET_PARAM}; // 0x0
        PeripheralInquiredType inquiredType{PeripheralInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetPeripheralParam);
    };
#pragma pack(pop)

    // THMSGV1T2GetPeripheralStatus
#pragma pack(push, 1)
    struct GetPeripheralStatus
    {
        // CODEGEN EnumRange Command::PERIPHERAL_GET_STATUS
        Command command{Command::PERIPHERAL_GET_STATUS}; // 0x0
        PeripheralInquiredType inquiredType{PeripheralInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetPeripheralStatus);
    };
#pragma pack(pop)

    // THMSGV1T2GetVoiceGuidanceCapability
#pragma pack(push, 1)
    struct GetVoiceGuidanceCapability
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_GET_CAPABILITY
        Command command{Command::VOICE_GUIDANCE_GET_CAPABILITY}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetVoiceGuidanceCapability);
    };
#pragma pack(pop)

    // THMSGV1T2GetVoiceGuidanceParam
#pragma pack(push, 1)
    struct GetVoiceGuidanceParam
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_GET_PARAM
        Command command{Command::VOICE_GUIDANCE_GET_PARAM}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1
        DetailedDataType detailedDataType{DetailedDataType::NO_USE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetVoiceGuidanceParam);
    };
#pragma pack(pop)

    // THMSGV1T2GetVoiceGuidanceStatus
#pragma pack(push, 1)
    struct GetVoiceGuidanceStatus
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_GET_STATUS
        Command command{Command::VOICE_GUIDANCE_GET_STATUS}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1
        StatusType statusType{StatusType::NO_USE}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(GetVoiceGuidanceStatus);
    };
#pragma pack(pop)

    // THMSGV1T2NotifyPeripheralExParam
#pragma pack(push, 1)
    struct NotifyPeripheralExParam
    {
        // CODEGEN EnumRange Command::PERIPHERAL_NTFY_EX_PARAM
        Command command{Command::PERIPHERAL_NTFY_EX_PARAM}; // 0x0
        PeripheralInquiredType settingType{PeripheralInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyPeripheralExParam);
    };
#pragma pack(pop)

    // THMSGV1T2NotifyPeripheralExParamPairingDeviceManagementClassicBt
#pragma pack(push, 1)
    struct NotifyPeripheralExParamPairingDeviceManagementClassicBt
    {
        // CODEGEN EnumRange Command::PERIPHERAL_NTFY_EX_PARAM
        Command command{Command::PERIPHERAL_NTFY_EX_PARAM}; // 0x0
        // CODEGEN EnumRange PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
        PeripheralInquiredType settingType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1
        PeripheralDetailDataType detailType{PeripheralDetailDataType::CONNECTION_CONTROL}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyPeripheralExParamPairingDeviceManagementClassicBt);
    };
#pragma pack(pop)

    // THMSGV1T2NotifyPeripheralExParamPairingDeviceManagementClassicBtConnectionControl
    struct NotifyPeripheralExParamPairingDeviceManagementClassicBtConnectionControl
    {
        // CODEGEN EnumRange Command::PERIPHERAL_NTFY_EX_PARAM
        Command command{Command::PERIPHERAL_NTFY_EX_PARAM}; // 0x0
        PeripheralDetailDataType detailType{PeripheralDetailDataType::CONNECTION_CONTROL}; // 0x1
        ActionType actionType{ActionType::DISCONNECT}; // 0x2
        ResultType resultType{ResultType::DISCONNECTION_SUCCESS}; // 0x3
        Array<UInt8, 17> btDeviceAddress{}; // 0x4

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifyPeripheralExParamPairingDeviceManagementClassicBtConnectionControl);
    };

    // THMSGV1T2NotifyPeripheralParam
#pragma pack(push, 1)
    struct NotifyPeripheralParam
    {
        // CODEGEN EnumRange Command::PERIPHERAL_NTFY_PARAM
        Command command{Command::PERIPHERAL_NTFY_PARAM}; // 0x0
        PeripheralInquiredType inquiredType{PeripheralInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyPeripheralParam);
    };
#pragma pack(pop)

    // THMSGV1T2NotifyPeripheralStatus
#pragma pack(push, 1)
    struct NotifyPeripheralStatus
    {
        // CODEGEN EnumRange Command::PERIPHERAL_NTFY_STATUS
        Command command{Command::PERIPHERAL_NTFY_STATUS}; // 0x0
        PeripheralInquiredType inquiredType{PeripheralInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyPeripheralStatus);
    };
#pragma pack(pop)

    // THMSGV1T2NotifyPeripheralStatusPairingDeviceManagementClassicBt
#pragma pack(push, 1)
    struct NotifyPeripheralStatusPairingDeviceManagementClassicBt
    {
        // CODEGEN EnumRange Command::PERIPHERAL_NTFY_STATUS
        Command command{Command::PERIPHERAL_NTFY_STATUS}; // 0x0
        // CODEGEN EnumRange PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralBluetoothModeStatus bluetoothModeStatus{PeripheralBluetoothModeStatus::NORMAL_MODE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyPeripheralStatusPairingDeviceManagementClassicBt);
    };
#pragma pack(pop)

    // THMSGV1T2NotifyVoiceGuidanceParam
#pragma pack(push, 1)
    struct NotifyVoiceGuidanceParam
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_NTFY_PARAM
        Command command{Command::VOICE_GUIDANCE_NTFY_PARAM}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyVoiceGuidanceParam);
    };
#pragma pack(pop)

    // THMSGV1T2NotifyVoiceGuidanceParamSettingOnOff
#pragma pack(push, 1)
    struct NotifyVoiceGuidanceParamSettingOnOff
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_NTFY_PARAM
        Command command{Command::VOICE_GUIDANCE_NTFY_PARAM}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1
        // CODEGEN EnumRange DetailedDataType::ON_OFF
        DetailedDataType detailedDataType{DetailedDataType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceSettingValue settingValue{VoiceGuidanceSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyVoiceGuidanceParamSettingOnOff);
    };
#pragma pack(pop)

    // THMSGV1T2NotifyVoiceGuidanceStatus
#pragma pack(push, 1)
    struct NotifyVoiceGuidanceStatus
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_NTFY_STATUS
        Command command{Command::VOICE_GUIDANCE_NTFY_STATUS}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyVoiceGuidanceStatus);
    };
#pragma pack(pop)

    // THMSGV1T2NotifyVoiceGuidanceStatusSettingLanguage
#pragma pack(push, 1)
    struct NotifyVoiceGuidanceStatusSettingLanguage
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_NTFY_STATUS
        Command command{Command::VOICE_GUIDANCE_NTFY_STATUS}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1
        // CODEGEN EnumRange StatusType::LANGUAGE
        StatusType statusType{StatusType::LANGUAGE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyVoiceGuidanceStatusSettingLanguage);
    };
#pragma pack(pop)

    // THMSGV1T2NotifyVoiceGuidanceStatusSettingOnOff
#pragma pack(push, 1)
    struct NotifyVoiceGuidanceStatusSettingOnOff
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_NTFY_STATUS
        Command command{Command::VOICE_GUIDANCE_NTFY_STATUS}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1
        // CODEGEN EnumRange StatusType::ON_OFF
        StatusType statusType{StatusType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NotifyVoiceGuidanceStatusSettingOnOff);
    };
#pragma pack(pop)

    // THMSGV1T2Payload
    struct Payload
    {
        Command command2{Command::PERIPHERAL_GET_CAPABILITY}; // 0x0

        MDR_DEFINE_EXTERN_READ_WRITE(Payload);
    };

    // THMSGV1T2PeripheralDeviceInfo
    struct PeripheralDeviceInfo
    {
        Array<UInt8, 17> btDeviceAddress; // 0x0
        UInt8 connectedStatus{};
        MDRPrefixedString btFriendlyName;

        MDR_DEFINE_EXTERN_READ_WRITE(PeripheralDeviceInfo);
    };

    // THMSGV1T2RetPeripheralCapability
#pragma pack(push, 1)
    struct RetPeripheralCapability
    {
        // CODEGEN EnumRange Command::PERIPHERAL_RET_CAPABILITY
        Command command{Command::PERIPHERAL_RET_CAPABILITY}; // 0x0
        PeripheralInquiredType inquiredType{PeripheralInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetPeripheralCapability);
    };
#pragma pack(pop)

    // THMSGV1T2RetPeripheralCapabilityPairingDeviceManagementClassicBt
#pragma pack(push, 1)
    struct RetPeripheralCapabilityPairingDeviceManagementClassicBt
    {
        // CODEGEN EnumRange Command::PERIPHERAL_RET_CAPABILITY
        Command command{Command::PERIPHERAL_RET_CAPABILITY}; // 0x0
        // CODEGEN EnumRange PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1
        // CODEGEN Range 0 255
        UInt8 maxOfPairedDevice{}; // 0x2
        // CODEGEN Range 0 255
        UInt8 maxOfConnectedDevice{}; // 0x3
        // CODEGEN Ignore OUT_OF_RANGE is expected
        FileTransferInMultiConnection fileTransferInMultiConnection{FileTransferInMultiConnection::POSSIBLE}; // 0x4

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetPeripheralCapabilityPairingDeviceManagementClassicBt);
    };
#pragma pack(pop)

    // THMSGV1T2RetPeripheralParam
#pragma pack(push, 1)
    struct RetPeripheralParam
    {
        // CODEGEN EnumRange Command::PERIPHERAL_RET_PARAM
        Command command{Command::PERIPHERAL_RET_PARAM}; // 0x0
        PeripheralInquiredType inquiredType{PeripheralInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetPeripheralParam);
    };
#pragma pack(pop)

    // THMSGV1T2RetPeripheralStatus
#pragma pack(push, 1)
    struct RetPeripheralStatus
    {
        // CODEGEN EnumRange Command::PERIPHERAL_RET_STATUS
        Command command{Command::PERIPHERAL_RET_STATUS}; // 0x0
        PeripheralInquiredType inquiredType{PeripheralInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralBluetoothModeStatus bluetoothModeStatus{PeripheralBluetoothModeStatus::NORMAL_MODE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetPeripheralStatus);
    };
#pragma pack(pop)

    // THMSGV1T2RetVoiceGuidanceCapability
    struct RetVoiceGuidanceCapability
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_CAPABILITY
        Command command{Command::VOICE_GUIDANCE_RET_CAPABILITY}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType inquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SupportsSwitch supportsOnOffSwitching{SupportsSwitch::NOT_SUPPORT}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        SupportsSwitch supportsLanguageSwitching{SupportsSwitch::NOT_SUPPORT}; // 0x3
        MDRPodArray<VoiceGuidanceLanguage> supportLanguages; // 0x4

        MDR_DEFINE_EXTERN_SERIALIZATION(RetVoiceGuidanceCapability);
    };

    // THMSGV1T2RetVoiceGuidanceParam
#pragma pack(push, 1)
    struct RetVoiceGuidanceParam
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetVoiceGuidanceParam);
    };
#pragma pack(pop)

    // THMSGV1T2RetVoiceGuidanceParamSettingDownloadServerMethodAutomagic_ServiceInformation
    struct RetVoiceGuidanceParamSettingDownloadServerMethodAutomagic_ServiceInformation
    {
        VoiceGuidanceLanguage language{VoiceGuidanceLanguage::UNDEFINED_LANGUAGE}; // 0x0
        MDRPrefixedString serviceId; // 0x1

        MDR_DEFINE_EXTERN_READ_WRITE(RetVoiceGuidanceParamSettingDownloadServerMethodAutomagic_ServiceInformation);
    };

    // THMSGV1T2RetVoiceGuidanceParamSettingLanguage
#pragma pack(push, 1)
    struct RetVoiceGuidanceParamSettingLanguage
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1
        // CODEGEN EnumRange DetailedDataType::LANGUAGE
        DetailedDataType detailedDataType{DetailedDataType::LANGUAGE}; // 0x2
        VoiceGuidanceLanguage currentLangauge{VoiceGuidanceLanguage::UNDEFINED_LANGUAGE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetVoiceGuidanceParamSettingLanguage);
    };
#pragma pack(pop)

    // THMSGV1T2RetVoiceGuidanceParamSettingOnOff
#pragma pack(push, 1)
    struct RetVoiceGuidanceParamSettingOnOff
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1
        // CODEGEN EnumRange DetailedDataType::ON_OFF
        DetailedDataType detailedDataType{DetailedDataType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceSettingValue settingValue{VoiceGuidanceSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetVoiceGuidanceParamSettingOnOff);
    };
#pragma pack(pop)

    // THMSGV1T2RetVoiceGuidanceParamSettingRequiredTime
#pragma pack(push, 1)
    struct RetVoiceGuidanceParamSettingRequiredTime
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1
        // CODEGEN EnumRange DetailedDataType::REQUIRED_TIME
        DetailedDataType detailedDataType{DetailedDataType::REQUIRED_TIME}; // 0x2
        UInt8 requiredTime{}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetVoiceGuidanceParamSettingRequiredTime);
    };
#pragma pack(pop)

    // THMSGV1T2RetVoiceGuidanceParamSettingUpdateMethod
#pragma pack(push, 1)
    struct RetVoiceGuidanceParamSettingUpdateMethod
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        UpdateMethod updateMethod{UpdateMethod::TANDEM_METHOD}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetVoiceGuidanceParamSettingUpdateMethod);
    };
#pragma pack(pop)

    // THMSGV1T2RetVoiceGuidanceParamSettingUpdateMethodCsr
#pragma pack(push, 1)
    struct RetVoiceGuidanceParamSettingUpdateMethodCsr
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1
        // CODEGEN EnumRange DetailedDataType::UPDATE_METHOD
        DetailedDataType detailedDataType{DetailedDataType::UPDATE_METHOD}; // 0x2
        UpdateMethod updateMethod{UpdateMethod::TANDEM_METHOD}; // 0x3
        UInt8 bleTxPower{}; // 0x4
        UInt8 batteryPowerThresh{}; // 0x5

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetVoiceGuidanceParamSettingUpdateMethodCsr);
    };
#pragma pack(pop)

    // THMSGV1T2RetVoiceGuidanceParamSettingUpdateMethodMtk
    struct RetVoiceGuidanceParamSettingUpdateMethodMtk
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1
        // CODEGEN EnumRange DetailedDataType::UPDATE_METHOD
        DetailedDataType detailedDataType{DetailedDataType::UPDATE_METHOD}; // 0x2
        UpdateMethod updateMethod{UpdateMethod::TANDEM_METHOD}; // 0x3
        UInt8 batteryPowerThresh{}; // 0x4
        UInt8 batteryPowerThreshForInterrupting{}; // 0x5
        MDRPrefixedString uniqueId; // 0x6

        MDR_DEFINE_EXTERN_SERIALIZATION(RetVoiceGuidanceParamSettingUpdateMethodMtk);
    };

    // THMSGV1T2RetVoiceGuidanceStatus
#pragma pack(push, 1)
    struct RetVoiceGuidanceStatus
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_STATUS
        Command command{Command::VOICE_GUIDANCE_RET_STATUS}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetVoiceGuidanceStatus);
    };
#pragma pack(pop)

    // THMSGV1T2RetVoiceGuidanceStatusSettingLanguage
#pragma pack(push, 1)
    struct RetVoiceGuidanceStatusSettingLanguage
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_STATUS
        Command command{Command::VOICE_GUIDANCE_RET_STATUS}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1
        // CODEGEN EnumRange StatusType::LANGUAGE
        StatusType statusType{StatusType::LANGUAGE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetVoiceGuidanceStatusSettingLanguage);
    };
#pragma pack(pop)

    // THMSGV1T2RetVoiceGuidanceStatusSettingOnOff
#pragma pack(push, 1)
    struct RetVoiceGuidanceStatusSettingOnOff
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_STATUS
        Command command{Command::VOICE_GUIDANCE_RET_STATUS}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1
        // CODEGEN EnumRange StatusType::ON_OFF
        StatusType statusType{StatusType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(RetVoiceGuidanceStatusSettingOnOff);
    };
#pragma pack(pop)

    // THMSGV1T2SetPeripheralExParam
#pragma pack(push, 1)
    struct SetPeripheralExParam
    {
        // CODEGEN EnumRange Command::PERIPHERAL_SET_EX_PARAM
        Command command{Command::PERIPHERAL_SET_EX_PARAM}; // 0x0
        PeripheralInquiredType settingType{PeripheralInquiredType::NO_USE}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetPeripheralExParam);
    };
#pragma pack(pop)

    // THMSGV1T2SetPeripheralExParamPairingDeviceManagementClassicBt
#pragma pack(push, 1)
    struct SetPeripheralExParamPairingDeviceManagementClassicBt
    {
        // CODEGEN EnumRange Command::PERIPHERAL_SET_EX_PARAM
        Command command{Command::PERIPHERAL_SET_EX_PARAM}; // 0x0
        // CODEGEN EnumRange PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
        PeripheralInquiredType settingType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1
        PeripheralDetailDataType detailType{PeripheralDetailDataType::CONNECTION_CONTROL}; // 0x2

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetPeripheralExParamPairingDeviceManagementClassicBt);
    };
#pragma pack(pop)

    // THMSGV1T2SetPeripheralExParamPairingDeviceManagementClassicBtConnectionControl
    struct SetPeripheralExParamPairingDeviceManagementClassicBtConnectionControl
    {
        // CODEGEN EnumRange Command::PERIPHERAL_SET_EX_PARAM
        Command command{Command::PERIPHERAL_SET_EX_PARAM}; // 0x0
        PeripheralDetailDataType detailType{PeripheralDetailDataType::CONNECTION_CONTROL}; // 0x1
        ResultType resultType{ResultType::DISCONNECTION_SUCCESS}; // 0x2
        ActionType actionType{ActionType::DISCONNECT}; // 0x3
        Array<UInt8, 17> btDeviceAddress{}; // 0x4

        MDR_DEFINE_EXTERN_SERIALIZATION(SetPeripheralExParamPairingDeviceManagementClassicBtConnectionControl);
    };

    // THMSGV1T2SetPeripheralStatus
#pragma pack(push, 1)
    struct SetPeripheralStatus
    {
        // CODEGEN EnumRange Command::PERIPHERAL_SET_STATUS
        Command command{Command::PERIPHERAL_SET_STATUS}; // 0x0
        PeripheralInquiredType inquiredType{PeripheralInquiredType::NO_USE}; // 0x1
        // CODEGEN Ignore OUT_OF_RANGE is expected
        PeripheralBluetoothModeStatus bluetoothModeStatus{PeripheralBluetoothModeStatus::NORMAL_MODE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetPeripheralStatus);
    };
#pragma pack(pop)

    // THMSGV1T2SetVoiceGuidanceParam
#pragma pack(push, 1)
    struct SetVoiceGuidanceParam
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_SET_PARAM
        Command command{Command::VOICE_GUIDANCE_SET_PARAM}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetVoiceGuidanceParam);
    };
#pragma pack(pop)

    // THMSGV1T2SetVoiceGuidanceParamSettingOnOff
#pragma pack(push, 1)
    struct SetVoiceGuidanceParamSettingOnOff
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_SET_PARAM
        Command command{Command::VOICE_GUIDANCE_SET_PARAM}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1
        // CODEGEN EnumRange DetailedDataType::ON_OFF
        DetailedDataType detailedDataType{DetailedDataType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        VoiceGuidanceSettingValue settingValue{VoiceGuidanceSettingValue::OFF}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetVoiceGuidanceParamSettingOnOff);
    };
#pragma pack(pop)

    // THMSGV1T2SetVoiceGuidanceStatus
#pragma pack(push, 1)
    struct SetVoiceGuidanceStatus
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_SET_STATUS
        Command command{Command::VOICE_GUIDANCE_SET_STATUS}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetVoiceGuidanceStatus);
    };
#pragma pack(pop)

    // THMSGV1T2SetVoiceGuidanceStatusSettingLanguage
#pragma pack(push, 1)
    struct SetVoiceGuidanceStatusSettingLanguage
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_SET_STATUS
        Command command{Command::VOICE_GUIDANCE_SET_STATUS}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1
        // CODEGEN EnumRange StatusType::LANGUAGE
        StatusType statusType{StatusType::LANGUAGE}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetVoiceGuidanceStatusSettingLanguage);
    };
#pragma pack(pop)

    // THMSGV1T2SetVoiceGuidanceStatusSettingOnOff
#pragma pack(push, 1)
    struct SetVoiceGuidanceStatusSettingOnOff
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_SET_STATUS
        Command command{Command::VOICE_GUIDANCE_SET_STATUS}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1
        // CODEGEN EnumRange StatusType::ON_OFF
        StatusType statusType{StatusType::ON_OFF}; // 0x2
        // CODEGEN Ignore OUT_OF_RANGE is expected
        CommonStatus status{CommonStatus::ENABLE}; // 0x3

        MDR_DEFINE_TRIVIAL_SERIALIZATION(SetVoiceGuidanceStatusSettingOnOff);
    };
#pragma pack(pop)

    // THMSGV1T2UnknownCommand
#pragma pack(push, 1)
    struct UnknownCommand
    {
        // CODEGEN EnumRange Command::UNKNOWN
        Command command{Command::UNKNOWN}; // 0x0

        MDR_DEFINE_TRIVIAL_SERIALIZATION(UnknownCommand);
    };
#pragma pack(pop)

    // THMSGV1T2NotifyPeripheralParamPairingDeviceManagementClassicBt
    struct NotifyPeripheralParamPairingDeviceManagementClassicBt
    {
        // CODEGEN EnumRange Command::PERIPHERAL_NTFY_PARAM
        Command command{Command::PERIPHERAL_NTFY_PARAM}; // 0x0
        // CODEGEN EnumRange PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1
        MDRArray<PeripheralDeviceInfo> deviceInfo; // 0x2
        // CODEGEN Range 0 255
        UInt8 playbackrightDevice{};

        MDR_DEFINE_EXTERN_SERIALIZATION(NotifyPeripheralParamPairingDeviceManagementClassicBt);
    };

    // THMSGV1T2RetPeripheralParamPairingDeviceManagementClassicBt
    struct RetPeripheralParamPairingDeviceManagementClassicBt
    {
        // CODEGEN EnumRange Command::PERIPHERAL_RET_PARAM
        Command command{Command::PERIPHERAL_RET_PARAM}; // 0x0
        // CODEGEN EnumRange PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
        PeripheralInquiredType inquiredType{PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT}; // 0x1
        MDRArray<PeripheralDeviceInfo> deviceInfo; // 0x2
        // CODEGEN Range 0 255
        UInt8 playbackrightDevice{};

        MDR_DEFINE_EXTERN_SERIALIZATION(RetPeripheralParamPairingDeviceManagementClassicBt);
    };

    // THMSGV1T2RetVoiceGuidanceParamSettingDownloadServerMethodAutomagic
    struct RetVoiceGuidanceParamSettingDownloadServerMethodAutomagic
    {
        // CODEGEN EnumRange Command::VOICE_GUIDANCE_RET_PARAM
        Command command{Command::VOICE_GUIDANCE_RET_PARAM}; // 0x0
        // CODEGEN EnumRange VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING
        VoiceGuidanceInquiredType voiceGuidanceInquiredType{VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING}; // 0x1
        // CODEGEN EnumRange DetailedDataType::DOWNLOAD_SERVER_METHOD
        DetailedDataType detailedDataType{DetailedDataType::DOWNLOAD_SERVER_METHOD}; // 0x2
        DownloadServerMethod downloadServerMethod{DownloadServerMethod::NO_USE}; // 0x3
        MDRPrefixedString categoryId; // 0x4
        MDRPrefixedString serialNumber;
        MDRArray<RetVoiceGuidanceParamSettingDownloadServerMethodAutomagic_ServiceInformation> serviceInformationList;

        MDR_DEFINE_EXTERN_SERIALIZATION(RetVoiceGuidanceParamSettingDownloadServerMethodAutomagic);
    };
#pragma endregion Declarations
} // namespace mdr::v1::t2

#pragma pack(pop)

#include "Generated/ProtocolV1T2Enum.hpp"
#include "Generated/ProtocolV1T2Traits.hpp"
