#include "../Platform.hpp"
#define NOMINMAX
#include <Windows.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <array>
#include <climits>
#include <cstdint>
#include <string>
#include <mdr/Protocol.hpp>
#include <mdr-bt/ConnectionWindows.h>

namespace
{
using Microsoft::WRL::ComPtr;

struct CachedFont
{
    bool queried{};
    void* data{};
    int size{};
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

bool LoadFont(IDWriteFontCollection* collection, const wchar_t* name,
              const wchar_t* sample, CachedFont& cache)
{
    UINT32 familyIndex{};
    BOOL exists{};
    if (FAILED(collection->FindFamilyName(name, &familyIndex, &exists)) || !exists)
        return false;
    ComPtr<IDWriteFontFamily> family;
    ComPtr<IDWriteFont> font;
    ComPtr<IDWriteFontFace> face;
    if (FAILED(collection->GetFontFamily(familyIndex, &family)) ||
        FAILED(family->GetFirstMatchingFont(DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                                           DWRITE_FONT_STYLE_NORMAL, &font)) ||
        font->GetSimulations() != DWRITE_FONT_SIMULATIONS_NONE)
        return false;
    for (const wchar_t* ch = sample; *ch; ++ch)
    {
        BOOL covered{};
        if (FAILED(font->HasCharacter(*ch, &covered)) || !covered)
            return false;
    }
    if (FAILED(font->CreateFontFace(&face)))
        return false;
    UINT32 fileCount{};
    if (FAILED(face->GetFiles(&fileCount, nullptr)) || fileCount != 1)
        return false;
    ComPtr<IDWriteFontFile> file;
    if (FAILED(face->GetFiles(&fileCount, file.GetAddressOf())))
        return false;
    BOOL supported{};
    DWRITE_FONT_FILE_TYPE fileType{};
    DWRITE_FONT_FACE_TYPE faceType{};
    UINT32 faceCount{};
    if (FAILED(file->Analyze(&supported, &fileType, &faceType, &faceCount)) || !supported ||
        (faceType != DWRITE_FONT_FACE_TYPE_TRUETYPE && faceType != DWRITE_FONT_FACE_TYPE_CFF &&
         faceType != DWRITE_FONT_FACE_TYPE_TRUETYPE_COLLECTION) ||
        face->GetIndex() >= faceCount || face->GetIndex() > static_cast<UINT32>(INT_MAX))
        return false;
    const void* key{};
    UINT32 keySize{};
    UINT32 pathLength{};
    ComPtr<IDWriteFontFileLoader> loader;
    ComPtr<IDWriteLocalFontFileLoader> localLoader;
    if (FAILED(file->GetReferenceKey(&key, &keySize)) || FAILED(file->GetLoader(&loader)) ||
        FAILED(loader.As(&localLoader)) ||
        FAILED(localLoader->GetFilePathLengthFromKey(key, keySize, &pathLength)))
        return false;
    std::wstring path(pathLength + 1, L'\0');
    if (FAILED(localLoader->GetFilePathFromKey(key, keySize, path.data(), pathLength + 1)))
        return false;
    void* addr{};
    size_t size{};
    if (MapFile(path.c_str(), &addr, &size) != MDR_RESULT_OK)
        return false;
    if (size > static_cast<size_t>(INT_MAX))
    {
        clientPlatformMemoryUnmapFile(addr, size);
        return false;
    }
    cache.data = addr;
    cache.size = static_cast<int>(size);
    cache.faceIndex = static_cast<int>(face->GetIndex());
    return true;
}

void LocateFont(AppLocale locale, CachedFont& cache)
{
    if (locale == AppLocale::DEFAULT)
        return;
    ComPtr<IDWriteFactory> factory;
    ComPtr<IDWriteFontCollection> collection;
    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                  reinterpret_cast<IUnknown**>(factory.GetAddressOf()))) ||
        FAILED(factory->GetSystemFontCollection(&collection)))
        return;
    const wchar_t* const sc[] = {L"Microsoft YaHei", L"Microsoft YaHei UI", L"SimSun", nullptr};
    const wchar_t* const tc[] = {L"Microsoft JhengHei", L"Microsoft JhengHei UI", nullptr};
    const wchar_t* const jp[] = {L"Yu Gothic", L"Yu Gothic UI", L"Meiryo", L"MS Gothic", nullptr};
    const wchar_t* const kr[] = {L"Malgun Gothic", nullptr};
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
    for (; *candidates; ++candidates)
        if (LoadFont(collection.Get(), *candidates, sample, cache))
            return;
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
    *outData = static_cast<const char*>(cache.data);
    *outFaceIndex = cache.faceIndex;
    return cache.size;
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
    std::wstring widePath(static_cast<size_t>(length), L'\0');
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, widePath.data(), length) != length)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    return MapFile(widePath.c_str(), outAddr, outSize);
}

void clientPlatformMemoryUnmapFile(void* addr, size_t)
{
    if (addr)
        UnmapViewOfFile(addr);
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
    for (auto& font : gFonts)
    {
        clientPlatformMemoryUnmapFile(font.data, static_cast<size_t>(font.size));
        font = {};
    }
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
