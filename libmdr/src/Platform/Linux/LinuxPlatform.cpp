#include <mdr/ConnectionLinux.h>
#include "DBusHelper.hpp"

#include <unistd.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <bluetooth/rfcomm.h>

struct MDRConnectionLinux
{
    MDRConnection mdrConn;

    char const* lastError;
    DBusConnection* dbusConn;
    int fd;

    MDRConnectionLinux() noexcept :
        lastError(nullptr),
        dbusConn(dbus_open_system_bus()), fd(0),
        mdrConn({
            .user = this,
            .connect = Connect,
            .disconnect = Disconnect,
            .recv = Recv,
            .send = Send,
            .list = List,
            .freeList = FreeList,
            .getLastError = GetLastError
        })
    {
    }

    static int Connect(void* user, const char* macAddress, const uint8_t* uuid) noexcept
    {
        auto* ptr = static_cast<MDRConnectionLinux*>(user);
        ptr->fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        if (!ptr->fd)
        {
            ptr->lastError = "Failed to create RFCOMM socket";
            return MDR_RESULT_ERROR_NET;
        }
        unsigned int linkmode = RFCOMM_LM_AUTH | RFCOMM_LM_ENCRYPT;
        setsockopt(ptr->fd, SOL_RFCOMM, RFCOMM_LM, &linkmode, sizeof(linkmode));
        uint8_t ch = sdp_getServiceChannel(macAddress, uuid);
        if (!ch)
        {
            ptr->lastError = "Failed to get service channel via SDP";
            return MDR_RESULT_ERROR_NET;
        }
        sockaddr_rc addr{
            .rc_family = AF_BLUETOOTH,
            .rc_channel = ch
        };
        str2ba(macAddress, &addr.rc_bdaddr);
        int res = connect(ptr->fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
        if (res != 0 && res != EINPROGRESS)
        {
            close(ptr->fd);
            ptr->fd = 0;
            ptr->lastError = strerror(errno);
            return MDR_RESULT_ERROR_NET;
        }
        return MDR_RESULT_INPROGRESS;
    }

    static void Disconnect(void* user) noexcept
    {
        auto* ptr = static_cast<MDRConnectionLinux*>(user);
        if (ptr->fd)
            close(ptr->fd), ptr->fd = 0;
    }

    static int Recv(void* user, char* dst, int size, int* pReceived) noexcept
    {
        auto* ptr = static_cast<MDRConnectionLinux*>(user);
        if (!ptr->fd)
            return MDR_RESULT_ERROR_NO_CONNECTION;
        pollfd pfd{.fd = ptr->fd, .events = POLLIN};
        int res = poll(&pfd, 1, 0);
        if (res == 0)
            return MDR_RESULT_INPROGRESS;
        if (res < 0)
        {
            ptr->lastError = strerror(errno);
            return MDR_RESULT_ERROR_NET;
        }
        ssize_t received = recv(ptr->fd, dst, size, 0);
        if (received <= 0)
            return MDR_RESULT_ERROR_NET;
        *pReceived = static_cast<int>(received);
        return MDR_RESULT_OK;
    }

    static int Send(void* user, const char* src, int size, int* pSent) noexcept
    {
        auto* ptr = static_cast<MDRConnectionLinux*>(user);
        if (!ptr->fd)
            return MDR_RESULT_ERROR_NO_CONNECTION;
        pollfd pfd{.fd = ptr->fd, .events = POLLOUT};
        int res = poll(&pfd, 1, 0);
        if (res == 0)
            return MDR_RESULT_INPROGRESS;
        if (res < 0)
        {
            ptr->lastError = strerror(errno);
            return MDR_RESULT_ERROR_NET;
        }
        ssize_t sent = send(ptr->fd, src, size, 0);
        if (sent <= 0)
            return MDR_RESULT_ERROR_NET;
        *pSent = static_cast<int>(sent);
        return MDR_RESULT_OK;
    }

    static int List(void* user, MDRDeviceInfo** ppList, int* pCount) noexcept
    {
        auto* ptr = static_cast<MDRConnectionLinux*>(user);
        auto paths = dbus_list_adapters(ptr->dbusConn);
        *ppList = new MDRDeviceInfo[paths.size()];
        *pCount = static_cast<int>(paths.size());
        for (int i = 0; i < paths.size(); ++i)
        {
            std::string name = dbus_get_property(ptr->dbusConn, paths[i].c_str(), "Name");
            std::string address = dbus_get_property(ptr->dbusConn, paths[i].c_str(), "Address");
            strncpy((*ppList)[i].szDeviceName, name.c_str(), sizeof((*ppList)[i].szDeviceName) - 1);
            strncpy((*ppList)[i].szDeviceMacAddress, address.c_str(), sizeof((*ppList)[i].szDeviceMacAddress) - 1);
        }
        return MDR_RESULT_OK;
    }

    static int FreeList(void*, MDRDeviceInfo** ppList) noexcept
    {
        delete[] *ppList;
        *ppList = nullptr;
        return MDR_RESULT_OK;
    }

    static const char* GetLastError(void* user) noexcept
    {
        auto* ptr = static_cast<MDRConnectionLinux*>(user);
        return ptr->lastError;
    }
};


extern "C" {

MDRConnectionLinux* mdrConnectionLinuxCreate()
{
    return new MDRConnectionLinux();
}

void mdrConnectionLinuxDestroy(MDRConnectionLinux* instance)
{
    delete instance;
}

MDRConnection* mdrConnectionLinuxGet(MDRConnectionLinux* instance)
{
    return &instance->mdrConn;
}
}
