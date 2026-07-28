#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// C++/WinRT headers
#include <Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#include <Windows.Devices.Bluetooth.h>
#include <Windows.Devices.Enumeration.h>
#include <Windows.Foundation.Collections.h>
#include <Windows.Foundation.h>
#include <Windows.Storage.Streams.h>
#include <wrl/event.h>
#include "WaitForCompletion.h"

#include <atomic>
#include <fmt/format.h>
#include <mdr/Protocol.hpp>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <mdr-c/Platform/PlatformWindowsBLE.h>
#include "../Platform.hpp"

using namespace Microsoft::WRL;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::Devices::Bluetooth;
using namespace ABI::Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace ABI::Windows::Devices::Enumeration;
using namespace ABI::Windows::Storage::Streams;

#define RETURN_IF_FAILED(expr)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        HRESULT hr = (expr);                                                                                           \
        if (FAILED(hr))                                                                                                \
        { /*MDR_LOG("[BLE] HRESULT 0x{:08X} at {}:{}", hr, __FILE__, __LINE__);*/                                      \
            return hr;                                                                                                 \
        }                                                                                                              \
    }                                                                                                                  \
    while (0)
#define RETURN_IF_NULL_ALLOC(ptr)                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((ptr) == nullptr)                                                                                          \
        { /*MDR_LOG("[BLE] Out of memory at {}:{}", __FILE__, __LINE__);*/                                             \
            return E_OUTOFMEMORY;                                                                                      \
        }                                                                                                              \
    }                                                                                                                  \
    while (0)

mdr::String format_as(std::wstring_view value)
{
    int const size = WideCharToMultiByte(65001 /*CP_UTF8*/, 0, value.data(), static_cast<int32_t>(value.size()),
                                         nullptr, 0, nullptr, nullptr);

    if (size == 0)
    {
        return {};
    }

    mdr::String result(size, '?');
    WideCharToMultiByte(65001 /*CP_UTF8*/, 0, value.data(), static_cast<int32_t>(value.size()), result.data(), size,
                        nullptr, nullptr);
    return result;
}

namespace Microsoft::WRL::Wrappers
{
    mdr::String format_as(HString const& hstr)
    {
        UINT32 len;
        const wchar_t* wstr = hstr.GetRawBuffer(&len);
        return ::format_as(std::wstring_view(wstr, len));
    }
} // namespace Microsoft::WRL::Wrappers

HRESULT IInspectable_Close(IInspectable* pInspectable)
{
    ComPtr<IClosable> spClosable;
    HRESULT hr = pInspectable->QueryInterface(IID_PPV_ARGS(&spClosable));
    if (SUCCEEDED(hr))
    {
        hr = spClosable->Close();
    }
    return hr;
}

constexpr WORD FACILITY_MDR = 0x200;

constexpr HRESULT HResultFromMdr(WORD code)
{
    return MAKE_HRESULT(code >= MDR_RESULT_ERROR_GENERAL, FACILITY_MDR, code);
}

constexpr bool IsMdrHResult(HRESULT hr) { return HRESULT_FACILITY(hr) == FACILITY_MDR; }

constexpr int HResultToMdrResult(HRESULT hr)
{
    if (!IsMdrHResult(hr))
        return MDR_RESULT_ERROR_NET;
    return static_cast<int>(HRESULT_CODE(hr));
}

// GATT characteristic property bitmask values
enum GattPropertyBit
{
    GATT_PROP_READ = 1,
    GATT_PROP_WRITE = 2,
    GATT_PROP_WRITE_NO_RESP = 4,
    GATT_PROP_NOTIFY = 8,
    GATT_PROP_INDICATE = 16,
};

// Helper: convert winrt::guid to string "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX"
// Used both for logging and for service UUID matching in Connect().
static mdr::String GuidToString(const GUID& g)
{
    mdr::String res =
        mdr::Format("{:08X}-{:04X}-{:04X}-{:02X}{:02X}-{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}", g.Data1, g.Data2, g.Data3,
                    g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3], g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]);
    return res;
}

// Helper: convert GattCharacteristicProperties to our bitmask
static int PropertiesToBitmask(GattCharacteristicProperties props)
{
    int mask = 0;
    if ((props & GattCharacteristicProperties_Read) != GattCharacteristicProperties_None)
        mask |= GATT_PROP_READ;
    if ((props & GattCharacteristicProperties_Write) != GattCharacteristicProperties_None)
        mask |= GATT_PROP_WRITE;
    if ((props & GattCharacteristicProperties_WriteWithoutResponse) != GattCharacteristicProperties_None)
        mask |= GATT_PROP_WRITE_NO_RESP;
    if ((props & GattCharacteristicProperties_Notify) != GattCharacteristicProperties_None)
        mask |= GATT_PROP_NOTIFY;
    if ((props & GattCharacteristicProperties_Indicate) != GattCharacteristicProperties_None)
        mask |= GATT_PROP_INDICATE;
    return mask;
}

