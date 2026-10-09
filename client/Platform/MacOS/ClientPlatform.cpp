#include "../Platform.hpp"
#include <CoreText/CoreText.h>
#include <array>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <new>
#include <type_traits>
#include <mdr/Protocol.hpp>
#include <mdr-bt/ConnectionMacOS.h>

namespace
{
struct CFReleaser
{
    void operator()(CFTypeRef value) const { CFRelease(value); }
};

template <typename T>
using CFPtr = std::unique_ptr<std::remove_pointer_t<T>, CFReleaser>;

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

bool MatchesNameTable(const unsigned char* data, size_t size, size_t offset, CFDataRef name)
{
    if (offset > size || size - offset < 12)
        return false;
    const auto* header = data + offset;
    const uint32_t version = ReadUInt32BE(header);
    if (version != 0x00010000 && version != 0x4f54544f && version != 0x74727565)
        return false;
    const size_t tableCount = (size_t(header[4]) << 8) | size_t(header[5]);
    if (tableCount > (size - offset - 12) / 16)
        return false;
    for (size_t table = 0; table < tableCount; ++table)
    {
        const auto* record = header + 12 + table * 16;
        if (ReadUInt32BE(record) != static_cast<uint32_t>(kCTFontTableName))
            continue;
        const size_t tableOffset = ReadUInt32BE(record + 8);
        const size_t tableSize = ReadUInt32BE(record + 12);
        return tableOffset <= size && tableSize <= size - tableOffset &&
               tableSize == static_cast<size_t>(CFDataGetLength(name)) &&
               std::memcmp(data + tableOffset, CFDataGetBytePtr(name), tableSize) == 0;
    }
    return false;
}

bool FindFaceIndex(CTFontRef font, const char* bytes, size_t size, int& outIndex)
{
    CFPtr<CFDataRef> name(CTFontCopyTable(font, kCTFontTableName, kCTFontTableOptionNoOptions));
    if (!name || CFDataGetLength(name.get()) <= 0 || size < 12)
        return false;
    const auto* data = reinterpret_cast<const unsigned char*>(bytes);
    if (ReadUInt32BE(data) != 0x74746366)
    {
        outIndex = 0;
        return MatchesNameTable(data, size, 0, name.get());
    }
    const size_t faceCount = ReadUInt32BE(data + 8);
    if (!faceCount || faceCount > (size - 12) / 4 || faceCount > static_cast<size_t>(INT_MAX))
        return false;
    for (size_t face = 0; face < faceCount; ++face)
    {
        if (MatchesNameTable(data, size, ReadUInt32BE(data + 12 + face * 4), name.get()))
        {
            outIndex = static_cast<int>(face);
            return true;
        }
    }
    return false;
}

bool LoadFont(CFStringRef requestedName, const UniChar* sample, CFIndex sampleSize, CachedFont& cache)
{
    CFPtr<CTFontRef> font(CTFontCreateWithNameAndOptions(
        requestedName, 15.0, nullptr, kCTFontOptionsPreventAutoActivation));
    if (!font)
        return false;
    CFPtr<CFStringRef> name(CTFontCopyPostScriptName(font.get()));
    if (!name || !CFEqual(name.get(), requestedName))
        return false;
    std::array<CGGlyph, 3> glyphs{};
    if (sampleSize <= 0 || sampleSize > static_cast<CFIndex>(glyphs.size()) ||
        !CTFontGetGlyphsForCharacters(font.get(), sample, glyphs.data(), sampleSize))
        return false;
    CFPtr<CFTypeRef> url(CTFontCopyAttribute(font.get(), kCTFontURLAttribute));
    if (!url || CFGetTypeID(url.get()) != CFURLGetTypeID())
        return false;
    std::array<UInt8, PATH_MAX> path{};
    if (!CFURLGetFileSystemRepresentation(static_cast<CFURLRef>(url.get()), true, path.data(), path.size()))
        return false;
    CFPtr<CFArrayRef> tables(CTFontCopyAvailableTables(font.get(), kCTFontTableOptionNoOptions));
    bool hasGlyf = false;
    bool hasCFF = false;
    for (CFIndex table = 0; tables && table < CFArrayGetCount(tables.get()); ++table)
    {
        const auto tag = static_cast<CTFontTableTag>(
            reinterpret_cast<uintptr_t>(CFArrayGetValueAtIndex(tables.get(), table)));
        hasGlyf |= tag == kCTFontTableGlyf;
        hasCFF |= tag == kCTFontTableCFF;
    }
    if (!hasGlyf && !hasCFF)
    {
        MDR_LOG("Skipping font without STB-compatible outlines: {}", reinterpret_cast<const char*>(path.data()));
        return false;
    }
    std::unique_ptr<FILE, decltype(&std::fclose)> file(
        std::fopen(reinterpret_cast<const char*>(path.data()), "rb"), &std::fclose);
    if (!file || std::fseek(file.get(), 0, SEEK_END) != 0)
        return false;
    const long size = std::ftell(file.get());
    if (size <= 0 || size > INT_MAX || std::fseek(file.get(), 0, SEEK_SET) != 0)
        return false;
    std::unique_ptr<char[]> data(new (std::nothrow) char[static_cast<size_t>(size)]);
    if (!data || std::fread(data.get(), 1, static_cast<size_t>(size), file.get()) != static_cast<size_t>(size))
        return false;
    int faceIndex{};
    if (!FindFaceIndex(font.get(), data.get(), static_cast<size_t>(size), faceIndex))
        return false;
    cache.data = std::move(data);
    cache.size = static_cast<int>(size);
    cache.faceIndex = faceIndex;
    std::array<char, 256> postScriptName{};
    CFStringGetCString(name.get(), postScriptName.data(), postScriptName.size(), kCFStringEncodingUTF8);
    MDR_LOG("Located macOS font: {}, {} bytes, face {}", postScriptName.data(), cache.size, cache.faceIndex);
    return true;
}

void LocateFont(AppLocale locale, CachedFont& cache)
{
    const CFStringRef sc[] = {CFSTR("NotoSansCJKsc-Regular"), CFSTR("PingFangSC-Regular"),
                             CFSTR("STHeitiSC-Medium"), CFSTR("HiraginoSansGB-W4"), nullptr};
    const CFStringRef tc[] = {CFSTR("NotoSansCJKtc-Regular"), CFSTR("PingFangTC-Regular"),
                             CFSTR("PingFangHK-Regular"), CFSTR("STHeitiTC-Medium"), nullptr};
    const CFStringRef jp[] = {CFSTR("NotoSansCJKjp-Regular"), CFSTR("HiraginoSans-W4"),
                             CFSTR("HiraKakuProN-W4"), CFSTR("YuGothic-Regular"), nullptr};
    const CFStringRef kr[] = {CFSTR("NotoSansCJKkr-Regular"), CFSTR("AppleSDGothicNeo-Regular"),
                             CFSTR("AppleGothic"), nullptr};
    const UniChar scSample[] = {0x4e2d, 0x6c49};
    const UniChar tcSample[] = {0x4e2d, 0x6f22};
    const UniChar jpSample[] = {0x65e5, 0x3042, 0x30a2};
    const UniChar krSample[] = {0xd55c, 0xae00};
    const CFStringRef* candidates{};
    const UniChar* sample{};
    CFIndex sampleSize = 2;
    switch (locale)
    {
    case AppLocale::SIMPLIFIED_CHINESE: candidates = sc; sample = scSample; break;
    case AppLocale::TRADITIONAL_CHINESE: candidates = tc; sample = tcSample; break;
    case AppLocale::JAPANESE: candidates = jp; sample = jpSample; sampleSize = 3; break;
    case AppLocale::KOREAN: candidates = kr; sample = krSample; break;
    default: return;
    }
    for (; *candidates; ++candidates)
        if (LoadFont(*candidates, sample, sampleSize, cache))
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

static MDRConnectionMacOS* gConn = nullptr;

int clientPlatformConnectionInit(int flags)
{
    if (flags & MDR_INIT_BT_BLE)
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
    gConn = mdrConnectionMacOSCreate();
    return MDR_RESULT_OK;
}

void clientPlatformConnectionDestroy()
{
    if (gConn) { mdrConnectionMacOSDestroy(gConn); gConn = nullptr; }
}

MDRConnection* clientPlatformConnectionGet()
{
    if (gConn) return mdrConnectionMacOSGet(gConn);
    [[unlikely]] return nullptr;
}

void clientPlatformDestroy()
{
    clientPlatformConnectionDestroy();
    for (auto& font : gFonts)
        font = CachedFont{};
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
