// SDL_Renderer backend from https://github.com/ocornut/imgui/blob/master/examples/example_sdl3_sdlrenderer3
#include <cstdio>
#include <cstring>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_main.h>

#include "Platform/Platform.hpp"
#include "PayloadRecorder.hpp"
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
#ifdef MDR_CLIENT_DEBUGGER
extern void clientEnterDebuggerReplayMode();
#endif

bool gShouldClose = false;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

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
            if (clientDebuggerReplayDirectory(event.drop.data, &replayed))
            {
                clientEnterDebuggerReplayMode();
                SDL_Log("Replayed %zu packet(s) from %s", replayed, event.drop.data);
            }
            else
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to replay %s: %s",
                             event.drop.data, SDL_GetError());
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
        // Platform font loading - if available
        // This is only done once per session. See @ref clientPlatformLocateFontBinary for more info.
        static int platformFontSize = 0;
        if (!platformFontSize)
        {
            const char* fontData = nullptr;
            platformFontSize = clientPlatformLocateFontBinary(&fontData);
            if (platformFontSize)
            {
                SDL_Log("Loading platform font of size %d bytes", platformFontSize);
                ImFontConfig merge_config{};
                merge_config.MergeMode = true;
                // XXX: PlexSansIcon covered latin-1 pages. New ones won't overwrite them.
                // External fonts are meant to cover missing glyphs e.g. CJK ones anyway - so this is fine.
                io.Fonts->AddFontFromMemoryTTF((void*)fontData, platformFontSize, 15.0f, &merge_config);
            }
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
        emscripten_cancel_main_loop();
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

    struct ClientOptions
    {
        const char* recordDirectory{};
        const char* replayDirectory{};
        bool showHelp{};
    };

    void PrintUsage(FILE* output)
    {
        std::fprintf(
            output,
            "Usage: SonyHeadphonesClient [-con] [--record <capture-folder>]\n"
            "       SonyHeadphonesClient [-con] [--replay <packet-folder>]\n"
            "\n"
            "-con opens a diagnostic console on Windows.\n"
            "Packet replay requires a client build with the debugger enabled.\n"
        );
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

            const bool record = std::strcmp(argument, "--record") == 0;
            const bool replay = std::strcmp(argument, "--replay") == 0;
            if (record || replay)
            {
                if (index + 1 >= argc)
                {
                    std::fprintf(stderr, "Missing folder after %s.\n", argument);
                    return false;
                }
                const char* directory = argv[++index];
                const char*& destination = record ? options.recordDirectory : options.replayDirectory;
                if (destination)
                {
                    std::fprintf(stderr, "%s may only be specified once.\n", argument);
                    return false;
                }
                destination = directory;
                continue;
            }

            std::fprintf(stderr, "Unknown argument: %s\n", argument);
            return false;
        }

        if (options.recordDirectory && options.replayDirectory)
        {
            std::fprintf(stderr, "--record and --replay cannot be used together.\n");
            return false;
        }
        return true;
    }
}

int main(int argc, char** argv)
{
    ClientOptions options;
    if (!ParseOptions(argc, argv, options))
    {
        PrintUsage(stderr);
        return 2;
    }
    if (options.showHelp)
    {
        PrintUsage(stdout);
        return 0;
    }
#ifndef MDR_CLIENT_DEBUGGER
    if (options.replayDirectory)
    {
        std::fprintf(stderr, "Packet replay is unavailable because this client was built without the debugger.\n");
        return 2;
    }
#endif

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    if (options.recordDirectory)
    {
        if (!clientPayloadRecorderConfigure(options.recordDirectory))
        {
            std::fprintf(
                stderr,
                "Unable to prepare capture folder %s: %s\n",
                options.recordDirectory,
                SDL_GetError()
            );
            SDL_Quit();
            return 1;
        }
        std::fprintf(
            stderr,
            "Recording MDR packets to %s. Existing mdr-packet-*.bin "
            "files were cleared. Captures may contain "
            "device addresses, names, and playback metadata.\n",
            options.recordDirectory
        );
    }
#ifdef MDR_CLIENT_DEBUGGER
    if (options.replayDirectory)
    {
        size_t replayed{};
        if (!clientDebuggerReplayDirectory(options.replayDirectory, &replayed))
        {
            std::fprintf(stderr, "Unable to replay packet folder %s: %s\n",
                         options.replayDirectory, SDL_GetError());
            SDL_Quit();
            return 1;
        }
        clientEnterDebuggerReplayMode();
        std::fprintf(stderr, "Replayed %zu packet(s) from %s in debugger-only mode.\n",
                     replayed, options.replayDirectory);
    }
#endif
    // https://github.com/libsdl-org/SDL/blob/main/docs/README-highdpi.md#numeric-example
    // This should only be effective (!=1.0f) on Windows and X11 platforms
    float displayScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    gWindow = SDL_CreateWindow(
        "SonyHeadphonesClient",
        CLIENT_WINDOW_WIDTH * displayScale, CLIENT_WINDOW_HEIGHT * displayScale,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );
    if (!gWindow)
    {
        SDL_Log("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return 1;
    }
    gRenderer = SDL_CreateRenderer(gWindow, nullptr);
    SDL_SetRenderVSync(gRenderer, 1);
    if (!gRenderer)
    {
        SDL_Log("Error: SDL_CreateRenderer()\n");
        return 1;
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

        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        SDL_Quit();

        clientPlatformDestroy();
    }
    return 0;
}
