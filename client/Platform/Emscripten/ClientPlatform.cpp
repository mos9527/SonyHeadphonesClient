#include "../Platform.hpp"

#include <mdr-bt/ConnectionEmscripten.h>
#include <emscripten.h>

#ifdef MDR_CLIENT_DEBUGGER
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_video.h>
#include <string>

static std::string gDroppedDirectory;

extern "C" EMSCRIPTEN_KEEPALIVE bool clientPlatformDropPending()
{
    return SDL_HasEvent(SDL_EVENT_DROP_FILE);
}

extern "C" EMSCRIPTEN_KEEPALIVE const char* clientPlatformDropDirectory(const char* path)
{
    if (!path || !*path)
        return "Folder drop path is empty";
    if (clientPlatformDropPending())
        return "A file or folder drop is already pending";
    int count{};
    SDL_Window** windows = SDL_GetWindows(&count);
    if (!windows)
        return SDL_GetError();
    const SDL_WindowID windowID = count ? SDL_GetWindowID(windows[0]) : 0;
    SDL_free(windows);
    if (!windowID)
        return "No window available for folder drop";

    gDroppedDirectory = path;
    SDL_Event event{};
    event.type = SDL_EVENT_DROP_FILE;
    event.drop.windowID = windowID;
    event.drop.data = gDroppedDirectory.c_str();
    if (!SDL_PushEvent(&event))
    {
        gDroppedDirectory.clear();
        return "Unable to queue folder drop";
    }
    return nullptr;
}
#endif

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

EM_JS(void, clientPlatformDestroyFonts, (), {
    const fonts = globalThis.SonyHeadphonesClientFonts;
    if (!fonts) return;
    fonts.destroyed = true;
    for (const entry of fonts.entries.values()) {
        entry.controller.abort(new DOMException('Font loading cancelled because the application is shutting down', 'AbortError'));
        if (entry.ptr) _free(entry.ptr);
        entry.ptr = entry.size = 0;
        entry.data = null;
        entry.state = 'unavailable';
    }
});

void clientPlatformDestroy()
{
    clientPlatformConnectionDestroy();
    clientPlatformDestroyFonts();
}

EM_JS(int, clientPlatformLocateFontBinaryImpl, (int locale, const char** outData, int* outFaceIndex), {
    if (outData) setValue(outData, 0, '*');
    if (outFaceIndex) setValue(outFaceIndex, 0, 'i32');
    if (!outData || !outFaceIndex)
        return 0;
    const fonts = globalThis.SonyHeadphonesClientFonts;
    if (!fonts || fonts.destroyed) return 0;
    fonts.load(locale);
    const entry = fonts.entries.get(locale);
    if (entry?.state === 'loading') return -1;
    if (!entry || entry.state !== 'ready') return 0;
    if (entry.data) {
        const size = entry.data.byteLength;
        const ptr = _malloc(size);
        if (!ptr) {
            entry.data = null;
            entry.state = 'unavailable';
            return 0;
        }
        HEAPU8.set(entry.data, ptr);
        entry.ptr = ptr;
        entry.size = size;
        entry.data = null;
    }
    if (!entry.ptr || !entry.size) return 0;
    setValue(outData, entry.ptr, '*');
    return entry.size;
});

int clientPlatformLocateFontBinary(AppLocale locale, const char** outData, int* outFaceIndex)
{
    if (locale <= AppLocale::UNDEFINED || locale >= AppLocale::NUM_LOCALES)
    {
        if (outData) *outData = nullptr;
        if (outFaceIndex) *outFaceIndex = 0;
        return 0;
    }
    return clientPlatformLocateFontBinaryImpl(static_cast<int>(locale), outData, outFaceIndex);
}

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
    clientPlatformLocateFontBinary(AppLocale::UNDEFINED, nullptr, nullptr);
    clientPlatformDownloadFileImpl(nullptr, nullptr, 0, nullptr);
}
}

extern "C" {
int clientPlatformIsLocalBluetoothAddress(const char*, int*)
{
    return MDR_RESULT_ERROR_NOT_SUPPORTED;
}

struct ClientMediaPause* clientPlatformMediaPause()
{
    return nullptr;
}

void clientPlatformMediaResume(struct ClientMediaPause*)
{
}
}
