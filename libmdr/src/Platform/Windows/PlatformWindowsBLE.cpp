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

// Helper: convert winrt::guid to string "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX"
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
    if ((props & GattCharacteristicProperties::Read) != GattCharacteristicProperties::None) mask |= 1;
    if ((props & GattCharacteristicProperties::Write) != GattCharacteristicProperties::None) mask |= 2;
    if ((props & GattCharacteristicProperties::WriteWithoutResponse) != GattCharacteristicProperties::None) mask |= 4;
    if ((props & GattCharacteristicProperties::Notify) != GattCharacteristicProperties::None) mask |= 8;
    if ((props & GattCharacteristicProperties::Indicate) != GattCharacteristicProperties::None) mask |= 16;
    return mask;
}

// Helper: convert properties bitmask to human-readable string
static std::string PropertiesToString(int mask)
{
    std::string result;
    if (mask & 1) result += "Read ";
    if (mask & 2) result += "Write ";
    if (mask & 4) result += "WriteNoResp ";
    if (mask & 8) result += "Notify ";
    if (mask & 16) result += "Indicate ";
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
        fprintf(stderr, "[BLE-DEBUG] MDRConnectionWindowsBLE created\n");
    }

    ~MDRConnectionWindowsBLE()
    {
        if (connectThread.joinable())
            connectThread.request_stop();
        CloseHandle(rxEvent);
        CloseHandle(connectEvent);
        fprintf(stderr, "[BLE-DEBUG] MDRConnectionWindowsBLE destroyed\n");
    }

    // Run a callable on an MTA thread to avoid STA assertion failures.
    // The UI thread (SDL/ImGui) is STA, but WinRT BLE APIs require MTA.
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
        fprintf(stderr, "[BLE-DEBUG] GetDevicesList called\n");

        try
        {
            return RunOnMTA([&]() -> int
            {
            // Get AQS selector for paired BLE devices
            auto selector = BluetoothLEDevice::GetDeviceSelectorFromPairingState(true);
            fprintf(stderr, "[BLE-DEBUG] Using BLE device selector for paired devices\n");

            // Find all matching devices
            auto deviceInfos = DeviceInformation::FindAllAsync(selector).get();
            fprintf(stderr, "[BLE-DEBUG] FindAllAsync returned %u device(s)\n", deviceInfos.Size());

            std::vector<MDRDeviceInfo> devices;

            for (uint32_t i = 0; i < deviceInfos.Size(); i++)
            {
                auto devInfo = deviceInfos.GetAt(i);
                std::string name = winrt::to_string(devInfo.Name());
                fprintf(stderr, "[BLE-DEBUG] Device #%u: name=\"%s\" id=\"%s\"\n",
                    i, name.c_str(), winrt::to_string(devInfo.Id()).c_str());

                // Open the BLE device to get its Bluetooth address
                try
                {
                    auto bleDevice = BluetoothLEDevice::FromIdAsync(devInfo.Id()).get();
                    if (!bleDevice)
                    {
                        fprintf(stderr, "[BLE-DEBUG]   Skipping device #%u: FromIdAsync returned null\n", i);
                        continue;
                    }

                    uint64_t addr = bleDevice.BluetoothAddress();
                    std::string macAddress = fmt::format("{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}",
                        (addr >> 40) & 0xFF, (addr >> 32) & 0xFF, (addr >> 24) & 0xFF,
                        (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF);

                    fprintf(stderr, "[BLE-DEBUG]   BLE address: %s\n", macAddress.c_str());

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
                    fprintf(stderr, "[BLE-DEBUG]   Error opening device #%u: %ls\n", i, ex.message().c_str());
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

            fprintf(stderr, "[BLE-DEBUG] GetDevicesList returning %d BLE device(s)\n", *pCount);
            return MDR_RESULT_OK;
            }); // end RunOnMTA
        }
        catch (const winrt::hresult_error& ex)
        {
            ptr->lastError = winrt::to_string(ex.message());
            fprintf(stderr, "[BLE-DEBUG] GetDevicesList error: %s\n", ptr->lastError.c_str());
            return MDR_RESULT_ERROR_NET;
        }
        catch (const std::exception& ex)
        {
            ptr->lastError = ex.what();
            fprintf(stderr, "[BLE-DEBUG] GetDevicesList exception: %s\n", ptr->lastError.c_str());
            return MDR_RESULT_ERROR_NET;
        }
    }

    static int Connect(void* user, const char* macAddress, const char* serviceUUID) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        fprintf(stderr, "[BLE-DEBUG] Connect called: mac=%s serviceUUID=%s\n", macAddress, serviceUUID);

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
            fprintf(stderr, "[BLE-DEBUG] Connect failed: invalid MAC address\n");
            return MDR_RESULT_ERROR_BAD_ADDRESS;
        }

        std::string svcUUID(serviceUUID);

        // Launch connection on background thread
        ptr->connectThread = std::jthread([ptr, btAddr, svcUUID](std::stop_token stop)
        {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
            fprintf(stderr, "[BLE-DEBUG] Connect thread started (MTA), addr=0x%llX\n", (unsigned long long)btAddr);
            try
            {
                // Open BLE device by address
                ptr->device = BluetoothLEDevice::FromBluetoothAddressAsync(btAddr).get();
                if (!ptr->device)
                {
                    ptr->lastError = "BLE device not found or not reachable";
                    fprintf(stderr, "[BLE-DEBUG] FromBluetoothAddressAsync returned null\n");
                    ptr->connectResult = MDR_RESULT_ERROR_NOT_FOUND;
                    SetEvent(ptr->connectEvent);
                    return;
                }
                fprintf(stderr, "[BLE-DEBUG] BLE device opened: %s\n",
                    winrt::to_string(ptr->device.Name()).c_str());

                if (stop.stop_requested()) return;

                // Parse service UUID and find matching GATT service
                winrt::guid svcGuid;
                {
                    uint8_t uuidBytes[16];
                    if (serviceUUIDtoBytes(svcUUID.c_str(), uuidBytes) != 0)
                    {
                        ptr->lastError = "Invalid service UUID format";
                        fprintf(stderr, "[BLE-DEBUG] Invalid service UUID: %s\n", svcUUID.c_str());
                        ptr->connectResult = MDR_RESULT_ERROR_BAD_ADDRESS;
                        SetEvent(ptr->connectEvent);
                        return;
                    }
                    std::memcpy(&svcGuid, uuidBytes, 16);
                    // Fix byte order: winrt::guid uses native endianness for Data1-3
                    // but we need to handle the conversion properly
                }

                fprintf(stderr, "[BLE-DEBUG] Looking for GATT service: %s\n", svcUUID.c_str());

                auto servicesResult = ptr->device.GetGattServicesAsync(BluetoothCacheMode::Uncached).get();
                fprintf(stderr, "[BLE-DEBUG] GetGattServicesAsync status: %d, count: %u\n",
                    (int)servicesResult.Status(), servicesResult.Services().Size());

                if (servicesResult.Status() != GattCommunicationStatus::Success)
                {
                    ptr->lastError = fmt::format("GATT service discovery failed (status={})",
                        (int)servicesResult.Status());
                    fprintf(stderr, "[BLE-DEBUG] %s\n", ptr->lastError.c_str());
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
                    fprintf(stderr, "[BLE-DEBUG]   Found service: %s\n", foundUUID.c_str());
                    if (_stricmp(foundUUID.c_str(), svcUUID.c_str()) == 0)
                    {
                        targetService = svc;
                        fprintf(stderr, "[BLE-DEBUG]   -> Matched target service!\n");
                        break;
                    }
                }

                if (!targetService)
                {
                    ptr->lastError = fmt::format("GATT service {} not found on device", svcUUID);
                    fprintf(stderr, "[BLE-DEBUG] %s\n", ptr->lastError.c_str());
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
                    fprintf(stderr, "[BLE-DEBUG] GetCharacteristicsAsync failed: status=%d\n",
                        (int)charsResult.Status());
                    ptr->connectResult = MDR_RESULT_ERROR_NET;
                    SetEvent(ptr->connectEvent);
                    return;
                }

                fprintf(stderr, "[BLE-DEBUG] Found %u characteristic(s) in service\n",
                    charsResult.Characteristics().Size());

                for (auto const& ch : charsResult.Characteristics())
                {
                    auto props = ch.CharacteristicProperties();
                    int mask = PropertiesToBitmask(props);
                    std::string charUUID = GuidToString(ch.Uuid());
                    fprintf(stderr, "[BLE-DEBUG]   Characteristic: %s [%s]\n",
                        charUUID.c_str(), PropertiesToString(mask).c_str());

                    // Pick the first writable characteristic (prefer WriteWithoutResponse for lower latency)
                    if (!ptr->writeChar &&
                        ((props & GattCharacteristicProperties::Write) != GattCharacteristicProperties::None ||
                         (props & GattCharacteristicProperties::WriteWithoutResponse) != GattCharacteristicProperties::None))
                    {
                        ptr->writeChar = ch;
                        ptr->useWriteWithoutResponse =
                            (props & GattCharacteristicProperties::WriteWithoutResponse) != GattCharacteristicProperties::None;
                        fprintf(stderr, "[BLE-DEBUG]   -> Selected as WRITE characteristic (writeNoResp=%d)\n",
                            (int)ptr->useWriteWithoutResponse);
                    }

                    // Pick the first notifiable characteristic
                    if (!ptr->notifyChar &&
                        ((props & GattCharacteristicProperties::Notify) != GattCharacteristicProperties::None ||
                         (props & GattCharacteristicProperties::Indicate) != GattCharacteristicProperties::None))
                    {
                        ptr->notifyChar = ch;
                        fprintf(stderr, "[BLE-DEBUG]   -> Selected as NOTIFY characteristic\n");
                    }
                }

                if (!ptr->writeChar)
                {
                    ptr->lastError = "No writable GATT characteristic found in service";
                    fprintf(stderr, "[BLE-DEBUG] %s\n", ptr->lastError.c_str());
                    ptr->connectResult = MDR_RESULT_ERROR_NOT_FOUND;
                    SetEvent(ptr->connectEvent);
                    return;
                }

                if (!ptr->notifyChar)
                {
                    ptr->lastError = "No notifiable GATT characteristic found in service";
                    fprintf(stderr, "[BLE-DEBUG] %s\n", ptr->lastError.c_str());
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
                    fprintf(stderr, "[BLE-DEBUG] Notify subscription failed (status=%d), trying Indicate\n",
                        (int)cccdResult);
                    cccdResult = ptr->notifyChar.WriteClientCharacteristicConfigurationDescriptorAsync(
                        GattClientCharacteristicConfigurationDescriptorValue::Indicate).get();
                }

                if (cccdResult != GattCommunicationStatus::Success)
                {
                    ptr->lastError = fmt::format("Failed to subscribe to notifications (status={})",
                        (int)cccdResult);
                    fprintf(stderr, "[BLE-DEBUG] %s\n", ptr->lastError.c_str());
                    ptr->connectResult = MDR_RESULT_ERROR_NET;
                    SetEvent(ptr->connectEvent);
                    return;
                }

                fprintf(stderr, "[BLE-DEBUG] Notification subscription OK\n");

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

                        fprintf(stderr, "[BLE-DEBUG] Notification received: %u bytes (buffer now %zu bytes)\n",
                            len, ptr->rxBuffer.size());
                        SetEvent(ptr->rxEvent);
                    });

                ptr->connected = true;
                ptr->connectResult = MDR_RESULT_OK;
                ptr->lastError = "Connected via BLE GATT";
                fprintf(stderr, "[BLE-DEBUG] BLE GATT connection established!\n");
                SetEvent(ptr->connectEvent);
            }
            catch (const winrt::hresult_error& ex)
            {
                ptr->lastError = winrt::to_string(ex.message());
                fprintf(stderr, "[BLE-DEBUG] Connect thread exception: %s (HRESULT=0x%08X)\n",
                    ptr->lastError.c_str(), (unsigned)ex.code());
                ptr->connectResult = MDR_RESULT_ERROR_NET;
                SetEvent(ptr->connectEvent);
            }
            catch (const std::exception& ex)
            {
                ptr->lastError = ex.what();
                fprintf(stderr, "[BLE-DEBUG] Connect thread std::exception: %s\n", ptr->lastError.c_str());
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
        fprintf(stderr, "[BLE-DEBUG] Disconnect called\n");

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
            fprintf(stderr, "[BLE-DEBUG] Disconnect: exception during WinRT cleanup (ignored)\n");
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

        fprintf(stderr, "[BLE-DEBUG] Disconnected\n");
    }

    static int Recv(void* user, char* dst, int size, int* pReceived) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);
        if (!ptr->connected)
            return MDR_RESULT_ERROR_NO_CONNECTION;

        std::lock_guard lock(ptr->rxMutex);
        if (ptr->rxBuffer.empty())
            return MDR_RESULT_INPROGRESS;

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

        fprintf(stderr, "[BLE-DEBUG] Send: %d bytes\n", size);

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
                    fprintf(stderr, "[BLE-DEBUG] %s\n", ptr->lastError.c_str());
                    return MDR_RESULT_ERROR_NET;
                }

                *pSent = size;
                fprintf(stderr, "[BLE-DEBUG] Send OK: %d bytes written\n", size);
                return MDR_RESULT_OK;
            });
            return result;
        }
        catch (const winrt::hresult_error& ex)
        {
            ptr->lastError = winrt::to_string(ex.message());
            fprintf(stderr, "[BLE-DEBUG] Send exception: %s\n", ptr->lastError.c_str());
            return MDR_RESULT_ERROR_NET;
        }
        catch (const std::exception& ex)
        {
            ptr->lastError = ex.what();
            fprintf(stderr, "[BLE-DEBUG] Send std::exception: %s\n", ptr->lastError.c_str());
            return MDR_RESULT_ERROR_NET;
        }
    }

    static int Poll(void* user, int timeout) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindowsBLE*>(user);

        // If not yet connected, wait for connection completion
        if (!ptr->connected)
        {
            if (ptr->connectResult != MDR_RESULT_INPROGRESS)
                return ptr->connectResult.load();

            DWORD waitMs = (timeout < 0) ? INFINITE : static_cast<DWORD>(timeout);
            DWORD waitResult = WaitForSingleObject(ptr->connectEvent, waitMs);

            if (waitResult == WAIT_OBJECT_0)
                return ptr->connectResult.load();
            return MDR_RESULT_ERROR_TIMEOUT;
        }

        // Check if data is already available in the buffer (no need to wait)
        {
            std::lock_guard lock(ptr->rxMutex);
            if (!ptr->rxBuffer.empty())
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
    fprintf(stderr, "[BLE-DEBUG] mdrConnectionWindowsBLECreate\n");
    return new MDRConnectionWindowsBLE();
}

