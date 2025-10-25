#include <mdr/ConnectionLinux.h>
#include <mdr/Headphones.h>
#include <fmt/base.h>

const char* kTestDeviceName = "WF-1000XM5";

int main()
{
    MDRConnectionLinux* platform = mdrConnectionLinuxCreate();
    MDRConnection* conn = mdrConnectionLinuxGet(platform);
    MDRDeviceInfo* devices;
    int numDevices;
    conn->list(conn->user, &devices, &numDevices);
    int targetDevice = -1;
    for (int i = 0; i < numDevices; i++)
    {
        fmt::println("Device {}, {}, {}", i, devices[i].szDeviceName, devices[i].szDeviceMacAddress);
        if (strcmp(devices[i].szDeviceName, kTestDeviceName) == 0)
            targetDevice = i;
    }
    if (targetDevice == -1)
        return -1;
    {
        int res = conn->connect(conn->user, devices[targetDevice].szDeviceMacAddress, MDR_SERVICE_UUID_XM5);
        conn->freeList(conn->user, &devices);
        fmt::println("Connect: {}", mdrResultString(res));
        if (res != MDR_RESULT_OK)
        {
            fmt::println("connect failed ({}, {})", mdrResultString(res), conn->getLastError(conn->user));
            return -1;
        }
    }
    MDRHeadphones* headphones = mdrHeadphonesCreate(conn);
    mdrHeadphonesRequestInit(headphones);
    while (true)
    {
        int res = mdrHeadphonesPollEvents(headphones);
        if (res != MDR_RESULT_OK)
        {
            fmt::println("headphones died: {} (headphones={}, conn={})", mdrResultString(res),
                         mdrHeadphonesGetLastError(headphones), conn->getLastError(conn->user));
            return -1;
        }
    }
}
