#ifdef MDR_PLATFORM_LINUX
#include <mdr/LinuxPlatform.h>
#elif MDR_PLATFORM_WIN32
#include <mdr/WindowsPlatform.h>
#endif

#include <mdr/Headphones.h>
#include <stdio.h>
#include <string.h>

const char* kTestDeviceName = "WF-1000XM5";

int main()
{
#ifdef MDR_PLATFORM_LINUX
    MDRConnectionLinux* platform = mdrConnectionLinuxCreate();
    MDRConnection* conn = mdrConnectionLinuxGet(platform);
#elif MDR_PLATFORM_WIN32
    MDRConnectionWindows* platform = mdrConnectionWindowsCreate();
    MDRConnection* conn = mdrConnectionWindowsGet(platform);
#endif
    MDRDeviceInfo* devices;
    int numDevices;
    mdrConnectionGetDevicesList(conn, &devices, &numDevices);

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
        mdrConnectionFreeDevicesList(conn, &devices);
        return -1;
    }

    {
        int res = mdrConnectionConnect(conn, devices[targetDevice].szDeviceMacAddress, MDR_SERVICE_UUID_XM5);
        mdrConnectionFreeDevicesList(conn, &devices);
        printf("Connect: %s\n", mdrResultString(res));

        if (res != MDR_RESULT_OK)
        {
            printf("connect failed (%s, %s)\n", mdrResultString(res), mdrConnectionGetLastError(conn));
            return -1;
        }
    }

    MDRHeadphones* headphones = mdrHeadphonesCreate(conn);
    mdrHeadphonesRequestInit(headphones);

    while (1)
    {
        int evt = mdrHeadphonesPollEvents(headphones);
        switch (evt)
        {
        case MDR_HEADPHONES_ERROR:
            printf("headphones died: %s (headphones=%s, conn=%s)\n", mdrHeadphonesEventString(evt),
                   mdrHeadphonesGetLastError(headphones),  mdrConnectionGetLastError(conn));
            return -1;
        case MDR_HEADPHONES_INITIALIZED:
            printf("Initialized. We're done.\n");
            goto END;
        default:
            break;
        }
    }
END:
    mdrHeadphonesDestroy(headphones);
#ifdef MDR_PLATFORM_LINUX
    mdrConnectionLinuxDestroy(platform);
#elif MDR_PLATFORM_WIN32
    mdrConnectionWindowsDestroy(platform);
#endif
    return 0;
}
