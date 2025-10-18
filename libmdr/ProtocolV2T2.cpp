#include "ProtocolV2T2.hpp"

namespace mdr::v2::t2
{
    size_t ConnectRetSupportFunction::Serialize(const ConnectRetSupportFunction& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.inquiredType, &ptr);
        MDRPodArray<MessageMdrV2SupportFunction>::Write(data.supportFunctions, &ptr);
        return ptr - out;
    }

    void ConnectRetSupportFunction::Deserialize(UInt8* data, ConnectRetSupportFunction& out)
    {
        MDRPod::Read(&data, out.command);
        MDRPod::Read(&data, out.inquiredType);
        MDRPodArray<MessageMdrV2SupportFunction>::Read(&data,out.supportFunctions);
    }

    void PeripheralDeviceInfo::Read(UInt8** ppSrcBuffer, PeripheralDeviceInfo& out, size_t maxSize)
    {
        MDRPod::Read(ppSrcBuffer, out.btDeviceAddress);
        MDRPod::Read(ppSrcBuffer, out.connectedStatus);
        MDRPrefixedString::Read(ppSrcBuffer, out.btFriendlyName, maxSize);
    }

    size_t PeripheralDeviceInfo::Write(const PeripheralDeviceInfo& data, UInt8** ppDstBuffer)
    {
        UInt8* ptr = *ppDstBuffer;
        MDRPod::Write(data.btDeviceAddress, ppDstBuffer);
        MDRPod::Write(data.connectedStatus, ppDstBuffer);
        MDRPrefixedString::Write(data.btFriendlyName, ppDstBuffer);
        return *ppDstBuffer - ptr;
    }

    void PeripheralDeviceInfoWithBluetoothClassOfDevice::Read(UInt8** ppSrcBuffer,
        PeripheralDeviceInfoWithBluetoothClassOfDevice& out, size_t maxSize)
    {
        MDRPod::Read(ppSrcBuffer, out.btDeviceAddress);
        MDRPod::Read(ppSrcBuffer, out.connectedStatus);
        MDRPod::Read(ppSrcBuffer, out.bluetoothClassOfDevice);
        MDRPrefixedString::Read(ppSrcBuffer, out.btFriendlyName, maxSize);
    }

    size_t PeripheralDeviceInfoWithBluetoothClassOfDevice::Write(
        const PeripheralDeviceInfoWithBluetoothClassOfDevice& data, UInt8** ppDstBuffer)
    {
        UInt8* ptr = *ppDstBuffer;
        MDRPod::Write(data.btDeviceAddress, ppDstBuffer);
        MDRPod::Write(data.connectedStatus, ppDstBuffer);
        MDRPod::Write(data.bluetoothClassOfDevice, ppDstBuffer);
        MDRPrefixedString::Write(data.btFriendlyName, ppDstBuffer);
        return *ppDstBuffer - ptr;
    }

    size_t PeripheralParamPairingDeviceManagementClassicBt::Serialize(
        const PeripheralParamPairingDeviceManagementClassicBt& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &out);
        MDRArray<PeripheralDeviceInfo>::Write(data.deviceList, &out);
        MDRPod::Write(data.playbackDevice, &out);
        return out - ptr;
    }

    void PeripheralParamPairingDeviceManagementClassicBt::Deserialize(UInt8* data,
        PeripheralParamPairingDeviceManagementClassicBt& out)
    {
        MDRPod::Read(&data, out.base);
        MDRArray<PeripheralDeviceInfo>::Read(&data, out.deviceList);
        MDRPod::Read(&data, out.playbackDevice);
    }

    size_t PeripheralParamPairingDeviceManagementWithBluetoothClassOfDevice::Serialize(
        const PeripheralParamPairingDeviceManagementWithBluetoothClassOfDevice& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &out);
        MDRArray<PeripheralDeviceInfoWithBluetoothClassOfDevice>::Write(data.deviceList, &out);
        MDRPod::Write(data.playbackDevice, &out);
        return out - ptr;
    }

    void PeripheralParamPairingDeviceManagementWithBluetoothClassOfDevice::Deserialize(UInt8* data,
        PeripheralParamPairingDeviceManagementWithBluetoothClassOfDevice& out)
    {
        MDRPod::Read(&data, out.base);
        MDRArray<PeripheralDeviceInfoWithBluetoothClassOfDevice>::Read(&data, out.deviceList);
        MDRPod::Read(&data, out.playbackDevice);
    }

    size_t SafeListeningNotifyStatusHbs1::Serialize(const SafeListeningNotifyStatusHbs1& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &ptr);
        MDRPodArray<SafeListeningData1>::Write(data.data, &ptr);
        return ptr - out;
    }

    void SafeListeningNotifyStatusHbs1::Deserialize(UInt8* data, SafeListeningNotifyStatusHbs1& out)
    {
        MDRPod::Read(&data, out.base);
        MDRPodArray<SafeListeningData1>::Read(&data, out.data);
    }

    size_t SafeListeningNotifyStatusHbs2::Serialize(const SafeListeningNotifyStatusHbs2& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &ptr);
        MDRPodArray<SafeListeningData2>::Write(data.data, &ptr);
        return ptr - out;
    }

    void SafeListeningNotifyStatusHbs2::Deserialize(UInt8* data, SafeListeningNotifyStatusHbs2& out)
    {
        MDRPod::Read(&data, out.base);
        MDRPodArray<SafeListeningData2>::Read(&data, out.data);
    }

    size_t SafeListeningNotifyStatusTws1::Serialize(const SafeListeningNotifyStatusTws1& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &ptr);
        MDRPodArray<SafeListeningData1>::Write(data.data, &ptr);
        return ptr - out;
    }

    void SafeListeningNotifyStatusTws1::Deserialize(UInt8* data, SafeListeningNotifyStatusTws1& out)
    {
        MDRPod::Read(&data, out.base);
        MDRPodArray<SafeListeningData1>::Read(&data, out.data);
    }

    size_t SafeListeningNotifyStatusTws2::Serialize(const SafeListeningNotifyStatusTws2& data, UInt8* out)
    {
        UInt8* ptr = out;
        MDRPod::Write(data.base, &ptr);
        MDRPodArray<SafeListeningData2>::Write(data.data, &ptr);
        return ptr - out;
    }

    void SafeListeningNotifyStatusTws2::Deserialize(UInt8* data, SafeListeningNotifyStatusTws2& out)
    {
        MDRPod::Read(&data, out.base);
        MDRPodArray<SafeListeningData2>::Read(&data, out.data);
    }
}
