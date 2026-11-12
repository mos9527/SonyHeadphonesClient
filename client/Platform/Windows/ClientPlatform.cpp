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
#include <dwmapi.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

int clientPlatformSaveBlob(const char* filename, const unsigned char* data, size_t dataSize, const char* mimeType,
                           SDL_Window* window, ClientPlatformSaveBlobCallback callback, void* userdata)
{
    return clientSDLSaveBlob(filename, data, dataSize, mimeType, window, callback, userdata);
}

float clientPlatformGetDisplayScale()
{
    const float scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    return scale > 0.0f ? scale : 1.0f;
}

namespace
{
    // Window backdrops (DWM)
    // ---
    // - Windows 11 22H2+: System backdrop, see https://learn.microsoft.com/en-us/windows/apps/design/style/mica
    // - Windows Vista/7: Aero glass.
    //   Windows 8/8.1/10 are skipped on purpose: there's no frame transparency to extend.
    // - XP: Never attempted. YY-Thunks stubs dwmapi out anyway.
    // Both extend the frame into the whole client area. See ExtendFrameIntoClientArea
    // The window must be created with SDL_WINDOW_TRANSPARENT: this keeps SDL's D3D11 swap chain on the bitblt model,
    // since DWM ignores alpha from flip-model HWND swap chains.
    // Values are spelled out here since DWMWA_SYSTEMBACKDROP_TYPE and co. only exist in recent (22621+) SDKs.
    constexpr DWORD kDwmUseImmersiveDarkMode = 20; // DWMWA_USE_IMMERSIVE_DARK_MODE
    constexpr DWORD kDwmSystemBackdropType = 38; // DWMWA_SYSTEMBACKDROP_TYPE
    // DWM_SYSTEMBACKDROP_TYPE: 2 = DWMSBT_MAINWINDOW (Mica), 3 = DWMSBT_TRANSIENTWINDOW (Acrylic), 4 = DWMSBT_TABBEDWINDOW
    constexpr DWORD kDwmSystemBackdropMainWindow = 3;
    constexpr DWORD kSystemBackdropMinimumBuild = 22523; // First build accepting DWMWA_SYSTEMBACKDROP_TYPE

    enum class BackdropKind
    {
        None,
        AeroGlass,
        SystemBackdrop,
    };

    WNDPROC gPrevWndProc{};
    bool gBackdropEnabled{};

    const OSVERSIONINFOW& GetWindowsVersion()
    {
        using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW*);
        static const OSVERSIONINFOW info = []
        {
            // GetVersionEx lies to unmanifested processes; RtlGetVersion doesn't. Present since NT 5.0.
            OSVERSIONINFOW result{};
            result.dwOSVersionInfoSize = sizeof(result);
            HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
            auto fn = ntdll ? reinterpret_cast<RtlGetVersion_t>(GetProcAddress(ntdll, "RtlGetVersion")) : nullptr;
            if (!fn || fn(&result) != 0)
                result = {};
            return result;
        }();
        return info;
    }

    BackdropKind GetBackdropKind()
    {
        const OSVERSIONINFOW& version = GetWindowsVersion();
        if (version.dwBuildNumber >= kSystemBackdropMinimumBuild)
            return BackdropKind::SystemBackdrop;
        if (version.dwMajorVersion == 6 && version.dwMinorVersion <= 1) // Vista (6.0), 7 (6.1)
            return BackdropKind::AeroGlass;
        return BackdropKind::None;
    }

    /**
     * @return true if the backdrop is in effect, i.e. ImGui backgrounds may go transparent.
     */
    // "Sheet of glass": the frame extends over the entire client area, so DWM composites the frame's material
    // (Aero glass on Vista/7, the system backdrop on 11) underneath our pixels, honoring their alpha.
    // Must be re-applied on WM_DWMCOMPOSITIONCHANGED.
    bool ExtendFrameIntoClientArea(HWND hwnd)
    {
        const MARGINS margins{-1, -1, -1, -1};
        return SUCCEEDED(DwmExtendFrameIntoClientArea(hwnd, &margins));
    }

    bool ApplyBackdrop(HWND hwnd)
    {
        switch (GetBackdropKind())
        {
        case BackdropKind::SystemBackdrop:
        {
            // The client UI is always dark (Sony's fixed dark surfaces), so the backdrop has to be dark too.
            // Otherwise a light system theme would get us a light backdrop under light text.
            const BOOL dark = TRUE;
            DwmSetWindowAttribute(hwnd, kDwmUseImmersiveDarkMode, &dark, sizeof(dark));
            if (!ExtendFrameIntoClientArea(hwnd))
                return false;
            const DWORD backdrop = kDwmSystemBackdropMainWindow;
            return SUCCEEDED(DwmSetWindowAttribute(hwnd, kDwmSystemBackdropType, &backdrop, sizeof(backdrop)));
        }
        case BackdropKind::AeroGlass:
        {
            // Without composition (Basic/Classic themes) DWM ignores alpha, and a transparent clear shows as black.
            BOOL composition = FALSE;
            if (FAILED(DwmIsCompositionEnabled(&composition)) || !composition)
                return false;
            return ExtendFrameIntoClientArea(hwnd);
        }
        default:
            return false;
        }
    }

    // Subclassed since these messages are *sent*, not posted, so SDL_SetWindowsMessageHook never sees them.
    // - WM_SETTINGCHANGE("ImmersiveColorSet"): SDL resets immersive dark mode to match the system theme;
    //   put ours back after SDL is done with it.
    // - WM_DWMCOMPOSITIONCHANGED: Aero toggled (Vista/7 theme change, or some fullscreen apps). The extended frame
    //   needs re-applying, and the ImGui backgrounds need to follow suit.
    LRESULT CALLBACK BackdropWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        const LRESULT result = CallWindowProcW(gPrevWndProc, hwnd, msg, wParam, lParam);
        bool reapply = false;
        if (msg == WM_SETTINGCHANGE && lParam &&
            std::wcscmp(reinterpret_cast<LPCWSTR>(lParam), L"ImmersiveColorSet") == 0)
            reapply = GetBackdropKind() == BackdropKind::SystemBackdrop;
        if (msg == WM_DWMCOMPOSITIONCHANGED)
            reapply = true;
        if (reapply)
        {
            const bool enabled = ApplyBackdrop(hwnd);
            if (enabled != gBackdropEnabled)
            {
                gBackdropEnabled = enabled;
                MDR_LOG("Window backdrop: {}", enabled ? "enabled" : "disabled");
                // Wake the main loop up for a redraw, it may be idling
                SDL_Event wake{};
                wake.type = SDL_EVENT_USER;
                SDL_PushEvent(&wake);
            }
        }
        return result;
    }
} // namespace

