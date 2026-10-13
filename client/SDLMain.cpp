// SDL_Renderer backend from https://github.com/ocornut/imgui/blob/master/examples/example_sdl3_sdlrenderer3
#include <atomic>
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
#include "I18N/Strings.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef MDR_CLIENT_DEBUGGER
#include "Debugger.hpp"
#endif

bool gShouldClose = false;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

int64_t clientGetUnixEpochMS()
{
    SDL_Time now{};
    if (!SDL_GetCurrentTime(&now))
        return -1;
    return now / 1'000'000;
}

namespace
{
    bool gClientInitialized{};
    bool gPlatformBackendInitialized{};
    bool gRendererBackendInitialized{};
    std::atomic_bool gAppActive{true};
    float gAppliedDisplayScale{1.0f};

    void ShutdownClient()
    {
        if (gRendererBackendInitialized)
            ImGui_ImplSDLRenderer3_Shutdown();
        if (gPlatformBackendInitialized)
            ImGui_ImplSDL3_Shutdown();
        if (gClientInitialized)
            clientDestroy();
        gRendererBackendInitialized = false;
        gPlatformBackendInitialized = false;
        gClientInitialized = false;
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        gRenderer = nullptr;
        gWindow = nullptr;
        SDL_Quit();
        clientPlatformDestroy();
    }

    bool ProcessClientEvent(SDL_Event& event)
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT ||
            (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(gWindow)))
            return true;
        if (event.type == SDL_EVENT_WILL_ENTER_BACKGROUND)
            gAppActive = false;
        if (event.type == SDL_EVENT_DID_ENTER_FOREGROUND)
            gAppActive = true;
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
        return false;
    }
}

void mainLoop()
{
    ImGuiIO& io = ImGui::GetIO();
#ifndef SDL_MAIN_USE_CALLBACKS
    SDL_Event event;
    while (SDL_PollEvent(&event))
        gShouldClose |= ProcessClientEvent(event);
#endif
    if (!gAppActive || (SDL_GetWindowFlags(gWindow) & SDL_WINDOW_MINIMIZED))
    {
#ifndef SDL_MAIN_USE_CALLBACKS
        SDL_Delay(10);
#endif
        return;
    }
    const float displayScale = clientPlatformGetDisplayScale();
    if (displayScale != gAppliedDisplayScale)
    {
        ImGui::GetStyle().ScaleAllSizes(displayScale / gAppliedDisplayScale);
        ImGui::GetStyle().FontScaleDpi = displayScale;
        gAppliedDisplayScale = displayScale;
    }
    clientPrepareFrame();
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    gShouldClose |= clientShouldExit();
    // Rendering
    {
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
        ShutdownClient();
    }
#endif
}

#define CLIENT_WINDOW_WIDTH 800
#define CLIENT_WINDOW_HEIGHT 600

namespace
{
#ifdef _WIN32
    void OpenConsole(bool allocateIfUnavailable)
    {
        const HANDLE standardError = GetStdHandle(STD_ERROR_HANDLE);
        if (standardError && standardError != INVALID_HANDLE_VALUE)
            return;

        bool attached = AttachConsole(ATTACH_PARENT_PROCESS) != FALSE;
        const DWORD attachError = attached ? ERROR_SUCCESS : GetLastError();
        if (!attached && attachError == ERROR_ACCESS_DENIED)
            attached = true;

        bool allocated = false;
        if (!attached && allocateIfUnavailable)
        {
            allocated = AllocConsole() != FALSE;
            attached = allocated;
        }
        if (!attached)
            return;

        std::freopen("CONOUT$", "w", stdout);
        std::freopen("CONOUT$", "w", stderr);
        std::freopen("CONIN$", "r", stdin);
        if (allocated)
            SetConsoleOutputCP(CP_UTF8);
    }
#endif

