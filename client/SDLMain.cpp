// SDL_Renderer backend from https://github.com/ocornut/imgui/blob/master/examples/example_sdl2_sdlrenderer2
#include <stdio.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
extern bool ShouldClientExit();

bool gShouldClose = false;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

void mainLoop()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            gShouldClose = true;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID ==
            SDL_GetWindowID(gWindow))
            gShouldClose = true;
    }
    if (SDL_GetWindowFlags(gWindow) & SDL_WINDOW_MINIMIZED)
    {
        SDL_Delay(10);
        return;
    }
    // Start the Dear ImGui frame
    {
        // New frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }
    gShouldClose |= ShouldClientExit();
    // Rendering
    {
        ImGui::Render();
        ImGuiIO& io = ImGui::GetIO();
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
        SDL_RenderClear(gRenderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), gRenderer);
        SDL_RenderPresent(gRenderer);
    }
#ifdef EMSCRIPTEN
    if (gShouldClose)
        emscripten_cancel_main_loop();
#endif
}

extern "C" int main(int, char**)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
    gWindow = SDL_CreateWindow(
        "SonyHeadphonesClient",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
        );
    if (!gWindow)
    {
        SDL_Log("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return 1;
    }
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (!gRenderer)
    {
        SDL_Log("Error: SDL_CreateRenderer()\n");
        return 1;
    }
    SDL_RendererInfo info;
    SDL_GetRendererInfo(gRenderer, &info);
    SDL_Log("Using SDL_Renderer: %s", info.name);
    // Setup Dear ImGui context
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
    }
    ImGuiIO& io = ImGui::GetIO();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    {
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
        ImGui_ImplSDL2_InitForSDLRenderer(gWindow, gRenderer);
        ImGui_ImplSDLRenderer2_Init(gRenderer);
    }
    // Main loop

#ifdef EMSCRIPTEN
    emscripten_set_main_loop(mainLoop, -1, 1);
#else
    while (!gShouldClose)
        mainLoop();
#endif

    // Cleanup
    {
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
    }
    return 0;
}
