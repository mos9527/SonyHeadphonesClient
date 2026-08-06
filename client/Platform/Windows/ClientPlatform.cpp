#include <mdr/Protocol.hpp>
#include "../Platform.hpp"
#include "ConnectionWindows.hpp"
#include "ConnectionWindowsBLE.hpp"


// Override global operator new for reference Windows Client only
// As to why this is needed, MS thought it's a good idea to make the default non-throwing operator new
// a try-catch, around the throwing operator new.
// Find it here and see for yourself
//   C:\Program Files\Microsoft Visual
//   Studio\18\Community\VC\Tools\MSVC\14.51.36231\crt\src\vcruntime\new_scalar_nothrow.cpp
void* operator new(std::size_t n) { return mdr::MDRAllocator<char>().allocate(n); }
void* operator new(size_t const n, std::nothrow_t const&) noexcept { return mdr::MDRAllocator<char>().allocate(n); }
void operator delete(void* p) { mdr::MDRAllocator<char>().deallocate(static_cast<char*>(p)); }

static MDRConnectionWindows* gConnClassic = nullptr;
static MDRConnectionWindowsBLE* gConnBLE = nullptr;
extern "C" {
int clientPlatformConnectionInit(int flags)
{
    if (gConnBLE != nullptr || gConnClassic != nullptr)
        return MDR_RESULT_ERROR_GENERAL;
    if (flags & MDR_INIT_BT_BLE)
    {
#ifdef MDR_BLE
        gConnBLE = clientPlatformWindowsBLEConnectionCreate(), gConnClassic = nullptr;
#else
        gConnBLE = nullptr, gConnClassic = nullptr;
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
#endif
    }
    else
        gConnClassic = clientPlatformWindowsConnectionCreate(), gConnBLE = nullptr;
    return MDR_RESULT_OK;
}

void clientPlatformConnectionDestroy()
{
    if (gConnClassic)
        clientPlatformWindowsConnectionDestroy(gConnClassic), gConnClassic = nullptr;
#ifdef MDR_BLE
    if (gConnBLE)
        clientPlatformWindowsBLEConnectionDestroy(gConnBLE), gConnBLE = nullptr;
#endif
}

MDRConnection* clientPlatformConnectionGet()
{
    if (gConnClassic)
        return clientPlatformWindowsConnectionGet(gConnClassic);
#ifdef MDR_BLE
    if (gConnBLE)
        return clientPlatformWindowsBLEConnectionGet(gConnBLE);
#endif
    [[unlikely]] return nullptr;
}

int clientPlatformLocateFontBinary(const char** outData)
{
    // TODO
    *outData = nullptr;
    return 0;
}
void clientPlatformDestroy()
{
    clientPlatformConnectionDestroy();
    // TODO
}
}