    AppLocale GetPreferredAppLocale()
    {
        AppLocale result = AppLocale::DEFAULT;
        SDL_Locale** locales = SDL_GetPreferredLocales(nullptr);
        for (SDL_Locale** current = locales;
             current && *current && result == AppLocale::DEFAULT; ++current)
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
                result = traditional ? AppLocale::TRADITIONAL_CHINESE : AppLocale::SIMPLIFIED_CHINESE;
            }
            else if (SDL_strcasecmp(locale.language, "ja") == 0)
                result = AppLocale::JAPANESE;
            else if (SDL_strcasecmp(locale.language, "ko") == 0)
                result = AppLocale::KOREAN;
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
        AppLocale locale{AppLocale::DEFAULT};
        bool localeSpecified{};
    };

    void PrintUsage()
    {
        std::fprintf(stderr,
                     "Usage: SonyHeadphonesClient\n");
        std::fprintf(stderr,
                     "    [--record <capture-folder>] Records device packets automatically to folder\n");
        std::fprintf(stderr,
                     "    [--locale %s] Override application locale selected from the system\n",
                     i18n::kLocaleOptionString);
        std::fprintf(stderr,
                     "    [--font <font-file>] Load an external font without changing application locale\n");
#ifdef MDR_CLIENT_DEBUGGER
        std::fprintf(stderr,
                     "    [--replay <packet-file-or-folder>] Replays devices packets from folder\n");
#endif
        // Windows specific
#ifdef _WIN32
        std::fprintf(stderr,
                     "    [--con] Opens console for diagnostic logs\n");
#endif
        // Linux specific (DBus)
#ifdef __linux__
        std::fprintf(stderr,
                     "    [--pause-media-on-remove] Auto-pause system media playback when device "
                     "is removed when unsupported by OS otherwise.\n");
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
            if (std::strcmp(argument, "--con") == 0 || std::strcmp(argument, "-con") == 0)
            {
#ifdef _WIN32
                OpenConsole(true);
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
                const auto locale = i18n::ParseLocale(argv[index]);
                if (!locale)
                {
                    MDR_LOG("Invalid locale: {}. Expected default, sc, tc, jp, or kr.", argv[index]);
                    return false;
                }
                options.locale = *locale;
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

static int InitializeClient(int argc, char** argv)
{
#ifdef _WIN32
    OpenConsole(false);
#endif
    ClientOptions options;
    if (!ParseOptions(argc, argv, options))
    {
        PrintUsage();
        return 2;
    }
    clientSetAppLocale(options.locale);
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
        clientSetAppLocale(GetPreferredAppLocale());
    MDR_LOG("Selected locale: {}", clientGetAppLocale());
    if (options.recordDirectory)
    {
        if (!clientPayloadRecorderConfigure(options.recordDirectory))
        {
            MDR_LOG("Unable to prepare capture folder {}: {}", options.recordDirectory, SDL_GetError());
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
            return 1;
        }
        clientEnterDebuggerReplayMode();
        MDR_LOG("Replayed {} packet(s) from {} in debugger-only mode.", replayed, options.replayPath);
    }
#endif
    const float displayScale = clientPlatformGetDisplayScale();
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
    if (!gRenderer)
    {
        SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
        return 1;
    }
    SDL_Log("Using SDL_Renderer: %s", SDL_GetRendererName(gRenderer));
    SDL_SetRenderVSync(gRenderer, 1);
    if (!clientInit(displayScale, options.fontPath))
        return 1;
    gClientInitialized = true;
    gAppliedDisplayScale = displayScale;
    gPlatformBackendInitialized = ImGui_ImplSDL3_InitForSDLRenderer(gWindow, gRenderer);
    if (!gPlatformBackendInitialized)
    {
        SDL_Log("Error: ImGui_ImplSDL3_InitForSDLRenderer()");
        return 1;
    }
    gRendererBackendInitialized = ImGui_ImplSDLRenderer3_Init(gRenderer);
    if (!gRendererBackendInitialized)
    {
        SDL_Log("Error: ImGui_ImplSDLRenderer3_Init()");
        return 1;
    }
    return 0;
}

#ifdef SDL_MAIN_USE_CALLBACKS
SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
    *appstate = nullptr;
    if (InitializeClient(argc, argv) != 0)
        return SDL_APP_FAILURE;
    return gClientInitialized ? SDL_APP_CONTINUE : SDL_APP_SUCCESS;
}

SDL_AppResult SDL_AppEvent(void*, SDL_Event* event)
{
    return ProcessClientEvent(*event) ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void*)
{
    mainLoop();
    return gShouldClose ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
}

void SDL_AppQuit(void*, SDL_AppResult)
{
    ShutdownClient();
}
#else
int main(int argc, char** argv)
{
    const int result = InitializeClient(argc, argv);
    if (result != 0 || !gClientInitialized)
    {
        ShutdownClient();
        return result;
    }
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainLoop, 0, 1);
#else
    while (!gShouldClose)
        mainLoop();
    ShutdownClient();
#endif
    return 0;
}
#endif
