#include <mdr/ConnectionLinux.h>
#include <mdr/Headphones.h>
#include <stdio.h>
#include <string.h>

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
        printf("Device %d, %s, %s\n", i, devices[i].szDeviceName, devices[i].szDeviceMacAddress);
        if (strcmp(devices[i].szDeviceName, kTestDeviceName) == 0)
            targetDevice = i;
    }

    if (targetDevice == -1)
    {
        printf("Target device %s not found.\n", kTestDeviceName);
        conn->freeList(conn->user, &devices);
        return -1;
    }

    {
        int res = conn->connect(conn->user, devices[targetDevice].szDeviceMacAddress, MDR_SERVICE_UUID_XM5);
        conn->freeList(conn->user, &devices);
        printf("Connect: %s\n", mdrResultString(res));

        if (res != MDR_RESULT_OK)
        {
            printf("connect failed (%s, %s)\n", mdrResultString(res), conn->getLastError(conn->user));
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
            printf("headphones died: %s (headphones=%s, conn=%s)\n",
                   mdrResultString(res),
                   mdrHeadphonesGetLastError(headphones),
                   conn->getLastError(conn->user));
            return -1;
        }
    }
    mdrHeadphonesDestroy(headphones);
    mdrConnectionLinuxDestroy(platform);
    return 0;
}