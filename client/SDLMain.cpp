// SDL_Renderer backend from https://github.com/ocornut/imgui/blob/master/examples/example_sdl3_sdlrenderer3
#include <climits>
#include <cstdio>
#include <cstring>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <mdr/Protocol.hpp>

#include "Recorder.hpp"
#include "Platform/Platform.hpp"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Fonts/PlexSansIcon.h"
#include "MaterialYouTheme.hpp"
#ifdef MDR_CLIENT_DEBUGGER
#include "Debugger.hpp"
#endif
// Implemented by Client.cpp
extern bool clientShouldExit();
extern void clientSetPauseMediaOnRemove(bool enabled);
#ifdef MDR_CLIENT_DEBUGGER
extern void clientEnterDebuggerReplayMode();
#endif

bool gShouldClose = false;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
static FontLocale gFontLocale = FontLocale::UNDEFINED;
static bool gPlatformFontLoaded = false;
static int gFontFallbackIndex = static_cast<int>(FontLocale::SIMPLIFIED_CHINESE);
static char* gFontFallbackData = nullptr;
static int gFontFallbackSize{};
static const char* gFontFallbackPath = nullptr;
static constexpr ImWchar gIconGlyphRanges[] = {0xf000, 0xf2ff, 0};

static void DestroyFontFallback()
{
    SDL_free(gFontFallbackData);
    gFontFallbackData = nullptr;
    gFontFallbackSize = 0;
    gFontFallbackPath = nullptr;
}

FontLocale clientPlatformGetFontLocale()
{
    return gFontLocale;
}

void clientPlatformSetFontLocale(FontLocale locale)
{
    gFontLocale = locale;
    gPlatformFontLoaded = false;
}

void mainLoop()
{
    ImGuiIO& io = ImGui::GetIO();
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT)
            gShouldClose = true;
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(gWindow))
            gShouldClose = true;
#ifdef MDR_CLIENT_DEBUGGER
        if (event.type == SDL_EVENT_DROP_FILE && event.drop.windowID == SDL_GetWindowID(gWindow))
        {
            size_t replayed{};
            if (clientDebuggerReplayPath(event.drop.data, &replayed))
            {
                clientEnterDebuggerReplayMode();
                SDL_Log("Replayed %zu packet(s) from %s", replayed, event.drop.data);
            }
            else
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to replay %s: %s", event.drop.data, SDL_GetError());
            }
        }
#endif
    }
    if (SDL_GetWindowFlags(gWindow) & SDL_WINDOW_MINIMIZED)
    {
        SDL_Delay(10);
        return;
    }
    // Start the Dear ImGui frame
    {
        while (!gPlatformFontLoaded)
        {
            const bool useFontFallback = gFontFallbackData && gFontFallbackSize > 0;
            const FontLocale locale = !useFontFallback && gFontLocale == FontLocale::UNDEFINED ?
                static_cast<FontLocale>(gFontFallbackIndex) : gFontLocale;
            const char* fontData = gFontFallbackData;
            int faceIndex{};
            const int fontSize = useFontFallback ? gFontFallbackSize :
                clientPlatformLocateFontBinary(locale, &fontData, &faceIndex);
            if (fontSize < 0)
                break;
            if (fontSize > 0 && fontData && faceIndex >= 0)
            {
                MDR_LOG("Loading {} font: locale {}, {} bytes, face {}",
                        useFontFallback ? "file" : "platform", locale, fontSize, faceIndex);
                ImFontConfig config{};
                config.FontDataOwnedByAtlas = false;
                config.FontNo = static_cast<ImU32>(faceIndex);
                config.GlyphExcludeRanges = gIconGlyphRanges;
                if (ImFont* font = io.Fonts->AddFontFromMemoryTTF(
                        const_cast<char*>(fontData), fontSize, 15.0f, &config))
                {
                    ImFontConfig iconConfig{};
                    iconConfig.MergeMode = true;
                    iconConfig.DstFont = font;
                    if (io.Fonts->AddFontFromMemoryCompressedBase85TTF(
                            kEmbedFontPlexSansIcon, 15.0f, &iconConfig, gIconGlyphRanges))
                    {
                        io.FontDefault = font;
                        gPlatformFontLoaded = true;
                        MDR_LOG("Loaded {} font: locale {}, face {}",
                                useFontFallback ? "file" : "platform", locale, faceIndex);
                        break;
                    }
                }
                if (useFontFallback)
                {
                    MDR_LOG("Unable to load font file {}.", gFontFallbackPath);
                }
                else
                {
                    MDR_LOG("Unable to load platform font: locale {}, face {}", locale, faceIndex);
                }
            }
            if (useFontFallback || gFontLocale != FontLocale::UNDEFINED ||
                ++gFontFallbackIndex >= static_cast<int>(FontLocale::NUM_LOCALES))
                gPlatformFontLoaded = true;
        }
        // New frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }
    gShouldClose |= clientShouldExit();
    // Rendering
    {
        ImGui::Render();
        SDL_SetRenderScale(gRenderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
        SDL_RenderClear(gRenderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), gRenderer);
        SDL_RenderPresent(gRenderer);
    }
#ifdef __EMSCRIPTEN__
    if (gShouldClose)
    {
        emscripten_cancel_main_loop();
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        DestroyFontFallback();
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
        clientPlatformDestroy();
    }
#endif
}