MDRConnection* mdrConnectionWindowsBLEGet(MDRConnectionWindowsBLE* pConn)
{
    return &pConn->mdrConn;
}

void mdrConnectionWindowsBLEDestroy(MDRConnectionWindowsBLE* pConn)
{
    fprintf(stderr, "[BLE-DEBUG] mdrConnectionWindowsBLEDestroy\n");
    if (pConn)
    {
        MDRConnectionWindowsBLE::Disconnect(pConn);
        delete pConn;
    }
}

int mdrConnectionBLEEnumerateGatt(
    MDRConnectionWindowsBLE* pConn,
    const char* macAddress,
    MDRBLEGattEnumCallback callback,
    void* ctx)
{
    fprintf(stderr, "[BLE-DEBUG] EnumerateGatt called: mac=%s\n", macAddress);

    uint64_t btAddr = macAddressToULL(macAddress);
    if (btAddr == ~0ULL)
    {
        pConn->lastError = "Invalid MAC address format";
        fprintf(stderr, "[BLE-DEBUG] EnumerateGatt: invalid MAC\n");
        return MDR_RESULT_ERROR_BAD_ADDRESS;
    }

    try
    {
        return MDRConnectionWindowsBLE::RunOnMTA([&]() -> int
        {
        auto device = BluetoothLEDevice::FromBluetoothAddressAsync(btAddr).get();
        if (!device)
        {
            pConn->lastError = "BLE device not found or not reachable";
            fprintf(stderr, "[BLE-DEBUG] EnumerateGatt: device not found\n");
            return MDR_RESULT_ERROR_NOT_FOUND;
        }

        fprintf(stderr, "[BLE-DEBUG] EnumerateGatt: opened device \"%s\"\n",
            winrt::to_string(device.Name()).c_str());

        auto servicesResult = device.GetGattServicesAsync(BluetoothCacheMode::Uncached).get();
        if (servicesResult.Status() != GattCommunicationStatus::Success)
        {
            pConn->lastError = fmt::format("GATT service discovery failed (status={})",
                (int)servicesResult.Status());
            fprintf(stderr, "[BLE-DEBUG] EnumerateGatt: %s\n", pConn->lastError.c_str());
            device.Close();
            return MDR_RESULT_ERROR_NET;
        }

        fprintf(stderr, "[BLE-DEBUG] === GATT Service Enumeration ===\n");
        fprintf(stderr, "[BLE-DEBUG] Found %u service(s)\n", servicesResult.Services().Size());

        int serviceIndex = 0;
        for (auto const& svc : servicesResult.Services())
        {
            std::string svcUUID = GuidToString(svc.Uuid());
            fprintf(stderr, "[BLE-DEBUG] Service #%d: %s\n", serviceIndex, svcUUID.c_str());

            auto charsResult = svc.GetCharacteristicsAsync(BluetoothCacheMode::Uncached).get();
            if (charsResult.Status() != GattCommunicationStatus::Success)
            {
                fprintf(stderr, "[BLE-DEBUG]   (failed to enumerate characteristics, status=%d)\n",
                    (int)charsResult.Status());
                serviceIndex++;
                continue;
            }

            int charIndex = 0;
            for (auto const& ch : charsResult.Characteristics())
            {
                std::string charUUID = GuidToString(ch.Uuid());
                int props = PropertiesToBitmask(ch.CharacteristicProperties());
                fprintf(stderr, "[BLE-DEBUG]   Char #%d: %s [%s]\n",
                    charIndex, charUUID.c_str(), PropertiesToString(props).c_str());

                if (callback)
                    callback(ctx, svcUUID.c_str(), charUUID.c_str(), props);

                charIndex++;
            }

            serviceIndex++;
        }

        fprintf(stderr, "[BLE-DEBUG] === End GATT Enumeration ===\n");
        device.Close();
        return MDR_RESULT_OK;
        }); // end RunOnMTA
    }
    catch (const winrt::hresult_error& ex)
    {
        pConn->lastError = winrt::to_string(ex.message());
        fprintf(stderr, "[BLE-DEBUG] EnumerateGatt exception: %s (HRESULT=0x%08X)\n",
            pConn->lastError.c_str(), (unsigned)ex.code());
        return MDR_RESULT_ERROR_NET;
    }
    catch (const std::exception& ex)
    {
        pConn->lastError = ex.what();
        fprintf(stderr, "[BLE-DEBUG] EnumerateGatt std::exception: %s\n", pConn->lastError.c_str());
        return MDR_RESULT_ERROR_NET;
    }
}

} // extern "C"
