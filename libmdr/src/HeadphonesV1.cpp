#include <algorithm>
#include "Details.hpp"

namespace mdr
{
    using namespace v1;

    void MDRHeadphones::RefreshSupportV1()
    {
        auto& state = mDetailsV1;
        state.mProtocol.hasTable2 =
            state.mSupport.contains(t1::FunctionType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT) ||
            state.mSupport.contains(t1::FunctionType::VOICE_GUIDANCE);
        if (!state.mSupport.contains(t1::FunctionType::NC_OPTIMIZER))
            state.mNCOptimizer = {};
    }

    namespace
    {
        UInt8 V1NcValueForAmbientLevel(int level)
        {
            if (level <= -1)
                return static_cast<UInt8>(t1::NcDualSingleValue::DUAL); // Noise Cancelling
            if (level == 0)
                return static_cast<UInt8>(t1::NcDualSingleValue::SINGLE); // Wind Noise Reduction
            return static_cast<UInt8>(t1::NcDualSingleValue::OFF);
        }

        bool SkipV1String(Span<const UInt8>& data)
        {
            if (data.empty()) return false;
            const size_t length = data[0];
            if (length > data.size() - 1) return false;
            data = data.subspan(1 + length);
            return true;
        }

        template<class T>
        bool AcceptV1FixedResponse(Span<const UInt8> data)
        {
            return static_cast<bool>((T::Deserialize)(data.data(), data.size()));
        }
    }

    bool MDRHeadphones::AcceptInitResponseV1(MDRDataType table, Span<const UInt8> cmd)
    {
        if (cmd.size() < 2) return false;
        if (table == MDRDataType::DATA_MDR_NO2)
        {
            switch (static_cast<t2::Command>(cmd[0]))
            {
            case t2::Command::PERIPHERAL_RET_CAPABILITY:
                return AcceptV1FixedResponse<t2::RetPeripheralCapabilityPairingDeviceManagementClassicBt>(cmd);
            case t2::Command::VOICE_GUIDANCE_RET_CAPABILITY:
                if (cmd.size() < 4 || cmd[2] > 1 || cmd[3] > 1) return false;
                return cmd[3] == 0 ? cmd.size() == 4
                    : cmd.size() >= 5 && cmd.size() == static_cast<size_t>(cmd[4]) + 5;
            case t2::Command::VOICE_GUIDANCE_RET_STATUS:
                if (cmd.size() < 3) return false;
                if (static_cast<t2::StatusType>(cmd[2]) == t2::StatusType::LANGUAGE)
                    return AcceptV1FixedResponse<t2::RetVoiceGuidanceStatusSettingLanguage>(cmd);
                return true; // ON_OFF is decoded by its state handler.
            case t2::Command::VOICE_GUIDANCE_RET_PARAM:
                break;
            default:
                return true;
            }
            if (cmd.size() < 3 ||
                static_cast<t2::VoiceGuidanceInquiredType>(cmd[1]) != t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING)
                return false;
            switch (static_cast<t2::DetailedDataType>(cmd[2]))
            {
            case t2::DetailedDataType::ON_OFF:
                return true; // Decoded once by the state handler.
            case t2::DetailedDataType::LANGUAGE:
                return AcceptV1FixedResponse<t2::RetVoiceGuidanceParamSettingLanguage>(cmd);
            case t2::DetailedDataType::REQUIRED_TIME:
                return AcceptV1FixedResponse<t2::RetVoiceGuidanceParamSettingRequiredTime>(cmd);
            case t2::DetailedDataType::DOWNLOAD_SERVER_METHOD:
            {
                if (cmd.size() < 4 || static_cast<t2::DownloadServerMethod>(cmd[3]) != t2::DownloadServerMethod::AUTOMAGIC)
                    return false;
                auto data = cmd.subspan(4);
                if (!SkipV1String(data) || !SkipV1String(data) || data.empty()) return false;
                const UInt8 count = data[0];
                data = data.subspan(1);
                for (UInt8 i = 0; i < count; ++i)
                {
                    t2::VoiceGuidanceLanguage language{};
                    if (!detail::ReadEnumTag(data, language, 0)) return false;
                    data = data.subspan(1);
                    if (!SkipV1String(data)) return false;
                }
                return true;
            }
            case t2::DetailedDataType::UPDATE_METHOD:
            {
                if (cmd.size() < 4) return false;
                const auto method = static_cast<UpdateMethod>(cmd[3]);
                const UInt8 module = static_cast<UInt8>(method) & 0xF0;
                if (!is_valid(method)) return false;
                if (module == 0x10)
                    return AcceptV1FixedResponse<t2::RetVoiceGuidanceParamSettingUpdateMethodCsr>(cmd);
                if (module != 0x20 || cmd.size() < 8) return false;
                auto data = cmd.subspan(6);
                return SkipV1String(data);
            }
            default:
                return false;
            }
        }
        if (table != MDRDataType::DATA_MDR) return false;
        switch (static_cast<t1::Command>(cmd[0]))
        {
        case t1::Command::VPT_RET_CAPABILITY:
            if (static_cast<t1::VptInquiredType>(cmd[1]) == t1::VptInquiredType::SOUND_POSITION)
            {
                auto result = (t1::RetVptCapability_SoundPositionCapability::Deserialize)(cmd.data(), cmd.size());
                return result && result.value.soundPositionType == t1::SoundPositionType::TYPE1;
            }
            if (static_cast<t1::VptInquiredType>(cmd[1]) != t1::VptInquiredType::VPT || cmd.size() < 3)
                return false;
            {
                auto data = cmd.subspan(3);
                for (UInt8 i = 0; i < cmd[2]; ++i)
                {
                    t1::VptPresetId preset{};
                    if (!detail::ReadEnumTag(data, preset, 0)) return false;
                    data = data.subspan(1);
                    if (!SkipV1String(data)) return false;
                }
                return true;
            }
        case t1::Command::SPORTS_RET_CAPABILITY:
        {
            auto result = (t1::RetSportsCapability::Deserialize)(cmd.data(), cmd.size());
            return result && (result.value.trainingCapability.availableEffectType == t1::TrainingModeAvailableEffectType::TYPE1 ||
                result.value.trainingCapability.availableEffectType == t1::TrainingModeAvailableEffectType::TYPE2);
        }
        case t1::Command::PLAY_RET_CAPABILITY:
        {
            auto result = (t1::RetPlayCapability::Deserialize)(cmd.data(), cmd.size());
            return result && is_valid(result.value.playbackControlType) && is_valid(result.value.metaDataDisplayType) &&
                result.value.playbackControlType != t1::PlaybackControlType::OUT_OF_RANGE &&
                result.value.metaDataDisplayType != t1::MetaDataDisplayType::OUT_OF_RANGE;
        }
        case t1::Command::COMMON_RET_BLUETOOTH_DEVICE_INFO:
        {
            if (cmd.size() < 3) return false;
            const size_t length = cmd[2];
            return length > 0 && length <= cmd.size() - 3;
        }
        case t1::Command::CONNECT_RET_DEVICE_INFO:
            if (static_cast<t1::DeviceInfoInquiredType>(cmd[1]) == t1::DeviceInfoInquiredType::INSTRUCTION_GUIDE)
            {
                if (cmd.size() < 3 || cmd[2] > cmd.size() - 3) return false;
                for (size_t i = 0; i < cmd[2]; ++i)
                    if (!is_valid(static_cast<t1::GuidanceCategory>(cmd[3 + i]))) return false;
                return true;
            }
            return true;
        case t1::Command::VPT_RET_STATUS:
            return AcceptV1FixedResponse<t1::RetVptStatus>(cmd);
        case t1::Command::VPT_RET_PARAM:
            if (static_cast<t1::VptInquiredType>(cmd[1]) == t1::VptInquiredType::VPT)
                return AcceptV1FixedResponse<t1::RetVptParamVptParam>(cmd);
            if (static_cast<t1::VptInquiredType>(cmd[1]) == t1::VptInquiredType::SOUND_POSITION)
                return AcceptV1FixedResponse<t1::RetVptParamSoundPositionParam>(cmd);
            return false;
        case t1::Command::UPDT_RET_PARAM:
            switch (static_cast<t1::UpdateInquiredType>(cmd[1]))
            {
            case t1::UpdateInquiredType::BLE_TX_POWER:
                return AcceptV1FixedResponse<t1::RetUpdateParamUpdateBleTxPowerParam>(cmd);
            case t1::UpdateInquiredType::BATTERY_POWER_THRESHOLD:
                return AcceptV1FixedResponse<t1::RetUpdateParamUpdateBatteryPowerThresholdParam>(cmd);
            case t1::UpdateInquiredType::UPDATE_METHOD:
                return AcceptV1FixedResponse<t1::RetUpdateParamUpdateMethodParam>(cmd);
            case t1::UpdateInquiredType::BATTERY_POWER_THRESHOLD_FOR_INTERRUPTIONG_FW_UPDATE:
                return AcceptV1FixedResponse<t1::RetUpdateParamUpdateBatteryPowerThresholdForInterruptiongUpdtParam>(cmd);
            case t1::UpdateInquiredType::UNIQUE_ID_FOR_DEVICE_BINDING:
            case t1::UpdateInquiredType::CATEGORY_ID:
            case t1::UpdateInquiredType::SERVICE_ID:
            case t1::UpdateInquiredType::NATION_CODE:
            case t1::UpdateInquiredType::LANGUAGE:
            case t1::UpdateInquiredType::SERIAL_NUMBER:
            {
                auto data = cmd.subspan(2);
                return SkipV1String(data);
            }
            default:
                return false;
            }
        case t1::Command::SPORTS_RET_STATUS:
            return AcceptV1FixedResponse<t1::RetSportsStatusTrainingModeStatus>(cmd);
        case t1::Command::SPORTS_RET_PARAM:
            return AcceptV1FixedResponse<t1::RetSportsParamTrainingModeParam>(cmd);
        case t1::Command::SPORTS_RET_EXTENDED_PARAM:
        {
            if (cmd.size() < 3 ||
                static_cast<t1::SportsInquiredType>(cmd[1]) != t1::SportsInquiredType::TRAINING_MODE) return false;
            switch (static_cast<t1::TrainingModeExParameterType>(cmd[2]))
            {
            case t1::TrainingModeExParameterType::NCASM_SETTINGS:
            case t1::TrainingModeExParameterType::NCASM_ACTUAL_EFFECTS:
                return cmd.size() >= 9;
            case t1::TrainingModeExParameterType::ASM_SETTINGS:
            case t1::TrainingModeExParameterType::ASM_ACTUAL_EFFECTS:
                return cmd.size() >= 7;
            case t1::TrainingModeExParameterType::PRESET_EQ_SETTINGS:
            case t1::TrainingModeExParameterType::PRESET_EQ_ACTUAL_EFFECTS:
                return cmd.size() >= 5 && is_valid(static_cast<t1::EqPresetId>(cmd[3])) &&
                    cmd[4] <= cmd.size() - 5;
            default:
                return false;
            }
        }
        case t1::Command::SYSTEM_RET_CAPABILITY:
            switch (static_cast<t1::SystemInquiredType>(cmd[1]))
            {
            case t1::SystemInquiredType::VIBRATOR:
                return AcceptV1FixedResponse<t1::RetSystemCapability_VibratorCapability>(cmd);
            case t1::SystemInquiredType::POWER_SAVING_MODE:
                return AcceptV1FixedResponse<t1::RetSystemCapability_PowerSavingModeCapability>(cmd);
            case t1::SystemInquiredType::CONTROL_BY_WEARING:
                return AcceptV1FixedResponse<t1::RetSystemCapability_ControlByWearingCapability>(cmd);
            case t1::SystemInquiredType::AUTO_POWER_OFF:
                return cmd.size() >= 3 && cmd[2] <= cmd.size() - 3;
            case t1::SystemInquiredType::SMART_TALKING_MODE:
                return cmd.size() >= 8 &&
                    (cmd[7] != static_cast<UInt8>(t1::SmartTalkingModeModeOutTimeType::TYPE_1) || cmd.size() >= 12);
            case t1::SystemInquiredType::ASSIGNABLE_SETTINGS:
                return true; // Decoded once by the state handler.
            default:
                return false;
            }
        case t1::Command::SYSTEM_RET_PARAM:
            if (static_cast<t1::SystemInquiredType>(cmd[1]) == t1::SystemInquiredType::VIBRATOR)
                return AcceptV1FixedResponse<t1::RetSystemParamVibratorParam>(cmd);
            if (static_cast<t1::SystemInquiredType>(cmd[1]) == t1::SystemInquiredType::POWER_SAVING_MODE)
                return AcceptV1FixedResponse<t1::RetSystemParamPowerSavingModeParam>(cmd);
            return true;
        case t1::Command::GENERAL_SETTING_RET_CAPABILITY:
        {
            // Validate GsSettingInfo without allocating copies of its two strings.
            // The state handler deserializes the accepted title once afterward.
            auto data = cmd.subspan(2);
            const auto skipString = [&data]() { return SkipV1String(data); };
            const auto acceptInfo = [&data, &skipString]()
            {
                t1::GsStringFormat format{};
                if (!detail::ReadEnumTag(data, format, 0) ||
                    (format != t1::GsStringFormat::RAW_NAME && format != t1::GsStringFormat::ENUM_NAME)) return false;
                data = data.subspan(1);
                return skipString() && skipString();
            };
            if (!acceptInfo() || data.empty()) return false;
            t1::GsSettingType type{};
            if (!detail::ReadEnumTag(data, type, 0)) return false;
            data = data.subspan(1);
            if (type == t1::GsSettingType::BOOLEAN_TYPE) return true;
            if (type != t1::GsSettingType::LIST_TYPE || data.empty()) return false;
            const UInt8 count = data[0];
            if (count < 1 || count > 64) return false;
            data = data.subspan(1);
            for (UInt8 i = 0; i < count; ++i)
                if (!acceptInfo()) return false;
            return true;
        }
        default:
            return true;
        }
    }