// Helper: convert properties bitmask to human-readable string
static mdr::String PropertiesToString(int mask)
{
    mdr::String result;
    if (mask & GATT_PROP_READ)
        result += "Read ";
    if (mask & GATT_PROP_WRITE)
        result += "Write ";
    if (mask & GATT_PROP_WRITE_NO_RESP)
        result += "WriteNoResp ";
    if (mask & GATT_PROP_NOTIFY)
        result += "Notify ";
    if (mask & GATT_PROP_INDICATE)
        result += "Indicate ";
    if (result.empty())
        result = "None";
    return result;
}

struct MDRConnectionWindowsBLE
{
    MDRConnection mdrConn;
    mdr::String lastError;

    // WinRT device handles
    ComPtr<IBluetoothLEDevice> device;
    ComPtr<IGattDeviceService> service;
    ComPtr<IGattCharacteristic> writeChar;
    ComPtr<IGattCharacteristic> notifyChar;
    EventRegistrationToken notifyToken{};

    // Receive buffer (filled by GATT notifications)
    std::mutex rxMutex;
    mdr::Vector<uint8_t> rxBuffer;
    HANDLE rxEvent;

    // Write mode (cached at connect time to avoid STA WinRT access)
    bool useWriteWithoutResponse{false};

    // Connection state
    std::atomic<bool> connected{false};
    HANDLE connectEvent;
    std::atomic<int> connectResult{MDR_RESULT_INPROGRESS};
    std::jthread connectThread;

    // GATT enumeration async state
    std::atomic<int> enumResult{MDR_RESULT_INPROGRESS};
    std::jthread enumThread;
    HANDLE enumEvent;

    MDRConnectionWindowsBLE() noexcept :
        lastError(""), mdrConn({.user = this,
                                .connect = Connect,
                                .disconnect = Disconnect,
                                .recv = Recv,
                                .send = Send,
                                .poll = Poll,
                                .getDevicesList = GetDevicesList,
                                .freeDevicesList = FreeDevicesList,
                                .getLastError = GetLastError})
    {
        rxEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
        connectEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
        enumEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
        MDR_LOG("[BLE] MDRConnectionWindowsBLE created");
    }

    ~MDRConnectionWindowsBLE()
    {
        if (connectThread.joinable())
            connectThread.request_stop();
        if (enumThread.joinable())
            enumThread.request_stop();
        CloseHandle(rxEvent);
        CloseHandle(connectEvent);
        CloseHandle(enumEvent);
        MDR_LOG("[BLE] MDRConnectionWindowsBLE destroyed");
    }

    // Run a callable on an MTA thread to avoid STA assertion failures.
    // The UI thread (SDL/ImGui) is STA, but WinRT BLE APIs require MTA.
    // See:
    // https://learn.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/concurrency#programming-with-thread-affinity-in-mind
    // See: https://learn.microsoft.com/en-us/windows/win32/com/multithreaded-apartments
    template <typename F>
    static HRESULT RunOnMTA(F&& func)
    {
        struct Context
        {
            F* Func;
            HRESULT Hr;
        };

        Context ctx = {&func, E_FAIL};

        HANDLE thread = CreateThread(
            nullptr, 0,
            [](LPVOID p) -> DWORD
            {
                auto* ctx = static_cast<Context*>(p);

                HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
                if (FAILED(hr))
                {
                    ctx->Hr = hr;
                    return 0;
                }

                ctx->Hr = (*ctx->Func)();

                CoUninitialize();
                return 0;
            },
            &ctx, 0, nullptr);

        if (!thread)
            return HRESULT_FROM_WIN32(::GetLastError());

        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);

