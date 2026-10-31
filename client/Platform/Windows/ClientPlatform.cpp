#include "../Platform.hpp"
#define NOMINMAX
#include <Windows.h>
#include <array>
#include <climits>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <mdr/Protocol.hpp>
#include <mdr-bt/ConnectionWindows.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>

int clientPlatformSaveBlob(
    const char* filename, const unsigned char* data, size_t dataSize, const char* mimeType,
    SDL_Window* window, ClientPlatformSaveBlobCallback callback, void* userdata)
{
    return clientSDLSaveBlob(filename, data, dataSize, mimeType, window, callback, userdata);
}

float clientPlatformGetDisplayScale()
{
    const float scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    return scale > 0.0f ? scale : 1.0f;
}

bool clientPlatformInitGraphics(SDL_Renderer*)
{
    return true;
}

bool clientPlatformInit()
{
    return true;
}

bool clientPlatformWaitEventTimeout(int timeoutMS)
{
    return SDL_WaitEventTimeout(nullptr, timeoutMS);
}

namespace
{
struct CachedFont
{
    bool queried{};
    mdr::Vector<unsigned char> data;
    int faceIndex{};
};

int MapFile(const wchar_t* path, void** outAddr, size_t* outSize)
{
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, nullptr);
    if (file == INVALID_HANDLE_VALUE)
        return MDR_RESULT_ERROR_NOT_FOUND;
    LARGE_INTEGER size{};
    HANDLE mapping{};
    if (GetFileSizeEx(file, &size) && size.QuadPart > 0 &&
        static_cast<unsigned long long>(size.QuadPart) <= SIZE_MAX)
        mapping = CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr);
    CloseHandle(file);
    if (!mapping)
        return MDR_RESULT_ERROR_GENERAL;
    void* view = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
    CloseHandle(mapping);
    if (!view)
        return MDR_RESULT_ERROR_GENERAL;
    *outAddr = view;
    *outSize = static_cast<size_t>(size.QuadPart);
    return MDR_RESULT_OK;
}

std::array<CachedFont, static_cast<size_t>(AppLocale::NUM_LOCALES)> gFonts;

uint32_t ReadFontUInt32(const unsigned char* data)
{
    return (uint32_t(data[0]) << 24) | (uint32_t(data[1]) << 16) |
           (uint32_t(data[2]) << 8) | uint32_t(data[3]);
}

bool ReadFont(HDC dc, CachedFont& cache)
{
    constexpr DWORD collectionTag = 0x66637474;
    DWORD table = collectionTag;
    DWORD size = GetFontData(dc, table, 0, nullptr, 0);
    if (size == GDI_ERROR)
    {
        table = 0;
        size = GetFontData(dc, table, 0, nullptr, 0);
    }
    if (size == GDI_ERROR || size < 12 || size > static_cast<DWORD>(INT_MAX))
        return false;
    mdr::Vector<unsigned char> data(size);
    if (GetFontData(dc, table, 0, data.data(), size) != size)
        return false;
    int faceIndex{};
    if (table == collectionTag)
    {
        if (std::memcmp(data.data(), "ttcf", 4) != 0)
            return false;
        const uint32_t faces = ReadFontUInt32(data.data() + 8);
        if (faces == 0 || faces > (size - 12) / 4)
            return false;
        std::array<unsigned char, 12> header{};
        if (GetFontData(dc, 0, 0, header.data(), static_cast<DWORD>(header.size())) != header.size())
            return false;
        const size_t tables = (size_t(header[4]) << 8) | header[5];
        const size_t directorySize = header.size() + tables * 16;
        if (directorySize > size)
            return false;
        mdr::Vector<unsigned char> directory(directorySize);
        if (GetFontData(dc, 0, 0, directory.data(), static_cast<DWORD>(directorySize)) != directorySize)
            return false;
        bool found{};
        for (uint32_t face = 0; face < faces; ++face)
        {
            const uint32_t offset = ReadFontUInt32(data.data() + 12 + size_t(face) * 4);
            if (offset > size - directorySize ||
                std::memcmp(data.data() + offset, directory.data(), directorySize) != 0)
                continue;
            faceIndex = static_cast<int>(face);
            found = true;
            break;
        }
        if (!found)
            return false;
    }
    cache.data = std::move(data);
    cache.faceIndex = faceIndex;
    return true;
}

bool LoadFont(HDC dc, const wchar_t* name, const wchar_t* sample, CachedFont& cache)
{
    LOGFONTW description{};
    description.lfHeight = -16;
    description.lfWeight = FW_NORMAL;
    description.lfCharSet = DEFAULT_CHARSET;
    description.lfOutPrecision = OUT_TT_ONLY_PRECIS;
    if (std::wcslen(name) >= LF_FACESIZE)
        return false;
    std::wcscpy(description.lfFaceName, name);
    HFONT font = CreateFontIndirectW(&description);
    if (!font)
        return false;
    HGDIOBJ previous = SelectObject(dc, font);
    if (!previous || previous == HGDI_ERROR)
    {
        DeleteObject(font);
        return false;
    }
    bool loaded = [&] {
        wchar_t actualName[LF_FACESIZE]{};
        if (!GetTextFaceW(dc, LF_FACESIZE, actualName) || _wcsicmp(actualName, name) != 0)
            return false;
        for (const wchar_t* ch = sample; *ch; ++ch)
        {
            WORD glyph{};
            if (GetGlyphIndicesW(dc, ch, 1, &glyph, GGI_MARK_NONEXISTING_GLYPHS) != 1 ||
                glyph == 0xffff || glyph == 0)
                return false;
        }
        return ReadFont(dc, cache);
    }();
    SelectObject(dc, previous);
    DeleteObject(font);
    return loaded;
}

