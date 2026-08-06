#include <Windows.h>
#include <new>
#include <mdr/Protocol.hpp>
#include <mdr-bt/ConnectionWindows.h>

// Override global operator new for reference Windows Client only
// As to why this is needed, MS thought it's a good idea to make the default non-throwing operator new
// a try-catch, around the throwing operator new.
// Find it here and see for yourself
//   C:\Program Files\Microsoft Visual
//   Studio\18\Community\VC\Tools\MSVC\14.51.36231\crt\src\vcruntime\new_scalar_nothrow.cpp
void* operator new(std::size_t n) { return mdr::MDRAllocator<char>().allocate(n); }
void* operator new(size_t const n, std::nothrow_t const&) noexcept { return mdr::MDRAllocator<char>().allocate(n); }
void operator delete(void* p) { mdr::MDRAllocator<char>().deallocate(static_cast<char*>(p)); }

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