#define CLIENT_WINDOW_WIDTH 800
#define CLIENT_WINDOW_HEIGHT 600

namespace
{
#ifdef _WIN32
    void OpenConsole()
    {
        if (!AllocConsole() && GetLastError() != ERROR_ACCESS_DENIED)
            return;

        std::freopen("CONOUT$", "w", stdout);
        std::freopen("CONOUT$", "w", stderr);
        std::freopen("CONIN$", "r", stdin);
        SetConsoleOutputCP(CP_UTF8);
    }
#endif

    FontLocale GetPreferredFontLocale()
    {
        FontLocale result = FontLocale::UNDEFINED;
        SDL_Locale** locales = SDL_GetPreferredLocales(nullptr);
        for (SDL_Locale** current = locales;
             current && *current && result == FontLocale::UNDEFINED; ++current)
        {
            const auto& locale = **current;
            if (!locale.language)
                continue;
            if (SDL_strcasecmp(locale.language, "zh") == 0)
            {
                const char* country = locale.country;
                const bool traditional = country &&
                    (SDL_strcasecmp(country, "Hant") == 0 || SDL_strcasecmp(country, "TW") == 0 ||
                     SDL_strcasecmp(country, "HK") == 0 || SDL_strcasecmp(country, "MO") == 0);
                result = traditional ? FontLocale::TRADITIONAL_CHINESE : FontLocale::SIMPLIFIED_CHINESE;
            }
            else if (SDL_strcasecmp(locale.language, "ja") == 0)
                result = FontLocale::JAPANESE;
            else if (SDL_strcasecmp(locale.language, "ko") == 0)
                result = FontLocale::KOREAN;
        }
        SDL_free(locales);
        return result;
    }

    struct ClientOptions
    {
        const char* recordDirectory{};
        const char* replayPath{};
        const char* fontPath{};
        bool showHelp{};
        bool pauseMediaOnRemove{};
        FontLocale locale{FontLocale::UNDEFINED};
        bool localeSpecified{};
    };

    void PrintUsage()
    {
        MDR_LOG(
            "Usage: SonyHeadphonesClient [--record <capture-folder>]\tRecords device packets automatically to folder");
        MDR_LOG("                            [--locale undefined|sc|tc|jp|kr]\tOverride system locale");
        MDR_LOG("                            [--font <font-file>]\tLoad an external font without changing locale");
#ifdef MDR_CLIENT_DEBUGGER
        MDR_LOG("                            [--replay <packet-file-or-folder>]\tReplays devices packets from folder");
#endif
        // Windows specific
#ifdef _WIN32
        MDR_LOG("                            [--con]\tOpens console for diagnostic logs");
#endif
        // Linux specific (DBus)
#ifdef __linux__
        MDR_LOG("                            [--pause-media-on-remove]\tAuto-pause system media playback when device "
                "is removed when unsupported by OS otherwise.");
#endif
    }