    MDRTask MDRHeadphones::RequestInitV1()
    {
        auto& state = mDetailsV1;
        if (mProtocolFamily != ProtocolFamily::V1)
            co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "Device does not use MDR V1");

        state.mProtocol = {};
        state.mSupport = {};
        state.mAutoPowerOffCandidates.clear();
        state.mNCOptimizer = {};
        state.mEqType = t1::EqEbbInquiredType::NO_USE;
        state.mEqAvailable.overwrite(false);
        state.mEbbAvailable = false;
        state.mVoiceGuidanceSupportsOnOff = false;
        state.mVoiceGuidanceSupportsLanguage = false;
        state.mNcAsmAvailable = false;
        state.mAudioPriorityAvailable = false;
        state.mPlayAvailable = false;
        state.mVoiceGuidanceAvailable = false;
        state.mUpscalingAvailable = false;
        state.mSystemAvailable = {};
        state.mGsAvailable = {};

        const auto discoveryDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(30);
        const auto remainingMS = [](std::chrono::steady_clock::time_point deadline)
        {
            return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(
                deadline - std::chrono::steady_clock::now()).count());
        };
        // The phase/read deadline includes ACK retries as well as the typed RET.
#define V1_SEND_INIT_ACK(Type, Deadline, Result, ...) \
        { \
            Result = MDR_RESULT_ERROR_TIMEOUT; \
            for (int retry = 0; retry < mACKRetryCount; ++retry) \
            { \
                const int remaining = remainingMS(Deadline); \
                if (remaining <= 0) break; \
                Result = SendCommandImpl<Type>(__VA_ARGS__); \
                if (Result != MDR_RESULT_OK) break; \
                Result = co_await Await(AWAIT_ACK, std::min(mACKRetryTimeoutMS, remaining)); \
                if (Result == MDR_RESULT_OK) break; \
            } \
        }
        // Sony's fresh discovery path: each GET completes on its RET, not its ACK.
