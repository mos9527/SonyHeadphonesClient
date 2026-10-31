#include "../Platform.hpp"
#include <CoreText/CoreText.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <array>
#include <cerrno>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>
#include <mdr/Protocol.hpp>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#import <QuartzCore/CAMetalLayer.h>
#ifdef SDL_PLATFORM_IOS
extern "C" void clientIOSDestroy();
#else
bool clientPlatformInit()
{
    return true;
}

bool clientPlatformWaitEventTimeout(int timeoutMS)
{
    return SDL_WaitEventTimeout(nullptr, timeoutMS);
}

int clientPlatformSaveBlob(
    const char* filename, const unsigned char* data, size_t dataSize, const char* mimeType,
    SDL_Window* window, ClientPlatformSaveBlobCallback callback, void* userdata)
{
    return clientSDLSaveBlob(filename, data, dataSize, mimeType, window, callback, userdata);
}
#endif

float clientPlatformGetDisplayScale()
{
    const float scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    return scale > 0.0f ? scale : 1.0f;
}

bool clientPlatformInitGraphics(SDL_Renderer* renderer)
{
    if (!renderer)
        return true;

    // Opt for double buffering (instead of triple buffering by default) on Apple platforms
    // Saves one CAMetalDrawable worth of memory (iDevices run the app fullscreen, so that's *quite* a lot)
    if (@available(iOS 11.2, macOS 10.13.2, *))
    {
        CAMetalLayer* layer = (__bridge CAMetalLayer*)SDL_GetRenderMetalLayer(renderer);
        if (layer)
        {
            layer.maximumDrawableCount = 2;
            MDR_LOG("Metal drawable count: {}", static_cast<long>(layer.maximumDrawableCount));
        }
    }
    return true;
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
    std::string path;
};

void ReleaseFont(CachedFont& font)
{
    clientPlatformMemoryUnmapFile(font.data, static_cast<size_t>(font.size));
    if (!font.path.empty())
        unlink(font.path.c_str());
    font = CachedFont{};
}

std::array<CachedFont, static_cast<size_t>(AppLocale::NUM_LOCALES)> gFonts;

struct TemporaryFontFile
{
    int fd{-1};
    std::string path;

    ~TemporaryFontFile()
    {
        if (fd >= 0)
        {
            close(fd);
            if (!path.empty())
                unlink(path.c_str());
        }
    }
};

bool WriteFontFile(int fd, const unsigned char* data, size_t size, size_t offset)
{
    while (size > 0)
    {
        const ssize_t written = pwrite(fd, data, size, static_cast<off_t>(offset));
        if (written < 0 && errno == EINTR)
            continue;
        if (written <= 0)
            return false;
        const auto count = static_cast<size_t>(written);
        data += count;
        size -= count;
        offset += count;
    }
    return true;
}

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
    const CFIndex tagCount = tags ? CFArrayGetCount(tags) : 0;
    if (tagCount <= 0 || tagCount > 0xFFFF)
        return false;
    std::vector<uint32_t> sortedTags;
    sortedTags.reserve(static_cast<size_t>(tagCount));
    for (CFIndex i = 0; i < tagCount; ++i)
        sortedTags.push_back(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(CFArrayGetValueAtIndex(tags, i))));
    std::sort(sortedTags.begin(), sortedTags.end());

    const char* directory = std::getenv("TMPDIR");
    std::unique_ptr<char, decltype(&SDL_free)> fallbackPath(nullptr, SDL_free);
    if (!directory || !*directory)
    {
        fallbackPath.reset(SDL_GetPrefPath(nullptr, "SonyHeadphonesClient"));
        directory = fallbackPath.get();
    }
    if (!directory || !*directory)
        return false;
    TemporaryFontFile file;
    file.path = directory;
    if (file.path.back() != '/')
        file.path += '/';
    file.path += "SonyHeadphonesClient-font-XXXXXX";
    file.fd = mkstemp(file.path.data());
    if (file.fd < 0 || fcntl(file.fd, F_SETFD, FD_CLOEXEC) < 0)
        return false;

    struct Table
    {
        uint32_t tag;
        uint32_t checksum;
        uint32_t offset;
        uint32_t length;
    };
    std::vector<Table> tables;
    tables.reserve(sortedTags.size());
    size_t offset = 12 + 16 * sortedTags.size();
    for (const auto tag : sortedTags)
    {
        CFPtr<CFDataRef> data(CTFontCopyTable(font, tag, kCTFontTableOptionNoOptions));
        if (!data || CFDataGetLength(data.get()) <= 0)
            continue;
        const size_t length = static_cast<size_t>(CFDataGetLength(data.get()));
        if (length > static_cast<size_t>(INT_MAX))
            return false;
        const size_t paddedLength = (length + 3) & ~size_t(3);
        if (paddedLength > static_cast<size_t>(INT_MAX) - offset)
            return false;
        const auto* bytes = CFDataGetBytePtr(data.get());
        if (!WriteFontFile(file.fd, bytes, length, offset))
            return false;
        uint32_t checksum = 0;
        const size_t wholeLength = length & ~size_t(3);
        for (size_t word = 0; word < wholeLength; word += 4)
            checksum += ReadUInt32BE(bytes + word);
        if (wholeLength < length)
        {
            std::array<unsigned char, 4> tail{};
            std::memcpy(tail.data(), bytes + wholeLength, length - wholeLength);
            checksum += ReadUInt32BE(tail.data());
        }
        tables.push_back({tag, checksum, static_cast<uint32_t>(offset), static_cast<uint32_t>(length)});
        offset += paddedLength;
    }
    if (tables.empty())
        return false;

    const size_t count = tables.size();
    std::vector<unsigned char> header(12 + 16 * count);
    uint32_t entrySelector = 0;
    while ((2u << entrySelector) <= count)
        ++entrySelector;
    const uint32_t searchRange = 16u << entrySelector;
    WriteUInt32BE(header.data(), cffOnly ? 0x4f54544f : 0x00010000);
    WriteUInt16BE(header.data() + 4, static_cast<uint32_t>(count));
    WriteUInt16BE(header.data() + 6, searchRange);
    WriteUInt16BE(header.data() + 8, entrySelector);
    WriteUInt16BE(header.data() + 10, static_cast<uint32_t>(count * 16) - searchRange);
    for (size_t i = 0; i < count; ++i)
    {
        auto* record = header.data() + 12 + i * 16;
        WriteUInt32BE(record, tables[i].tag);
        WriteUInt32BE(record + 4, tables[i].checksum);
        WriteUInt32BE(record + 8, tables[i].offset);
        WriteUInt32BE(record + 12, tables[i].length);
    }
    if (!WriteFontFile(file.fd, header.data(), header.size(), 0) || ftruncate(file.fd, static_cast<off_t>(offset)) < 0)
        return false;
    void* mapped{};
    size_t mappedSize{};
    if (clientPlatformMemoryMapFile(file.path.c_str(), &mapped, &mappedSize) != MDR_RESULT_OK)
        return false;
    cache.path.swap(file.path);
    cache.data = mapped;
    cache.size = static_cast<int>(mappedSize);
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
#ifdef SDL_PLATFORM_IOS
    clientIOSDestroy();
#endif
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
