#include <memory>
#include <filesystem>
#include <string>
#include <algorithm>
#include <stdio.h>

#include "App.h"
#include "Constants.h"
#include "fonts/CascadiaCode.cpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"

#if defined(_WIN32)
#define NOMINMAX
#include "platform/windows/WindowsBluetoothConnector.h"
#include "backends/imgui_impl_opengl3.h"
#elif defined(__linux__)
#include "platform/linux/LinuxBluetoothConnector.h"
#include "backends/imgui_impl_opengl3.h"
#elif defined(__APPLE__)
#include "platform/macos/MacOSBluetoothConnector.h"
#include "backends/imgui_impl_metal.h"
#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>
#endif

#include <GLFW/glfw3.h>
#if defined(__APPLE__)
#include <GLFW/glfw3native.h>
#endif


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void EnterGUIMainLoop(std::unique_ptr<IBluetoothConnector> btConnector)
{
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);

    printf("Glfw version: %s\n", glfwGetVersionString());

#if defined(__linux__)
    if (glfwPlatformSupported(GLFW_PLATFORM_WAYLAND))
    {
        printf("Enabling wayland backend\n");
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    }
#endif

    if (!glfwInit())
    {
        printf("Glfw init error. Quitting.");
        return;
    }

#if defined(__APPLE__)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#else
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif
    glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(GUI_DEFAULT_WIDTH, GUI_DEFAULT_HEIGHT, APP_NAME, NULL, NULL);
    if (window == NULL)
        return;

    // init GUI   
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
#if defined(__APPLE__)
    ImGui_ImplGlfw_InitForMetal(window, true);
    ImGui_ImplMetal_Init(device);
#else
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
#endif


    // --- Cross-platform config path handling ---
    std::filesystem::path config_path;
    const char* config_path_env = getenv("SONYHEADPHONESCLIENT_CONFIG_PATH");
    if (config_path_env) {
        config_path = config_path_env;
    }

    if (config_path.empty() || !std::filesystem::exists(config_path)) {
#ifdef _WIN32
        const char* user_profile = getenv("USERPROFILE");
        if (user_profile) {
            config_path = user_profile;
        }
#elif defined(__APPLE__)
        const char* home_dir = getenv("HOME");
        if (home_dir) {
            config_path = std::string(home_dir) + "/Library/Preferences";
        }
#else // Linux
        const char* home_dir = getenv("HOME");
        if (home_dir) {
            config_path = home_dir;
        }
#endif
        if (!config_path.empty()) {
            config_path /= APP_CONFIG_NAME;
        }
    }

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    {
        printf("config path: %s\n", config_path.string().c_str());
        AppConfig app_config;
        app_config.load(config_path.string());
        App app = App(BluetoothWrapper(std::move(btConnector)), app_config);

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            static float sCurrentScale = 1.0f;
            float scaleX, scaleY, scale;
            glfwGetWindowContentScale(window, &scaleX, &scaleY);
            scale = std::max(scaleX, scaleY);
            static bool sNeedRebuildFonts = false;
            if (sCurrentScale != scale)
            {
                sCurrentScale = scale;
                sNeedRebuildFonts = true;
            }
            // Monitor font change
            static std::string sFontFile;
            if (sFontFile != app_config.imguiFontFile && std::filesystem::exists(sFontFile))
            {
                sFontFile = app_config.imguiFontFile;
                sNeedRebuildFonts = true;
            }

            static float sSetFontSize = DEFAULT_FONT_SIZE;
            if (sSetFontSize != app_config.imguiFontSize)
            {
                sSetFontSize = app_config.imguiFontSize;
                sNeedRebuildFonts = true;
            }

            // Rebuild fonts if necessary
            if (sNeedRebuildFonts && !io.MouseDown[0])
            {
                io.Fonts->Clear();
                ImFontConfig font_cfg;
                font_cfg.FontDataOwnedByAtlas = false;
                if (!sFontFile.empty() && std::filesystem::exists(sFontFile))
                {
                    io.Fonts->AddFontFromFileTTF(sFontFile.c_str(), sSetFontSize * sCurrentScale, &font_cfg);
                }
                else
                {
                    io.Fonts->AddFontFromMemoryCompressedBase85TTF(CascadiaCode_compressed_data_base85, sSetFontSize * sCurrentScale, &font_cfg);
                }
                io.Fonts->Build();
#if !defined(__APPLE__)
                ImGui_ImplOpenGL3_DestroyDeviceObjects();
#endif
                sNeedRebuildFonts = false;
            }

            bool shouldDraw = app.OnUpdate();

#if defined(__APPLE__)
            
#else
            ImGui_ImplOpenGL3_NewFrame();
#endif
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            app.OnFrame();

            ImGui::Render();
#if defined(__APPLE__)
            
#else
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

            glfwSwapBuffers(window);
        }

        app_config.save(config_path.string());
    }

    // Cleanup
#if defined(__APPLE__)
    ImGui_ImplMetal_Shutdown();
#else
    ImGui_ImplOpenGL3_Shutdown();
#endif
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

#if defined(_WIN32)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    try
    {
        EnterGUIMainLoop(std::make_unique<WindowsBluetoothConnector>());
    }
    catch (const std::exception& e)
    {
        MessageBoxA(NULL, e.what(), "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    return 0;
}
#else
int main(int argc, char** argv)
{
    try
    {
#if defined(__linux__)
        EnterGUIMainLoop(std::make_unique<LinuxBluetoothConnector>());
#elif defined(__APPLE__)
        EnterGUIMainLoop(std::make_unique<MacOSBluetoothConnector>());
#else
        #error "Unsupported platform"
#endif
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "Error: %s\n", e.what());
        return 1;
    }
    return 0;
}
#endif