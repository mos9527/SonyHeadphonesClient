#include <mdr/WindowsPlatform.h>
#include "../Platform.hpp"

#include <BluetoothAPIs.h>
#include <rpc.h>
#include <winsock2.h>
#include <ws2bth.h>

#include <fmt/format.h>
#include <string>
#include <vector>
static bool gWSAStartup = false;
struct MDRConnectionWindows
{
    MDRConnection mdrConn;
    std::string lastError;

    SOCKET conn;
    MDRConnectionWindows() noexcept :
        lastError(MDR_DEFAULT_ERROR_STRING), mdrConn({.user = this,
                                                      .connect = Connect,
                                                      .disconnect = Disconnect,
                                                      .recv = Recv,
                                                      .send = Send,
                                                      .list = List,
                                                      .freeList = FreeList,
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
        return ToU8String(szMessage);
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
                ptr->lastError = FormatErrorString(::GetLastError());
                return MDR_RESULT_ERROR_NET;
            }
            // Wait for connect to complete
            WSAPOLLFD pfd{.fd = ptr->conn, .events = POLLOUT};
            int res = WSAPoll(&pfd, 1, -1);
            if (res <= 0)
            {
                ptr->lastError = FormatErrorString(WSAGetLastError());
                return MDR_RESULT_ERROR_NET;
            }
        }
        return MDR_RESULT_OK;
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
            if (err == WSAEWOULDBLOCK && err == WSAEINPROGRESS)
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
            if (err == WSAEWOULDBLOCK && err == WSAEINPROGRESS)
                return MDR_RESULT_INPROGRESS;
            ptr->lastError = FormatErrorString(err);
            return MDR_RESULT_ERROR_NET;
        }
        *pSent = sent;
        return MDR_RESULT_OK;
    }

    static int List(void* user, MDRDeviceInfo** ppList, int* pCount) noexcept
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
                devices.emplace_back(ToU8String(deviceInfo.szName),
                                     fmt::format("{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}", pBytes[0], pBytes[1],
                                                 pBytes[2], pBytes[3], pBytes[4], pBytes[5], pBytes[6]));
            }
            while (BluetoothFindNextDevice(deviceFindHandle, &deviceInfo));
            if (!BluetoothFindDeviceClose(deviceFindHandle))
            {
                ptr->lastError = FormatErrorString(::GetLastError());
                return MDR_RESULT_ERROR_NET;
            }
        }
        while (FindHandleBluetoothFindNextRadio(radioFindHandle, &radio));
        if (!BluetoothFindRadioClose(radioFindHandle))
        {
            ptr->lastError = FormatErrorString(::GetLastError());
            return MDR_RESULT_ERROR_NET;
        }
        return MDR_RESULT_OK;
    }

    static int FreeList(void*, MDRDeviceInfo** ppList) noexcept
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