bool clientPlatformWindowsSupportsBackdrop()
{
    return GetBackdropKind() != BackdropKind::None;
}

bool clientPlatformWindowsBackdropEnabled()
{
    return gBackdropEnabled;
}

bool clientPlatformInitGraphics(SDL_Renderer* renderer)
{
    if (!renderer || !clientPlatformWindowsSupportsBackdrop())
        return true;
    SDL_Window* window = SDL_GetRenderWindow(renderer);
    if (!window || !(SDL_GetWindowFlags(window) & SDL_WINDOW_TRANSPARENT))
        return true;
    auto hwnd = static_cast<HWND>(
        SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
    if (!hwnd)
        return true;
    // If DWM refuses, the ImGui backgrounds stay opaque so we don't end up with a see-through (or black) window.
    gBackdropEnabled = ApplyBackdrop(hwnd);
    // Hooked regardless, since composition may come on later (Vista/7)
    if (!gPrevWndProc)
        gPrevWndProc = reinterpret_cast<WNDPROC>(
            SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(BackdropWndProc)));
    MDR_LOG("Window backdrop ({}): {}",
            GetBackdropKind() == BackdropKind::SystemBackdrop ? "system backdrop" : "Aero glass",
            gBackdropEnabled ? "enabled" : "unavailable");
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
        HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, nullptr);
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
        return (uint32_t(data[0]) << 24) | (uint32_t(data[1]) << 16) | (uint32_t(data[2]) << 8) | uint32_t(data[3]);
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
        bool loaded = [&]
        {
            wchar_t actualName[LF_FACESIZE]{};
            if (!GetTextFaceW(dc, LF_FACESIZE, actualName) || _wcsicmp(actualName, name) != 0)
                return false;
            for (const wchar_t* ch = sample; *ch; ++ch)
            {
                WORD glyph{};
                if (GetGlyphIndicesW(dc, ch, 1, &glyph, GGI_MARK_NONEXISTING_GLYPHS) != 1 || glyph == 0xffff ||
                    glyph == 0)
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
        case AppLocale::SIMPLIFIED_CHINESE:
            candidates = sc;
            sample = L"\u4e2d\u6c49";
            break;
        case AppLocale::TRADITIONAL_CHINESE:
            candidates = tc;
            sample = L"\u4e2d\u6f22";
            break;
        case AppLocale::JAPANESE:
            candidates = jp;
            sample = L"\u65e5\u3042\u30a2";
            break;
        case AppLocale::KOREAN:
            candidates = kr;
            sample = L"\ud55c\uae00";
            break;
        default:
            return;
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
    if (outData)
        *outData = nullptr;
    if (outFaceIndex)
        *outFaceIndex = 0;
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
    if (outAddr)
        *outAddr = nullptr;
    if (outSize)
        *outSize = 0;
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

    if (flags & MDR_INIT_BT_BLE)
    {
#ifdef MDR_BT_BLE_SUPPORT
        gConnBLE = mdrConnectionWindowsBLECreate();
        gConnClassic = nullptr;
#else
        return MDR_RESULT_ERROR_NOT_SUPPORTED;
#endif
    }
    else
    {
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
    if (gConnBLE)
    {
        mdrConnectionWindowsBLEDestroy(gConnBLE);
        gConnBLE = nullptr;
    }
#endif
    if (gConnClassic)
    {
        mdrConnectionWindowsDestroy(gConnClassic);
        gConnClassic = nullptr;
    }
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