        return ctx.Hr;
    }

    // --- MDRConnection vtable implementation ---

    static int GetDevicesList(void* user, MDRDeviceInfo** ppList, int* pCount) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        MDR_LOG("[BLE] GetDevicesList called");

        HRESULT hr = RunOnMTA(
            [&]() -> HRESULT
            {
                ComPtr<IBluetoothLEDeviceStatics> spBluetoothLEDeviceStatics;
                RETURN_IF_FAILED(GetActivationFactory(
                    Wrappers::HStringReference(RuntimeClass_Windows_Devices_Bluetooth_BluetoothLEDevice).Get(),
                    &spBluetoothLEDeviceStatics));

                ComPtr<IBluetoothLEDeviceStatics2> bluetoothLEDeviceStatics2;
                RETURN_IF_FAILED(spBluetoothLEDeviceStatics.As(&bluetoothLEDeviceStatics2));

                // Get AQS selector for paired BLE devices
                Wrappers::HString hstrSelector;
                RETURN_IF_FAILED(
                    bluetoothLEDeviceStatics2->GetDeviceSelectorFromPairingState(true, hstrSelector.GetAddressOf()));
                MDR_LOG("[BLE] Using BLE device selector for paired devices");

                // Find all matching devices
                ComPtr<IDeviceInformationStatics> spDeviceInfoStatics;
                RETURN_IF_FAILED(GetActivationFactory(
                    Wrappers::HStringReference(RuntimeClass_Windows_Devices_Enumeration_DeviceInformation).Get(),
                    &spDeviceInfoStatics));

                ComPtr<IAsyncOperation<DeviceInformationCollection*>> spDeviceInfosOp;
                RETURN_IF_FAILED(spDeviceInfoStatics->FindAllAsyncAqsFilter(hstrSelector.Get(), &spDeviceInfosOp));

                ComPtr<IVectorView<DeviceInformation*>> spDeviceInfos;
                RETURN_IF_FAILED(WaitForCompletionAndGetResults(spDeviceInfosOp.Get(), &spDeviceInfos));

                uint32_t cDeviceInfos;
                RETURN_IF_FAILED(spDeviceInfos->get_Size(&cDeviceInfos));
                MDR_LOG("[BLE] FindAllAsync returned {} device(s)", cDeviceInfos);

                mdr::Vector<MDRDeviceInfo> devices;

                for (uint32_t i = 0; i < cDeviceInfos; i++)
                {
                    ComPtr<IDeviceInformation> spDevInfo;
                    RETURN_IF_FAILED(spDeviceInfos->GetAt(i, &spDevInfo));

                    Wrappers::HString hstrId;
                    RETURN_IF_FAILED(spDevInfo->get_Id(hstrId.GetAddressOf()));

                    Wrappers::HString hstrName;
                    RETURN_IF_FAILED(spDevInfo->get_Name(hstrName.GetAddressOf()));

                    uint32_t cchName;
                    LPCWSTR pszName = hstrName.GetRawBuffer(&cchName);
                    mdr::String name = format_as(std::wstring_view(pszName, cchName));

                    MDR_LOG("[BLE] Device #{}: id=\"{}\" name=\"{}\"", i, hstrId, hstrName);

                    // Open the BLE device to get its Bluetooth address
                    auto openDevice = [&]() -> HRESULT
                    {
                        ComPtr<IAsyncOperation<BluetoothLEDevice*>> bleDeviceOp;
                        RETURN_IF_FAILED(spBluetoothLEDeviceStatics->FromIdAsync(hstrId.Get(), &bleDeviceOp));

                        ComPtr<IBluetoothLEDevice> bleDevice;
                        RETURN_IF_FAILED(WaitForCompletionAndGetResults(bleDeviceOp.Get(), &bleDevice));
                        if (bleDevice == nullptr)
                        {
                            MDR_LOG("[BLE]   Skipping device #{}: FromIdAsync returned null", i);
                            return S_FALSE;
                        }

                        uint64_t addr;
                        RETURN_IF_FAILED(bleDevice->get_BluetoothAddress(&addr));
                        mdr::String macAddress = mdr::Format(
                            "{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}", (addr >> 40) & 0xFF, (addr >> 32) & 0xFF,
                            (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF);

                        MDR_LOG("[BLE]   BLE address: {}", macAddress);

                        devices.emplace_back();
                        auto& back = devices.back();
                        strncpy(back.szDeviceName, name.c_str(), sizeof(back.szDeviceName) - 1);
                        back.szDeviceName[sizeof(back.szDeviceName) - 1] = '\0';
                        strncpy(back.szDeviceMacAddress, macAddress.c_str(), sizeof(back.szDeviceMacAddress) - 1);
                        back.szDeviceMacAddress[sizeof(back.szDeviceMacAddress) - 1] = '\0';

                        (void)IInspectable_Close(bleDevice.Get());

                        return S_OK;
                    };
                    HRESULT hrOpenDevice = openDevice();
                    if (FAILED(hrOpenDevice))
                    {
                        MDR_LOG("[BLE]   Error opening device #{}: HRESULT 0x{:08X}", i, hrOpenDevice);
                    }
                }

                if (devices.empty())
                {
                    *ppList = nullptr;
                    *pCount = 0;
                }
                else
                {
                    *ppList = mdr::MDRAllocator<MDRDeviceInfo>().allocate(devices.size());
                    RETURN_IF_NULL_ALLOC(*ppList);
                    std::memcpy(*ppList, devices.data(), devices.size() * sizeof(MDRDeviceInfo));
                    *pCount = static_cast<int>(devices.size());
                }

                MDR_LOG("[BLE] GetDevicesList returning {} BLE device(s)", *pCount);
                return S_OK;
            }); // end RunOnMTA
        if (FAILED(hr))
        {
            ptr->lastError = mdr::Format("GetDevicesList failed with HRESULT 0x{:08X}", (uint32_t)hr);
            MDR_LOG("[BLE] GetDevicesList error: {}", ptr->lastError);
            return MDR_RESULT_ERROR_NET;
        }

        return MDR_RESULT_OK;
    }

    static int Connect(void* user, const char* macAddress, const char* serviceUUID) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        MDR_LOG("[BLE] Connect called: mac={} serviceUUID={}", macAddress, serviceUUID);

        // Explicitly stop/join and clear state before starting a new connect
        Disconnect(ptr);

        // Reset state
        ptr->connected = false;
        ptr->connectResult = MDR_RESULT_INPROGRESS;
        ResetEvent(ptr->connectEvent);
        ResetEvent(ptr->rxEvent);
        {
            std::lock_guard lock(ptr->rxMutex);
            ptr->rxBuffer.clear();
        }

        uint64_t btAddr = macAddressToULL(macAddress);
        if (btAddr == ~0ULL)
        {
            ptr->SetLastError("Invalid MAC address format");
            MDR_LOG("[BLE] Connect failed: invalid MAC address");
            return MDR_RESULT_ERROR_BAD_ADDRESS;
        }

        mdr::String svcUUID(serviceUUID);

        // Launch connection on background thread
        ptr->connectThread = std::jthread(
            [ptr, btAddr, svcUUID](std::stop_token stop)
            {
                MDR_LOG("[BLE] Connect thread started (MTA), addr=0x{:X}", (unsigned long long)btAddr);
                auto inner = [&]() -> HRESULT
                {
                    RETURN_IF_FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

                    ComPtr<IBluetoothLEDeviceStatics> spBluetoothLEDeviceStatics;
                    RETURN_IF_FAILED(GetActivationFactory(
                        Wrappers::HStringReference(RuntimeClass_Windows_Devices_Bluetooth_BluetoothLEDevice).Get(),
                        &spBluetoothLEDeviceStatics));

                    // Open BLE device by address
                    ComPtr<IAsyncOperation<BluetoothLEDevice*>> spBleDeviceOp;
                    RETURN_IF_FAILED(spBluetoothLEDeviceStatics->FromBluetoothAddressAsync(btAddr, &spBleDeviceOp));
                    RETURN_IF_FAILED(WaitForCompletionAndGetResults(spBleDeviceOp.Get(), &ptr->device));
                    if (ptr->device == nullptr)
                    {
                        ptr->lastError = "BLE device not found or not reachable";
                        MDR_LOG("[BLE] FromBluetoothAddressAsync returned null");
                        return HResultFromMdr(MDR_RESULT_ERROR_NOT_FOUND);
                    }

                    Wrappers::HString hstrName;
                    RETURN_IF_FAILED(ptr->device->get_Name(hstrName.GetAddressOf()));
                    MDR_LOG("[BLE] BLE device opened: {}", hstrName);

                    if (stop.stop_requested())
                        return E_ABORT;

                    // Parse service UUID and find matching GATT service
                    GUID svcGuid;
                    {
                        uint8_t uuidBytes[16];
                        if (serviceUUIDtoBytes(svcUUID.c_str(), uuidBytes) != 0)
                        {
                            ptr->lastError = "Invalid service UUID format";
                            MDR_LOG("[BLE] Invalid service UUID: {}", svcUUID);
                            return HResultFromMdr(MDR_RESULT_ERROR_BAD_ADDRESS);
                        }
                        std::memcpy(&svcGuid, uuidBytes, 16);
                    }

                    MDR_LOG("[BLE] Looking for GATT service: {}", svcUUID);

                    ComPtr<IBluetoothLEDevice3> spBleDevice3;
                    RETURN_IF_FAILED(ptr->device.As(&spBleDevice3));

                    ComPtr<IAsyncOperation<GattDeviceServicesResult*>> spServicesOp;
                    RETURN_IF_FAILED(
                        spBleDevice3->GetGattServicesWithCacheModeAsync(BluetoothCacheMode_Uncached, &spServicesOp));

                    ComPtr<IGattDeviceServicesResult> spServicesResult;
                    RETURN_IF_FAILED(WaitForCompletionAndGetResults(spServicesOp.Get(), &spServicesResult));

                    GattCommunicationStatus status;
                    RETURN_IF_FAILED(spServicesResult->get_Status(&status));

                    ComPtr<IVectorView<GattDeviceService*>> spServices;
                    RETURN_IF_FAILED(spServicesResult->get_Services(&spServices));

                    uint32_t cServices;
                    RETURN_IF_FAILED(spServices->get_Size(&cServices));

                    MDR_LOG("[BLE] GetGattServicesAsync status: {}, count: {}", (int)status, cServices);

                    if (status != GattCommunicationStatus_Success)
                    {
                        ptr->lastError = mdr::Format("GATT service discovery failed (status={})", (int)status);
                        MDR_LOG("[BLE] {}", ptr->lastError);
                        return HResultFromMdr(MDR_RESULT_ERROR_NET);
                    }

                    if (stop.stop_requested())
                        return E_ABORT;

                    // Find the target service by UUID string comparison
                    ComPtr<IGattDeviceService> spTargetService;
                    for (uint32_t i = 0; i < cServices; i++)
                    {
                        ComPtr<IGattDeviceService> spSvc;
                        RETURN_IF_FAILED(spServices->GetAt(i, &spSvc));

                        GUID uuid;
                        RETURN_IF_FAILED(spSvc->get_Uuid(&uuid));

                        mdr::String foundUUID = GuidToString(uuid);
                        MDR_LOG("[BLE]   Found service: {}", foundUUID);
                        if (_stricmp(foundUUID.c_str(), svcUUID.c_str()) == 0)
                        {
                            spTargetService = spSvc;
                            MDR_LOG("[BLE]   -> Matched target service!");
                            break;
                        }
                    }

                    if (spTargetService == nullptr)
                    {
                        ptr->lastError = mdr::Format("GATT service {} not found on device", svcUUID);
                        MDR_LOG("[BLE] {}", ptr->lastError);
                        return HResultFromMdr(MDR_RESULT_ERROR_NOT_FOUND);
                    }

                    ptr->service = spTargetService;

                    // Enumerate characteristics to find Write and Notify ones
                    ComPtr<IGattDeviceService3> spService3;
                    RETURN_IF_FAILED(spTargetService.As(&spService3));

                    ComPtr<IAsyncOperation<GattCharacteristicsResult*>> spCharsOp;
                    RETURN_IF_FAILED(
                        spService3->GetCharacteristicsWithCacheModeAsync(BluetoothCacheMode_Uncached, &spCharsOp));

                    ComPtr<IGattCharacteristicsResult> spCharsResult;
                    RETURN_IF_FAILED(WaitForCompletionAndGetResults(spCharsOp.Get(), &spCharsResult));

                    GattCommunicationStatus charsStatus;
                    RETURN_IF_FAILED(spCharsResult->get_Status(&charsStatus));

                    if (charsStatus != GattCommunicationStatus_Success)
                    {
                        ptr->lastError = "Failed to enumerate GATT characteristics";
                        MDR_LOG("[BLE] GetCharacteristicsAsync failed: status={}", (int)charsStatus);
                        return HResultFromMdr(MDR_RESULT_ERROR_NET);
                    }

                    ComPtr<IVectorView<GattCharacteristic*>> spCharacteristics;
                    RETURN_IF_FAILED(spCharsResult->get_Characteristics(&spCharacteristics));

                    uint32_t cCharacteristics;
                    RETURN_IF_FAILED(spCharacteristics->get_Size(&cCharacteristics));

                    MDR_LOG("[BLE] Found {} characteristic(s) in service", cCharacteristics);

                    // for (auto const& ch : charsResult.Characteristics())
                    for (uint32_t i = 0; i < cCharacteristics; i++)
                    {
                        ComPtr<IGattCharacteristic> spCh;
                        RETURN_IF_FAILED(spCharacteristics->GetAt(i, &spCh));

                        GattCharacteristicProperties props;
                        RETURN_IF_FAILED(spCh->get_CharacteristicProperties(&props));

                        GUID uuid;
                        RETURN_IF_FAILED(spCh->get_Uuid(&uuid));
                        MDR_LOG("[BLE]   Characteristic: {} [{}]", GuidToString(uuid),
                                PropertiesToString(PropertiesToBitmask(props)));

                        // Pick the first writable characteristic (prefer WriteWithoutResponse for lower latency)
                        if (ptr->writeChar == nullptr &&
                            ((props & GattCharacteristicProperties_Write) != GattCharacteristicProperties_None ||
                             (props & GattCharacteristicProperties_WriteWithoutResponse) !=
                                 GattCharacteristicProperties_None))
                        {
                            ptr->writeChar = spCh;
                            ptr->useWriteWithoutResponse =
                                (props & GattCharacteristicProperties_WriteWithoutResponse) !=
                                GattCharacteristicProperties_None;
                            MDR_LOG("[BLE]   -> Selected as WRITE characteristic (writeNoResp={})",
                                    (int)ptr->useWriteWithoutResponse);
                        }

                        // Pick the first notifiable characteristic
                        if (ptr->notifyChar == nullptr &&
                            ((props & GattCharacteristicProperties_Notify) != GattCharacteristicProperties_None ||
                             (props & GattCharacteristicProperties_Indicate) != GattCharacteristicProperties_None))
                        {
                            ptr->notifyChar = spCh;
                            MDR_LOG("[BLE]   -> Selected as NOTIFY characteristic");
                        }
                    }

                    if (ptr->writeChar == nullptr)
                    {
                        ptr->lastError = "No writable GATT characteristic found in service";
                        MDR_LOG("[BLE] {}", ptr->lastError);
                        return HResultFromMdr(MDR_RESULT_ERROR_NOT_FOUND);
                    }

                    if (ptr->notifyChar == nullptr)
                    {
                        ptr->lastError = "No notifiable GATT characteristic found in service";
                        MDR_LOG("[BLE] {}", ptr->lastError);
                        return HResultFromMdr(MDR_RESULT_ERROR_NOT_FOUND);
                    }

                    if (stop.stop_requested())
                        return E_ABORT;

                    // Subscribe to notifications
                    ComPtr<IAsyncOperation<GattCommunicationStatus>> spCccdOp;
                    RETURN_IF_FAILED(ptr->notifyChar->WriteClientCharacteristicConfigurationDescriptorAsync(
                        GattClientCharacteristicConfigurationDescriptorValue_Notify, &spCccdOp));

                    GattCommunicationStatus cccdResult;
                    RETURN_IF_FAILED(WaitForCompletionAndGetResults(spCccdOp.Get(), &cccdResult));

                    if (cccdResult != GattCommunicationStatus_Success)
                    {
                        // Try Indicate if Notify fails
                        MDR_LOG("[BLE] Notify subscription failed (status={}), trying Indicate", (int)cccdResult);
                        RETURN_IF_FAILED(ptr->notifyChar->WriteClientCharacteristicConfigurationDescriptorAsync(
                            GattClientCharacteristicConfigurationDescriptorValue_Indicate, &spCccdOp));
                        RETURN_IF_FAILED(WaitForCompletionAndGetResults(spCccdOp.Get(), &cccdResult));
                    }

                    if (cccdResult != GattCommunicationStatus_Success)
                    {
                        ptr->lastError =
                            mdr::Format("Failed to subscribe to notifications (status={})", (int)cccdResult);
                        MDR_LOG("[BLE] {}", ptr->lastError);
                        return HResultFromMdr(MDR_RESULT_ERROR_NET);
                    }

                    MDR_LOG("[BLE] Notification subscription OK");

                    // Register ValueChanged handler
                    auto callback = Callback<ITypedEventHandler<GattCharacteristic*, GattValueChangedEventArgs*>>(
                        [ptr](IGattCharacteristic* sender, IGattValueChangedEventArgs* args) -> HRESULT
                        {
                            ComPtr<IBuffer> spBuffer;
                            RETURN_IF_FAILED(args->get_CharacteristicValue(&spBuffer));

                            ComPtr<IDataReaderStatics> spDataReaderStatics;
                            RETURN_IF_FAILED(GetActivationFactory(
                                Wrappers::HStringReference(RuntimeClass_Windows_Storage_Streams_DataReader).Get(),
                                &spDataReaderStatics));

                            ComPtr<IDataReader> spReader;
                            RETURN_IF_FAILED(spDataReaderStatics->FromBuffer(spBuffer.Get(), &spReader));

                            uint32_t len;
                            RETURN_IF_FAILED(spReader->get_UnconsumedBufferLength(&len));

                            std::lock_guard lock(ptr->rxMutex);
                            size_t oldSize = ptr->rxBuffer.size();
                            ptr->rxBuffer.resize(oldSize + len);
                            RETURN_IF_FAILED(spReader->ReadBytes(len, ptr->rxBuffer.data() + oldSize));

                            MDR_LOG("[BLE] Notification received: {} bytes (buffer now {} bytes)", len,
                                    ptr->rxBuffer.size());
                            SetEvent(ptr->rxEvent);
                            return S_OK;
                        });
                    RETURN_IF_NULL_ALLOC(callback);
                    RETURN_IF_FAILED(ptr->notifyChar->add_ValueChanged(callback.Get(), &ptr->notifyToken));

                    return S_OK;
                };

                HRESULT hr = inner();
                if (SUCCEEDED(hr))
                {
                    ptr->connected = true;
                    ptr->connectResult = MDR_RESULT_OK;
                    ptr->lastError = "Connected via BLE GATT";
                    MDR_LOG("[BLE] BLE GATT connection established!");
                    SetEvent(ptr->connectEvent);
                }
                else if (hr != E_ABORT)
                {
                    if (IsMdrHResult(hr))
                    {
                        ptr->connectResult = HResultToMdrResult(hr);
                    }
                    else
                    {
                        ptr->lastError = mdr::Format("Connect thread failed with HRESULT 0x{:08X}", (uint32_t)hr);
                        MDR_LOG("[BLE] Connect thread error: {}", ptr->lastError);
                        ptr->connectResult = MDR_RESULT_ERROR_NET;
                    }
                    SetEvent(ptr->connectEvent);
                }
            });

        ptr->SetLastError("Connecting via BLE GATT...");
        return MDR_RESULT_INPROGRESS;
    }

    static void Disconnect(void* user) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        MDR_LOG("[BLE] Disconnect called");

        // Stop connect thread if still running
        if (ptr->connectThread.joinable())
        {
            ptr->connectThread.request_stop();
            ptr->connectThread.join();
        }

        // WinRT cleanup must run on MTA thread
        HRESULT hr = RunOnMTA(
            [ptr]() -> HRESULT
            {
                // Unsubscribe from notifications
                if (ptr->notifyChar != nullptr)
                {
                    (void)ptr->notifyChar->remove_ValueChanged(ptr->notifyToken);
                    {
                        ComPtr<IAsyncOperation<GattCommunicationStatus>> cccdOp;
                        if (SUCCEEDED(ptr->notifyChar->WriteClientCharacteristicConfigurationDescriptorAsync(
                                GattClientCharacteristicConfigurationDescriptorValue_None, &cccdOp)))
                        {
                            GattCommunicationStatus cccdResult;
                            WaitForCompletion(cccdOp.Get());
                        }
                    }
                    ptr->notifyChar = nullptr;
                }

                ptr->writeChar = nullptr;

                if (ptr->service != nullptr)
                {
                    (void)IInspectable_Close(ptr->service.Get());
                    ptr->service = nullptr;
                }

                if (ptr->device != nullptr)
                {
                    (void)IInspectable_Close(ptr->device.Get());
                    ptr->device = nullptr;
                }

                return S_OK;
            });
        if (FAILED(hr))
        {
            MDR_LOG("[BLE] Disconnect: error during WinRT cleanup (ignored)");
            ptr->notifyChar = nullptr;
            ptr->writeChar = nullptr;
            ptr->service = nullptr;
            ptr->device = nullptr;
        }

        ptr->connected = false;
        ResetEvent(ptr->connectEvent);
        ResetEvent(ptr->rxEvent);
        {
            std::lock_guard lock(ptr->rxMutex);
            ptr->rxBuffer.clear();
        }

        MDR_LOG("[BLE] Disconnected");
    }

    static int Recv(void* user, char* dst, int size, int* pReceived) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        if (!ptr->connected)
            return MDR_RESULT_ERROR_NO_CONNECTION;

        // Use try_lock to avoid blocking the UI thread if the notification
        // handler currently holds the mutex.
        std::unique_lock lock(ptr->rxMutex, std::try_to_lock);
        if (!lock.owns_lock())
            return MDR_RESULT_INPROGRESS;

        if (ptr->rxBuffer.empty())
            return MDR_RESULT_INPROGRESS;

        // Parentheses around std::min prevent Windows min/max macro interference
        int toCopy = (std::min)(size, static_cast<int>(ptr->rxBuffer.size()));
        std::memcpy(dst, ptr->rxBuffer.data(), toCopy);
        ptr->rxBuffer.erase(ptr->rxBuffer.begin(), ptr->rxBuffer.begin() + toCopy);

        if (ptr->rxBuffer.empty())
            ResetEvent(ptr->rxEvent);

        *pReceived = toCopy;
        return MDR_RESULT_OK;
    }

    static int Send(void* user, const char* src, int size, int* pSent) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        if (!ptr->connected || !ptr->writeChar)
            return MDR_RESULT_ERROR_NO_CONNECTION;

        MDR_LOG("[BLE] Send: {} bytes", size);

        // Copy data for the MTA thread
        uint8_t* data = mdr::MDRAllocator<uint8_t>().allocate(size);
        if (data == nullptr)
        {
            ptr->lastError = "Out of memory for send buffer";
            MDR_LOG("[BLE] Send error: {}", ptr->lastError);
            return MDR_RESULT_ERROR_NET;
        }
        std::memcpy(data, src, size);

        HRESULT hr = RunOnMTA(
            [&]() -> HRESULT
            {
                ComPtr<IDataWriter> spWriter;
                RETURN_IF_FAILED(ActivateInstance(
                    Wrappers::HStringReference(RuntimeClass_Windows_Storage_Streams_DataWriter).Get(), &spWriter));
                RETURN_IF_FAILED(spWriter->WriteBytes(size, data));

                auto writeOption = ptr->useWriteWithoutResponse ? GattWriteOption_WriteWithoutResponse
                                                                : GattWriteOption_WriteWithResponse;

                ComPtr<IBuffer> spBuffer;
                RETURN_IF_FAILED(spWriter->DetachBuffer(&spBuffer));

                ComPtr<IAsyncOperation<GattCommunicationStatus>> spWriteOp;
                RETURN_IF_FAILED(ptr->writeChar->WriteValueWithOptionAsync(spBuffer.Get(), writeOption, &spWriteOp));

                GattCommunicationStatus status;
                RETURN_IF_FAILED(WaitForCompletionAndGetResults(spWriteOp.Get(), &status));

                if (status != GattCommunicationStatus_Success)
                {
                    ptr->lastError = mdr::Format("GATT write failed (status={})", (int)status);
                    MDR_LOG("[BLE] {}", ptr->lastError);
                    return HResultFromMdr(MDR_RESULT_ERROR_NET);
                }

                *pSent = size;
                MDR_LOG("[BLE] Send OK: {} bytes written", size);
                return S_OK;
            });

        mdr::MDRAllocator<uint8_t>().deallocate(data);

        if (FAILED(hr))
        {
            if (IsMdrHResult(hr))
            {
                return HResultToMdrResult(hr);
            }
            else
            {
                ptr->lastError = mdr::Format("Send failed with HRESULT 0x{:08X}", (uint32_t)hr);
                MDR_LOG("[BLE] Send error: {}", ptr->lastError);
                return MDR_RESULT_ERROR_NET;
            }
        }

        return MDR_RESULT_OK;
    }

    static int Poll(void* user, int timeout) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);

        // If not yet connected, non-blocking check for connection completion
        if (!ptr->connected)
        {
            if (ptr->connectResult != MDR_RESULT_INPROGRESS)
                return ptr->connectResult.load();

            // Non-blocking check: only poll with timeout 0 to avoid blocking the UI thread
            DWORD waitResult = WaitForSingleObject(ptr->connectEvent, 0);
            if (waitResult == WAIT_OBJECT_0)
                return ptr->connectResult.load();
            return MDR_RESULT_INPROGRESS;
        }

        // Non-blocking check if data is already available in the buffer
        {
            std::unique_lock lock(ptr->rxMutex, std::try_to_lock);
            if (lock.owns_lock() && !ptr->rxBuffer.empty())
                return MDR_RESULT_OK;
        }

        // Also return OK if there's pending send data (connection is alive and writable)
        if (timeout == 0)
            return MDR_RESULT_OK;

        // Wait for incoming data
        DWORD waitMs = (timeout < 0) ? INFINITE : static_cast<DWORD>(timeout);
        DWORD waitResult = WaitForSingleObject(ptr->rxEvent, waitMs);

        if (waitResult == WAIT_OBJECT_0)
            return MDR_RESULT_OK;
        if (waitResult == WAIT_TIMEOUT)
            return MDR_RESULT_ERROR_TIMEOUT;

        ptr->SetLastError("Poll wait failed");
        return MDR_RESULT_ERROR_NET;
    }

    static int FreeDevicesList(void*, MDRDeviceInfo** ppList) noexcept
    {
        if (*ppList)
        {
            mdr::MDRAllocator<MDRDeviceInfo>().deallocate(*ppList);
            *ppList = nullptr;
        }
        return MDR_RESULT_OK;
    }

    static const char* GetLastError(void* user) noexcept
    {
        auto* self = static_cast<MDRConnectionWindowsBLE*>(user);
        std::lock_guard<std::mutex> lock(self->lastErrorMutex);
        self->lastErrorSnapshot = self->lastError;
        return self->lastErrorSnapshot.c_str();
    }
};

// --- C API ---

extern "C" {

MDRConnectionWindowsBLE* mdrConnectionWindowsBLECreate()
{
    MDR_LOG("[BLE] mdrConnectionWindowsBLECreate");
    return mdr::Construct<MDRConnectionWindowsBLE>();
}

MDRConnection* mdrConnectionWindowsBLEGet(MDRConnectionWindowsBLE* pConn) { return &pConn->mdrConn; }

void mdrConnectionWindowsBLEDestroy(MDRConnectionWindowsBLE* pConn)
{
    MDR_LOG("[BLE] mdrConnectionWindowsBLEDestroy");
    if (pConn)
    {
        MDRConnectionWindowsBLE::Disconnect(pConn);
        mdr::Destruct(pConn);
    }
}
} // extern "C"
