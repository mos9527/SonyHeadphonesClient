#include "../Platform.hpp"

#include <mdr-bt/ConnectionEmscripten.h>
#include <emscripten.h>

extern "C" {
static MDRConnectionEmscripten* gConn = nullptr;

int clientPlatformConnectionInit(int flags)
{
    if (flags & MDR_INIT_BT_BLE)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    gConn = mdrConnectionEmscriptenCreate();
    return MDR_RESULT_OK;
}

void clientPlatformConnectionDestroy()
{
    if (gConn) { mdrConnectionEmscriptenDestroy(gConn); gConn = nullptr; }
}

MDRConnection* clientPlatformConnectionGet()
{
    if (gConn) return mdrConnectionEmscriptenGet(gConn);
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

EM_JS(int, clientPlatformDownloadFileImpl,
      (const char* filename, const unsigned char* data, int dataSize, const char* mimeType), {
    if (!filename || !data || dataSize <= 0 || !mimeType)
        return 0;
    try {
        const bytes = HEAPU8.slice(data, data + dataSize);
        const blob = new Blob([bytes], {type: UTF8ToString(mimeType)});
        const url = URL.createObjectURL(blob);
        const anchor = document.createElement('a');
        anchor.href = url;
        anchor.download = UTF8ToString(filename);
        anchor.style.display = 'none';
        document.body.appendChild(anchor);
        anchor.click();
        anchor.remove();
        setTimeout(() => URL.revokeObjectURL(url), 0);
        return 1;
    } catch (error) {
        console.error('Unable to export file', error);
        return 0;
    }
});

int clientPlatformDownloadFile(
    const char* filename,
    const unsigned char* data,
    size_t dataSize,
    const char* mimeType)
{
    return clientPlatformDownloadFileImpl(filename, data, static_cast<int>(dataSize), mimeType);
}

// LTO and MinSizeRel causes this function referenced below to get deleted with GCC
// See also https://stackoverflow.com/questions/38389702/prevent-gcc-lto-from-deleting-function
// TODO: Figure out the actual why. Shouldn't have happened by any means...
void __dont_touch_my_garbage_exclamation_marks__() __attribute__((used));
void __dont_touch_my_garbage_exclamation_marks__()
{
    clientPlatformLocateFontBinary(nullptr);
    clientPlatformDownloadFileImpl(nullptr, nullptr, 0, nullptr);
}
}
