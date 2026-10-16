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
#include <condition_variable>
#include <functional>
#include <mdr/Protocol.hpp>
#include <memory>
#include <mutex>
#include <thread>

#include <mdr-bt/ConnectionWindows.h>
#include "../Utils.hpp"

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
        { /*MDR_LOG("HRESULT 0x{:08X} at {}:{}", hr, __FILE__, __LINE__);*/                                      \
            return hr;                                                                                                 \
        }                                                                                                              \
    }                                                                                                                  \
    while (0)
#define RETURN_IF_NULL_ALLOC(ptr)                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((ptr) == nullptr)                                                                                          \
        { /*MDR_LOG("Out of memory at {}:{}", __FILE__, __LINE__);*/                                             \
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

    struct ReceiveState
    {
        std::mutex mutex;
        mdr::Vector<uint8_t> buffer;
        HANDLE event{CreateEventW(nullptr, TRUE, FALSE, nullptr)};
        bool active{};

        ~ReceiveState() { CloseHandle(event); }
    };

    std::shared_ptr<ReceiveState> receive{std::make_shared<ReceiveState>()};
    bool useWriteWithoutResponse{false};
    std::atomic<bool> connected{false};
    std::atomic<int> connectResult{MDR_RESULT_ERROR_NO_CONNECTION};
    HANDLE cancelEvent{CreateEventW(nullptr, TRUE, FALSE, nullptr)};

    std::mutex stateMutex;
    uint64_t generation{};
    mdr::String publishedError;
    mdr::String errorSnapshot;
    bool writePending{};
    MDRResult writeResult{MDR_RESULT_OK};
    bool enumPending{};
    bool enumReady{};
    MDRResult enumResult{MDR_RESULT_INPROGRESS};
    MDRDeviceInfo* enumDevices{};
    int enumCount{};

    std::mutex workMutex;
    std::condition_variable workCondition;
    mdr::Deque<std::function<void()>> workQueue;
    HRESULT workerResult{E_FAIL};
    std::jthread worker;

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
        worker = std::jthread([this](std::stop_token stop)
        {
            workerResult = cancelEvent && receive->event
                ? CoInitializeEx(nullptr, COINIT_MULTITHREADED) : E_OUTOFMEMORY;
            while (!stop.stop_requested())
            {
                std::function<void()> work;
                {
                    std::unique_lock lock(workMutex);
                    workCondition.wait(lock, [&] { return stop.stop_requested() || !workQueue.empty(); });
                    if (stop.stop_requested())
                        break;
                    work = std::move(workQueue.front());
                    workQueue.pop_front();
                }
                work();
            }
            DisconnectImpl(this);
            if (SUCCEEDED(workerResult))
                CoUninitialize();
        });
        MDR_LOG("MDRConnectionWindowsBLE created");
    }

    ~MDRConnectionWindowsBLE()
    {
        Disconnect(this);
        {
            std::scoped_lock lock(stateMutex, workMutex);
            worker.request_stop();
            SetEvent(cancelEvent);
        }
        workCondition.notify_one();
        worker.join();
        FreeDevicesList(this, &enumDevices);
        CloseHandle(cancelEvent);
        MDR_LOG("MDRConnectionWindowsBLE destroyed");
    }

    void Enqueue(std::function<void()> work)
    {
        {
            std::scoped_lock lock(workMutex);
            workQueue.push_back(std::move(work));
        }
        workCondition.notify_one();
    }

    bool Cancelled() const { return WaitForSingleObject(cancelEvent, 0) == WAIT_OBJECT_0; }

    template <typename F>
    HRESULT RunOnMTA(F&& func)
    {
        return FAILED(workerResult) ? workerResult : func();
    }

    template <typename Result>
    HRESULT WaitForResults(IAsyncOperation<Result>* operation,
        typename ABI::Windows::Foundation::Internal::GetAbiType<typename IAsyncOperation<Result>::TResult_complex>::type* result)
    {
        return WaitForCompletionAndGetResults(operation, result, (COWAIT_FLAGS)-1, cancelEvent);
    }

    // --- MDRConnection vtable implementation ---

    static MDRResult GetDevicesList(void* user, MDRDeviceInfo** ppList, int* pCount) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        std::scoped_lock lock(ptr->stateMutex);
        if (ptr->enumReady)
        {
            const MDRResult result = ptr->enumResult;
            *ppList = ptr->enumDevices;
            *pCount = ptr->enumCount;
            ptr->enumDevices = nullptr;
            ptr->enumCount = 0;
            ptr->enumReady = false;
            return result;
        }
        if (!ptr->enumPending)
        {
            ptr->enumPending = true;
            const uint64_t generation = ptr->generation;
            ptr->Enqueue([ptr, generation]
            {
                {
                    std::scoped_lock stateLock(ptr->stateMutex);
                    if (ptr->worker.get_stop_token().stop_requested() || ptr->generation != generation)
                    {
                        ptr->enumPending = false;
                        ptr->enumReady = true;
                        ptr->enumResult = MDR_RESULT_ERROR_NO_CONNECTION;
                        return;
                    }
                    ResetEvent(ptr->cancelEvent);
                }
                MDRDeviceInfo* devices{};
                int count{};
                const MDRResult result = GetDevicesListImpl(ptr, &devices, &count);
                std::scoped_lock stateLock(ptr->stateMutex);
                ptr->enumDevices = devices;
                ptr->enumCount = count;
                ptr->enumResult = result;
                ptr->enumPending = false;
                ptr->enumReady = true;
                if (result != MDR_RESULT_OK && ptr->generation == generation)
                    ptr->publishedError = ptr->lastError;
            });
        }
        return MDR_RESULT_INPROGRESS;
    }

    static MDRResult GetDevicesListImpl(void* user, MDRDeviceInfo** ppList, int* pCount) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        MDR_LOG("GetDevicesList called");

        HRESULT hr = ptr->RunOnMTA(
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
                MDR_LOG("Using BLE device selector for paired devices");

                // Find all matching devices
                ComPtr<IDeviceInformationStatics> spDeviceInfoStatics;
                RETURN_IF_FAILED(GetActivationFactory(
                    Wrappers::HStringReference(RuntimeClass_Windows_Devices_Enumeration_DeviceInformation).Get(),
                    &spDeviceInfoStatics));

                ComPtr<IAsyncOperation<DeviceInformationCollection*>> spDeviceInfosOp;
                RETURN_IF_FAILED(spDeviceInfoStatics->FindAllAsyncAqsFilter(hstrSelector.Get(), &spDeviceInfosOp));

                ComPtr<IVectorView<DeviceInformation*>> spDeviceInfos;
                RETURN_IF_FAILED(ptr->WaitForResults(spDeviceInfosOp.Get(), &spDeviceInfos));

                uint32_t cDeviceInfos;
                RETURN_IF_FAILED(spDeviceInfos->get_Size(&cDeviceInfos));
                MDR_LOG("FindAllAsync returned {} device(s)", cDeviceInfos);

                mdr::Vector<MDRDeviceInfo> devices;

                for (uint32_t i = 0; i < cDeviceInfos; i++)
                {
                    if (ptr->Cancelled())
                        return E_ABORT;
                    ComPtr<IDeviceInformation> spDevInfo;
                    RETURN_IF_FAILED(spDeviceInfos->GetAt(i, &spDevInfo));

                    Wrappers::HString hstrId;
                    RETURN_IF_FAILED(spDevInfo->get_Id(hstrId.GetAddressOf()));

                    Wrappers::HString hstrName;
                    RETURN_IF_FAILED(spDevInfo->get_Name(hstrName.GetAddressOf()));

                    uint32_t cchName;
                    LPCWSTR pszName = hstrName.GetRawBuffer(&cchName);
                    mdr::String name = format_as(std::wstring_view(pszName, cchName));

                    MDR_LOG("Device #{}: id=\"{}\" name=\"{}\"", i, hstrId, hstrName);

                    // Open the BLE device to get its Bluetooth address
                    auto openDevice = [&]() -> HRESULT
                    {
                        ComPtr<IAsyncOperation<BluetoothLEDevice*>> bleDeviceOp;
                        RETURN_IF_FAILED(spBluetoothLEDeviceStatics->FromIdAsync(hstrId.Get(), &bleDeviceOp));

                        ComPtr<IBluetoothLEDevice> bleDevice;
                        RETURN_IF_FAILED(ptr->WaitForResults(bleDeviceOp.Get(), &bleDevice));
                        if (bleDevice == nullptr)
                        {
                            MDR_LOG("  Skipping device #{}: FromIdAsync returned null", i);
                            return S_FALSE;
                        }

                        uint64_t addr;
                        RETURN_IF_FAILED(bleDevice->get_BluetoothAddress(&addr));
                        mdr::String macAddress = mdr::Format(
                            "{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}", (addr >> 40) & 0xFF, (addr >> 32) & 0xFF,
                            (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF);

                        MDR_LOG("  BLE address: {}", macAddress);

                        devices.emplace_back();
                        auto& back = devices.back();
                        CopyUtf8(back.szDeviceName, name.c_str());
                        CopyUtf8(back.szDeviceMacAddress, macAddress.c_str());

                        (void)IInspectable_Close(bleDevice.Get());

                        return S_OK;
                    };
                    HRESULT hrOpenDevice = openDevice();
                    if (ptr->Cancelled())
                        return E_ABORT;
                    if (FAILED(hrOpenDevice))
                    {
                        MDR_LOG("  Error opening device #{}: HRESULT 0x{:08X}", i, hrOpenDevice);
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

                MDR_LOG("GetDevicesList returning {} BLE device(s)", *pCount);
                return S_OK;
            }); // end RunOnMTA
        if (FAILED(hr))
        {
            ptr->lastError = mdr::Format("GetDevicesList failed with HRESULT 0x{:08X}", (uint32_t)hr);
            MDR_LOG("GetDevicesList error: {}", ptr->lastError);
            return MDR_RESULT_ERROR_NET;
        }

        return MDR_RESULT_OK;
    }

    static MDRResult Connect(void* user, const char* macAddress, const char* serviceUUID) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        MDR_LOG("Connect called: mac={} serviceUUID={}", macAddress, serviceUUID);

        const uint64_t btAddr = macAddressToULL(macAddress);
        std::scoped_lock lock(ptr->stateMutex);
        if (btAddr == ~0ULL)
        {
            ptr->publishedError = "Invalid MAC address format";
            return MDR_RESULT_ERROR_BAD_ADDRESS;
        }
        const uint64_t generation = ++ptr->generation;
        ptr->connected = false;
        ptr->connectResult = MDR_RESULT_INPROGRESS;
        ptr->writePending = false;
        ptr->writeResult = MDR_RESULT_OK;
        ptr->publishedError = "Connecting via BLE GATT...";
        SetEvent(ptr->cancelEvent);
        SetEvent(ptr->receive->event);
        ptr->Enqueue([ptr, btAddr, svcUUID = mdr::String(serviceUUID), generation]
        {
            DisconnectImpl(ptr);
            {
                std::scoped_lock stateLock(ptr->stateMutex);
                if (ptr->generation != generation || ptr->worker.get_stop_token().stop_requested())
                    return;
                ResetEvent(ptr->cancelEvent);
                SetEvent(ptr->receive->event);
                ptr->receive = std::make_shared<ReceiveState>();
                if (!ptr->receive->event)
                {
                    ptr->connectResult = MDR_RESULT_ERROR_NET;
                    ptr->publishedError = "Unable to create BLE receive event";
                    return;
                }
                ptr->receive->active = true;
            }
            const MDRResult result = ConnectImpl(ptr, btAddr, svcUUID);
            std::scoped_lock stateLock(ptr->stateMutex);
            if (ptr->generation != generation)
                return;
            ptr->publishedError = ptr->lastError;
            ptr->connectResult = result;
            ptr->connected = result == MDR_RESULT_OK;
            SetEvent(ptr->receive->event);
        });
        return MDR_RESULT_INPROGRESS;
    }

    static MDRResult ConnectImpl(MDRConnectionWindowsBLE* ptr, uint64_t btAddr, const mdr::String& svcUUID)
    {
        auto inner = [&]() -> HRESULT
                {

                    ComPtr<IBluetoothLEDeviceStatics> spBluetoothLEDeviceStatics;
                    RETURN_IF_FAILED(GetActivationFactory(
                        Wrappers::HStringReference(RuntimeClass_Windows_Devices_Bluetooth_BluetoothLEDevice).Get(),
                        &spBluetoothLEDeviceStatics));

                    // Open BLE device by address
                    ComPtr<IAsyncOperation<BluetoothLEDevice*>> spBleDeviceOp;
                    RETURN_IF_FAILED(spBluetoothLEDeviceStatics->FromBluetoothAddressAsync(btAddr, &spBleDeviceOp));
                    RETURN_IF_FAILED(ptr->WaitForResults(spBleDeviceOp.Get(), &ptr->device));
                    if (ptr->device == nullptr)
                    {
                        ptr->lastError = "BLE device not found or not reachable";
                        MDR_LOG("FromBluetoothAddressAsync returned null");
                        return HResultFromMdr(MDR_RESULT_ERROR_NOT_FOUND);
                    }

                    Wrappers::HString hstrName;
                    RETURN_IF_FAILED(ptr->device->get_Name(hstrName.GetAddressOf()));
                    MDR_LOG("BLE device opened: {}", hstrName);

                    if (ptr->Cancelled())
                        return E_ABORT;

                    // Parse service UUID and find matching GATT service
                    GUID svcGuid;
                    {
                        uint8_t uuidBytes[16];
                        if (serviceUUIDtoBytes(svcUUID.c_str(), uuidBytes) != 0)
                        {
                            ptr->lastError = "Invalid service UUID format";
                            MDR_LOG("Invalid service UUID: {}", svcUUID);
                            return HResultFromMdr(MDR_RESULT_ERROR_BAD_ADDRESS);
                        }
                        std::memcpy(&svcGuid, uuidBytes, 16);
                    }

                    MDR_LOG("Looking for GATT service: {}", svcUUID);

                    ComPtr<IBluetoothLEDevice3> spBleDevice3;
                    RETURN_IF_FAILED(ptr->device.As(&spBleDevice3));

                    ComPtr<IAsyncOperation<GattDeviceServicesResult*>> spServicesOp;
                    RETURN_IF_FAILED(
                        spBleDevice3->GetGattServicesWithCacheModeAsync(BluetoothCacheMode_Uncached, &spServicesOp));

                    ComPtr<IGattDeviceServicesResult> spServicesResult;
                    RETURN_IF_FAILED(ptr->WaitForResults(spServicesOp.Get(), &spServicesResult));

                    GattCommunicationStatus status;
                    RETURN_IF_FAILED(spServicesResult->get_Status(&status));

                    ComPtr<IVectorView<GattDeviceService*>> spServices;
                    RETURN_IF_FAILED(spServicesResult->get_Services(&spServices));

                    uint32_t cServices;
                    RETURN_IF_FAILED(spServices->get_Size(&cServices));

                    MDR_LOG("GetGattServicesAsync status: {}, count: {}", (int)status, cServices);

                    if (status != GattCommunicationStatus_Success)
                    {
                        ptr->lastError = mdr::Format("GATT service discovery failed (status={})", (int)status);
                        MDR_LOG("{}", ptr->lastError);
                        return HResultFromMdr(MDR_RESULT_ERROR_NET);
                    }

                    if (ptr->Cancelled())
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
                        MDR_LOG("  Found service: {}", foundUUID);
                        if (_stricmp(foundUUID.c_str(), svcUUID.c_str()) == 0)
                        {
                            spTargetService = spSvc;
                            MDR_LOG("  -> Matched target service!");
                            break;
                        }
                    }

                    if (spTargetService == nullptr)
                    {
                        ptr->lastError = mdr::Format("GATT service {} not found on device", svcUUID);
                        MDR_LOG("{}", ptr->lastError);
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
                    RETURN_IF_FAILED(ptr->WaitForResults(spCharsOp.Get(), &spCharsResult));

                    GattCommunicationStatus charsStatus;
                    RETURN_IF_FAILED(spCharsResult->get_Status(&charsStatus));

                    if (charsStatus != GattCommunicationStatus_Success)
                    {
                        ptr->lastError = "Failed to enumerate GATT characteristics";
                        MDR_LOG("GetCharacteristicsAsync failed: status={}", (int)charsStatus);
                        return HResultFromMdr(MDR_RESULT_ERROR_NET);
                    }

                    ComPtr<IVectorView<GattCharacteristic*>> spCharacteristics;
                    RETURN_IF_FAILED(spCharsResult->get_Characteristics(&spCharacteristics));

                    uint32_t cCharacteristics;
                    RETURN_IF_FAILED(spCharacteristics->get_Size(&cCharacteristics));

                    MDR_LOG("Found {} characteristic(s) in service", cCharacteristics);

                    // for (auto const& ch : charsResult.Characteristics())
                    for (uint32_t i = 0; i < cCharacteristics; i++)
                    {
                        ComPtr<IGattCharacteristic> spCh;
                        RETURN_IF_FAILED(spCharacteristics->GetAt(i, &spCh));

                        GattCharacteristicProperties props;
                        RETURN_IF_FAILED(spCh->get_CharacteristicProperties(&props));

                        GUID uuid;
                        RETURN_IF_FAILED(spCh->get_Uuid(&uuid));
                        MDR_LOG("  Characteristic: {} [{}]", GuidToString(uuid),
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
                            MDR_LOG("  -> Selected as WRITE characteristic (writeNoResp={})",
                                    (int)ptr->useWriteWithoutResponse);
                        }

                        // Pick the first notifiable characteristic
                        if (ptr->notifyChar == nullptr &&
                            ((props & GattCharacteristicProperties_Notify) != GattCharacteristicProperties_None ||
                             (props & GattCharacteristicProperties_Indicate) != GattCharacteristicProperties_None))
                        {
                            ptr->notifyChar = spCh;
                            MDR_LOG("  -> Selected as NOTIFY characteristic");
                        }
                    }

                    if (ptr->writeChar == nullptr)
                    {
                        ptr->lastError = "No writable GATT characteristic found in service";
                        MDR_LOG("{}", ptr->lastError);
                        return HResultFromMdr(MDR_RESULT_ERROR_NOT_FOUND);
                    }

                    if (ptr->notifyChar == nullptr)
                    {
                        ptr->lastError = "No notifiable GATT characteristic found in service";
                        MDR_LOG("{}", ptr->lastError);
                        return HResultFromMdr(MDR_RESULT_ERROR_NOT_FOUND);
                    }

                    if (ptr->Cancelled())
                        return E_ABORT;

                    // Subscribe to notifications
                    ComPtr<IAsyncOperation<GattCommunicationStatus>> spCccdOp;
                    RETURN_IF_FAILED(ptr->notifyChar->WriteClientCharacteristicConfigurationDescriptorAsync(
                        GattClientCharacteristicConfigurationDescriptorValue_Notify, &spCccdOp));

                    GattCommunicationStatus cccdResult;
                    RETURN_IF_FAILED(ptr->WaitForResults(spCccdOp.Get(), &cccdResult));

                    if (cccdResult != GattCommunicationStatus_Success)
                    {
                        // Try Indicate if Notify fails
                        MDR_LOG("Notify subscription failed (status={}), trying Indicate", (int)cccdResult);
                        RETURN_IF_FAILED(ptr->notifyChar->WriteClientCharacteristicConfigurationDescriptorAsync(
                            GattClientCharacteristicConfigurationDescriptorValue_Indicate, &spCccdOp));
                        RETURN_IF_FAILED(ptr->WaitForResults(spCccdOp.Get(), &cccdResult));
                    }

                    if (cccdResult != GattCommunicationStatus_Success)
                    {
                        ptr->lastError =
                            mdr::Format("Failed to subscribe to notifications (status={})", (int)cccdResult);
                        MDR_LOG("{}", ptr->lastError);
                        return HResultFromMdr(MDR_RESULT_ERROR_NET);
                    }

                    MDR_LOG("Notification subscription OK");

                    // Register ValueChanged handler
                    auto callback = Callback<ITypedEventHandler<GattCharacteristic*, GattValueChangedEventArgs*>>(
                        [receive = ptr->receive](IGattCharacteristic*, IGattValueChangedEventArgs* args) -> HRESULT
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

                            mdr::Vector<uint8_t> bytes(len);
                            RETURN_IF_FAILED(spReader->ReadBytes(len, bytes.data()));
                            std::scoped_lock lock(receive->mutex);
                            if (!receive->active)
                                return S_OK;
                            receive->buffer.insert(receive->buffer.end(), bytes.begin(), bytes.end());
                            SetEvent(receive->event);
                            return S_OK;
                        });
                    RETURN_IF_NULL_ALLOC(callback);
                    RETURN_IF_FAILED(ptr->notifyChar->add_ValueChanged(callback.Get(), &ptr->notifyToken));

                    return S_OK;
                };

        const HRESULT hr = ptr->RunOnMTA(inner);
        if (FAILED(hr))
        {
            if (IsMdrHResult(hr))
                return HResultToMdrResult(hr);
            ptr->lastError = mdr::Format("Connect failed with HRESULT 0x{:08X}", (uint32_t)hr);
            return MDR_RESULT_ERROR_NET;
        }
        ptr->lastError = "Connected via BLE GATT";
        return MDR_RESULT_OK;
    }

    static void Disconnect(void* user) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        std::scoped_lock lock(ptr->stateMutex);
        ++ptr->generation;
        ptr->connected = false;
        ptr->connectResult = MDR_RESULT_ERROR_NO_CONNECTION;
        ptr->writePending = false;
        ptr->writeResult = MDR_RESULT_OK;
        SetEvent(ptr->cancelEvent);
        SetEvent(ptr->receive->event);
        ptr->Enqueue([ptr] { DisconnectImpl(ptr); });
    }

    static void DisconnectImpl(void* user) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        {
            std::scoped_lock lock(ptr->receive->mutex);
            ptr->receive->active = false;
            ptr->receive->buffer.clear();
        }

        // WinRT cleanup must run on MTA thread
        HRESULT hr = ptr->RunOnMTA(
            [ptr]() -> HRESULT
            {
                // Unsubscribe from notifications
                if (ptr->notifyChar != nullptr)
                {
                    (void)ptr->notifyChar->remove_ValueChanged(ptr->notifyToken);
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
            MDR_LOG("Disconnect: error during WinRT cleanup (ignored)");
            ptr->notifyChar = nullptr;
            ptr->writeChar = nullptr;
            ptr->service = nullptr;
            ptr->device = nullptr;
        }

        MDR_LOG("Disconnected");
    }

    static MDRResult Recv(void* user, char* dst, int size, int* pReceived) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        *pReceived = 0;
        std::shared_ptr<ReceiveState> receive;
        {
            std::scoped_lock lock(ptr->stateMutex);
            if (!ptr->connected)
                return MDR_RESULT_ERROR_NO_CONNECTION;
            if (ptr->writeResult >= MDR_RESULT_ERROR_GENERAL)
                return ptr->writeResult;
            receive = ptr->receive;
        }
        std::unique_lock lock(receive->mutex, std::try_to_lock);
        if (!lock.owns_lock() || receive->buffer.empty())
            return MDR_RESULT_INPROGRESS;
        const int count = static_cast<int>(std::min(static_cast<size_t>(size), receive->buffer.size()));
        std::memcpy(dst, receive->buffer.data(), count);
        receive->buffer.erase(receive->buffer.begin(), receive->buffer.begin() + count);
        if (receive->buffer.empty())
            ResetEvent(receive->event);
        *pReceived = count;
        return MDR_RESULT_OK;
    }

    static MDRResult Send(void* user, const char* src, int size, int* pSent) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        *pSent = 0;
        std::scoped_lock lock(ptr->stateMutex);
        if (!ptr->connected)
            return MDR_RESULT_ERROR_NO_CONNECTION;
        if (ptr->writeResult != MDR_RESULT_OK)
            return ptr->writeResult;
        if (ptr->writePending)
            return MDR_RESULT_INPROGRESS;
        const uint64_t generation = ptr->generation;
        const auto* bytes = reinterpret_cast<const uint8_t*>(src);
        ptr->writePending = true;
        ptr->Enqueue([ptr, data = mdr::Vector<uint8_t>(bytes, bytes + size), generation]() mutable
        {
            {
                std::scoped_lock stateLock(ptr->stateMutex);
                if (ptr->generation != generation)
                    return;
            }
            const HRESULT hr = ptr->RunOnMTA([&]() -> HRESULT
            {
                ComPtr<IDataWriter> writer;
                RETURN_IF_FAILED(ActivateInstance(
                    Wrappers::HStringReference(RuntimeClass_Windows_Storage_Streams_DataWriter).Get(), &writer));
                RETURN_IF_FAILED(writer->WriteBytes(static_cast<uint32_t>(data.size()), data.data()));
                ComPtr<IBuffer> buffer;
                RETURN_IF_FAILED(writer->DetachBuffer(&buffer));
                ComPtr<IAsyncOperation<GattCommunicationStatus>> operation;
                const auto option = ptr->useWriteWithoutResponse
                    ? GattWriteOption_WriteWithoutResponse : GattWriteOption_WriteWithResponse;
                RETURN_IF_FAILED(ptr->writeChar->WriteValueWithOptionAsync(buffer.Get(), option, &operation));
                GattCommunicationStatus status;
                RETURN_IF_FAILED(ptr->WaitForResults(operation.Get(), &status));
                if (status != GattCommunicationStatus_Success)
                {
                    ptr->lastError = mdr::Format("GATT write failed (status={})", (int)status);
                    return HResultFromMdr(MDR_RESULT_ERROR_NET);
                }
                return S_OK;
            });
            std::scoped_lock stateLock(ptr->stateMutex);
            if (ptr->generation != generation)
                return;
            ptr->writePending = false;
            if (FAILED(hr))
            {
                ptr->writeResult = HResultToMdrResult(hr);
                ptr->publishedError = IsMdrHResult(hr) ? ptr->lastError
                    : mdr::Format("Send failed with HRESULT 0x{:08X}", (uint32_t)hr);
            }
            SetEvent(ptr->receive->event);
        });
        *pSent = size;
        return MDR_RESULT_OK;
    }

    static MDRResult Poll(void* user, int timeout) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        const ULONGLONG deadline = timeout > 0 ? GetTickCount64() + static_cast<DWORD>(timeout) : 0;
        while (true)
        {
            std::shared_ptr<ReceiveState> receive;
            {
                std::scoped_lock lock(ptr->stateMutex);
                if (!ptr->connected && ptr->connectResult != MDR_RESULT_INPROGRESS)
                    return ptr->connectResult.load();
                if (ptr->connected && ptr->writeResult != MDR_RESULT_OK)
                    return ptr->writeResult;
                if (ptr->connected && !ptr->writePending)
                    return MDR_RESULT_OK;
                receive = ptr->receive;
                std::scoped_lock receiveLock(receive->mutex);
                if (ptr->connected && !receive->buffer.empty())
                    return MDR_RESULT_OK;
                if (timeout == 0)
                    return ptr->connected ? MDR_RESULT_ERROR_TIMEOUT : MDR_RESULT_INPROGRESS;
                ResetEvent(receive->event);
            }
            DWORD waitMs = INFINITE;
            if (timeout > 0)
            {
                const ULONGLONG now = GetTickCount64();
                if (now >= deadline)
                    return MDR_RESULT_ERROR_TIMEOUT;
                waitMs = static_cast<DWORD>(deadline - now);
            }
            const DWORD result = WaitForSingleObject(receive->event, waitMs);
            if (result == WAIT_TIMEOUT)
                return MDR_RESULT_ERROR_TIMEOUT;
            if (result != WAIT_OBJECT_0)
            {
                std::scoped_lock lock(ptr->stateMutex);
                ptr->publishedError = "Poll wait failed";
                return MDR_RESULT_ERROR_NET;
            }
        }
    }

    static MDRResult FreeDevicesList(void*, MDRDeviceInfo** ppList) noexcept
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
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        std::scoped_lock lock(ptr->stateMutex);
        ptr->errorSnapshot = ptr->publishedError;
        return ptr->errorSnapshot.c_str();
    }
};

// --- C API ---

extern "C" {

MDR_API MDRConnectionWindowsBLE* mdrConnectionWindowsBLECreate()
{
    MDR_LOG("mdrConnectionWindowsBLECreate");
    return mdr::Construct<MDRConnectionWindowsBLE>();
}

MDR_API MDRConnection* mdrConnectionWindowsBLEGet(MDRConnectionWindowsBLE* pConn) { return &pConn->mdrConn; }

MDR_API void mdrConnectionWindowsBLEDestroy(MDRConnectionWindowsBLE* pConn)
{
    MDR_LOG("mdrConnectionWindowsBLEDestroy");
    if (pConn)
    {
        MDRConnectionWindowsBLE::Disconnect(pConn);
        mdr::Destruct(pConn);
    }
}
} // extern "C"