    bool ParseOptions(int argc, char** argv, ClientOptions& options)
    {
        for (int index = 1; index < argc; ++index)
        {
            const char* argument = argv[index];
            if (std::strcmp(argument, "--help") == 0 || std::strcmp(argument, "-h") == 0)
            {
                options.showHelp = true;
                continue;
            }
            if (std::strcmp(argument, "-con") == 0)
            {
#ifdef _WIN32
                OpenConsole();
#endif
                continue;
            }
            if (std::strcmp(argument, "--pause-media-on-remove") == 0)
            {
                options.pauseMediaOnRemove = true;
                continue;
            }

            if (std::strcmp(argument, "--locale") == 0)
            {
                if (++index >= argc)
                {
                    MDR_LOG("Missing locale after {}.", argument);
                    return false;
                }
                constexpr const char* names[] = {"undefined", "sc", "tc", "jp", "kr"};
                constexpr int localeCount = static_cast<int>(FontLocale::NUM_LOCALES);
                static_assert(sizeof(names) / sizeof(names[0]) == localeCount);
                int locale{};
                for (; locale < localeCount; ++locale)
                    if (std::strcmp(argv[index], names[locale]) == 0)
                        break;
                if (locale == localeCount)
                {
                    MDR_LOG("Invalid locale: {}. Expected undefined, sc, tc, jp, or kr.", argv[index]);
                    return false;
                }
                options.locale = static_cast<FontLocale>(locale);
                options.localeSpecified = true;
                continue;
            }

            const bool record = std::strcmp(argument, "--record") == 0;
            const bool replay = std::strcmp(argument, "--replay") == 0;
            const bool font = std::strcmp(argument, "--font") == 0;
            if (record || replay || font)
            {
                if (index + 1 >= argc)
                {
                    MDR_LOG("Missing path after {}.", argument);
                    return false;
                }
                const char* path = argv[++index];
                const char*& destination = font ? options.fontPath :
                    (record ? options.recordDirectory : options.replayPath);
                if (destination)
                {
                    MDR_LOG("{} may only be specified once.", argument);
                    return false;
                }
                destination = path;
                continue;
            }

            MDR_LOG("Unknown argument: {}", argument);
            return false;
        }

        if (options.recordDirectory && options.replayPath)
        {
            MDR_LOG("--record and --replay cannot be used together.");
            return false;
        }
        return true;
    }
} // namespace

int main(int argc, char** argv)
{
    ClientOptions options;
    if (!ParseOptions(argc, argv, options))
    {
        PrintUsage();
        return 2;
    }
    gFontLocale = options.locale;
    gPlatformFontLoaded = false;
    gFontFallbackIndex = static_cast<int>(FontLocale::SIMPLIFIED_CHINESE);
    clientSetPauseMediaOnRemove(options.pauseMediaOnRemove);
    if (options.showHelp)
    {
        PrintUsage();
        return 0;
    }
#ifndef MDR_CLIENT_DEBUGGER
    if (options.replayPath)
    {
        MDR_LOG("Packet replay is unavailable because this client was built without the debugger.");
        return 2;
    }
#endif

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        MDR_LOG("SDL_Init Error: {}", SDL_GetError());
        return 1;
    }
    if (!options.localeSpecified)
        gFontLocale = GetPreferredFontLocale();
    MDR_LOG("Selected locale: {}", gFontLocale);
    if (options.recordDirectory)
    {
        if (!clientPayloadRecorderConfigure(options.recordDirectory))
        {
            MDR_LOG("Unable to prepare capture folder {}: {}", options.recordDirectory, SDL_GetError());
            SDL_Quit();
            return 1;
        }
        MDR_LOG("Recording MDR packets to {}. Existing mdr-packet-*.bin files were cleared. Captures may contain "
                "device addresses, names, and playback metadata.",
                options.recordDirectory);
    }
