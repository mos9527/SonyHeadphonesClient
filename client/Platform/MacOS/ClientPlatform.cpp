#include "../Platform.hpp"
#include <CoreText/CoreText.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <array>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <type_traits>
#include <vector>
#include <mdr/Protocol.hpp>
#include <SDL3/SDL_video.h>

float clientPlatformGetDisplayScale()
{
    const float scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    return scale > 0.0f ? scale : 1.0f;
}

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
    void* data{};
    int size{};
};

void ReleaseFont(CachedFont& font)
{
    std::free(font.data);
    font = CachedFont{};
}

std::array<CachedFont, static_cast<size_t>(AppLocale::NUM_LOCALES)> gFonts;

uint32_t ReadUInt32BE(const unsigned char* data)
{
    return (uint32_t(data[0]) << 24) | (uint32_t(data[1]) << 16) |
           (uint32_t(data[2]) << 8) | uint32_t(data[3]);
}

void WriteUInt16BE(unsigned char* data, uint32_t value)
{
    data[0] = static_cast<unsigned char>(value >> 8);
    data[1] = static_cast<unsigned char>(value);
}

void WriteUInt32BE(unsigned char* data, uint32_t value)
{
    WriteUInt16BE(data, value >> 16);
    WriteUInt16BE(data + 2, value);
}

// Rebuild a standalone sfnt from the tables CoreText exposes. Avoids touching font files
// directly, which are not readable from the iOS app sandbox (system/asset fonts).
bool CopyFontTables(CTFontRef font, CFArrayRef tags, bool cffOnly, CachedFont& cache)
{
    struct Table
    {
        uint32_t tag;
        CFPtr<CFDataRef> data;
    };
    std::vector<Table> tables;
    for (CFIndex i = 0; i < CFArrayGetCount(tags); ++i)
    {
        const auto tag = static_cast<CTFontTableTag>(reinterpret_cast<uintptr_t>(CFArrayGetValueAtIndex(tags, i)));
        CFPtr<CFDataRef> data(CTFontCopyTable(font, tag, kCTFontTableOptionNoOptions));
        if (data && CFDataGetLength(data.get()) > 0)
            tables.push_back({tag, std::move(data)});
    }
    if (tables.empty() || tables.size() > 0xFFFF)
        return false;
    std::sort(tables.begin(), tables.end(), [](const Table& a, const Table& b) { return a.tag < b.tag; });
    const size_t count = tables.size();
    size_t total = 12 + 16 * count;
    for (const auto& table : tables)
    {
        total += (static_cast<size_t>(CFDataGetLength(table.data.get())) + 3) & ~size_t(3);
        if (total > static_cast<size_t>(INT_MAX))
            return false;
    }
    auto* out = static_cast<unsigned char*>(std::calloc(total, 1));
    if (!out)
        return false;
    uint32_t entrySelector = 0;
    while ((2u << entrySelector) <= count)
        ++entrySelector;
    const uint32_t searchRange = 16u << entrySelector;
    WriteUInt32BE(out, cffOnly ? 0x4f54544f : 0x00010000);
    WriteUInt16BE(out + 4, static_cast<uint32_t>(count));
    WriteUInt16BE(out + 6, searchRange);
    WriteUInt16BE(out + 8, entrySelector);
    WriteUInt16BE(out + 10, static_cast<uint32_t>(count * 16) - searchRange);
    size_t offset = 12 + 16 * count;
    for (size_t i = 0; i < count; ++i)
    {
        const auto* bytes = CFDataGetBytePtr(tables[i].data.get());
        const size_t length = static_cast<size_t>(CFDataGetLength(tables[i].data.get()));
        std::memcpy(out + offset, bytes, length);
        uint32_t checksum = 0;
        for (size_t word = 0; word < length; word += 4)
            checksum += ReadUInt32BE(out + offset + word);
        auto* record = out + 12 + i * 16;
        WriteUInt32BE(record, tables[i].tag);
        WriteUInt32BE(record + 4, checksum);
        WriteUInt32BE(record + 8, static_cast<uint32_t>(offset));
        WriteUInt32BE(record + 12, static_cast<uint32_t>(length));
        offset += (length + 3) & ~size_t(3);
    }
    cache.data = out;
    cache.size = static_cast<int>(total);
    return true;
}

