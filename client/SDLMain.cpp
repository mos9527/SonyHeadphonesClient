// SDL_Renderer backend from https://github.com/ocornut/imgui/blob/master/examples/example_sdl3_sdlrenderer3
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <mutex>
#ifdef SDL_DIALOG
#include <cctype>
#include <string_view>
#include <mdr/Command.hpp>
#endif
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

const char* clientPlatformGetProcessorArchitecture()
{
#if defined(__aarch64__) || defined(__arm64__) || defined(_M_ARM64)
    return "arm64";
#elif defined(__arm__) || defined(_M_ARM)
    return "arm";
#elif defined(__x86_64__) || defined(_M_X64)
    return "x86_64";
#elif defined(__i386__) || defined(_M_IX86)
    return "x86";
#elif defined(__wasm64__)
    return "wasm64";
#elif defined(__wasm32__) || defined(__EMSCRIPTEN__)
    return "wasm32";
#else
    return "unknown";
#endif
}

bool gShouldClose{false};

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

constexpr int kRedrawFrames = 60;
constexpr int kClientPollIntervalMS = 25; // 40Hz
constexpr int kClientIdleWaitTimeoutMS = 1000;

namespace
{
    bool gClientInitialized{};
    bool gPlatformBackendInitialized{};
    bool gRendererBackendInitialized{};
    bool gAppActive{true};
    float gAppliedDisplayScale{1.0f};
    SDL_WindowID gWindowID{};
    int gRedrawFrames{kRedrawFrames};

#ifndef __EMSCRIPTEN__
    SDL_Thread* gClientThread{};
    std::mutex gWorkerMutex;
    bool gStopClient{};
    bool gClientRedraw{};
#endif

    bool PollEvents(SDL_Event& event, Uint32 minType = SDL_EVENT_FIRST, Uint32 maxType = SDL_EVENT_LAST)
    {
        const int result = SDL_PeepEvents(&event, 1, SDL_GETEVENT, minType, maxType);
        if (result < 0)
        {
            MDR_LOG("SDL_PeepEvents(): {}", SDL_GetError());
            gShouldClose = true;
        }
        return result > 0;
    }

    bool ShouldAppPresent()
    {
        return !gShouldClose && gAppActive && !(SDL_GetWindowFlags(gWindow) & SDL_WINDOW_MINIMIZED);
    }

#ifndef __EMSCRIPTEN__
    // Worker thread polls no faster than kClientPollIntervalMS and handles everything
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
                if (clientOnUpdate() && !gClientRedraw)
                {
                    gClientRedraw = true;
                    SDL_Event wake{};
                    wake.type = SDL_EVENT_USER;
                    SDL_PushEvent(&wake);
                }
            }
            SDL_Delay(kClientPollIntervalMS);
        }
        return 0;
    }
#endif
} // namespace

void mainLoop()
{
#ifndef __EMSCRIPTEN__
    if ((!gRedrawFrames || !ShouldAppPresent()) && !clientPlatformWaitEventTimeout(kClientIdleWaitTimeoutMS))
        gRedrawFrames = std::max(gRedrawFrames, 1);
#endif
    SDL_PumpEvents();
    SDL_Event event;
    bool redraw = false;
    bool present = false;

    auto CheckIsActive = [&redraw](SDL_Event& event)
    {
        redraw = true;
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
        redraw |= gClientRedraw;
        gClientRedraw = false;
#else
        redraw |= clientOnUpdate();
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
                    MDR_LOG("Replayed {} packet(s) from {}", replayed, path.c_str());
                }
                else
                    MDR_LOG("Unable to replay {}: {}", path.c_str(), SDL_GetError());
                continue;
            }
#endif
            ImGui_ImplSDL3_ProcessEvent(&event);
        }
        if (redraw)
            gRedrawFrames = kRedrawFrames;
        present = gRedrawFrames > 0 && ShouldAppPresent();
        if (present)
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
            if (clientOnFrame(gWindow))
                gRedrawFrames = kRedrawFrames;
        }
    }
    if (present)
    {
        const ImGuiIO& io = ImGui::GetIO();
        SDL_SetRenderScale(gRenderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
        SDL_RenderClear(gRenderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), gRenderer);
        SDL_RenderPresent(gRenderer);
        --gRedrawFrames;
    }
}

#define CLIENT_WINDOW_WIDTH 800
#define CLIENT_WINDOW_HEIGHT 600

