// SDL_Renderer backend from https://github.com/ocornut/imgui/blob/master/examples/example_sdl3_sdlrenderer3
#include <cstdio>
#include <cstring>
#include <mutex>
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

#include "I18N/Strings.hpp"
#include "Platform/Platform.hpp"
#include "Recorder.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef MDR_CLIENT_DEBUGGER
#include "Debugger.hpp"
#endif

bool gShouldClose{false};

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

namespace
{
    bool gClientInitialized{};
    bool gPlatformBackendInitialized{};
    bool gRendererBackendInitialized{};
    bool gAppActive{true};
    float gAppliedDisplayScale{1.0f};
    SDL_WindowID gWindowID{};

#ifndef __EMSCRIPTEN__
    SDL_Thread* gClientThread{};
    std::mutex gWorkerMutex;
    bool gStopClient{};
#endif

    bool PollEvents(SDL_Event& event, Uint32 minType = SDL_EVENT_FIRST, Uint32 maxType = SDL_EVENT_LAST)
    {
        const int result = SDL_PeepEvents(&event, 1, SDL_GETEVENT, minType, maxType);
        if (result < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_PeepEvents(): %s", SDL_GetError());
            gShouldClose = true;
        }
        return result > 0;
    }

    bool ShouldAppPresent()
    {
        return !gShouldClose && gAppActive && !(SDL_GetWindowFlags(gWindow) & SDL_WINDOW_MINIMIZED);
    }

#ifndef __EMSCRIPTEN__
    // Worker thread ticks at a fixed 100Hz maximum and does everything
    // that needs interaction with the physical connection.
    // See also @ref clientOnUpdate, @ref clientOnFrame
    int SDLCALL RunOnWorkerThread(void*)
    {
        while (true)
        {
            {
                std::scoped_lock lock(gWorkerMutex);
                if (gStopClient)
                    break;
                clientOnUpdate();
            }
            SDL_Delay(10);
        }
        return 0;
    }
#endif
} // namespace

void mainLoop()
{
    SDL_PumpEvents();
    SDL_Event event;

    auto CheckIsActive = [](SDL_Event& event)
    {
        if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_TERMINATING ||
            (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == gWindowID))
            gShouldClose = true;
        if (event.type == SDL_EVENT_WILL_ENTER_BACKGROUND || event.type == SDL_EVENT_DID_ENTER_BACKGROUND)
            gAppActive = false;
        if (event.type == SDL_EVENT_DID_ENTER_FOREGROUND)
            gAppActive = true;
    };

    while (PollEvents(event, SDL_EVENT_QUIT, SDL_EVENT_DID_ENTER_FOREGROUND))
        CheckIsActive(event);
    while (PollEvents(event, SDL_EVENT_WINDOW_CLOSE_REQUESTED, SDL_EVENT_WINDOW_CLOSE_REQUESTED))
        CheckIsActive(event);

    {
#ifndef __EMSCRIPTEN__
        // Serialize Worker (mdr) and Main (Client SDL+ImGui) thread
        // NOTE: libmdr does not offer thread-safety guarantees (as the connection *itself* is serial)
        //       In your own app you will want to do the same with whatever synchronization primitives your runtime may
        //       provide.
        std::scoped_lock lock(gWorkerMutex);
#else
        clientOnUpdate();
#endif
        while (PollEvents(event))
        {
            CheckIsActive(event);
            if (gShouldClose)
                break;
#ifdef MDR_CLIENT_DEBUGGER
            if (event.type == SDL_EVENT_DROP_FILE && event.drop.windowID == gWindowID && event.drop.data)
            {
                const mdr::String path = event.drop.data;
                size_t replayed{};
                if (clientDebuggerReplayPath(path.c_str(), &replayed))
                {
                    clientEnterDebuggerReplayMode();
                    SDL_Log("Replayed %zu packet(s) from %s", replayed, path.c_str());
                }
                else
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to replay %s: %s", path.c_str(), SDL_GetError());
                continue;
            }
#endif
            ImGui_ImplSDL3_ProcessEvent(&event);
        }
        if (ShouldAppPresent())
        {
            const float displayScale = clientPlatformGetDisplayScale();
            if (displayScale != gAppliedDisplayScale)
            {
                ImGui::GetStyle().ScaleAllSizes(displayScale / gAppliedDisplayScale);
                ImGui::GetStyle().FontScaleDpi = displayScale;
                gAppliedDisplayScale = displayScale;
            }
            ImGui_ImplSDLRenderer3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            clientOnFrame();
        }
    }
    if (ShouldAppPresent())
    {
        const ImGuiIO& io = ImGui::GetIO();
        SDL_SetRenderScale(gRenderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
        SDL_RenderClear(gRenderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), gRenderer);
        SDL_RenderPresent(gRenderer);
    }
