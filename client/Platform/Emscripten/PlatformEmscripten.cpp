#include "../Platform.hpp"
#include <mdr-c/Platform/PlatformEmscripten.h>
#include <emscripten.h>

MDRConnectionEmscripten* gConn;
extern "C" {
    void clientPlatformInit()
    {
        gConn = mdrConnectionEmscriptenCreate();
    }
    void clientPlatformDestroy()
    {
        mdrConnectionEmscriptenDestroy(gConn);
    }
    MDRConnection* clientPlatformConnectionGet()
    {
        return mdrConnectionEmscriptenGet(gConn);
    }
    // vvv TODO: These should be pretty easy.
    void clientPlatformSetUseBLE(bool) {}
    bool clientPlatformGetUseBLE() { return false; }
    int clientPlatformBLEEnumerateGatt(const char*) { return -1; }
    // ^^^ TODO
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