#define V1_DISCOVER(Type, Ret, Inquiry, Detail, ...) \
        { \
            if (remainingMS(discoveryDeadline) <= 0) \
                co_return SetLastError(MDR_RESULT_ERROR_TIMEOUT, "Timeout waiting for V1 capability response"); \
            const auto responseType = ResponseType(MDRTraits<Type>::kDataType, static_cast<UInt8>(Ret), \
                static_cast<int>(Inquiry), static_cast<int>(Detail)); \
            BeginAwait(responseType); \
            int result; \
            V1_SEND_INIT_ACK(Type, discoveryDeadline, result, __VA_ARGS__); \
            if (result == MDR_RESULT_OK) \
            { \
                const int remaining = remainingMS(discoveryDeadline); \
                result = remaining > 0 ? co_await Await(responseType, remaining) : MDR_RESULT_ERROR_TIMEOUT; \
            } \
            if (result != MDR_RESULT_OK) \
            { \
                CancelAwait(responseType, result); \
                co_return SetLastError(result, "Timeout waiting for V1 capability response"); \
            } \
        }
        V1_DISCOVER(t1::GetProtocolInfo, t1::Command::CONNECT_RET_PROTOCOL_INFO,
            t1::CommonCapabilityInquiredType::FIXED_VALUE, -1);
        constexpr int acceptedVersions[] = {0x1000, 0x2000, 0x3000, 0x4000, 0x4010,
            0x5000, 0x6000, 0x7000, 0x7010};
        if (!state.mProtocol.hasTable1 ||
            std::ranges::find(acceptedVersions, state.mProtocol.version) == std::end(acceptedVersions))
            co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "Unsupported MDR V1 protocol version");
        V1_DISCOVER(t1::GetCapabilityInfo, t1::Command::CONNECT_RET_CAPABILITY_INFO,
            t1::CommonCapabilityInquiredType::FIXED_VALUE, -1);
        for (const auto type : {t1::DeviceInfoInquiredType::MODEL_NAME,
                t1::DeviceInfoInquiredType::FW_VERSION, t1::DeviceInfoInquiredType::SERIES_AND_COLOR_INFO})
            V1_DISCOVER(t1::GetDeviceInfo, t1::Command::CONNECT_RET_DEVICE_INFO, type, -1,
                {.inquiredType = type});
        if (state.mProtocol.version >= 0x5000)
            V1_DISCOVER(t1::GetDeviceInfo, t1::Command::CONNECT_RET_DEVICE_INFO,
                t1::DeviceInfoInquiredType::INSTRUCTION_GUIDE, -1,
                {.inquiredType = t1::DeviceInfoInquiredType::INSTRUCTION_GUIDE});
        V1_DISCOVER(t1::GetSupportFunction, t1::Command::CONNECT_RET_SUPPORT_FUNCTION,
            t1::CommonCapabilityInquiredType::FIXED_VALUE, -1);

        for (const auto function : {t1::FunctionType::VPT, t1::FunctionType::SOUND_POSITION})
        {
            if (!state.mSupport.contains(function)) continue;
            const auto type = function == t1::FunctionType::VPT
                ? t1::VptInquiredType::VPT : t1::VptInquiredType::SOUND_POSITION;
            V1_DISCOVER(t1::GetVptCapability, t1::Command::VPT_RET_CAPABILITY, type, -1,
                {.inquiredType = type, .displayLanguage = t1::DisplayLanguage::ENGLISH});
        }
        for (const auto function : {t1::FunctionType::PRESET_EQ,
                t1::FunctionType::PRESET_EQ_NONCUSTOMIZABLE, t1::FunctionType::EBB})
        {
            if (!state.mSupport.contains(function)) continue;
            const auto type = function == t1::FunctionType::PRESET_EQ ? t1::EqEbbInquiredType::PRESET_EQ
                : function == t1::FunctionType::PRESET_EQ_NONCUSTOMIZABLE
                ? t1::EqEbbInquiredType::PRESET_EQ_NONCUSTOMIZABLE : t1::EqEbbInquiredType::EBB;
            V1_DISCOVER(t1::GetEqEbbCapability, t1::Command::EQEBB_RET_CAPABILITY, type, -1,
                {.type = type, .language = t1::DisplayLanguage::ENGLISH});
        }
        for (const auto function : {t1::FunctionType::NOISE_CANCELLING,
                t1::FunctionType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE, t1::FunctionType::AMBIENT_SOUND_MODE})
        {
            if (!state.mSupport.contains(function)) continue;
            const auto type = function == t1::FunctionType::NOISE_CANCELLING
                ? t1::NcAsmInquiredType::NOISE_CANCELLING
                : function == t1::FunctionType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE
                ? t1::NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE
                : t1::NcAsmInquiredType::AMBIENT_SOUND_MODE;
            V1_DISCOVER(t1::GetNcAsmCapability, t1::Command::NCASM_RET_CAPABILITY, type, -1, {.type = type});
        }
        if (state.mSupport.contains(t1::FunctionType::NC_OPTIMIZER))
            V1_DISCOVER(t1::GetOptimizerCapability, t1::Command::OPT_RET_CAPABILITY,
                t1::OptimizerInquiredType::NC_OPTIMIZER, -1);
        if (state.mSupport.contains(t1::FunctionType::PLAYBACK_CONTROLLER))
            V1_DISCOVER(t1::GetPlayCapability, t1::Command::PLAY_RET_CAPABILITY,
                t1::PlayInquiredType::PLAYBACK_CONTROLLER, -1, {.type = t1::PlayInquiredType::PLAYBACK_CONTROLLER});
        for (const auto function : {t1::FunctionType::CONNECTION_MODE, t1::FunctionType::UPSCALING})
        {
            if (!state.mSupport.contains(function)) continue;
            const auto type = function == t1::FunctionType::CONNECTION_MODE
                ? t1::AudioInquiredType::CONNECTION_MODE : t1::AudioInquiredType::UPSCALING;
            V1_DISCOVER(t1::GetAudioCapability, t1::Command::AUDIO_RET_CAPABILITY, type, -1, {.inquiredType = type});
        }
        for (const auto function : {t1::FunctionType::VIBRATOR, t1::FunctionType::POWER_SAVING_MODE,
                t1::FunctionType::CONTROL_BY_WEARING, t1::FunctionType::AUTO_POWER_OFF,
                t1::FunctionType::SMART_TALKING_MODE, t1::FunctionType::ASSIGNABLE_SETTINGS})
        {
            if (!state.mSupport.contains(function)) continue;
            const auto type = function == t1::FunctionType::VIBRATOR ? t1::SystemInquiredType::VIBRATOR
                : function == t1::FunctionType::POWER_SAVING_MODE ? t1::SystemInquiredType::POWER_SAVING_MODE
                : function == t1::FunctionType::CONTROL_BY_WEARING ? t1::SystemInquiredType::CONTROL_BY_WEARING
                : function == t1::FunctionType::AUTO_POWER_OFF ? t1::SystemInquiredType::AUTO_POWER_OFF
                : function == t1::FunctionType::SMART_TALKING_MODE ? t1::SystemInquiredType::SMART_TALKING_MODE
                : t1::SystemInquiredType::ASSIGNABLE_SETTINGS;
            V1_DISCOVER(t1::GetSystemCapability, t1::Command::SYSTEM_RET_CAPABILITY, type, -1, {.inquiredType = type});
        }
        if (state.mSupport.contains(t1::FunctionType::TRAINING_MODE))
            V1_DISCOVER(t1::GetSportsCapability, t1::Command::SPORTS_RET_CAPABILITY,
                t1::SportsInquiredType::TRAINING_MODE, -1, {.type = t1::SportsInquiredType::TRAINING_MODE});
        // Discovery follows support-list order; the live holders below have fixed GS order.
        for (const auto function : state.mSupport.order)
        {
            if (function != t1::FunctionType::GENERAL_SETTING1 &&
                function != t1::FunctionType::GENERAL_SETTING2 && function != t1::FunctionType::GENERAL_SETTING3) continue;
            const auto type = static_cast<t1::GsInquiredType>(function);
            V1_DISCOVER(t1::GetGsCapability, t1::Command::GENERAL_SETTING_RET_CAPABILITY, type, -1,
                {.type = type, .displayLanguage = t1::DisplayLanguage::ENGLISH});
        }
        if (state.mSupport.contains(t1::FunctionType::BLE_SETUP))
        {
            for (const auto type : {t1::BluetoothDeviceInfoType::BLE_HASH_VALUE,
                    t1::BluetoothDeviceInfoType::BLUETOOTH_DEVICE_ADDRESS})
                V1_DISCOVER(t1::GetBluetoothDeviceInfo, t1::Command::COMMON_RET_BLUETOOTH_DEVICE_INFO,
                    type, -1, {.type = type});
        }
        if (state.mSupport.contains(t1::FunctionType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
            V1_DISCOVER(t2::GetPeripheralCapability, t2::Command::PERIPHERAL_RET_CAPABILITY,
                t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT, -1,
                {.inquiredType = t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT});
        if (state.mSupport.contains(t1::FunctionType::VOICE_GUIDANCE))
        {
            V1_DISCOVER(t2::GetVoiceGuidanceCapability, t2::Command::VOICE_GUIDANCE_RET_CAPABILITY,
                t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING, -1);
            V1_DISCOVER(t2::GetVoiceGuidanceParam, t2::Command::VOICE_GUIDANCE_RET_PARAM,
                t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING, t2::DetailedDataType::UPDATE_METHOD,
                {.detailedDataType = t2::DetailedDataType::UPDATE_METHOD});
        }
        if (state.mSupport.contains(t1::FunctionType::FW_UPDATE))
        {
            for (const auto type : {t1::UpdateInquiredType::BLE_TX_POWER,
                    t1::UpdateInquiredType::BATTERY_POWER_THRESHOLD, t1::UpdateInquiredType::UPDATE_METHOD,
                    t1::UpdateInquiredType::BATTERY_POWER_THRESHOLD_FOR_INTERRUPTIONG_FW_UPDATE,
                    t1::UpdateInquiredType::UNIQUE_ID_FOR_DEVICE_BINDING})
            {
                const int minimumVersion = type == t1::UpdateInquiredType::UPDATE_METHOD ? 0x4000
                    : (type == t1::UpdateInquiredType::BLE_TX_POWER ||
                       type == t1::UpdateInquiredType::BATTERY_POWER_THRESHOLD) ? 0x2000 : 0x5000;
                if (state.mProtocol.version < minimumVersion) continue;
                V1_DISCOVER(t1::GetUpdateParam, t1::Command::UPDT_RET_PARAM, type, -1,
                    {.updateInquiredType = type});
            }
        }
#undef V1_DISCOVER

        // A missing RET stops only the current Sony information holder's reload.
#define V1_RELOAD(Type, Ret, Inquiry, Detail, ...) \
        { \
            const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2); \
            const auto responseType = ResponseType(MDRTraits<Type>::kDataType, static_cast<UInt8>(Ret), \
                static_cast<int>(Inquiry), static_cast<int>(Detail)); \
            BeginAwait(responseType); \
            int result; \
            V1_SEND_INIT_ACK(Type, deadline, result, __VA_ARGS__); \
            if (result == MDR_RESULT_OK) \
            { \
                const int remaining = remainingMS(deadline); \
                result = remaining > 0 ? co_await Await(responseType, remaining) : MDR_RESULT_ERROR_TIMEOUT; \
            } \
            if (result != MDR_RESULT_OK) \
            { \
                CancelAwait(responseType, result); \
                break; \
            } \
        }
        if (state.mSupport.contains(t1::FunctionType::FW_UPDATE)) do
        {
            V1_RELOAD(t1::GetUpdateParam, t1::Command::UPDT_RET_PARAM, t1::UpdateInquiredType::CATEGORY_ID, -1,
                {.updateInquiredType = t1::UpdateInquiredType::CATEGORY_ID});
            V1_RELOAD(t1::GetUpdateParam, t1::Command::UPDT_RET_PARAM, t1::UpdateInquiredType::SERVICE_ID, -1,
                {.updateInquiredType = t1::UpdateInquiredType::SERVICE_ID});
            V1_RELOAD(t1::GetUpdateParam, t1::Command::UPDT_RET_PARAM, t1::UpdateInquiredType::NATION_CODE, -1,
                {.updateInquiredType = t1::UpdateInquiredType::NATION_CODE});
            V1_RELOAD(t1::GetUpdateParam, t1::Command::UPDT_RET_PARAM, t1::UpdateInquiredType::LANGUAGE, -1,
                {.updateInquiredType = t1::UpdateInquiredType::LANGUAGE});
            V1_RELOAD(t1::GetUpdateParam, t1::Command::UPDT_RET_PARAM, t1::UpdateInquiredType::SERIAL_NUMBER, -1,
                {.updateInquiredType = t1::UpdateInquiredType::SERIAL_NUMBER});
        } while (false);
        if (state.mSupport.contains(t1::FunctionType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT)) do
        {
            V1_RELOAD(t2::GetPeripheralStatus, t2::Command::PERIPHERAL_RET_STATUS,
                t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT, -1,
                {.inquiredType = t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT});
            V1_RELOAD(t2::GetPeripheralParam, t2::Command::PERIPHERAL_RET_PARAM,
                t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT, -1,
                {.inquiredType = t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT});
        } while (false);
        if (state.mSupport.contains(t1::FunctionType::VOICE_GUIDANCE)) do
        {
            if (state.mVoiceGuidanceSupportsOnOff)
            {
                V1_RELOAD(t2::GetVoiceGuidanceStatus, t2::Command::VOICE_GUIDANCE_RET_STATUS,
                    t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING, t2::StatusType::ON_OFF,
                    {.statusType = t2::StatusType::ON_OFF});
                V1_RELOAD(t2::GetVoiceGuidanceParam, t2::Command::VOICE_GUIDANCE_RET_PARAM,
                    t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING, t2::DetailedDataType::ON_OFF,
                    {.detailedDataType = t2::DetailedDataType::ON_OFF});
            }
            if (state.mVoiceGuidanceSupportsLanguage)
            {
                V1_RELOAD(t2::GetVoiceGuidanceStatus, t2::Command::VOICE_GUIDANCE_RET_STATUS,
                    t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING, t2::StatusType::LANGUAGE,
                    {.statusType = t2::StatusType::LANGUAGE});
                V1_RELOAD(t2::GetVoiceGuidanceParam, t2::Command::VOICE_GUIDANCE_RET_PARAM,
                    t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING, t2::DetailedDataType::LANGUAGE,
                    {.detailedDataType = t2::DetailedDataType::LANGUAGE});
                V1_RELOAD(t2::GetVoiceGuidanceParam, t2::Command::VOICE_GUIDANCE_RET_PARAM,
                    t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING, t2::DetailedDataType::REQUIRED_TIME,
                    {.detailedDataType = t2::DetailedDataType::REQUIRED_TIME});
                V1_RELOAD(t2::GetVoiceGuidanceParam, t2::Command::VOICE_GUIDANCE_RET_PARAM,
                    t2::VoiceGuidanceInquiredType::VOICE_GUIDANCE_SETTING, t2::DetailedDataType::DOWNLOAD_SERVER_METHOD,
                    {.detailedDataType = t2::DetailedDataType::DOWNLOAD_SERVER_METHOD});
            }
        } while (false);
        for (const auto function : {t1::FunctionType::VPT, t1::FunctionType::SOUND_POSITION})
        {
            if (!state.mSupport.contains(function)) continue;
            const auto type = function == t1::FunctionType::VPT
                ? t1::VptInquiredType::VPT : t1::VptInquiredType::SOUND_POSITION;
            do
            {
                V1_RELOAD(t1::GetVptStatus, t1::Command::VPT_RET_STATUS, type, -1, {.vptInquiredType = type});
                V1_RELOAD(t1::GetVptParam, t1::Command::VPT_RET_PARAM, type, -1, {.vptInquiredType = type});
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::PRESET_EQ) ||
            state.mSupport.contains(t1::FunctionType::PRESET_EQ_NONCUSTOMIZABLE)) do
        {
            const auto type = state.mEqType;
            V1_RELOAD(t1::GetEqEbbStatus, t1::Command::EQEBB_RET_STATUS, type, -1, {.type = type});
            V1_RELOAD(t1::GetEqEbbParam, t1::Command::EQEBB_RET_PARAM, type, -1, {.type = type});
            V1_RELOAD(t1::GetEqEbbExtendedInfo, t1::Command::EQEBB_RET_EXTENDED_INFO, type, -1, {.type = type});
        } while (false);
        if (state.mSupport.contains(t1::FunctionType::EBB)) do
        {
            V1_RELOAD(t1::GetEqEbbStatus, t1::Command::EQEBB_RET_STATUS, t1::EqEbbInquiredType::EBB, -1,
                {.type = t1::EqEbbInquiredType::EBB});
            V1_RELOAD(t1::GetEqEbbParam, t1::Command::EQEBB_RET_PARAM, t1::EqEbbInquiredType::EBB, -1,
                {.type = t1::EqEbbInquiredType::EBB});
        } while (false);
        for (const auto function : {t1::FunctionType::NOISE_CANCELLING,
                t1::FunctionType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE, t1::FunctionType::AMBIENT_SOUND_MODE})
        {
            if (!state.mSupport.contains(function)) continue;
            const auto type = function == t1::FunctionType::NOISE_CANCELLING ? t1::NcAsmInquiredType::NOISE_CANCELLING
                : function == t1::FunctionType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE
                ? t1::NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE : t1::NcAsmInquiredType::AMBIENT_SOUND_MODE;
            do
            {
                V1_RELOAD(t1::GetNcAsmStatus, t1::Command::NCASM_RET_STATUS, type, -1, {.type = type});
                V1_RELOAD(t1::GetNcAsmParam, t1::Command::NCASM_RET_PARAM, type, -1, {.type = type});
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::NC_OPTIMIZER)) do
        {
            V1_RELOAD(t1::GetOptimizerStatus, t1::Command::OPT_RET_STATUS, t1::OptimizerInquiredType::NC_OPTIMIZER, -1);
            V1_RELOAD(t1::GetOptimizerParam, t1::Command::OPT_RET_PARAM, t1::OptimizerInquiredType::NC_OPTIMIZER, -1);
        } while (false);
        if (state.mSupport.contains(t1::FunctionType::PLAYBACK_CONTROLLER)) do
        {
            V1_RELOAD(t1::GetPlayStatus, t1::Command::PLAY_RET_STATUS, t1::PlayInquiredType::PLAYBACK_CONTROLLER, -1,
                {.type = t1::PlayInquiredType::PLAYBACK_CONTROLLER});
            V1_RELOAD(t1::GetPlayParam, t1::Command::PLAY_RET_PARAM, t1::PlayInquiredType::PLAYBACK_CONTROLLER,
                t1::PlaybackDetailedDataType::VOLUME,
                {.type = t1::PlayInquiredType::PLAYBACK_CONTROLLER, .dataType = t1::PlaybackDetailedDataType::VOLUME});
            V1_RELOAD(t1::GetPlayParam, t1::Command::PLAY_RET_PARAM, t1::PlayInquiredType::PLAYBACK_CONTROLLER,
                t1::PlaybackDetailedDataType::TRACK_NAME,
                {.type = t1::PlayInquiredType::PLAYBACK_CONTROLLER, .dataType = t1::PlaybackDetailedDataType::TRACK_NAME});
            V1_RELOAD(t1::GetPlayParam, t1::Command::PLAY_RET_PARAM, t1::PlayInquiredType::PLAYBACK_CONTROLLER,
                t1::PlaybackDetailedDataType::ALBUM_NAME,
                {.type = t1::PlayInquiredType::PLAYBACK_CONTROLLER, .dataType = t1::PlaybackDetailedDataType::ALBUM_NAME});
            V1_RELOAD(t1::GetPlayParam, t1::Command::PLAY_RET_PARAM, t1::PlayInquiredType::PLAYBACK_CONTROLLER,
                t1::PlaybackDetailedDataType::ARTIST_NAME,
                {.type = t1::PlayInquiredType::PLAYBACK_CONTROLLER, .dataType = t1::PlaybackDetailedDataType::ARTIST_NAME});
            V1_RELOAD(t1::GetPlayParam, t1::Command::PLAY_RET_PARAM, t1::PlayInquiredType::PLAYBACK_CONTROLLER,
                t1::PlaybackDetailedDataType::GENRE_NAME,
                {.type = t1::PlayInquiredType::PLAYBACK_CONTROLLER, .dataType = t1::PlaybackDetailedDataType::GENRE_NAME});
        } while (false);
        for (const auto function : {t1::FunctionType::CONNECTION_MODE, t1::FunctionType::UPSCALING})
        {
            if (!state.mSupport.contains(function)) continue;
            const auto type = function == t1::FunctionType::CONNECTION_MODE
                ? t1::AudioInquiredType::CONNECTION_MODE : t1::AudioInquiredType::UPSCALING;
            do
            {
                V1_RELOAD(t1::GetAudioStatus, t1::Command::AUDIO_RET_STATUS, type, -1, {.audioInquiredType = type});
                V1_RELOAD(t1::GetAudioParam, t1::Command::AUDIO_RET_PARAM, type, -1, {.audioInquiredType = type});
            } while (false);
        }
        for (const auto function : {t1::FunctionType::VIBRATOR, t1::FunctionType::POWER_SAVING_MODE,
                t1::FunctionType::AUTO_POWER_OFF, t1::FunctionType::SMART_TALKING_MODE, t1::FunctionType::ASSIGNABLE_SETTINGS})
        {
            if (!state.mSupport.contains(function)) continue;
            const auto type = function == t1::FunctionType::VIBRATOR ? t1::SystemInquiredType::VIBRATOR
                : function == t1::FunctionType::POWER_SAVING_MODE ? t1::SystemInquiredType::POWER_SAVING_MODE
                : function == t1::FunctionType::AUTO_POWER_OFF ? t1::SystemInquiredType::AUTO_POWER_OFF
                : function == t1::FunctionType::SMART_TALKING_MODE ? t1::SystemInquiredType::SMART_TALKING_MODE
                : t1::SystemInquiredType::ASSIGNABLE_SETTINGS;
            do
            {
                V1_RELOAD(t1::GetSystemStatus, t1::Command::SYSTEM_RET_STATUS, type, -1, {.systemInquiredType = type});
                V1_RELOAD(t1::GetSystemParam, t1::Command::SYSTEM_RET_PARAM, type, -1, {.systemInquiredType = type});
                if (type == t1::SystemInquiredType::SMART_TALKING_MODE)
                    V1_RELOAD(t1::GetSystemExParam, t1::Command::SYSTEM_RET_EXTENDED_PARAM, type, -1,
                        {.systemInquiredType = type});
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::TRAINING_MODE)) do
        {
            V1_RELOAD(t1::GetSportsStatus, t1::Command::SPORTS_RET_STATUS, t1::SportsInquiredType::TRAINING_MODE, -1,
                {.type = t1::SportsInquiredType::TRAINING_MODE});
            V1_RELOAD(t1::GetSportsParam, t1::Command::SPORTS_RET_PARAM, t1::SportsInquiredType::TRAINING_MODE, -1,
                {.type = t1::SportsInquiredType::TRAINING_MODE});
            const auto setting = state.mTrainingEffect == t1::TrainingModeAvailableEffectType::TYPE1
                ? t1::TrainingModeExParameterType::NCASM_SETTINGS : t1::TrainingModeExParameterType::ASM_SETTINGS;
            const auto actual = state.mTrainingEffect == t1::TrainingModeAvailableEffectType::TYPE1
                ? t1::TrainingModeExParameterType::NCASM_ACTUAL_EFFECTS : t1::TrainingModeExParameterType::ASM_ACTUAL_EFFECTS;
            V1_RELOAD(t1::GetSportsExParam, t1::Command::SPORTS_RET_EXTENDED_PARAM, t1::SportsInquiredType::TRAINING_MODE,
                setting, {.type = t1::SportsInquiredType::TRAINING_MODE, .getSportsExParam_TrainingModeRequest = {setting}});
            V1_RELOAD(t1::GetSportsExParam, t1::Command::SPORTS_RET_EXTENDED_PARAM, t1::SportsInquiredType::TRAINING_MODE,
                actual, {.type = t1::SportsInquiredType::TRAINING_MODE, .getSportsExParam_TrainingModeRequest = {actual}});
            V1_RELOAD(t1::GetSportsExParam, t1::Command::SPORTS_RET_EXTENDED_PARAM, t1::SportsInquiredType::TRAINING_MODE,
                t1::TrainingModeExParameterType::PRESET_EQ_SETTINGS,
                {.type = t1::SportsInquiredType::TRAINING_MODE,
                 .getSportsExParam_TrainingModeRequest = {t1::TrainingModeExParameterType::PRESET_EQ_SETTINGS}});
            V1_RELOAD(t1::GetSportsExParam, t1::Command::SPORTS_RET_EXTENDED_PARAM, t1::SportsInquiredType::TRAINING_MODE,
                t1::TrainingModeExParameterType::PRESET_EQ_ACTUAL_EFFECTS,
                {.type = t1::SportsInquiredType::TRAINING_MODE,
                 .getSportsExParam_TrainingModeRequest = {t1::TrainingModeExParameterType::PRESET_EQ_ACTUAL_EFFECTS}});
            const auto eqType = state.mEqType == t1::EqEbbInquiredType::PRESET_EQ
                ? t1::EqEbbInquiredType::PRESET_EQ : t1::EqEbbInquiredType::PRESET_EQ_NONCUSTOMIZABLE;
            V1_RELOAD(t1::GetEqEbbExtendedInfo, t1::Command::EQEBB_RET_EXTENDED_INFO, eqType, -1, {.type = eqType});
        } while (false);
        if (state.mSupport.contains(t1::FunctionType::LEFT_RIGHT_CONNECTION_STATUS)) do
        {
            V1_RELOAD(t1::GetConnectionStatus, t1::Command::COMMON_RET_CONNECTION_STATUS,
                t1::ConnectionStatusInquiredType::LEFT_RIGHT_CONNECTION_STATUS, -1);
        } while (false);
        if (state.mSupport.contains(t1::FunctionType::BATTERY_LEVEL) ||
            state.mSupport.contains(t1::FunctionType::LEFT_RIGHT_BATTERY_LEVEL)) do
        {
            const auto type = state.mSupport.contains(t1::FunctionType::BATTERY_LEVEL)
                ? t1::BatteryInquiredType::BATTERY : t1::BatteryInquiredType::LEFT_RIGHT_BATTERY;
            V1_RELOAD(t1::GetBatteryLevel, t1::Command::COMMON_RET_BATTERY_LEVEL, type, -1,
                {.batteryInquiredType = type});
        } while (false);
        if (state.mSupport.contains(t1::FunctionType::CRADLE_BATTERY_LEVEL)) do
        {
            V1_RELOAD(t1::GetBatteryLevel, t1::Command::COMMON_RET_BATTERY_LEVEL,
                t1::BatteryInquiredType::CRADLE_BATTERY, -1,
                {.batteryInquiredType = t1::BatteryInquiredType::CRADLE_BATTERY});
        } while (false);
        if (state.mSupport.contains(t1::FunctionType::UPSCALING_INDICATOR)) do
        {
            V1_RELOAD(t1::GetUpscalingEffect, t1::Command::COMMON_RET_UPSCALING_EFFECT,
                t1::CommonCapabilityInquiredType::FIXED_VALUE, -1);
        } while (false);
        if (state.mSupport.contains(t1::FunctionType::CODEC_INDICATOR)) do
        {
            V1_RELOAD(t1::GetAudioCodec, t1::Command::COMMON_RET_AUDIO_CODEC,
                t1::CommonCapabilityInquiredType::FIXED_VALUE, -1);
        } while (false);
        for (const auto function : {t1::FunctionType::GENERAL_SETTING1,
                t1::FunctionType::GENERAL_SETTING2, t1::FunctionType::GENERAL_SETTING3})
        {
            if (!state.mSupport.contains(function)) continue;
            const auto type = static_cast<t1::GsInquiredType>(function);
            do
            {
                V1_RELOAD(t1::GetGsStatus, t1::Command::GENERAL_SETTING_RET_STATUS, type, -1, {.type = type});
                V1_RELOAD(t1::GetGsParam, t1::Command::GENERAL_SETTING_RET_PARAM, type, -1, {.type = type});
            } while (false);
        }
#undef V1_RELOAD
#undef V1_SEND_INIT_ACK
        // TANDEM_KEEP_ALIVE's initial holder reload has no wire request.
        mInitialized = true;
        co_return MDR_EVENT_INITIALIZE_COMPLETE;
    }

    MDRTask MDRHeadphones::RequestSyncV1()
    {
        auto& state = mDetailsV1;

        /* Playback metadata. Track changes are not always notified. */
        if (state.mSupport.contains(t1::FunctionType::PLAYBACK_CONTROLLER))
        {
            SendCommandACK(t1::GetPlayStatus, {.type = t1::PlayInquiredType::PLAYBACK_CONTROLLER});

            SendCommandACK(t1::GetPlayParam, {
                .type = t1::PlayInquiredType::PLAYBACK_CONTROLLER,
                .dataType = t1::PlaybackDetailedDataType::VOLUME
            });
            SendCommandACK(t1::GetPlayParam, {
                .type = t1::PlayInquiredType::PLAYBACK_CONTROLLER,
                .dataType = t1::PlaybackDetailedDataType::TRACK_NAME
            });
            SendCommandACK(t1::GetPlayParam, {
                .type = t1::PlayInquiredType::PLAYBACK_CONTROLLER,
                .dataType = t1::PlaybackDetailedDataType::ALBUM_NAME
            });
            SendCommandACK(t1::GetPlayParam, {
                .type = t1::PlayInquiredType::PLAYBACK_CONTROLLER,
                .dataType = t1::PlaybackDetailedDataType::ARTIST_NAME
            });
        }

        co_return MDR_EVENT_SYNC_COMPLETE;
    }

    void MDRHeadphones::SnapshotPropertiesV1()
    {
        auto& state = mDetailsV1;
        state.mShutdown.submit();
        state.mNcAsmEnabled.submit();
        state.mNcAsmFocusOnVoice.submit();
        state.mNcAsmLevel.submit();
        state.mNcAsmChangingLevel.submit();
        state.mNcAsmButtonFunction.submit();
        state.mPowerAutoOff.submit();
        state.mPowerAutoOffSelectedTime.submit();
        state.mPlayVolume.submit();
        state.mPlayControl.submit();
        state.mGsParamBool[0].submit();
        state.mGsParamBool[1].submit();
        state.mGsParamBool[2].submit();
        state.mUpscalingEnabled.submit();
        state.mAudioPriorityMode.submit();
        state.mBGMModeEnabled.submit();
        state.mBGMModeRoomSize.submit();
        state.mUpmixCinemaEnabled.submit();
        state.mAutoPauseEnabled.submit();
        state.mAssignableSettingsPresets.submit();
        state.mSpeakToChatEnabled.submit();
        state.mSpeakToChatDetectSensitivity.submit();
        state.mSpeakToModeOutTime.submit();
        state.mHeadGestureEnabled.submit();
        state.mEqAvailable.submit();
        state.mEqPresetId.submit();
        state.mEqClearBass.submit();
        state.mEqConfig.submit();
        state.mVoiceGuidanceEnabled.submit();
        state.mVoiceGuidanceVolume.submit();
        state.mPairingMode.submit();
        state.mMultipointDeviceMac.submit();
        state.mSourceSwitchControlEnabled.submit();
        state.mPairedDeviceDisconnectMac.submit();
        state.mPairedDeviceConnectMac.submit();
        state.mPairedDeviceUnpairMac.submit();
        state.mSafeListeningPreviewMode.submit();
    }

    MDRTask MDRHeadphones::RequestCommitV1()
    {
        auto& state = mDetailsV1;
        SnapshotPropertiesV1();

        if (state.mShutdown.pending())
        {
            if (state.mShutdown.submitted && state.mSupport.contains(t1::FunctionType::POWER_OFF))
                SendCommandACK(t1::SetPowerOff, {.settingValue = t1::PowerOffSettingValue::USER_POWER_OFF});
            state.mShutdown.override(false);
        }

        if (state.mNcAsmEnabled.pending() || state.mNcAsmFocusOnVoice.pending() || state.mNcAsmLevel.pending() ||
            state.mNcAsmChangingLevel.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE))
            {
                t1::SetNcAsmParamNcAsmParam payload;
                if (state.mNcAsmEnabled.submitted)
                {
                    if (state.mNcAsmChangingLevel.submitted)
                        payload.ncAsmEffect = t1::NcAsmEffect::ADJUSTMENT_IN_PROGRESS;
                    else if (!state.mNcAsmEnabled.pending())
                        payload.ncAsmEffect = t1::NcAsmEffect::ADJUSTMENT_COMPLETION;
                    else
                        payload.ncAsmEffect = t1::NcAsmEffect::ON;
                }
                else
                {
                    payload.ncAsmEffect = t1::NcAsmEffect::OFF;
                }
                payload.ncType = t1::NcAsmSettingType::LEVEL_ADJUSTMENT;
                payload.ncValue = V1NcValueForAmbientLevel(state.mNcAsmLevel.submitted);
                payload.asmType = t1::AsmSettingType::LEVEL_ADJUSTMENT;
                payload.asmId = state.mNcAsmFocusOnVoice.submitted ? t1::AsmId::VOICE : t1::AsmId::NORMAL;
                payload.asmValue = static_cast<UInt8>(
                    state.mNcAsmLevel.submitted <= -1 ? 0 : std::clamp(state.mNcAsmLevel.submitted, 0, 20));
                SendCommandACK(t1::SetNcAsmParamNcAsmParam, payload);
            }
            else if (state.mSupport.contains(t1::FunctionType::NOISE_CANCELLING))
            {
                SendCommandACK(t1::SetNcAsmParamNcParam, {
                    .ncSettingType = state.mNcSettingType,
                    .ncSettingValue = state.mNcAsmEnabled.submitted && state.mNcAsmLevel.submitted < 0
                        ? t1::NcSettingValue::ON : t1::NcSettingValue::OFF
                });
            }
            else if (state.mSupport.contains(t1::FunctionType::AMBIENT_SOUND_MODE))
            {
                SendCommandACK(t1::SetNcAsmParamAsmParam, {
                    .ncAsmEffect = state.mNcAsmEnabled.submitted ? t1::NcAsmEffect::ON : t1::NcAsmEffect::OFF,
                    .asmType = state.mAsmSettingType,
                    .asmId = state.mNcAsmFocusOnVoice.submitted ? t1::AsmId::VOICE : t1::AsmId::NORMAL,
                    .asmValue = static_cast<UInt8>(std::clamp(state.mNcAsmLevel.submitted, 0, 20))
                });
            }
            state.mNcAsmEnabled.commit();
            state.mNcAsmFocusOnVoice.commit();
            state.mNcAsmLevel.commit();
            state.mNcAsmChangingLevel.commit();
        }

        if (state.mPlayVolume.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::PLAYBACK_CONTROLLER))
            {
                SendCommandACK(t1::SetPlayParamPlaybackControllerVolumeData, {
                    .volumeValue = static_cast<UInt8>(std::clamp(state.mPlayVolume.submitted, 0, 30))
                });
            }
            state.mPlayVolume.commit();
        }
        if (state.mPlayControl.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::PLAYBACK_CONTROLLER))
            {
                SendCommandACK(t1::SetPlayStatus, {
                    .type = t1::PlayInquiredType::PLAYBACK_CONTROLLER,
                    .control = state.mPlayControl.submitted
                });
            }
            state.mPlayControl.override(t1::PlaybackControl::KEY_OFF);
        }

        // Only write what the caller changed; pending() is also true when just the device moved.
        const bool eqPending = state.mEqPresetId.pending() || state.mEqConfig.pending() ||
            state.mEqClearBass.pending();
        const bool eqAsked = state.mEqPresetId.dirty() || state.mEqConfig.dirty() ||
            state.mEqClearBass.dirty();
        if (eqPending && !eqAsked)
        {
            state.mEqPresetId.override(state.mEqPresetId.current);
            state.mEqConfig.override(state.mEqConfig.current);
            state.mEqClearBass.override(state.mEqClearBass.current);
        }
        else if (eqPending)
        {
            if (state.mEqType == t1::EqEbbInquiredType::PRESET_EQ ||
                state.mEqType == t1::EqEbbInquiredType::PRESET_EQ_NONCUSTOMIZABLE)
            {
                // Devices drop a frame carrying both a preset and band steps, so send one or the other.
                t1::SetEqEbbParamEqParam payload;
                payload.type = state.mEqType;
                if (state.mEqConfig.dirty() || state.mEqClearBass.dirty())
                {
                    if (state.mEqType != t1::EqEbbInquiredType::PRESET_EQ)
                        co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "V1 equalizer is not customizable");
                    if (state.mEqBandInformation.size() != state.mEqBandCount ||
                        state.mEqConfig.submitted.size() + (state.mEqClearBassBand >= 0 ? 1 : 0) != state.mEqBandCount)
                        co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "V1 equalizer band information is unavailable");
                    payload.presetId = t1::EqPresetId::UNSPECIFIED;
                    payload.bandSteps.value.reserve(state.mEqBandCount);
                    const int center = (static_cast<int>(state.mEqStepCount) - 1) / 2;
                    size_t band = 0;
                    for (int i = 0; i < state.mEqBandCount; ++i)
                    {
                        const int value = i == state.mEqClearBassBand
                            ? state.mEqClearBass.submitted : state.mEqConfig.submitted[band++];
                        payload.bandSteps.value.push_back(static_cast<UInt8>(std::clamp(value, -center, center) + center));
                    }
                }
                else
                    payload.presetId = state.mEqPresetId.submitted;
                SendCommandACK(t1::SetEqEbbParamEqParam, payload);
            }
            else if (state.mSupport.contains(t1::FunctionType::EBB) && state.mEqClearBass.dirty())
            {
                SendCommandACK(t1::SetEqEbbParamEbbParam, {
                    .level = static_cast<Int8>(std::clamp(state.mEqClearBass.submitted,
                        static_cast<int>(state.mEbbCapability.minValue), static_cast<int>(state.mEbbCapability.maxValue)))
                });
            }
            state.mEqPresetId.commit();
            state.mEqConfig.commit();
            state.mEqClearBass.commit();
        }

        if (state.mUpscalingEnabled.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::UPSCALING))
            {
                SendCommandACK(t1::SetAudioParamUpscalingParam, {
                    .settingValue = state.mUpscalingEnabled.submitted
                        ? t1::UpscalingSettingValue::AUTO
                        : t1::UpscalingSettingValue::OFF
                });
            }
            state.mUpscalingEnabled.commit();
        }

        if (state.mAudioPriorityMode.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::CONNECTION_MODE))
            {
                SendCommandACK(t1::SetAudioParamConnectionModeParam, {
                    .settingValue = state.mAudioPriorityMode.submitted
                });
            }
            state.mAudioPriorityMode.commit();
        }

        if (state.mPowerAutoOff.pending() || state.mPowerAutoOffSelectedTime.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::AUTO_POWER_OFF))
            {
                SendCommandACK(t1::SetSystemParamAutoPowerOffParam, {
                    .activeElementId = state.mPowerAutoOff.submitted,
                    .selectTimeElementId = state.mPowerAutoOffSelectedTime.submitted
                });
            }
            state.mPowerAutoOff.commit();
            state.mPowerAutoOffSelectedTime.commit();
        }

        if (state.mAutoPauseEnabled.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::CONTROL_BY_WEARING))
            {
                SendCommandACK(t1::SetSystemExParamControlByWearingParam, {
                    .settingValue = state.mAutoPauseEnabled.submitted
                        ? t1::ControlByWearingSettingValue::ON
                        : t1::ControlByWearingSettingValue::OFF
                });
            }
            state.mAutoPauseEnabled.commit();
        }

        if (state.mAssignableSettingsPresets.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::ASSIGNABLE_SETTINGS))
            {
                t1::SetSystemParamAssignableSettingsParam payload;
                payload.presets.value = state.mAssignableSettingsPresets.submitted;
                SendCommandACK(t1::SetSystemParamAssignableSettingsParam, payload);
            }
            state.mAssignableSettingsPresets.commit();
        }

        if (state.mSpeakToChatEnabled.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::SMART_TALKING_MODE))
            {
                SendCommandACK(t1::SetSystemParammartTalkingModeSetNtfyParam, {
                    .settingValue = state.mSpeakToChatEnabled.submitted
                        ? t1::SmartTalkingModeSettingValue::ON
                        : t1::SmartTalkingModeSettingValue::OFF
                });
            }
            state.mSpeakToChatEnabled.commit();
        }
        if (state.mSpeakToChatDetectSensitivity.pending() || state.mSpeakToModeOutTime.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::SMART_TALKING_MODE))
            {
                const UInt8 payload[] = {
                    static_cast<UInt8>(t1::Command::SYSTEM_SET_EXTENDED_PARAM),
                    static_cast<UInt8>(t1::SystemInquiredType::SMART_TALKING_MODE),
                    0,
                    static_cast<UInt8>(state.mSpeakToChatDetectSensitivity.submitted),
                    static_cast<UInt8>(state.mSpeakToChatVoiceFocus),
                    static_cast<UInt8>(state.mSpeakToModeOutTime.submitted)
                };
                SendCommandImpl(payload, MDRDataType::DATA_MDR, mSeqNumber);
                const int result = co_await Await(AWAIT_ACK);
                if (result != MDR_RESULT_OK)
                    co_return SetLastError(result, "Timeout waiting for V1 Speak-to-Chat options");
            }
            state.mSpeakToChatDetectSensitivity.commit();
            state.mSpeakToModeOutTime.commit();
        }

        if (state.mPairingMode.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
            {
                SendCommandACK(t2::SetPeripheralStatus, {
                    .inquiredType = t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT,
                    .bluetoothModeStatus = state.mPairingMode.submitted
                        ? t2::PeripheralBluetoothModeStatus::INQUIRY_SCAN_MODE
                        : t2::PeripheralBluetoothModeStatus::NORMAL_MODE,
                    .status = CommonStatus::ENABLE
                });
            }
            state.mPairingMode.commit();
        }

        if (state.mVoiceGuidanceEnabled.pending())
        {
            if (state.mSupport.contains(t1::FunctionType::VOICE_GUIDANCE))
            {
                SendCommandACK(t2::SetVoiceGuidanceParamSettingOnOff, {
                    .settingValue = state.mVoiceGuidanceEnabled.submitted
                        ? t2::VoiceGuidanceSettingValue::ON
                        : t2::VoiceGuidanceSettingValue::OFF
                });
            }
            state.mVoiceGuidanceEnabled.commit();
        }

        if (state.mGsParamBool[0].pending())
        {
            if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING1))
            {
                SendCommandACK(t1::SetGsParamGsBooleanTypeValue, {
                    .type = t1::GsInquiredType::GENERAL_SETTING1,
                    .settingValue = { state.mGsParamBool[0].submitted
                        ? t1::CommonOnOffSettingValue::ON : t1::CommonOnOffSettingValue::OFF }
                });
            }
            state.mGsParamBool[0].commit();
        }
        if (state.mGsParamBool[1].pending())
        {
            if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING2))
            {
                SendCommandACK(t1::SetGsParamGsBooleanTypeValue, {
                    .type = t1::GsInquiredType::GENERAL_SETTING2,
                    .settingValue = { state.mGsParamBool[1].submitted
                        ? t1::CommonOnOffSettingValue::ON : t1::CommonOnOffSettingValue::OFF }
                });
            }
            state.mGsParamBool[1].commit();
        }
        if (state.mGsParamBool[2].pending())
        {
            if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING3))
            {
                SendCommandACK(t1::SetGsParamGsBooleanTypeValue, {
                    .type = t1::GsInquiredType::GENERAL_SETTING3,
                    .settingValue = { state.mGsParamBool[2].submitted
                        ? t1::CommonOnOffSettingValue::ON : t1::CommonOnOffSettingValue::OFF }
                });
            }
            state.mGsParamBool[2].commit();
        }

        if (state.mPairedDeviceConnectMac.pending() ||
            state.mPairedDeviceDisconnectMac.pending() ||
            state.mPairedDeviceUnpairMac.pending())
        {
            co_return SetLastError(
                MDR_RESULT_ERROR_NOT_SUPPORTED,
                "V1 paired-device mutations require a capture-confirmed address layout");
        }

        co_return MDR_EVENT_APPLY_COMPLETE;
    }

    int MDRHeadphones::HandleProtocolInfoV1(Span<const UInt8> command)
    {
        auto& state = mDetailsV1;
        if (command.size() != sizeof(t1::RetProtocolInfo))
            return SetLastError(MDR_RESULT_ERROR_MALFORMED_PAYLOAD, "MDR V1 protocol info size is not valid");
        const auto result = (t1::RetProtocolInfo::Deserialize)(command.data(), command.size());
        if (!result)
            return SetLastError(
                result.error,
                result.errMessage ? result.errMessage : "Unable to deserialize MDR V1 protocol info");
        state.mProtocol = {
            .version = result.value.protocolVersion,
            .hasTable1 = true,
            .hasTable2 = false
        };
        Awake(AWAIT_PROTOCOL_INFO);
        return MDR_EVENT_IDENTITY_CHANGED;
    }

    MDRTask MDRHeadphones::RequestAlertResponseV1(int action)
    {
        auto& state = mDetailsV1;
        if (!state.mAlertAwaitingResponse)
            co_return SetLastError(MDR_RESULT_ERROR_NOT_FOUND, "The device has not asked anything");

        state.mAlertAwaitingResponse = false;

        using namespace t1;
        SetAlertParamFixedMessageParam res;
        res.type = AlertInquiredType::FIXED_MESSAGE;
        res.messageType = state.mLastAlertMessage;
        res.action = action == MDR_ALERT_ACTION_POSITIVE ? AlertAction::POSITIVE
                                                         : AlertAction::NEGATIVE;
        SendCommandACK(SetAlertParamFixedMessageParam, res);
        co_return MDR_EVENT_APPLY_COMPLETE;
    }

    MDRTask MDRHeadphones::RequestStartNCOptimizerV1()
    {
        SendCommandACK(t1::SetOptimizerStatus, {
            .optimizerInquiredType = t1::OptimizerInquiredType::NC_OPTIMIZER,
            .status = CommonStatus::ENABLE,
            .optimizerControl = t1::OptimizerControl::START
        });
        co_return MDR_EVENT_APPLY_COMPLETE;
    }

    bool MDRHeadphones::IsDirtyV1() const
    {
        const auto& state = mDetailsV1;
        return state.mShutdown.dirty() || state.mNcAsmEnabled.dirty() ||
            state.mNcAsmFocusOnVoice.dirty() || state.mNcAsmLevel.dirty() ||
            state.mNcAsmChangingLevel.dirty() || state.mNcAsmButtonFunction.dirty() ||
            state.mPowerAutoOff.dirty() || state.mPowerAutoOffSelectedTime.dirty() ||
            state.mPlayVolume.dirty() || state.mPlayControl.dirty() ||
            state.mGsParamBool[0].dirty() || state.mGsParamBool[1].dirty() ||
            state.mGsParamBool[2].dirty() ||
            state.mUpscalingEnabled.dirty() || state.mAudioPriorityMode.dirty() ||
            state.mBGMModeEnabled.dirty() || state.mBGMModeRoomSize.dirty() ||
            state.mUpmixCinemaEnabled.dirty() || state.mAutoPauseEnabled.dirty() ||
            state.mAssignableSettingsPresets.dirty() ||
            state.mSpeakToChatEnabled.dirty() || state.mSpeakToChatDetectSensitivity.dirty() ||
            state.mSpeakToModeOutTime.dirty() || state.mHeadGestureEnabled.dirty() ||
            state.mEqAvailable.dirty() || state.mEqPresetId.dirty() ||
            state.mEqClearBass.dirty() || state.mEqConfig.dirty() ||
            state.mVoiceGuidanceEnabled.dirty() || state.mVoiceGuidanceVolume.dirty() ||
            state.mPairingMode.dirty() || state.mMultipointDeviceMac.dirty() ||
            state.mSafeListeningPreviewMode.dirty() || state.mSourceSwitchControlEnabled.dirty() ||
            state.mPairedDeviceConnectMac.dirty() || state.mPairedDeviceDisconnectMac.dirty() ||
            state.mPairedDeviceUnpairMac.dirty();
    }

}