#ifndef __EMSCRIPTEN__
    else if (!gShouldClose)
        SDL_Delay(10);
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
        for (SDL_Locale** current = locales; current && *current && result == AppLocale::DEFAULT; ++current)
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

    struct Options
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
        std::fprintf(stderr, "Usage: SonyHeadphonesClient\n");
        std::fprintf(stderr, "    [--record <capture-folder>] Records device packets automatically to folder\n");
        std::fprintf(stderr, "    [--locale %s] Override application locale selected from the system\n",
                     i18n::kLocaleOptionString);
        std::fprintf(stderr, "    [--font <font-file>] Load an external font without changing application locale\n");
#ifdef MDR_CLIENT_DEBUGGER
        std::fprintf(stderr, "    [--replay <packet-file-or-folder>] Replays devices packets from folder\n");
#endif
        // Windows specific
#ifdef _WIN32
        std::fprintf(stderr, "    [--con] Opens console for diagnostic logs\n");
#endif
        // Linux specific (DBus)
#ifdef __linux__
        std::fprintf(stderr,
                     "    [--pause-media-on-remove] Auto-pause system media playback when device "
                     "is removed when unsupported by OS otherwise.\n");
#endif
    }

    bool ParseOptions(int argc, char** argv, Options& options)
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
                const char*& destination =
                    font ? options.fontPath : (record ? options.recordDirectory : options.replayPath);
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
    static const auto Shutdown = []
    {
#ifndef __EMSCRIPTEN__
        // Join the thread...
        {
            std::scoped_lock lock(gWorkerMutex);
            gStopClient = true;
        }
        SDL_WaitThread(gClientThread, nullptr);
        gClientThread = nullptr;
#endif
        gWindowID = 0;
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
    };

    int result = 1;
    do
    {
#ifdef _WIN32
        OpenConsole(false);
#endif
        Options options;
        if (!ParseOptions(argc, argv, options))
        {
            PrintUsage();
            result = 2;
            break;
        }
        clientSetAppLocale(options.locale);
        clientSetPauseMediaOnRemove(options.pauseMediaOnRemove);
        if (options.showHelp)
        {
            PrintUsage();
            result = 0;
            break;
        }
#ifndef MDR_CLIENT_DEBUGGER
        if (options.replayPath)
        {
            MDR_LOG("Packet replay is unavailable because this client was built without the debugger.");
            result = 2;
            break;
        }
#endif

        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            MDR_LOG("SDL_Init Error: {}", SDL_GetError());
            break;
        }
        if (!options.localeSpecified)
            clientSetAppLocale(GetPreferredAppLocale());
        MDR_LOG("Selected locale: {}", clientGetAppLocale());
        if (options.recordDirectory)
        {
            if (!clientPayloadRecorderConfigure(options.recordDirectory))
            {
                MDR_LOG("Unable to prepare capture folder {}: {}", options.recordDirectory, SDL_GetError());
                break;
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
                break;
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
            break;
        }
#ifdef MDR_CLIENT_DEBUGGER
        clientDebuggerSetWindow(gWindow);
#endif
        gWindowID = SDL_GetWindowID(gWindow);
        gRenderer = SDL_CreateRenderer(gWindow, nullptr);
        if (!gRenderer)
        {
            SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
            break;
        }
        SDL_Log("Using SDL_Renderer: %s", SDL_GetRendererName(gRenderer));
        SDL_SetRenderVSync(gRenderer, 1);
        if (!clientInit(displayScale, options.fontPath))
            break;
        gClientInitialized = true;
        gAppliedDisplayScale = displayScale;
        gPlatformBackendInitialized = ImGui_ImplSDL3_InitForSDLRenderer(gWindow, gRenderer);
        if (!gPlatformBackendInitialized)
        {
            SDL_Log("Error: ImGui_ImplSDL3_InitForSDLRenderer()");
            break;
        }
        gRendererBackendInitialized = ImGui_ImplSDLRenderer3_Init(gRenderer);
        if (!gRendererBackendInitialized)
        {
            SDL_Log("Error: ImGui_ImplSDLRenderer3_Init()");
            break;
        }
#ifndef __EMSCRIPTEN__
        gStopClient = false;
        gClientThread = SDL_CreateThread(RunOnWorkerThread, "client", nullptr);
        if (!gClientThread)
        {
            SDL_Log("Error: SDL_CreateThread(): %s", SDL_GetError());
            break;
        }
        SDL_Log("Client worker delay: 10 ms");
#else
        SDL_Log("Client updates and frames share the main thread (Web)");
#endif
        result = 0;
#ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(
            []
            {
                mainLoop();
                if (gShouldClose)
                {
                    emscripten_cancel_main_loop();
                    Shutdown();
                }
            },
            0, 1);
#else
        while (!gShouldClose)
            mainLoop();
#endif
    }
    while (false);
    Shutdown();
    return result;
}