bool LoadCTFont(CTFontRef font, const UniChar* sample, CFIndex sampleSize, CachedFont& cache)
{
    std::array<CGGlyph, 3> glyphs{};
    if (sampleSize <= 0 || sampleSize > static_cast<CFIndex>(glyphs.size()) ||
        !CTFontGetGlyphsForCharacters(font, sample, glyphs.data(), sampleSize))
        return false;
    CFPtr<CFStringRef> name(CTFontCopyPostScriptName(font));
    std::array<char, 256> postScriptName{};
    if (name)
        CFStringGetCString(name.get(), postScriptName.data(), postScriptName.size(), kCFStringEncodingUTF8);
    CFPtr<CFArrayRef> tables(CTFontCopyAvailableTables(font, kCTFontTableOptionNoOptions));
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
        MDR_LOG("Skipping font without STB-compatible outlines: {}", postScriptName.data());
        return false;
    }
    if (!CopyFontTables(font, tables.get(), !hasGlyf, cache))
        return false;
    MDR_LOG("Located Apple font: {}, {} bytes", postScriptName.data(), cache.size);
    return true;
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
    return LoadCTFont(font.get(), sample, sampleSize, cache);
}

bool LoadCascadeFont(CFStringRef language, const UniChar* sample, CFIndex sampleSize, CachedFont& cache)
{
    CFPtr<CTFontRef> base(CTFontCreateUIFontForLanguage(kCTFontUIFontSystem, 15.0, language));
    if (!base)
        return false;
    CFPtr<CFStringRef> text(CFStringCreateWithCharacters(kCFAllocatorDefault, sample, sampleSize));
    if (!text)
        return false;
    CFPtr<CTFontRef> font(CTFontCreateForString(base.get(), text.get(), CFRangeMake(0, sampleSize)));
    return font && LoadCTFont(font.get(), sample, sampleSize, cache);
}

void LocateFont(AppLocale locale, CachedFont& cache)
{
    const CFStringRef sc[] = {CFSTR("NotoSansCJKsc-Regular"), CFSTR("PingFangSC-Regular"),
                             CFSTR("STHeitiSC-Medium"), CFSTR("HiraginoSansGB-W4"), nullptr};
    const CFStringRef tc[] = {CFSTR("NotoSansCJKtc-Regular"), CFSTR("PingFangTC-Regular"),
                             CFSTR("PingFangHK-Regular"), CFSTR("STHeitiTC-Medium"), nullptr};
    const CFStringRef jp[] = {CFSTR("NotoSansCJKjp-Regular"), CFSTR("HiraginoSans-W4"),
                             CFSTR("HiraKakuProN-W4"), CFSTR("HiraginoSans-W3"),
                             CFSTR("HiraKakuProN-W3"), CFSTR("YuGothic-Regular"), nullptr};
    const CFStringRef kr[] = {CFSTR("NotoSansCJKkr-Regular"), CFSTR("AppleSDGothicNeo-Regular"),
                             CFSTR("AppleGothic"), nullptr};
    const UniChar scSample[] = {0x4e2d, 0x6c49};
    const UniChar tcSample[] = {0x4e2d, 0x6f22};
    const UniChar jpSample[] = {0x65e5, 0x3042, 0x30a2};
    const UniChar krSample[] = {0xd55c, 0xae00};
    const CFStringRef* candidates{};
    const UniChar* sample{};
    CFStringRef language{};
    CFIndex sampleSize = 2;
    switch (locale)
    {
    case AppLocale::SIMPLIFIED_CHINESE: candidates = sc; sample = scSample; language = CFSTR("zh-Hans"); break;
    case AppLocale::TRADITIONAL_CHINESE: candidates = tc; sample = tcSample; language = CFSTR("zh-Hant"); break;
    case AppLocale::JAPANESE: candidates = jp; sample = jpSample; sampleSize = 3; language = CFSTR("ja"); break;
    case AppLocale::KOREAN: candidates = kr; sample = krSample; language = CFSTR("ko"); break;
    default: return;
    }
    for (; *candidates; ++candidates)
        if (LoadFont(*candidates, sample, sampleSize, cache))
            return;
    LoadCascadeFont(language, sample, sampleSize, cache);
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
    return cache.size;
}

int clientPlatformMemoryMapFile(const char* path, void** outAddr, size_t* outSize)
{
    if (outAddr) *outAddr = nullptr;
    if (outSize) *outSize = 0;
    if (!path || !outAddr || !outSize)
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    const int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return MDR_RESULT_ERROR_NOT_FOUND;
    struct stat st{};
    void* addr = MAP_FAILED;
    if (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size > 0 &&
        static_cast<uintmax_t>(st.st_size) <= SIZE_MAX)
        addr = mmap(nullptr, static_cast<size_t>(st.st_size), PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (addr == MAP_FAILED)
        return MDR_RESULT_ERROR_GENERAL;
    *outAddr = addr;
    *outSize = static_cast<size_t>(st.st_size);
    madvise(addr, *outSize, MADV_RANDOM);
    return MDR_RESULT_OK;
}

void clientPlatformMemoryUnmapFile(void* addr, size_t size)
{
    if (addr)
        munmap(addr, size);
}

void clientPlatformDestroy()
{
    clientPlatformConnectionDestroy();
    for (auto& font : gFonts)
        ReleaseFont(font);
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
