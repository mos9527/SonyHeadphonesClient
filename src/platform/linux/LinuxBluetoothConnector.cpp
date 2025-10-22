#include "LinuxBluetoothConnector.h"

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <sys/poll.h>

#include "DBusHelper.h"

#include "errno.h"

LinuxBluetoothConnector::LinuxBluetoothConnector()
{
}

LinuxBluetoothConnector::~LinuxBluetoothConnector()
{
    //onclose event
    if (isConnected())
    {
        disconnect();
    }
}

int LinuxBluetoothConnector::recv(char* buf, size_t length)
{
    size_t read = ::read(this->_socket, buf, length);
    // printf("length: %ld, read: %ld\n", length, read);
    // for (int i = 0; i < read; i++)
    // {
    //   std::cout << std::setfill('0') << std::setw(2) << std::hex << (0xff & (unsigned int)buf[i]) << " ";
    // }
    // std::cout << '\n';
    return read;
}

int LinuxBluetoothConnector::send(char* buf, size_t length)
{
    size_t written = ::write(this->_socket, buf, length);
    // printf("length: %ld, written: %ld\n", length, written);
    // for (int i = 0; i < written; i++)
    // {
    //   std::cout << std::setfill('0') << std::setw(2) << std::hex << (0xff & (unsigned int)buf[i]) << " ";
    // }
    // std::cout << '\n';
    return written;
}

const int kConnectTimeout = 5000;

void LinuxBluetoothConnector::connect(const std::string& addrStr)
{
    const auto throw_on_error = []()
    {
        std::string msg = strerror(errno);
        throw RecoverableException("Error: could not connect to bluetooth socket: " + msg, true);
    };

    // allocate a socket
    this->_socket = ::socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (!this->_socket)
    {
        throw_on_error();
    }
    uint32_t linkmode = RFCOMM_LM_AUTH | RFCOMM_LM_ENCRYPT;
    int sl = sizeof(linkmode);

    if (setsockopt(this->_socket, SOL_RFCOMM, RFCOMM_LM, &linkmode, sl))
    {
        throw_on_error();
    }

    printf("connecting to %s\n", addrStr.c_str());

    struct sockaddr_rc addr = {0};
    // set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    uint8_t channel = sdp_getServiceChannel(addrStr.c_str(), SERVICE_UUID_IN_BYTES);
    printf("channel: %d\n", channel);
    addr.rc_channel = channel;
    str2ba(addrStr.c_str(), &addr.rc_bdaddr);
    // connect to server
    // RFCOMM socket connects are always non-blocking. See:
    // - https://github.com/torvalds/linux/blob/552c50713f273b494ac6c77052032a49bc9255e2/net/bluetooth/rfcomm/core.c#L811
    // - https://github.com/mos9527/SonyHeadphonesClient/issues/26
    int res = ::connect(this->_socket, (struct sockaddr*)&addr, sizeof(addr));
    if (res != 0 && res != EINPROGRESS)
        throw_on_error();
    // Wait for the connection to complete
    pollfd pfd{
        .fd = this->_socket,
        .events = POLLIN
    };
    int poll_res = ::poll(&pfd, 1, kConnectTimeout);
    if (poll_res == 0)
        throw RecoverableException("Error: bluetooth socket connection timed out", true);
    if (poll_res == -1)
        throw_on_error();
    this->_connected = true;
}

std::vector<BluetoothDevice> LinuxBluetoothConnector::getConnectedDevices()
{
    std::vector<BluetoothDevice> res;

    DBusConnection* connection = dbus_open_system_bus();
    if (NULL == connection)
    {
        return res;
    }

    std::vector<std::string> adapter_paths = dbus_list_adapters(connection);
    for (auto& adapter : adapter_paths)
    {
        printf("%s\n", adapter.c_str());
        std::string name = dbus_get_property(connection, adapter.c_str(), "Name");
        std::string address = dbus_get_property(connection, adapter.c_str(), "Address");
        res.push_back(BluetoothDevice(name, address));
    }

    return res;
}

void LinuxBluetoothConnector::disconnect() noexcept
{
    // close connection
    if (this->_socket != -1)
    {
        ::shutdown(this->_socket, SHUT_RDWR);
        ::close(this->_socket);
    }
    this->_connected = false;
    printf("closed\n");
}

bool LinuxBluetoothConnector::isConnected() noexcept
{
    return this->_connected;
}
