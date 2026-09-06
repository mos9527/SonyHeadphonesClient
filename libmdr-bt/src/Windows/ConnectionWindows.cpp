#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <BluetoothAPIs.h>
#include <rpc.h>
#include <winsock2.h>
#include <ws2bth.h>

#include "../Utils.hpp"
#include <mdr-bt/ConnectionWindows.h>

static bool gWSAStartup = false;
struct MDRConnectionWindows
{
    MDRConnection mdrConn;
    mdr::String lastError;

    SOCKET conn;
    MDRConnectionWindows() noexcept :
        lastError(""), mdrConn({.user = this,
                                .connect = Connect,
                                .disconnect = Disconnect,
                                .recv = Recv,
                                .send = Send,
                                .poll = Poll,
                                .getDevicesList = GetDevicesList,
                                .freeDevicesList = FreeDevicesList,
                                .getLastError = GetLastError}),
        conn(INVALID_SOCKET)
    {
    }
    static mdr::String ToU8String(WCHAR* szMessage)
    {
        char szMessageU8[1024];
        WideCharToMultiByte(CP_UTF8, 0, szMessage, -1, szMessageU8, sizeof(szMessageU8), NULL, NULL);
        return szMessageU8;
    }
    static mdr::String FormatErrorString(DWORD err)
    {
        WCHAR szMessage[1024];
        if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), szMessage,
                           sizeof(szMessage), NULL) == 0)
            return "Unknown Error";
        mdr::String res = ToU8String(szMessage);
        res.pop_back(); // \n
        res.pop_back(); // \r
        return res;
    }
    static MDRResult Connect(void* user, const char* macAddress, const char* serviceUUID) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindows*>(user);
        MDR_LOG("[BT] Connect called: mac={} uuid={}", macAddress, serviceUUID);
        if (!gWSAStartup)
        {
            WSAData data;
            if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
            {
                MDR_LOG("[BT] WSAStartup FAILED: {}", WSAGetLastError());
                ptr->lastError = FormatErrorString(WSAGetLastError());
                return MDR_RESULT_ERROR_NET;
            }
            gWSAStartup = true;
            MDR_LOG("[BT] WSAStartup OK");
        }
        ptr->conn = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
        MDR_LOG("[BT] socket() returned {}", (long long)ptr->conn);
        ULONG nonblock = 1;
        if (ioctlsocket(ptr->conn, FIONBIO, &nonblock) != 0)
        {
            MDR_LOG("[BT] ioctlsocket FIONBIO FAILED: {}", ::GetLastError());
            ptr->lastError = FormatErrorString(::GetLastError());
            closesocket(ptr->conn);
            return 1;
        }
        if (ptr->conn == INVALID_SOCKET)
        {
            MDR_LOG("[BT] socket is INVALID_SOCKET: {}", ::GetLastError());
            ptr->lastError = FormatErrorString(::GetLastError());
            return MDR_RESULT_ERROR_NET;
        }
        ULONG enable = TRUE;
        ::setsockopt(ptr->conn, SOL_RFCOMM, SO_BTH_AUTHENTICATE, reinterpret_cast<char*>(&enable), sizeof(enable));
        ::setsockopt(ptr->conn, SOL_RFCOMM, SO_BTH_ENCRYPT, reinterpret_cast<char*>(&enable), sizeof(enable));
        SOCKADDR_BTH sab = {0};
        sab.addressFamily = AF_BTH;
        RPC_STATUS errCode = ::UuidFromStringA((RPC_CSTR)serviceUUID, &sab.serviceClassId);
        if (errCode != RPC_S_OK)
        {
            MDR_LOG("[BT] UuidFromStringA FAILED: {}", errCode);
            closesocket(ptr->conn);
            ptr->lastError = FormatErrorString(errCode);
            return MDR_RESULT_ERROR_NET;
        }
        sab.btAddr = macAddressToULL(macAddress);
        MDR_LOG("[BT] Connecting to btAddr=0x{:X}, serviceClassId={{{:08X}-{:04X}-{:04X}-...}}",
            (unsigned long long)sab.btAddr, sab.serviceClassId.Data1, sab.serviceClassId.Data2, sab.serviceClassId.Data3);
        int ret = ::connect(ptr->conn, (sockaddr*)&sab, sizeof(sab));
        if (ret == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            MDR_LOG("[BT] connect() returned SOCKET_ERROR, WSA err={}", err);
            if (err != WSAEWOULDBLOCK && err != WSAEINPROGRESS)
            {
                MDR_LOG("[BT] Connect FAILED (not WOULDBLOCK): {}", FormatErrorString(err));
                closesocket(ptr->conn);
                ptr->lastError = FormatErrorString(err);
                return MDR_RESULT_ERROR_NET;
            }
            MDR_LOG("[BT] Connect in progress (WOULDBLOCK/INPROGRESS) - normal for non-blocking");
        }
        ptr->lastError = "Connecting to WSA Bluetooth socket";
        return MDR_RESULT_INPROGRESS;
    }

    static void Disconnect(void* user) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindows*>(user);
        if (ptr->conn != INVALID_SOCKET)
        {
            shutdown(ptr->conn, SD_BOTH);
            closesocket(ptr->conn);
            ptr->conn = INVALID_SOCKET;
        }
    }

    static MDRResult Recv(void* user, char* dst, int size, int* pReceived) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindows*>(user);
        if (ptr->conn == INVALID_SOCKET)
            return MDR_RESULT_ERROR_NO_CONNECTION;
        int receieved = recv(ptr->conn, dst, size, 0);
        if (receieved == 0)
            return MDR_RESULT_ERROR_NO_CONNECTION;
        if (receieved < 0)
        {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK || err == WSAEINPROGRESS)
                return MDR_RESULT_INPROGRESS;
            ptr->lastError = FormatErrorString(err);
            return MDR_RESULT_ERROR_NET;
        }
        *pReceived = receieved;
        return MDR_RESULT_OK;
    }

    static MDRResult Send(void* user, const char* src, int size, int* pSent) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindows*>(user);
        if (ptr->conn == INVALID_SOCKET)
            return MDR_RESULT_ERROR_NO_CONNECTION;
        int sent = send(ptr->conn, src, size, 0);
        if (sent == 0)
            return MDR_RESULT_ERROR_NO_CONNECTION;
        if (sent < 0)
        {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK || err == WSAEINPROGRESS)
                return MDR_RESULT_INPROGRESS;
            ptr->lastError = FormatErrorString(err);
            return MDR_RESULT_ERROR_NET;
        }
        *pSent = sent;
        return MDR_RESULT_OK;
    }