void LocateFont(AppLocale locale, CachedFont& cache)
{
    if (locale == AppLocale::DEFAULT)
        return;
    const wchar_t* const sc[] = {L"Microsoft YaHei", L"Microsoft YaHei UI", L"SimSun", nullptr};
    const wchar_t* const tc[] = {L"Microsoft JhengHei", L"Microsoft JhengHei UI", L"MingLiU", L"PMingLiU", nullptr};
    const wchar_t* const jp[] = {L"Yu Gothic", L"Yu Gothic UI", L"Meiryo", L"MS Gothic", nullptr};
    const wchar_t* const kr[] = {L"Malgun Gothic", L"Gulim", L"Batang", nullptr};
    const wchar_t* const* candidates{};
    const wchar_t* sample{};
    switch (locale)
    {
    case AppLocale::SIMPLIFIED_CHINESE: candidates = sc; sample = L"\u4e2d\u6c49"; break;
    case AppLocale::TRADITIONAL_CHINESE: candidates = tc; sample = L"\u4e2d\u6f22"; break;
    case AppLocale::JAPANESE: candidates = jp; sample = L"\u65e5\u3042\u30a2"; break;
    case AppLocale::KOREAN: candidates = kr; sample = L"\ud55c\uae00"; break;
    default: return;
    }
    HDC dc = CreateCompatibleDC(nullptr);
    if (!dc)
        return;
    for (; *candidates; ++candidates)
        if (LoadFont(dc, *candidates, sample, cache))
            break;
    DeleteDC(dc);
}
}

extern "C" {
int clientPlatformLocateFontBinary(AppLocale locale, const char** outData, int* outFaceIndex)
{
    if (outData) *outData = nullptr;
    if (outFaceIndex) *outFaceIndex = 0;
    const auto index = static_cast<unsigned int>(locale);
    if (!outData || !outFaceIndex || index >= gFonts.size())
        return 0;
    auto& cache = gFonts[index];
    if (!cache.queried)
    {
        cache.queried = true;
        LocateFont(locale, cache);
    }
    *outData = cache.data.empty() ? nullptr : reinterpret_cast<const char*>(cache.data.data());
    *outFaceIndex = cache.faceIndex;
    return static_cast<int>(cache.data.size());
}

int clientPlatformMemoryMapFile(const char* path, void** outAddr, size_t* outSize)
{
    if (outAddr) *outAddr = nullptr;
    if (outSize) *outSize = 0;
    if (!path || !outAddr || !outSize)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, nullptr, 0);
    if (length <= 0)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    mdr::Vector<wchar_t> widePath(static_cast<size_t>(length), L'\0');
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, widePath.data(), length) != length)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    return MapFile(widePath.data(), outAddr, outSize);
}

void clientPlatformMemoryUnmapFile(void* addr, size_t)
{
    if (addr)
        UnmapViewOfFile(addr);
}

static MDRConnectionWindows* gConnClassic = nullptr;
#ifdef MDR_BT_BLE_SUPPORT
static MDRConnectionWindowsBLE* gConnBLE = nullptr;
#endif

int clientPlatformConnectionInit(int flags)
{
    if (gConnClassic != nullptr
#ifdef MDR_BT_BLE_SUPPORT
        || gConnBLE != nullptr
#endif
    )
        return MDR_RESULT_ERROR_GENERAL;

    if (flags & MDR_INIT_BT_BLE) {
#ifdef MDR_BT_BLE_SUPPORT
        gConnBLE = mdrConnectionWindowsBLECreate();
        gConnClassic = nullptr;
#else
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
#endif
    } else {
        gConnClassic = mdrConnectionWindowsCreate();
#ifdef MDR_BT_BLE_SUPPORT
        gConnBLE = nullptr;
#endif
    }
    return MDR_RESULT_OK;
}

void clientPlatformConnectionDestroy()
{
#ifdef MDR_BT_BLE_SUPPORT
    if (gConnBLE) { mdrConnectionWindowsBLEDestroy(gConnBLE); gConnBLE = nullptr; }
#endif
    if (gConnClassic) { mdrConnectionWindowsDestroy(gConnClassic); gConnClassic = nullptr; }
}

MDRConnection* clientPlatformConnectionGet()
{
    if (gConnClassic != nullptr)
        return mdrConnectionWindowsGet(gConnClassic);
#ifdef MDR_BT_BLE_SUPPORT
    if (gConnBLE != nullptr)
        return mdrConnectionWindowsBLEGet(gConnBLE);
#endif
    [[unlikely]] return nullptr;
}

void clientPlatformDestroy()
{
    clientPlatformConnectionDestroy();
    for (auto& font : gFonts)
        font = {};
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