#ifdef MDR_CLIENT_DEBUGGER
    if (options.replayPath)
    {
        size_t replayed{};
        if (!clientDebuggerReplayPath(options.replayPath, &replayed))
        {
            MDR_LOG("Unable to replay packet path {}: {}", options.replayPath, SDL_GetError());
            SDL_Quit();
            return 1;
        }
        clientEnterDebuggerReplayMode();
        MDR_LOG("Replayed {} packet(s) from {} in debugger-only mode.", replayed, options.replayPath);
    }
#endif
    // https://github.com/libsdl-org/SDL/blob/main/docs/README-highdpi.md#numeric-example
    // This should only be effective (!=1.0f) on Windows and X11 platforms
    float displayScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    gWindow =
        SDL_CreateWindow("SonyHeadphonesClient", CLIENT_WINDOW_WIDTH * displayScale,
                         CLIENT_WINDOW_HEIGHT * displayScale, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!gWindow)
    {
        SDL_Log("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return 1;
    }
#ifdef MDR_CLIENT_DEBUGGER
    clientDebuggerSetWindow(gWindow);
#endif
    gRenderer = SDL_CreateRenderer(gWindow, nullptr);
    SDL_SetRenderVSync(gRenderer, 1);
    if (!gRenderer)
    {
        SDL_Log("Error: SDL_CreateRenderer()\n");
        return 1;
    }
    if (options.fontPath)
    {
        size_t fontSize{};
        char* fontData = static_cast<char*>(SDL_LoadFile(options.fontPath, &fontSize));
        if (!fontData || fontSize == 0 || fontSize > static_cast<size_t>(INT_MAX))
        {
            if (!fontData)
                MDR_LOG("Unable to read font file {}: {}", options.fontPath, SDL_GetError())
            else
                MDR_LOG("Invalid font file size for {}: {} bytes", options.fontPath, fontSize)
            SDL_free(fontData);
            SDL_DestroyRenderer(gRenderer);
            SDL_DestroyWindow(gWindow);
            SDL_Quit();
            return 1;
        }
        gFontFallbackData = fontData;
        gFontFallbackSize = static_cast<int>(fontSize);
        gFontFallbackPath = options.fontPath;
    }
    // Setup Dear ImGui context
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
    }
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    // Setup Material You theme (Sony Sound Connect style)
    ImGui::StyleColorsDark(); // Base fallback
    MaterialYouTheme::ApplyDefault();
    auto& style = ImGui::GetStyle();
    style.ScaleAllSizes(displayScale);
    style.FontScaleDpi = displayScale;
    style.FrameRounding = 8.0f;
    style.CircleTessellationMaxError = 0.01f;
    style.FramePadding = ImVec2(8.0f, 8.0f);
    // Setup Platform/Renderer backends
    {
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
        io.ConfigErrorRecoveryEnableAssert = true; // Don't assert on errors
        ImGui_ImplSDL3_InitForSDLRenderer(gWindow, gRenderer);
        ImGui_ImplSDLRenderer3_Init(gRenderer);
    }
    // Load our default font
    {
        io.Fonts->Clear();
#ifdef MDR_CLIENT_DEBUGGER
        ImFont* monospaceFont = io.Fonts->AddFontDefault();
#endif
        io.FontDefault = io.Fonts->AddFontFromMemoryCompressedBase85TTF(kEmbedFontPlexSansIcon, 15.0f);
#ifdef MDR_CLIENT_DEBUGGER
        clientDebuggerSetMonospaceFont(monospaceFont);
#endif
    }
    // Main loop

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainLoop, 0, 1);
#else
    while (!gShouldClose)
        mainLoop();
#endif

    // Cleanup
    {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        DestroyFontFallback();

        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        SDL_Quit();

        clientPlatformDestroy();
    }
    return 0;
}