namespace
{
#ifdef _WIN32
    void OpenConsole()
    {
        if (!AllocConsole())
            return;

        std::freopen("CONOUT$", "w", stdout);
        std::freopen("CONOUT$", "w", stderr);
        std::freopen("CONIN$", "r", stdin);
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

    void ShowUsage()
    {
        mdr::String usage = "Usage: SonyHeadphonesClient\n"
                            "    [-h | --help] Shows this help\n"
                            "    [--record <capture-folder>] Records device packets automatically to folder\n"
                            "    [--locale ";
        usage += i18n::kLocaleOptionString;
        usage += "] Override application locale selected from the system\n"
                 "    [--font <font-file>] Load an external font without changing application locale\n";
#ifdef MDR_CLIENT_DEBUGGER
        usage += "    [--replay <packet-file-or-folder>] Replays devices packets from folder\n";
#endif
        // Windows specific
#ifdef _WIN32
        usage += "    [-con | --con] Opens a separate console for diagnostic logs\n";
#endif
        // Linux specific (DBus)
#ifdef __linux__
        usage += "    [--pause-media-on-remove] Auto-pause system media playback when device "
                 "is removed when unsupported by OS otherwise.\n";
#endif
#ifdef _WIN32
        if (!SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "SonyHeadphonesClient - Help", usage.c_str(), gWindow))
            MDR_LOG("Unable to show help dialog: {}", SDL_GetError());
#else
        std::fputs(usage.c_str(), stderr);
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

            MDR_LOG("Ignoring unknown argument: {}", argument);
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
        clientPlatformDestroy();
        SDL_Quit();
    };