static MDRResult Poll(void* user, int timeout) noexcept
{
    auto* ptr = static_cast<MDRConnectionWindows*>(user);
    if (ptr->conn == INVALID_SOCKET)
        return MDR_RESULT_ERROR_NO_CONNECTION;

    // Set up the file descriptor sets
    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);

    // Add your single socket to all three sets
    FD_SET(ptr->conn, &readfds);
    FD_SET(ptr->conn, &writefds);
    FD_SET(ptr->conn, &exceptfds);

    // `select` uses a timeval struct, not a plain millisecond int
    struct timeval tv;
    tv.tv_sec = timeout / 1000; // Convert ms to seconds
    tv.tv_usec = (timeout % 1000) * 1000; // Convert remainder ms to microseconds

    // Note: The first parameter (nfds) is ignored in Winsock.
    // Pass a NULL timeval for an infinite wait (if timeout < 0)
    int res = select(0, &readfds, &writefds, &exceptfds, (timeout < 0) ? NULL : &tv);

    if (res > 0)
    {
        // A non-blocking connect *fails* by signaling the exception set.
        if (FD_ISSET(ptr->conn, &exceptfds))
        {
            int err = 0;
            int errLen = sizeof(err);
            // You MUST get the actual error code from the socket
            if (getsockopt(ptr->conn, SOL_SOCKET, SO_ERROR, (char*)&err, &errLen) == 0)
            {
                MDR_LOG("[BT] Poll: exceptfds signaled! SO_ERROR={} ({})", err, FormatErrorString(err));
                ptr->lastError = FormatErrorString(err);
            }
            else
            {
                MDR_LOG("[BT] Poll: exceptfds signaled! getsockopt failed, WSA={}", WSAGetLastError());
                // Fallback if getsockopt fails
                ptr->lastError = FormatErrorString(WSAGetLastError());
            }
            return MDR_RESULT_ERROR_NET;
        }

        // A non-blocking connect *succeeds* by signaling the write set.
        // Or, if already connected, data is available (read set).
        if (FD_ISSET(ptr->conn, &readfds) || FD_ISSET(ptr->conn, &writefds))
        {
            return MDR_RESULT_OK;
        }

        // Should be unreachable if res > 0, but good to be safe
        return MDR_RESULT_ERROR_TIMEOUT;
    }

    if (res == 0) // Timeout
        return MDR_RESULT_ERROR_TIMEOUT;

    // res < 0 (SOCKET_ERROR)
    ptr->lastError = FormatErrorString(WSAGetLastError());
    return MDR_RESULT_ERROR_NET;
}

    static MDRResult GetDevicesList(void* user, MDRDeviceInfo** ppList, int* pCount) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindows*>(user);
        MDR_LOG("[BT] GetDevicesList called");

        BLUETOOTH_FIND_RADIO_PARAMS radioFindParams = {sizeof(BLUETOOTH_FIND_RADIO_PARAMS)};
        BLUETOOTH_DEVICE_SEARCH_PARAMS deviceSearchParams = {.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS),
                                                             .fReturnAuthenticated = 0,
                                                             .fReturnRemembered = 0,
                                                             .fReturnUnknown = 0,
                                                             .fReturnConnected = 1,
                                                             .fIssueInquiry = 0,
                                                             .cTimeoutMultiplier = 15,
                                                             .hRadio = NULL};
        HANDLE radio;
        HBLUETOOTH_RADIO_FIND radioFindHandle = BluetoothFindFirstRadio(&radioFindParams, &radio);
        if (!radioFindHandle)
        {
            DWORD err = ::GetLastError();
            MDR_LOG("[BT] BluetoothFindFirstRadio FAILED, error={}", err);
            if (err == ERROR_NO_MORE_ITEMS)
            {
                MDR_LOG("[BT] No Bluetooth radios found on this system!");
                *ppList = nullptr;
                *pCount = 0;
            }
            else
            {
                ptr->lastError = FormatErrorString(err);
                MDR_LOG("[BT] Radio error: {}", ptr->lastError);
                return MDR_RESULT_ERROR_NET;
            }
        }
        else
        {
            MDR_LOG("[BT] BluetoothFindFirstRadio OK, radio handle={}", (void*)radio);
        }
        mdr::Vector<MDRDeviceInfo> devices;
        int radioIndex = 0;
        do
        {
            MDR_LOG("[BT] Scanning radio #{} (handle={})", radioIndex, (void*)radio);
            deviceSearchParams.hRadio = radio;
            BLUETOOTH_DEVICE_INFO deviceInfo = {sizeof(BLUETOOTH_DEVICE_INFO)};
            HBLUETOOTH_DEVICE_FIND deviceFindHandle = BluetoothFindFirstDevice(&deviceSearchParams, &deviceInfo);
            if (!deviceFindHandle)
            {
                DWORD err = ::GetLastError();
                MDR_LOG("[BT] BluetoothFindFirstDevice FAILED on radio #{} error={}", radioIndex, err);
                if (err == ERROR_NO_MORE_ITEMS)
                {
                    MDR_LOG("[BT] No devices found on radio #{}", radioIndex);
                    break;
                }
                else
                {
                    ptr->lastError = FormatErrorString(err);
                    MDR_LOG("[BT] Device search error: {}", ptr->lastError);
                    return MDR_RESULT_ERROR_NET;
                }
            }
            do
            {
                auto const& pBytes = deviceInfo.Address.rgBytes;
                mdr::String szName = ToU8String(deviceInfo.szName);
                mdr::String szMacAddress = mdr::Format("{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}", pBytes[5], pBytes[4],
                               pBytes[3], pBytes[2], pBytes[1], pBytes[0]);
                MDR_LOG("[BT] Found device: name=\"{}\" mac={} connected={} authenticated={} remembered={}",
                        szName.c_str(), szMacAddress.c_str(),
                        (int)deviceInfo.fConnected, (int)deviceInfo.fAuthenticated, (int)deviceInfo.fRemembered);
                devices.emplace_back();
                auto& back = devices.back();
                strncpy(back.szDeviceName, szName.c_str(), szName.size() + 1);
                strncpy(back.szDeviceMacAddress, szMacAddress.c_str(), szMacAddress.size() + 1);
            }
            while (BluetoothFindNextDevice(deviceFindHandle, &deviceInfo));
            if (!BluetoothFindDeviceClose(deviceFindHandle))
            {
                ptr->lastError = FormatErrorString(::GetLastError());
                return MDR_RESULT_ERROR_NET;
            }
            radioIndex++;
        }
        while (BluetoothFindNextRadio(radioFindHandle, &radio));
        if (!BluetoothFindRadioClose(radioFindHandle))
        {
            ptr->lastError = FormatErrorString(::GetLastError());
            return MDR_RESULT_ERROR_NET;
        }
        *ppList = mdr::MDRAllocator<MDRDeviceInfo>().allocate(devices.size());
        std::memcpy(*ppList, devices.data(), devices.size() * sizeof(MDRDeviceInfo));
        *pCount = devices.size();
        MDR_LOG("[BT] GetDevicesList returning {} device(s)", *pCount);
        return MDR_RESULT_OK;
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
        auto* ptr = static_cast<MDRConnectionWindows*>(user);
        return ptr->lastError.c_str();
    }
};

extern "C" {
MDR_API MDRConnectionWindows* mdrConnectionWindowsCreate() { return mdr::Construct<MDRConnectionWindows>(); }
MDR_API void mdrConnectionWindowsDestroy(MDRConnectionWindows* instance) { mdr::Destruct(instance); }
MDR_API MDRConnection* mdrConnectionWindowsGet(MDRConnectionWindows* instance) { return &instance->mdrConn; }
}
