#include "../Platform.hpp"
#define NOMINMAX
#include <Windows.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <array>
#include <cstring>
#include <climits>
#include <memory>
#include <new>
#include <mdr/Protocol.hpp>
#include <mdr-bt/ConnectionWindows.h>

namespace
{
using Microsoft::WRL::ComPtr;

struct CachedFont
{
    bool queried{};
    std::unique_ptr<char[]> data;
    int size{};
    int faceIndex{};
};

std::array<CachedFont, static_cast<size_t>(FontLocale::NUM_LOCALES)> gFonts;

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
    ComPtr<IDWriteFontFileLoader> loader;
    ComPtr<IDWriteLocalFontFileLoader> localLoader;
    ComPtr<IDWriteFontFileStream> stream;
    if (FAILED(file->GetReferenceKey(&key, &keySize)) || FAILED(file->GetLoader(&loader)) ||
        FAILED(loader.As(&localLoader)) ||
        FAILED(localLoader->CreateStreamFromKey(key, keySize, &stream)))
        return false;
    UINT64 size{};
    if (FAILED(stream->GetFileSize(&size)) || !size || size > static_cast<UINT64>(INT_MAX))
        return false;
    std::unique_ptr<char[]> data(new (std::nothrow) char[static_cast<size_t>(size)]);
    if (!data)
        return false;
    const void* fragment{};
    void* context{};
    if (FAILED(stream->ReadFileFragment(&fragment, 0, size, &context)))
        return false;
    if (fragment)
        std::memcpy(data.get(), fragment, static_cast<size_t>(size));
    stream->ReleaseFileFragment(context);
    if (!fragment)
        return false;
    cache.data = std::move(data);
    cache.size = static_cast<int>(size);
    cache.faceIndex = static_cast<int>(face->GetIndex());
    return true;
}

void LocateFont(FontLocale locale, CachedFont& cache)
{
    if (locale == FontLocale::UNDEFINED)
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
    case FontLocale::SIMPLIFIED_CHINESE: candidates = sc; sample = L"\u4e2d\u6c49"; break;
    case FontLocale::TRADITIONAL_CHINESE: candidates = tc; sample = L"\u4e2d\u6f22"; break;
    case FontLocale::JAPANESE: candidates = jp; sample = L"\u65e5\u3042\u30a2"; break;
    case FontLocale::KOREAN: candidates = kr; sample = L"\ud55c\uae00"; break;
    default: return;
    }
    for (; *candidates; ++candidates)
        if (LoadFont(collection.Get(), *candidates, sample, cache))
            return;
}
}

extern "C" {
int clientPlatformLocateFontBinary(FontLocale locale, const char** outData, int* outFaceIndex)
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
    *outData = cache.data.get();
    *outFaceIndex = cache.faceIndex;
    return cache.size;
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