    int result = 1;
    do
    {
        if (!clientPlatformInit())
        {
            MDR_LOG("Unable to initialize client platform: {}", SDL_GetError());
            break;
        }
        Options options;
        if (!ParseOptions(argc, argv, options))
        {
            ShowUsage();
            result = 2;
            break;
        }
        clientSetAppLocale(options.locale);
        clientSetPauseMediaOnRemove(options.pauseMediaOnRemove);
        if (options.showHelp)
        {
            ShowUsage();
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
        const SDL_WindowFlags windowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
        if (!clientPlatformInitGraphics(nullptr))
        {
            MDR_LOG("Error: clientPlatformInitGraphics() before device creation: {}", SDL_GetError());
            break;
        }
        gWindow = SDL_CreateWindow("SonyHeadphonesClient", CLIENT_WINDOW_WIDTH * displayScale,
                                   CLIENT_WINDOW_HEIGHT * displayScale, windowFlags);
        if (!gWindow)
        {
            MDR_LOG("Error: SDL_CreateWindow(): {}", SDL_GetError());
            break;
        }
#ifdef MDR_CLIENT_DEBUGGER
        clientDebuggerSetWindow(gWindow);
#endif
        gWindowID = SDL_GetWindowID(gWindow);
        gRenderer = SDL_CreateRenderer(gWindow, nullptr);
        if (!gRenderer)
        {
            MDR_LOG("Error: SDL_CreateRenderer(): {}", SDL_GetError());
            break;
        }
        MDR_LOG("Using SDL_Renderer: {}", SDL_GetRendererName(gRenderer));
        if (!clientPlatformInitGraphics(gRenderer))
        {
            MDR_LOG("Error: clientPlatformInitGraphics(): {}", SDL_GetError());
            break;
        }
        SDL_SetRenderVSync(gRenderer, 1);
        if (!clientInit(displayScale, options.fontPath))
            break;
        gClientInitialized = true;
        gAppliedDisplayScale = displayScale;
        gPlatformBackendInitialized = ImGui_ImplSDL3_InitForSDLRenderer(gWindow, gRenderer);
        if (!gPlatformBackendInitialized)
        {
            MDR_LOG("Error: ImGui_ImplSDL3_InitForSDLRenderer()");
            break;
        }
        gRendererBackendInitialized = ImGui_ImplSDLRenderer3_Init(gRenderer);
        if (!gRendererBackendInitialized)
        {
            MDR_LOG("Error: ImGui_ImplSDLRenderer3_Init()");
            break;
        }
#ifndef __EMSCRIPTEN__
        gStopClient = false;
        gClientThread = SDL_CreateThread(RunOnWorkerThread, "client", nullptr);
        if (!gClientThread)
        {
            MDR_LOG("Error: SDL_CreateThread(): {}", SDL_GetError());
            break;
        }        
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

// Implements file saving for Desktop platforms
#ifdef SDL_DIALOG
namespace
{
    struct SaveRequest
    {
        mdr::MDRBuffer data;
        mdr::String filename;
        mdr::String extension;
        SDL_DialogFileFilter filter{};
        SDL_Window* window{};
        ClientPlatformSaveBlobCallback callback{};
        void* userdata{};
    };

    bool WriteBlobFile(const char* path, const mdr::MDRBuffer& data)
    {
        SDL_IOStream* output = SDL_IOFromFile(path, "wb");
        if (!output)
            return false;
        const size_t written = SDL_WriteIO(output, data.data(), data.size());
        const mdr::String writeError = written != data.size() ? SDL_GetError() : "";
        const bool closed = SDL_CloseIO(output);
        if (written != data.size())
            return SDL_SetError("%s", writeError.empty() ? "Incomplete file write" : writeError.c_str());
        return closed;
    }

    mdr::String SavePathWithExtension(const char* path, const mdr::String& extension)
    {
        mdr::String result = path;
        const std::string_view value = result;
        if (value.size() < extension.size() ||
            !std::equal(extension.begin(), extension.end(), value.end() - extension.size(),
                        [](char lhs, char rhs)
                        {
                            return std::tolower(static_cast<unsigned char>(lhs)) ==
                                std::tolower(static_cast<unsigned char>(rhs));
                        }))
            result += extension;
        return result;
    }

    void CompleteSave(SaveRequest* request, ClientPlatformSaveBlobResult result,
                      const char* path, const char* error = nullptr)
    {
        request->callback(request->userdata, result, path, error);
        mdr::Destruct(request);
    }

    void SDLCALL SaveDialogCallback(void* userdata, const char* const* filelist, int)
    {
        auto* request = static_cast<SaveRequest*>(userdata);
        if (!filelist)
        {
            const mdr::String dialogError = SDL_GetError();
            if (WriteBlobFile(request->filename.c_str(), request->data))
                CompleteSave(request, CLIENT_PLATFORM_SAVE_BLOB_FALLBACK_SAVED,
                             request->filename.c_str(), dialogError.c_str());
            else
                CompleteSave(request, CLIENT_PLATFORM_SAVE_BLOB_FAILED, request->filename.c_str(), SDL_GetError());
        }
        else if (!filelist[0])
            CompleteSave(request, CLIENT_PLATFORM_SAVE_BLOB_CANCELLED, request->filename.c_str());
        else
        {
            const mdr::String path = SavePathWithExtension(filelist[0], request->extension);
            const bool saved = WriteBlobFile(path.c_str(), request->data);
            CompleteSave(request, saved ? CLIENT_PLATFORM_SAVE_BLOB_SAVED : CLIENT_PLATFORM_SAVE_BLOB_FAILED,
                         path.c_str(), saved ? nullptr : SDL_GetError());
        }
    }

    void SDLCALL ShowSaveDialog(void* userdata)
    {
        auto* request = static_cast<SaveRequest*>(userdata);
        SDL_ShowSaveFileDialog(SaveDialogCallback, request, request->window,
                               &request->filter, 1, request->filename.c_str());
    }
}

int clientSDLSaveBlob(
    const char* filename, const unsigned char* data, size_t dataSize, const char* mimeType,
    SDL_Window* window, ClientPlatformSaveBlobCallback callback, void* userdata)
{
    if (!filename || !*filename || !data || !dataSize || !mimeType || !*mimeType || !window || !callback)
    {
        SDL_SetError("Invalid blob save arguments");
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    }
    auto* request = mdr::Construct<SaveRequest>();
    request->data.assign(data, data + dataSize);
    request->filename = filename;
    request->window = window;
    request->callback = callback;
    request->userdata = userdata;
    const size_t separator = request->filename.find_last_of("/\\");
    const size_t dot = request->filename.find_last_of('.');
    if (dot != mdr::String::npos && (separator == mdr::String::npos || dot > separator) &&
        dot + 1 < request->filename.size())
        request->extension = request->filename.substr(dot);
    request->filter = {request->filename.c_str(), request->extension.empty() ? "*" : request->extension.c_str() + 1};
    if (!SDL_RunOnMainThread(ShowSaveDialog, request, true))
    {
        mdr::Destruct(request);
        return MDR_RESULT_ERROR_GENERAL;
    }
    return MDR_RESULT_OK;
}
#endif
