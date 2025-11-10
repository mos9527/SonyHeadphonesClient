#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <BluetoothAPIs.h>
#include <rpc.h>
#include <winsock2.h>
#include <ws2bth.h>

#include <fmt/format.h>
#include <string>
#include <vector>

#include "../Platform.hpp"
#include <mdr-c/Platform/PlatformWindows.h>

static bool gWSAStartup = false;
struct MDRConnectionWindows
{
    MDRConnection mdrConn;
    std::string lastError;

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
    static std::string ToU8String(WCHAR* szMessage)
    {
        char szMessageU8[1024];
        WideCharToMultiByte(CP_UTF8, 0, szMessage, -1, szMessageU8, sizeof(szMessageU8), NULL, NULL);
        return szMessageU8;
    }
    static std::string FormatErrorString(DWORD err)
    {
        WCHAR szMessage[1024];
        if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), szMessage,
                           sizeof(szMessage), NULL) == 0)
            return "Unknown Error";
        std::string res = ToU8String(szMessage);
        res.pop_back(); // \n
        res.pop_back(); // \r
        return res;
    }
    static int Connect(void* user, const char* macAddress, const char* serviceUUID) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindows*>(user);
        if (!gWSAStartup)
        {
            WSAData data;
            if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
            {
                ptr->lastError = FormatErrorString(WSAGetLastError());
                return MDR_RESULT_ERROR_NET;
            }
            gWSAStartup = true;
        }
        ptr->conn = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
        ULONG nonblock = 1;
        if (ioctlsocket(ptr->conn, FIONBIO, &nonblock) != 0)
        {
            ptr->lastError = FormatErrorString(::GetLastError());
            closesocket(ptr->conn);
            return 1;
        }
        if (ptr->conn == INVALID_SOCKET)
        {
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
            closesocket(ptr->conn);
            ptr->lastError = FormatErrorString(errCode);
            return MDR_RESULT_ERROR_NET;
        }
        sab.btAddr = macAddressToULL(macAddress);
        int ret = ::connect(ptr->conn, (sockaddr*)&sab, sizeof(sab));
        if (ret == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            if (err != WSAEWOULDBLOCK && err != WSAEINPROGRESS)
            {
                closesocket(ptr->conn);
                ptr->lastError = FormatErrorString(err);
                return MDR_RESULT_ERROR_NET;
            }
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

    static int Recv(void* user, char* dst, int size, int* pReceived) noexcept
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

    static int Send(void* user, const char* src, int size, int* pSent) noexcept
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

static int Poll(void* user, int timeout) noexcept
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
                ptr->lastError = FormatErrorString(err);
            }
            else
            {
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

    static int GetDevicesList(void* user, MDRDeviceInfo** ppList, int* pCount) noexcept
    {
        auto* ptr = static_cast<MDRConnectionWindows*>(user);
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
            if (::GetLastError() == ERROR_NO_MORE_ITEMS)
            {
                *ppList = nullptr;
                *pCount = 0;
            }
            else
            {
                ptr->lastError = FormatErrorString(::GetLastError());
                return MDR_RESULT_ERROR_NET;
            }
        }
        std::vector<MDRDeviceInfo> devices;
        do
        {
            deviceSearchParams.hRadio = radio;
            BLUETOOTH_DEVICE_INFO deviceInfo = {sizeof(BLUETOOTH_DEVICE_INFO)};
            HBLUETOOTH_DEVICE_FIND deviceFindHandle = BluetoothFindFirstDevice(&deviceSearchParams, &deviceInfo);
            if (!deviceFindHandle)
            {
                if (::GetLastError() == ERROR_NO_MORE_ITEMS)
                {
                    break;
                }
                else
                {
                    ptr->lastError = FormatErrorString(::GetLastError());
                    return MDR_RESULT_ERROR_NET;
                }
            }
            do
            {
                auto const& pBytes = deviceInfo.Address.rgBytes;
                std::string szName = ToU8String(deviceInfo.szName);
                std::string szMacAddress = fmt::format("{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}", pBytes[5], pBytes[4],
                                                 pBytes[3], pBytes[2], pBytes[1], pBytes[0]);
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
        }
        while (BluetoothFindNextRadio(radioFindHandle, &radio));
        if (!BluetoothFindRadioClose(radioFindHandle))
        {
            ptr->lastError = FormatErrorString(::GetLastError());
            return MDR_RESULT_ERROR_NET;
        } 
        *ppList = new MDRDeviceInfo[devices.size()];
        std::memcpy(*ppList, devices.data(), devices.size() * sizeof(MDRDeviceInfo));
        *pCount = devices.size();
        return MDR_RESULT_OK;
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
        auto* ptr = static_cast<MDRConnectionWindows*>(user);
        return ptr->lastError.c_str();
    }
};

extern "C" {
MDRConnectionWindows* mdrConnectionWindowsCreate() { return new MDRConnectionWindows(); }
void mdrConnectionWindowsDestroy(MDRConnectionWindows* instance) { delete instance; }
MDRConnection* mdrConnectionWindowsGet(MDRConnectionWindows* instance) { return &instance->mdrConn; }
}
