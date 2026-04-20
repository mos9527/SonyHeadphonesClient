#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// C++/WinRT headers
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Storage.Streams.h>

#include <fmt/format.h>
#include <mdr/Protocol.hpp>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>

#include "../Platform.hpp"
#include <mdr-c/Platform/PlatformWindowsBLE.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Storage::Streams;

// GATT characteristic property bitmask values
enum GattPropertyBit
{
    GATT_PROP_READ             = 1,
    GATT_PROP_WRITE            = 2,
    GATT_PROP_WRITE_NO_RESP    = 4,
    GATT_PROP_NOTIFY           = 8,
    GATT_PROP_INDICATE         = 16,
};

// Helper: convert winrt::guid to string "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX"
// Used both for logging and for service UUID matching in Connect().
static std::string GuidToString(const winrt::guid& g)
{
    return fmt::format("{:08X}-{:04X}-{:04X}-{:02X}{:02X}-{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}",
        g.Data1, g.Data2, g.Data3,
        g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3],
        g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]);
}

// Helper: convert GattCharacteristicProperties to our bitmask
static int PropertiesToBitmask(GattCharacteristicProperties props)
{
    int mask = 0;
    if ((props & GattCharacteristicProperties::Read) != GattCharacteristicProperties::None) mask |= GATT_PROP_READ;
    if ((props & GattCharacteristicProperties::Write) != GattCharacteristicProperties::None) mask |= GATT_PROP_WRITE;
    if ((props & GattCharacteristicProperties::WriteWithoutResponse) != GattCharacteristicProperties::None) mask |= GATT_PROP_WRITE_NO_RESP;
    if ((props & GattCharacteristicProperties::Notify) != GattCharacteristicProperties::None) mask |= GATT_PROP_NOTIFY;
    if ((props & GattCharacteristicProperties::Indicate) != GattCharacteristicProperties::None) mask |= GATT_PROP_INDICATE;
    return mask;
}

// Helper: convert properties bitmask to human-readable string
static std::string PropertiesToString(int mask)
{
    std::string result;
    if (mask & GATT_PROP_READ) result += "Read ";
    if (mask & GATT_PROP_WRITE) result += "Write ";
    if (mask & GATT_PROP_WRITE_NO_RESP) result += "WriteNoResp ";
    if (mask & GATT_PROP_NOTIFY) result += "Notify ";
    if (mask & GATT_PROP_INDICATE) result += "Indicate ";
    if (result.empty()) result = "None";
    return result;
}

struct MDRConnectionWindowsBLE
{
    MDRConnection mdrConn;
    std::string lastError;

    // WinRT device handles
    BluetoothLEDevice device{nullptr};
    GattDeviceService service{nullptr};
    GattCharacteristic writeChar{nullptr};
    GattCharacteristic notifyChar{nullptr};
    winrt::event_token notifyToken{};

