#include <Windows.h>
#include <new>
#include <mdr/Protocol.hpp>
#include <mdr-bt/ConnectionWindows.h>

extern "C" {
int clientPlatformLocateFontBinary(const char** outData)
{
    // TODO
    *outData = nullptr;
    return 0;
}

static MDRConnectionWindows* gConnClassic = nullptr;
#ifdef MDR_BLE
static MDRConnectionWindowsBLE* gConnBLE = nullptr;
#endif

int clientPlatformConnectionInit(int flags)
{
    if (gConnClassic != nullptr
#ifdef MDR_BLE
        || gConnBLE != nullptr
#endif
    )
        return MDR_RESULT_ERROR_GENERAL;

    if (flags & MDR_INIT_BT_BLE) {
#ifdef MDR_BLE
        gConnBLE = mdrConnectionWindowsBLECreate();
        gConnClassic = nullptr;
#else
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
#endif
    } else {
        gConnClassic = mdrConnectionWindowsCreate();
#ifdef MDR_BLE
        gConnBLE = nullptr;
#endif
    }
    return MDR_RESULT_OK;
}

void clientPlatformConnectionDestroy()
{
#ifdef MDR_BLE
    if (gConnBLE) { mdrConnectionWindowsBLEDestroy(gConnBLE); gConnBLE = nullptr; }
#endif
    if (gConnClassic) { mdrConnectionWindowsDestroy(gConnClassic); gConnClassic = nullptr; }
}

MDRConnection* clientPlatformConnectionGet()
{
    if (gConnClassic != nullptr)
        return mdrConnectionWindowsGet(gConnClassic);
#ifdef MDR_BLE
    if (gConnBLE != nullptr)
        return mdrConnectionWindowsBLEGet(gConnBLE);
#endif
    [[unlikely]] return nullptr;
}

void clientPlatformDestroy()
{
    clientPlatformConnectionDestroy();
    // TODO
}
}
