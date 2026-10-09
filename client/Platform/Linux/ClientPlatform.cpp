#include "../Platform.hpp"
#include <fontconfig/fontconfig.h>
#include <array>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <new>
#include <mdr/Protocol.hpp>
#include <mdr-bt/ConnectionLinux.h>

namespace
{
struct CachedFont
{
    bool queried{};
    std::unique_ptr<char[]> data;
    int size{};
    int faceIndex{};
};

std::array<CachedFont, static_cast<size_t>(AppLocale::NUM_LOCALES)> gFonts;

uint32_t ReadUInt32BE(const unsigned char* data)
{
    return (uint32_t(data[0]) << 24) | (uint32_t(data[1]) << 16) |
           (uint32_t(data[2]) << 8) | uint32_t(data[3]);
}

bool HasCompatibleOutlines(const char* bytes, size_t size, int faceIndex)
{
    if (size < 12 || faceIndex < 0 || faceIndex > 0xffff)
        return false;
    const auto* data = reinterpret_cast<const unsigned char*>(bytes);
    size_t offset = 0;
    if (ReadUInt32BE(data) == 0x74746366)
    {
        const size_t faceCount = ReadUInt32BE(data + 8);
        if (faceCount > (size - 12) / 4 || static_cast<size_t>(faceIndex) >= faceCount)
            return false;
        offset = ReadUInt32BE(data + 12 + static_cast<size_t>(faceIndex) * 4);
    }
    else if (faceIndex != 0)
        return false;
    if (offset > size || size - offset < 12)
        return false;
    const auto* header = data + offset;
    const uint32_t version = ReadUInt32BE(header);
    if (version != 0x00010000 && version != 0x4f54544f && version != 0x74727565)
        return false;
    const size_t tableCount = (size_t(header[4]) << 8) | size_t(header[5]);
    if (tableCount > (size - offset - 12) / 16)
        return false;
    bool hasOutlines = false;
    for (size_t table = 0; table < tableCount; ++table)
    {
        const auto* record = header + 12 + table * 16;
        const size_t tableOffset = ReadUInt32BE(record + 8);
        const size_t tableSize = ReadUInt32BE(record + 12);
        if (tableOffset > size || tableSize > size - tableOffset)
            return false;
        const uint32_t tag = ReadUInt32BE(record);
        hasOutlines |= tableSize != 0 && (tag == 0x676c7966 || tag == 0x43464620);
    }
    return hasOutlines;
}

bool LoadFont(FcPattern* font, const FcCharSet* sample, CachedFont& cache)
{
    FcChar8* path{};
    FcCharSet* charset{};
    FcBool scalable{};
    FcBool variable{};
    int faceIndex{};
    if (FcPatternGetString(font, FC_FILE, 0, &path) != FcResultMatch ||
        FcPatternGetInteger(font, FC_INDEX, 0, &faceIndex) != FcResultMatch ||
        FcPatternGetBool(font, FC_SCALABLE, 0, &scalable) != FcResultMatch || !scalable ||
        (FcPatternGetBool(font, FC_VARIABLE, 0, &variable) == FcResultMatch && variable) ||
        FcPatternGetCharSet(font, FC_CHARSET, 0, &charset) != FcResultMatch ||
        !FcCharSetIsSubset(sample, charset) || faceIndex < 0 || faceIndex > 0xffff)
        return false;
    std::unique_ptr<FILE, int (*)(FILE*)> file(
        std::fopen(reinterpret_cast<const char*>(path), "rb"), &std::fclose);
    if (!file || std::fseek(file.get(), 0, SEEK_END) != 0)
        return false;
    const long size = std::ftell(file.get());
    if (size <= 0 || size > INT_MAX || std::fseek(file.get(), 0, SEEK_SET) != 0)
        return false;
    std::unique_ptr<char[]> data(new (std::nothrow) char[static_cast<size_t>(size)]);
    if (!data || std::fread(data.get(), 1, static_cast<size_t>(size), file.get()) != static_cast<size_t>(size))
        return false;
    if (!HasCompatibleOutlines(data.get(), static_cast<size_t>(size), faceIndex))
    {
        MDR_LOG("Skipping font without STB-compatible outlines: {}", reinterpret_cast<const char*>(path));
        return false;
    }
    cache.data = std::move(data);
    cache.size = static_cast<int>(size);
    cache.faceIndex = faceIndex;
    MDR_LOG("Located Linux font: {}, {} bytes, face {}", reinterpret_cast<const char*>(path), cache.size, cache.faceIndex);
    return true;
}

void LocateFont(AppLocale locale, CachedFont& cache)
{
    const char* const sc[] = {"Noto Sans CJK SC", "Noto Sans SC", "Source Han Sans SC", nullptr};
    const char* const tc[] = {"Noto Sans CJK TC", "Noto Sans TC", "Source Han Sans TC", nullptr};
    const char* const jp[] = {"Noto Sans CJK JP", "Noto Sans JP", "Source Han Sans", nullptr};
    const char* const kr[] = {"Noto Sans CJK KR", "Noto Sans KR", "Source Han Sans K", nullptr};
    const FcChar32 scSample[] = {0x4e2d, 0x6c49, 0};
    const FcChar32 tcSample[] = {0x4e2d, 0x6f22, 0};
    const FcChar32 jpSample[] = {0x65e5, 0x3042, 0x30a2, 0};
    const FcChar32 krSample[] = {0xd55c, 0xae00, 0};
    const char* const* candidates{};
    const char* language{};
    const FcChar32* sample{};
    switch (locale)
    {
    case AppLocale::SIMPLIFIED_CHINESE: candidates = sc; language = "zh-cn"; sample = scSample; break;
    case AppLocale::TRADITIONAL_CHINESE: candidates = tc; language = "zh-tw"; sample = tcSample; break;
    case AppLocale::JAPANESE: candidates = jp; language = "ja"; sample = jpSample; break;
    case AppLocale::KOREAN: candidates = kr; language = "ko"; sample = krSample; break;
    default: return;
    }
    FcConfig* config = FcConfigGetCurrent();
    if (!config)
        return;
    std::unique_ptr<FcPattern, decltype(&FcPatternDestroy)> pattern(FcPatternCreate(), &FcPatternDestroy);
    std::unique_ptr<FcCharSet, decltype(&FcCharSetDestroy)> charset(FcCharSetCreate(), &FcCharSetDestroy);
    if (!pattern || !charset)
        return;
    for (; *candidates; ++candidates)
        if (!FcPatternAddString(pattern.get(), FC_FAMILY, reinterpret_cast<const FcChar8*>(*candidates)))
            return;
    for (; *sample; ++sample)
        if (!FcCharSetAddChar(charset.get(), *sample))
            return;
    if (!FcPatternAddString(pattern.get(), FC_FAMILY, reinterpret_cast<const FcChar8*>("sans-serif")) ||
        !FcPatternAddString(pattern.get(), FC_LANG, reinterpret_cast<const FcChar8*>(language)) ||
        !FcPatternAddInteger(pattern.get(), FC_WEIGHT, FC_WEIGHT_REGULAR) ||
        !FcPatternAddInteger(pattern.get(), FC_SLANT, FC_SLANT_ROMAN) ||
        !FcPatternAddInteger(pattern.get(), FC_WIDTH, FC_WIDTH_NORMAL) ||
        !FcPatternAddBool(pattern.get(), FC_SCALABLE, FcTrue) ||
        !FcPatternAddBool(pattern.get(), FC_VARIABLE, FcFalse) ||
        !FcPatternAddCharSet(pattern.get(), FC_CHARSET, charset.get()) ||
        !FcConfigSubstitute(config, pattern.get(), FcMatchPattern))
        return;
    FcDefaultSubstitute(pattern.get());
    FcResult result{};
    std::unique_ptr<FcFontSet, decltype(&FcFontSetDestroy)> fonts(
        FcFontSort(config, pattern.get(), FcFalse, nullptr, &result), &FcFontSetDestroy);
    if (!fonts)
        return;
    for (int font = 0; font < fonts->nfont; ++font)
        if (LoadFont(fonts->fonts[font], charset.get(), cache))
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
    *outData = cache.data.get();
    *outFaceIndex = cache.faceIndex;
    return cache.size;
}

static MDRConnectionLinux* gConn = nullptr;

int clientPlatformConnectionInit(int flags)
{
    if (flags & MDR_INIT_BT_BLE)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    gConn = mdrConnectionLinuxCreate();
    return MDR_RESULT_OK;
}

void clientPlatformConnectionDestroy()
{
    if (gConn) { mdrConnectionLinuxDestroy(gConn); gConn = nullptr; }
}

MDRConnection* clientPlatformConnectionGet()
{
    if (gConn) return mdrConnectionLinuxGet(gConn);
    [[unlikely]] return nullptr;
}

void clientPlatformDestroy()
{
    clientPlatformConnectionDestroy();
    for (auto& font : gFonts)
        font = CachedFont{};
}
}