    // Receive buffer (filled by GATT notifications)
    std::mutex rxMutex;
    std::vector<uint8_t> rxBuffer;
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
        lastError(""),
        mdrConn({.user = this,
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
    // See: https://learn.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/concurrency#programming-with-thread-affinity-in-mind
    // See: https://learn.microsoft.com/en-us/windows/win32/com/multithreaded-apartments
    template<typename F>
    static auto RunOnMTA(F&& func) -> decltype(func())
    {
        using RetType = decltype(func());
        RetType result{};
        std::exception_ptr ex;

        std::thread mtaThread([&]()
        {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
            try
            {
                result = func();
            }
            catch (...)
            {
                ex = std::current_exception();
            }
            winrt::uninit_apartment();
        });
        mtaThread.join();

        if (ex) std::rethrow_exception(ex);
        return result;
    }

    // Void overload for functions that return nothing
    template<typename F>
    static void RunOnMTAVoid(F&& func)
    {
        std::exception_ptr ex;

        std::thread mtaThread([&]()
        {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
            try
            {
                func();
            }
            catch (...)
            {
                ex = std::current_exception();
            }
            winrt::uninit_apartment();
        });
        mtaThread.join();

        if (ex) std::rethrow_exception(ex);
    }

    // --- MDRConnection vtable implementation ---

    static int GetDevicesList(void* user, MDRDeviceInfo** ppList, int* pCount) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        MDR_LOG("[BLE] GetDevicesList called");

        try
        {
            return RunOnMTA([&]() -> int
            {
            // Get AQS selector for paired BLE devices
            auto selector = BluetoothLEDevice::GetDeviceSelectorFromPairingState(true);
            MDR_LOG("[BLE] Using BLE device selector for paired devices");

            // Find all matching devices
            auto deviceInfos = DeviceInformation::FindAllAsync(selector).get();
            MDR_LOG("[BLE] FindAllAsync returned {} device(s)", deviceInfos.Size());

            std::vector<MDRDeviceInfo> devices;

            for (uint32_t i = 0; i < deviceInfos.Size(); i++)
            {
                auto devInfo = deviceInfos.GetAt(i);
                std::string name = winrt::to_string(devInfo.Name());
                MDR_LOG("[BLE] Device #{}: name=\"{}\" id=\"{}\"",
                    i, name, winrt::to_string(devInfo.Id()));

                // Open the BLE device to get its Bluetooth address
                try
                {
                    auto bleDevice = BluetoothLEDevice::FromIdAsync(devInfo.Id()).get();
                    if (!bleDevice)
                    {
                        MDR_LOG("[BLE]   Skipping device #{}: FromIdAsync returned null", i);
                        continue;
                    }

                    uint64_t addr = bleDevice.BluetoothAddress();
                    std::string macAddress = fmt::format("{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}",
                        (addr >> 40) & 0xFF, (addr >> 32) & 0xFF, (addr >> 24) & 0xFF,
                        (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF);

                    MDR_LOG("[BLE]   BLE address: {}", macAddress);

                    devices.emplace_back();
                    auto& back = devices.back();
                    strncpy(back.szDeviceName, name.c_str(), sizeof(back.szDeviceName) - 1);
                    back.szDeviceName[sizeof(back.szDeviceName) - 1] = '\0';
                    strncpy(back.szDeviceMacAddress, macAddress.c_str(), sizeof(back.szDeviceMacAddress) - 1);
                    back.szDeviceMacAddress[sizeof(back.szDeviceMacAddress) - 1] = '\0';

                    bleDevice.Close();
                }
                catch (const winrt::hresult_error& ex)
                {
                    MDR_LOG("[BLE]   Error opening device #{}: {}", i, winrt::to_string(ex.message()));
                }
            }

            if (devices.empty())
            {
                *ppList = nullptr;
                *pCount = 0;
            }
            else
            {
                *ppList = new MDRDeviceInfo[devices.size()];
                std::memcpy(*ppList, devices.data(), devices.size() * sizeof(MDRDeviceInfo));
                *pCount = static_cast<int>(devices.size());
            }

            MDR_LOG("[BLE] GetDevicesList returning {} BLE device(s)", *pCount);
            return MDR_RESULT_OK;
            }); // end RunOnMTA
        }
        catch (const winrt::hresult_error& ex)
        {
            ptr->lastError = winrt::to_string(ex.message());
            MDR_LOG("[BLE] GetDevicesList error: {}", ptr->lastError);
            return MDR_RESULT_ERROR_NET;
        }
        catch (const std::exception& ex)
        {
            ptr->lastError = ex.what();
            MDR_LOG("[BLE] GetDevicesList exception: {}", ptr->lastError);
            return MDR_RESULT_ERROR_NET;
        }
    }

    static int Connect(void* user, const char* macAddress, const char* serviceUUID) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        MDR_LOG("[BLE] Connect called: mac={} serviceUUID={}", macAddress, serviceUUID);

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
            ptr->lastError = "Invalid MAC address format";
            MDR_LOG("[BLE] Connect failed: invalid MAC address");
            return MDR_RESULT_ERROR_BAD_ADDRESS;
        }

        std::string svcUUID(serviceUUID);

        // Launch connection on background thread
        ptr->connectThread = std::jthread([ptr, btAddr, svcUUID](std::stop_token stop)
        {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
            MDR_LOG("[BLE] Connect thread started (MTA), addr=0x{:X}", (unsigned long long)btAddr);
            try
            {
                // Open BLE device by address
                ptr->device = BluetoothLEDevice::FromBluetoothAddressAsync(btAddr).get();
                if (!ptr->device)
                {
                    ptr->lastError = "BLE device not found or not reachable";
                    MDR_LOG("[BLE] FromBluetoothAddressAsync returned null");
                    ptr->connectResult = MDR_RESULT_ERROR_NOT_FOUND;
                    SetEvent(ptr->connectEvent);
                    return;
                }
                MDR_LOG("[BLE] BLE device opened: {}",
                    winrt::to_string(ptr->device.Name()));

                if (stop.stop_requested()) return;

                // Parse service UUID and find matching GATT service
                winrt::guid svcGuid;
                {
                    uint8_t uuidBytes[16];
                    if (serviceUUIDtoBytes(svcUUID.c_str(), uuidBytes) != 0)
                    {
                        ptr->lastError = "Invalid service UUID format";
                        MDR_LOG("[BLE] Invalid service UUID: {}", svcUUID);
                        ptr->connectResult = MDR_RESULT_ERROR_BAD_ADDRESS;
                        SetEvent(ptr->connectEvent);
                        return;
                    }
                    std::memcpy(&svcGuid, uuidBytes, 16);
                }

                MDR_LOG("[BLE] Looking for GATT service: {}", svcUUID);

                auto servicesResult = ptr->device.GetGattServicesAsync(BluetoothCacheMode::Uncached).get();
                MDR_LOG("[BLE] GetGattServicesAsync status: {}, count: {}",
                    (int)servicesResult.Status(), servicesResult.Services().Size());

                if (servicesResult.Status() != GattCommunicationStatus::Success)
                {
                    ptr->lastError = fmt::format("GATT service discovery failed (status={})",
                        (int)servicesResult.Status());
                    MDR_LOG("[BLE] {}", ptr->lastError);
                    ptr->connectResult = MDR_RESULT_ERROR_NET;
                    SetEvent(ptr->connectEvent);
                    return;
                }

                if (stop.stop_requested()) return;

                // Find the target service by UUID string comparison
                GattDeviceService targetService{nullptr};
                for (auto const& svc : servicesResult.Services())
                {
                    std::string foundUUID = GuidToString(svc.Uuid());
                    MDR_LOG("[BLE]   Found service: {}", foundUUID);
                    if (_stricmp(foundUUID.c_str(), svcUUID.c_str()) == 0)
                    {
                        targetService = svc;
                        MDR_LOG("[BLE]   -> Matched target service!");
                        break;
                    }
                }

                if (!targetService)
                {
                    ptr->lastError = fmt::format("GATT service {} not found on device", svcUUID);
                    MDR_LOG("[BLE] {}", ptr->lastError);
                    ptr->connectResult = MDR_RESULT_ERROR_NOT_FOUND;
                    SetEvent(ptr->connectEvent);
                    return;
                }

                ptr->service = targetService;

                // Enumerate characteristics to find Write and Notify ones
                auto charsResult = targetService.GetCharacteristicsAsync(BluetoothCacheMode::Uncached).get();
                if (charsResult.Status() != GattCommunicationStatus::Success)
                {
                    ptr->lastError = "Failed to enumerate GATT characteristics";
                    MDR_LOG("[BLE] GetCharacteristicsAsync failed: status={}",
                        (int)charsResult.Status());
                    ptr->connectResult = MDR_RESULT_ERROR_NET;
                    SetEvent(ptr->connectEvent);
                    return;
                }

                MDR_LOG("[BLE] Found {} characteristic(s) in service",
                    charsResult.Characteristics().Size());

                for (auto const& ch : charsResult.Characteristics())
                {
                    auto props = ch.CharacteristicProperties();
                    int mask = PropertiesToBitmask(props);
                    std::string charUUID = GuidToString(ch.Uuid());
                    MDR_LOG("[BLE]   Characteristic: {} [{}]",
                        charUUID, PropertiesToString(mask));

                    // Pick the first writable characteristic (prefer WriteWithoutResponse for lower latency)
                    if (!ptr->writeChar &&
                        ((props & GattCharacteristicProperties::Write) != GattCharacteristicProperties::None ||
                         (props & GattCharacteristicProperties::WriteWithoutResponse) != GattCharacteristicProperties::None))
                    {
                        ptr->writeChar = ch;
                        ptr->useWriteWithoutResponse =
                            (props & GattCharacteristicProperties::WriteWithoutResponse) != GattCharacteristicProperties::None;
                        MDR_LOG("[BLE]   -> Selected as WRITE characteristic (writeNoResp={})",
                            (int)ptr->useWriteWithoutResponse);
                    }

                    // Pick the first notifiable characteristic
                    if (!ptr->notifyChar &&
                        ((props & GattCharacteristicProperties::Notify) != GattCharacteristicProperties::None ||
                         (props & GattCharacteristicProperties::Indicate) != GattCharacteristicProperties::None))
                    {
                        ptr->notifyChar = ch;
                        MDR_LOG("[BLE]   -> Selected as NOTIFY characteristic");
                    }
                }

                if (!ptr->writeChar)
                {
                    ptr->lastError = "No writable GATT characteristic found in service";
                    MDR_LOG("[BLE] {}", ptr->lastError);
                    ptr->connectResult = MDR_RESULT_ERROR_NOT_FOUND;
                    SetEvent(ptr->connectEvent);
                    return;
                }

                if (!ptr->notifyChar)
                {
                    ptr->lastError = "No notifiable GATT characteristic found in service";
                    MDR_LOG("[BLE] {}", ptr->lastError);
                    ptr->connectResult = MDR_RESULT_ERROR_NOT_FOUND;
                    SetEvent(ptr->connectEvent);
                    return;
                }

                if (stop.stop_requested()) return;

                // Subscribe to notifications
                auto cccdResult = ptr->notifyChar.WriteClientCharacteristicConfigurationDescriptorAsync(
                    GattClientCharacteristicConfigurationDescriptorValue::Notify).get();

                if (cccdResult != GattCommunicationStatus::Success)
                {
                    // Try Indicate if Notify fails
                    MDR_LOG("[BLE] Notify subscription failed (status={}), trying Indicate",
                        (int)cccdResult);
                    cccdResult = ptr->notifyChar.WriteClientCharacteristicConfigurationDescriptorAsync(
                        GattClientCharacteristicConfigurationDescriptorValue::Indicate).get();
                }

                if (cccdResult != GattCommunicationStatus::Success)
                {
                    ptr->lastError = fmt::format("Failed to subscribe to notifications (status={})",
                        (int)cccdResult);
                    MDR_LOG("[BLE] {}", ptr->lastError);
                    ptr->connectResult = MDR_RESULT_ERROR_NET;
                    SetEvent(ptr->connectEvent);
                    return;
                }

                MDR_LOG("[BLE] Notification subscription OK");

                // Register ValueChanged handler
                ptr->notifyToken = ptr->notifyChar.ValueChanged(
                    [ptr](GattCharacteristic const&, GattValueChangedEventArgs const& args)
                    {
                        auto buffer = args.CharacteristicValue();
                        auto reader = DataReader::FromBuffer(buffer);
                        uint32_t len = reader.UnconsumedBufferLength();

                        std::lock_guard lock(ptr->rxMutex);
                        size_t oldSize = ptr->rxBuffer.size();
                        ptr->rxBuffer.resize(oldSize + len);
                        reader.ReadBytes(
                            winrt::array_view<uint8_t>(ptr->rxBuffer.data() + oldSize, ptr->rxBuffer.data() + oldSize + len));

                        MDR_LOG("[BLE] Notification received: {} bytes (buffer now {} bytes)",
                            len, ptr->rxBuffer.size());
                        SetEvent(ptr->rxEvent);
                    });

                ptr->connected = true;
                ptr->connectResult = MDR_RESULT_OK;
                ptr->lastError = "Connected via BLE GATT";
                MDR_LOG("[BLE] BLE GATT connection established!");
                SetEvent(ptr->connectEvent);
            }
            catch (const winrt::hresult_error& ex)
            {
                ptr->lastError = winrt::to_string(ex.message());
                MDR_LOG("[BLE] Connect thread exception: {} (HRESULT=0x{:08X})",
                    ptr->lastError, (unsigned)ex.code());
                ptr->connectResult = MDR_RESULT_ERROR_NET;
                SetEvent(ptr->connectEvent);
            }
            catch (const std::exception& ex)
            {
                ptr->lastError = ex.what();
                MDR_LOG("[BLE] Connect thread std::exception: {}", ptr->lastError);
                ptr->connectResult = MDR_RESULT_ERROR_NET;
                SetEvent(ptr->connectEvent);
            }
        });

        ptr->lastError = "Connecting via BLE GATT...";
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
        try
        {
            RunOnMTAVoid([ptr]()
            {
                // Unsubscribe from notifications
                if (ptr->notifyChar)
                {
                    try
                    {
                        ptr->notifyChar.ValueChanged(ptr->notifyToken);
                        ptr->notifyChar.WriteClientCharacteristicConfigurationDescriptorAsync(
                            GattClientCharacteristicConfigurationDescriptorValue::None).get();
                    }
                    catch (...) {}
                    ptr->notifyChar = nullptr;
                }

                ptr->writeChar = nullptr;

                if (ptr->service)
                {
                    ptr->service.Close();
                    ptr->service = nullptr;
                }

                if (ptr->device)
                {
                    ptr->device.Close();
                    ptr->device = nullptr;
                }
            });
        }
        catch (...)
        {
            MDR_LOG("[BLE] Disconnect: exception during WinRT cleanup (ignored)");
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

        try
        {
            // Copy data for the MTA thread
            std::vector<uint8_t> data(reinterpret_cast<const uint8_t*>(src),
                                      reinterpret_cast<const uint8_t*>(src) + size);

            int result = RunOnMTA([&]() -> int
            {
                DataWriter writer;
                writer.WriteBytes(winrt::array_view<const uint8_t>(data));

                auto writeOption = ptr->useWriteWithoutResponse
                    ? GattWriteOption::WriteWithoutResponse
                    : GattWriteOption::WriteWithResponse;

                auto status = ptr->writeChar.WriteValueAsync(writer.DetachBuffer(), writeOption).get();

                if (status != GattCommunicationStatus::Success)
                {
                    ptr->lastError = fmt::format("GATT write failed (status={})", (int)status);
                    MDR_LOG("[BLE] {}", ptr->lastError);
                    return MDR_RESULT_ERROR_NET;
                }

                *pSent = size;
                MDR_LOG("[BLE] Send OK: {} bytes written", size);
                return MDR_RESULT_OK;
            });
            return result;
        }
        catch (const winrt::hresult_error& ex)
        {
            ptr->lastError = winrt::to_string(ex.message());
            MDR_LOG("[BLE] Send exception: {}", ptr->lastError);
            return MDR_RESULT_ERROR_NET;
        }
        catch (const std::exception& ex)
        {
            ptr->lastError = ex.what();
            MDR_LOG("[BLE] Send std::exception: {}", ptr->lastError);
            return MDR_RESULT_ERROR_NET;
        }
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

        ptr->lastError = "Poll wait failed";
        return MDR_RESULT_ERROR_NET;
    }

    static int FreeDevicesList(void*, MDRDeviceInfo** ppList) noexcept
    {
        if (*ppList)
        {
            delete[] *ppList;
            *ppList = nullptr;
        }
        return MDR_RESULT_OK;
    }

    static const char* GetLastError(void* user) noexcept
    {
        return static_cast<MDRConnectionWindowsBLE*>(user)->lastError.c_str();
    }
};

// --- C API ---

extern "C" {

MDRConnectionWindowsBLE* mdrConnectionWindowsBLECreate()
{
    MDR_LOG("[BLE] mdrConnectionWindowsBLECreate");
    return new MDRConnectionWindowsBLE();
}

MDRConnection* mdrConnectionWindowsBLEGet(MDRConnectionWindowsBLE* pConn)
{
    return &pConn->mdrConn;
}

void mdrConnectionWindowsBLEDestroy(MDRConnectionWindowsBLE* pConn)
{
    MDR_LOG("[BLE] mdrConnectionWindowsBLEDestroy");
    if (pConn)
    {
        MDRConnectionWindowsBLE::Disconnect(pConn);
        delete pConn;
    }
}
} // extern "C"
