// SDL_Renderer backend from https://github.com/ocornut/imgui/blob/master/examples/example_sdl3_sdlrenderer3
#include <cstdio>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_main.h>

#include "Platform/Platform.hpp"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Fonts/PlexSansIcon.h"
// Implemented by Client.cpp
extern bool clientShouldExit();

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
#ifdef EMSCRIPTEN
    if (gShouldClose)
        emscripten_cancel_main_loop();
#endif
}

int main(int, char**)
{
    clientPlatformInit();
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    gWindow = SDL_CreateWindow(
        "Sony Headphones Client",
        1100, 720,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );
    if (!gWindow)
    {
        SDL_Log("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return 1;
    }
    gRenderer = SDL_CreateRenderer(gWindow, nullptr);
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
    SDL_SetWindowMinimumSize(gWindow, 900, 600);
    // Setup Default Dear ImGui styles
    ImGui::StyleColorsLight();
    auto& style = ImGui::GetStyle();
    style.WindowRounding = 12.0f;
    style.ChildRounding = 12.0f;
    style.PopupRounding = 12.0f;
    style.FrameRounding = 10.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding = 10.0f;
    style.TabRounding = 10.0f;
    style.WindowPadding = ImVec2(20.0f, 18.0f);
    style.FramePadding = ImVec2(12.0f, 8.0f);
    style.ItemSpacing = ImVec2(12.0f, 10.0f);
    style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
    style.ScrollbarSize = 12.0f;
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.TabBorderSize = 0.0f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.CircleTessellationMaxError = 0.01f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.95f, 0.96f, 0.97f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.85f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.86f, 0.87f, 0.88f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.92f, 0.93f, 0.94f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.86f, 0.88f, 0.92f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.80f, 0.85f, 0.92f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.92f, 0.93f, 0.95f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.92f, 0.93f, 0.95f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.75f, 0.84f, 0.98f, 0.65f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.65f, 0.80f, 0.98f, 0.85f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.52f, 0.74f, 0.98f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.22f, 0.53f, 0.95f, 0.9f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.18f, 0.49f, 0.90f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.14f, 0.44f, 0.85f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.25f, 0.55f, 0.95f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.20f, 0.50f, 0.90f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.20f, 0.50f, 0.90f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.88f, 0.89f, 0.92f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.72f, 0.82f, 0.98f, 0.85f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.72f, 0.82f, 0.98f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.85f, 0.86f, 0.88f, 1.0f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.65f, 0.75f, 0.92f, 1.0f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.55f, 0.70f, 0.90f, 1.0f);
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
        io.Fonts->AddFontFromMemoryCompressedBase85TTF(kEmbedFontPlexSansIcon, 16.0f);
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
