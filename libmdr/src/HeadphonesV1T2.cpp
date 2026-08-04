#include "Details.hpp"

namespace mdr
{
    using namespace v1::t2;

    namespace
    {
        template <typename Payload>
        void ApplyPairedDevices(MDRHeadphones* self, const Payload& payload)
        {
            self->mPairedDevicesPlaybackDeviceID = payload.playbackrightDevice;
            self->mPairedDevices.clear();
            self->mPairedDevices.reserve(payload.deviceInfo.size());
            for (const auto& device : payload.deviceInfo)
            {
                MDRHeadphones::PeripheralDevice state{
                    .macAddress = device.btDeviceAddress.value,
                    .name = device.btFriendlyName.value,
                    .connected = device.connectedStatus != 0
                };
                if (device.connectedStatus == payload.playbackrightDevice)
                    self->mMultipointDeviceMac.overwrite(state.macAddress);
                self->mPairedDevices.push_back(std::move(state));
            }
        }

        int HandlePeripheralStatus(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            if (cmd.size() < 2 ||
                static_cast<PeripheralInquiredType>(cmd[1]) !=
                    PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT)
                return MDR_HEADPHONES_EVT_UNHANDLED;
            if (static_cast<Command>(cmd[0]) == Command::PERIPHERAL_NTFY_STATUS)
            {
                Deserialize(NotifyPeripheralStatusPairingDeviceManagementClassicBt, result, cmd);
                self->mPairingMode.overwrite(
                    result.status == v1::CommonStatus::ENABLE &&
                    result.bluetoothModeStatus == PeripheralBluetoothModeStatus::INQUIRY_SCAN_MODE);
            }
            else
            {
                Deserialize(RetPeripheralStatus, result, cmd);
                self->mPairingMode.overwrite(
                    result.status == v1::CommonStatus::ENABLE &&
                    result.bluetoothModeStatus == PeripheralBluetoothModeStatus::INQUIRY_SCAN_MODE);
            }
            return MDR_HEADPHONES_EVT_BLUETOOTH_MODE;
        }

        int HandlePeripheralParam(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            if (cmd.size() < 2 ||
                static_cast<PeripheralInquiredType>(cmd[1]) !=
                    PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT)
                return MDR_HEADPHONES_EVT_UNHANDLED;
            if (static_cast<Command>(cmd[0]) == Command::PERIPHERAL_NTFY_PARAM)
            {
                Deserialize(NotifyPeripheralParamPairingDeviceManagementClassicBt, result, cmd);
                ApplyPairedDevices(self, result);
            }
            else
            {
                Deserialize(RetPeripheralParamPairingDeviceManagementClassicBt, result, cmd);
                ApplyPairedDevices(self, result);
            }
            return MDR_HEADPHONES_EVT_CONNECTED_DEVICES;
        }

        int HandleVoiceGuidance(MDRHeadphones* self, Span<const UInt8> cmd)
        {
            const Command command = static_cast<Command>(cmd[0]);
            if (command == Command::VOICE_GUIDANCE_NTFY_PARAM)
            {
                Deserialize(NotifyVoiceGuidanceParamSettingOnOff, result, cmd);
                self->mVoiceGuidanceEnabled.overwrite(
                    result.settingValue == VoiceGuidanceSettingValue::ON);
            }
            else if (command == Command::VOICE_GUIDANCE_RET_PARAM)
            {
                Deserialize(RetVoiceGuidanceParamSettingOnOff, result, cmd);
                self->mVoiceGuidanceEnabled.overwrite(
                    result.settingValue == VoiceGuidanceSettingValue::ON);
            }
            else if (command == Command::VOICE_GUIDANCE_NTFY_STATUS)
            {
                Deserialize(NotifyVoiceGuidanceStatusSettingOnOff, result, cmd);
                self->mVoiceGuidanceEnabled.overwrite(result.status == v1::CommonStatus::ENABLE);
            }
            else
            {
                Deserialize(RetVoiceGuidanceStatusSettingOnOff, result, cmd);
                self->mVoiceGuidanceEnabled.overwrite(result.status == v1::CommonStatus::ENABLE);
            }
            return MDR_HEADPHONES_EVT_VOICE_GUIDANCE_ENABLE;
        }
    }

    int MDRHeadphones::HandleCommandV1T2(Span<const UInt8> cmd, MDRCommandSeqNumber)
    {
        if (cmd.empty())
            return MDR_HEADPHONES_EVT_UNHANDLED;
        switch (static_cast<Command>(cmd[0]))
        {
        case Command::PERIPHERAL_RET_STATUS:
        case Command::PERIPHERAL_NTFY_STATUS:
            return HandlePeripheralStatus(this, cmd);
        case Command::PERIPHERAL_RET_PARAM:
        case Command::PERIPHERAL_NTFY_PARAM:
            return HandlePeripheralParam(this, cmd);
        case Command::VOICE_GUIDANCE_RET_PARAM:
        case Command::VOICE_GUIDANCE_NTFY_PARAM:
        case Command::VOICE_GUIDANCE_RET_STATUS:
        case Command::VOICE_GUIDANCE_NTFY_STATUS:
            return HandleVoiceGuidance(this, cmd);
        default:
            MDR_LOG_DEBUG("** Unhandled V1 T2 {}", static_cast<Command>(cmd[0]));
            return MDR_HEADPHONES_EVT_UNHANDLED;
        }
    }
}
