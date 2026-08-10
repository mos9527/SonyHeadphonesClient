#include <algorithm>
#include "Details.hpp"
namespace mdr
{
    using namespace v2;
    using namespace t2;

    template <typename T>
    bool ReadInquiredType(Span<const UInt8> cmd, T& out)
    {
        if (cmd.size() < 2)
            return false;
        out = static_cast<T>(cmd[1]);
        return true;
    }

    int HandleSupportFunctionT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(ConnectRetSupportFunction, res, cmd);
        std::ranges::fill(self->mSupport.table2Functions, false);
        for (auto fun : res.supportFunctions)
            self->mSupport.table2Functions[static_cast<UInt8>(fun.functionType)] = true;
        self->RefreshNeutralFeaturesV2();
        self->Awake(MDRHeadphones::AWAIT_SUPPORT_FUNCTION);
        return MDR_EVENT_IDENTITY_CHANGED;
    }

    int HandleVoiceGuidanceParamT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        VoiceGuidanceInquiredType type{};
        if (!ReadInquiredType(cmd, type))
            return MDR_EVENT_UNHANDLED;
        using enum VoiceGuidanceInquiredType;
        switch (type)
        {
        case MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH:
        {
            Deserialize(VoiceGuidanceRetParamSettingMtk, res, cmd);
            self->mVoiceGuidanceEnabled.overwrite(res.settingValue == OnOffSettingValue::ON);
            return MDR_EVENT_VOICE_GUIDANCE_CHANGED;
        }
        case VOLUME:
        {
            Deserialize(VoiceGuidanceRetParamVolume, res, cmd);
            self->mVoiceGuidanceVolume.overwrite(res.volumeValue);
            return MDR_EVENT_VOICE_GUIDANCE_CHANGED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandlePeripheralStatusT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        PeripheralInquiredType type{};
        if (!ReadInquiredType(cmd, type))
            return MDR_EVENT_UNHANDLED;
        using enum PeripheralInquiredType;
        switch (type)
        {
        case PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE:
        {
            const auto command = static_cast<Command>(cmd[0]);
            if (command == Command::PERI_NTFY_STATUS)
            {
                Deserialize(PeripheralNotifyStatusParingDeviceManagementCommon, res, cmd);
                self->mPairingMode.overwrite(res.enableDisableStatus == EnableDisable::ENABLE && res.btMode == PeripheralBluetoothMode::INQUIRY_SCAN_MODE);
            }
            else
            {
                Deserialize(PeripheralRetStatusPairingDeviceManagementCommon, res, cmd);
                self->mPairingMode.overwrite(res.enableDisableStatus == EnableDisable::ENABLE && res.btMode == PeripheralBluetoothMode::INQUIRY_SCAN_MODE);
            }
            return MDR_EVENT_PAIRING_CHANGED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandlePeripheralNotifyExtendedParamT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        PeripheralInquiredType type{};
        if (!ReadInquiredType(cmd, type))
            return MDR_EVENT_UNHANDLED;
        using enum PeripheralInquiredType;
        switch (type)
        {
        case SOURCE_SWITCH_CONTROL:
        {
            Deserialize(PeripheralNotifyExtendedParamSourceSwitchControl, res, cmd);
            const String target{res.targetBdAddress.begin(), res.targetBdAddress.end()};
            self->mMultipointDeviceMac.overwrite(target);
            for (auto& dev : self->mPairedDevices)
                dev.playbackDevice = dev.macAddress == target;
            return MDR_EVENT_PAIRED_DEVICES_CHANGED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandlePeripheralParamT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        PeripheralInquiredType type{};
        if (!ReadInquiredType(cmd, type))
            return MDR_EVENT_UNHANDLED;
        using enum PeripheralInquiredType;
        switch (type)
        {
        case PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT:
        {
            const auto command = static_cast<Command>(cmd[0]);
            if (command == Command::PERI_NTFY_PARAM)
            {
                Deserialize(PeripheralNotifyParamPairingDeviceManagementClassicBt, res, cmd);
                self->mPairedDevicesPlaybackDeviceID = res.playbackrightDevice;
                self->mPairedDevices.resize(res.deviceInfo.size());
                for (size_t i = 0; i < self->mPairedDevices.size(); ++i)
                {
                    auto& mac = res.deviceInfo.value[i].btDeviceAddress;
                    self->mPairedDevices[i].macAddress = {mac.begin(), mac.end()};
                    self->mPairedDevices[i].name = res.deviceInfo.value[i].btFriendlyName.value;
                    self->mPairedDevices[i].connected = res.deviceInfo.value[i].connectedStatus != 0;
                    self->mPairedDevices[i].playbackDevice =
                        res.deviceInfo.value[i].connectedStatus == res.playbackrightDevice;
                    if (self->mPairedDevices[i].playbackDevice)
                        self->mMultipointDeviceMac.overwrite(self->mPairedDevices[i].macAddress);
                }
                return MDR_EVENT_PAIRED_DEVICES_CHANGED;
            }
            Deserialize(PeripheralRetParamPairingDeviceManagementClassicBt, res, cmd);
            self->mPairedDevicesPlaybackDeviceID = res.playbackrightDevice;
            self->mPairedDevices.resize(res.deviceInfo.size());
            for (size_t i = 0; i < self->mPairedDevices.size(); ++i)
            {
                auto& mac = res.deviceInfo.value[i].btDeviceAddress;
                self->mPairedDevices[i].macAddress = {mac.begin(), mac.end()};
                self->mPairedDevices[i].name = res.deviceInfo.value[i].btFriendlyName.value;
                self->mPairedDevices[i].connected = res.deviceInfo.value[i].connectedStatus != 0;
                self->mPairedDevices[i].playbackDevice =
                    res.deviceInfo.value[i].connectedStatus == res.playbackrightDevice;
                if (res.deviceInfo.value[i].connectedStatus == res.playbackrightDevice)
                    self->mMultipointDeviceMac.overwrite(self->mPairedDevices[i].macAddress);
            }
            return MDR_EVENT_PAIRED_DEVICES_CHANGED;
        }
        case PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE:
        {
            const auto command = static_cast<Command>(cmd[0]);
            if (command == Command::PERI_NTFY_PARAM)
            {
                Deserialize(PeripheralNotifyParamPairingDeviceManagementWithBluetoothClassOfDevice, res, cmd);
                self->mPairedDevicesPlaybackDeviceID = res.playbackrightDevice;
                self->mPairedDevices.resize(res.deviceInfo.size());
                for (size_t i = 0; i < self->mPairedDevices.size(); ++i)
                {
                    auto& mac = res.deviceInfo.value[i].btDeviceAddress;
                    self->mPairedDevices[i].macAddress = {mac.begin(), mac.end()};
                    self->mPairedDevices[i].name = res.deviceInfo.value[i].btFriendlyName.value;
                    self->mPairedDevices[i].connected = res.deviceInfo.value[i].connectedStatus != 0;
                    self->mPairedDevices[i].playbackDevice =
                        res.deviceInfo.value[i].connectedStatus == res.playbackrightDevice;
                    if (self->mPairedDevices[i].playbackDevice)
                        self->mMultipointDeviceMac.overwrite(self->mPairedDevices[i].macAddress);
                }
                return MDR_EVENT_PAIRED_DEVICES_CHANGED;
            }
            Deserialize(PeripheralRetParamPairingDeviceManagementWithBluetoothClassOfDevice, res, cmd);
            self->mPairedDevicesPlaybackDeviceID = res.playbackrightDevice;
            self->mPairedDevices.resize(res.deviceInfo.size());
            for (size_t i = 0; i < self->mPairedDevices.size(); ++i)
            {
                auto& mac = res.deviceInfo.value[i].btDeviceAddress;
                self->mPairedDevices[i].macAddress = {mac.begin(), mac.end()};
                self->mPairedDevices[i].name = res.deviceInfo.value[i].btFriendlyName.value;
                self->mPairedDevices[i].connected = res.deviceInfo.value[i].connectedStatus != 0;
                self->mPairedDevices[i].playbackDevice =
                    res.deviceInfo.value[i].connectedStatus == res.playbackrightDevice;
                if (res.deviceInfo.value[i].connectedStatus == res.playbackrightDevice)
                    self->mMultipointDeviceMac.overwrite(self->mPairedDevices[i].macAddress);
            }
            return MDR_EVENT_PAIRED_DEVICES_CHANGED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandleSafeListeningParamsT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        SafeListeningInquiredType type{};
        if (!ReadInquiredType(cmd, type))
            return MDR_EVENT_UNHANDLED;
        using enum SafeListeningInquiredType;
        switch (type)
        {
        case SAFE_LISTENING_HBS_1:
        case SAFE_LISTENING_HBS_2:
        case SAFE_LISTENING_TWS_1:
        case SAFE_LISTENING_TWS_2:
        {
            Deserialize(SafeListeningNotifyParamSL, res, cmd);
            self->mSafeListeningPreviewMode.overwrite(res.previewMode == OnOffSettingValue::ON);
            return MDR_EVENT_SAFE_LISTENING_CHANGED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandleSafeListeningExtendedParamT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(SafeListeningRetExtendedParam, res, cmd);
        self->mSafeListeningSoundPressure = res.levelPerPeriod;
        return MDR_EVENT_SAFE_LISTENING_CHANGED;
    }

    int MDRHeadphones::HandleCommandV2T2(Span<const UInt8> cmd, MDRCommandSeqNumber)
    {
        auto* self = this;
        using enum Command;
        if (cmd.empty())
            return MDR_EVENT_UNHANDLED;
        const auto command = static_cast<Command>(cmd[0]);
        switch (command)
        {
        case CONNECT_RET_SUPPORT_FUNCTION:
            return HandleSupportFunctionT2(this, cmd);
        case VOICE_GUIDANCE_RET_PARAM:
            return HandleVoiceGuidanceParamT2(this, cmd);
        case PERI_RET_STATUS:
        case PERI_NTFY_STATUS:
            return HandlePeripheralStatusT2(this, cmd);
        case PERI_NTFY_EXTENDED_PARAM:
            return HandlePeripheralNotifyExtendedParamT2(this, cmd);
        case PERI_RET_PARAM:
        case PERI_NTFY_PARAM:
            return HandlePeripheralParamT2(this, cmd);
        case SAFE_LISTENING_NTFY_PARAM:
            return HandleSafeListeningParamsT2(this, cmd);
        case SAFE_LISTENING_RET_EXTENDED_PARAM:
            return HandleSafeListeningExtendedParamT2(this, cmd);
        default:
            MDR_LOG_DEBUG("** Unhandled {}", command);
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }
}
