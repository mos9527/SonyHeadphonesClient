#include "../Platform.hpp"
#include <mdr/Protocol.hpp>
#include <mdr-c/Platform/PlatformEmscripten.h>
#include <mdr-c/Platform/PlatformEmscriptenBLE.h>
#include <emscripten.h>

static MDRConnectionEmscripten* gConnClassic = nullptr;
static MDRConnectionEmscriptenBLE* gConnBLE = nullptr;
extern "C" {
    int clientPlatformConnectionInit(int flags)
    {
        MDR_CHECK_MSG(gConnBLE == nullptr && gConnClassic == nullptr,
            "Platform already initialized. You MUST call clientPlatformDestroy() before initializing again.");
        if (flags & MDR_INIT_BT_BLE)
            gConnBLE = mdrConnectionEmscriptenBLECreate(), gConnClassic = nullptr;
        else
            gConnClassic = mdrConnectionEmscriptenCreate(), gConnBLE = nullptr;
        return MDR_RESULT_OK;
    }
    void clientPlatformConnectionDestroy()
    {
        if (gConnClassic)
            mdrConnectionEmscriptenDestroy(gConnClassic), gConnClassic = nullptr;
        if (gConnBLE)
            mdrConnectionEmscriptenBLEDestroy(gConnBLE), gConnBLE = nullptr;
    }
    MDRConnection* clientPlatformConnectionGet()
    {
        if (gConnClassic)
            return mdrConnectionEmscriptenGet(gConnClassic);
        if (gConnBLE)
            return mdrConnectionEmscriptenBLEGet(gConnBLE);
        [[unlikely]] return nullptr;
    }
    void clientPlatformDestroy()
    {
        clientPlatformConnectionDestroy();
        // TODO
    }
    EM_JS(int, clientPlatformLocateFontBinary, (const char** outData), {
        if (navigator.externalFontSize > 0){
            setValue(outData, navigator.externalFontPtr, '*');
            return navigator.externalFontSize;
        }
        async function fetch_font(wakeUp) {
            return fetch(navigator.externalFont)
                .then(function(response) {
                    if (!response.ok) {
                        console.log(`Failed to fetch font binary from ${navigator.externalFont}`);
                        return;
                    }
                    return response.arrayBuffer();
                })
                .then(function(arrayBuffer) {
                    var size = arrayBuffer.byteLength;
                    var dataPtr = _malloc(size);
                    HEAPU8.set(new Uint8Array(arrayBuffer), dataPtr);
                    navigator.externalFontPtr = dataPtr;
                    navigator.externalFontSize = size;
                });
            }
        if (!navigator.externalFontFetch)
            navigator.externalFontFetch = fetch_font();
        return 0;
    });
}

// LTO and MinSizeRel causes this function referenced below to get deleted with GCC
// See also https://stackoverflow.com/questions/38389702/prevent-gcc-lto-from-deleting-function
// TODO: Figure out the actual why. Shouldn't have happened by any means...
void __dont_touch_my_garbage_exclamation_marks__() __attribute__((used));
void __dont_touch_my_garbage_exclamation_marks__()
{
    clientPlatformLocateFontBinary(nullptr);
}
