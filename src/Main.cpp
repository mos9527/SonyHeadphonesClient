#include <memory>
#include <filesystem>
#include <string>
#include <algorithm>
#include <stdio.h>

#include "App.h"
#include "Constants.h"
#include "fonts/CascadiaCode.cpp"

#ifdef _WIN32
#define NOMINMAX
#include "platform/windows/WindowsBluetoothConnector.h"
#include "backends/imgui_impl_opengl3.h"
#endif
#ifdef __linux__
#include "platform/linux/LinuxBluetoothConnector.h"
#include "backends/imgui_impl_opengl3.h"
#endif
#ifdef __APPLE__
#include "platform/macos/MacOSBluetoothConnector.h"
#include "backends/imgui_impl_metal.h"
#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>
#import <objc/runtime.h>
#endif

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

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

    fprintf(stderr,"Glfw version: %s\n", glfwGetVersionString());

#if defined(__linux__)
    if (glfwPlatformSupported(GLFW_PLATFORM_WAYLAND))
    {
        fprintf(stderr,"Enabling wayland backend\n");
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    }
#endif

    if (!glfwInit())
    {
        printf("Glfw init error. Quitting.");
        return;
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
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

    // Init GUI
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
#ifdef __APPLE__
    id <MTLDevice> device = MTLCreateSystemDefaultDevice();
    id <MTLCommandQueue> commandQueue = [device newCommandQueue];

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplMetal_Init(device);

    NSWindow *nswin = glfwGetCocoaWindow(window);
    CAMetalLayer *layer = [CAMetalLayer layer];
    layer.device = device;
    layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    nswin.contentView.layer = layer;
    nswin.contentView.wantsLayer = YES;

    MTLRenderPassDescriptor *renderPassDescriptor = [MTLRenderPassDescriptor new];
#else
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
#endif
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
#endif
#ifdef __APPLE__
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
#ifdef __APPLE__
    @autoreleasepool {
#else
    {
#endif
        fprintf(stderr,"config path: %s\n", config_path.string().c_str());
        AppConfig app_config;
        app_config.load(config_path.string());
        try
        {
            App app = App(BluetoothWrapper(std::move(btConnector)), app_config);
            while (!glfwWindowShouldClose(window))
            {
                glfwPollEvents();
                static float sCurrentScale = 1.0f;
                float scaleX, scaleY, scale = 1.0;
#ifndef __APPLE__
                // XXX: on macOS, Window content is already scaled.
                // On other platforms, we need to query the scale factor and apply them there.
                glfwGetWindowContentScale(window, &scaleX, &scaleY);
                scale = std::max(scaleX, scaleY);
#endif
                static bool sNeedRebuildFonts = true;
                if (sCurrentScale != scale)
                {
                    ImGui::GetStyle().ScaleAllSizes(scale / (float)sCurrentScale);
                    sCurrentScale = scale;
                    sNeedRebuildFonts = true;
                }
                // Monitor font change
                static std::string sFontFile;
                if (sFontFile != app_config.imguiFontFile && std::filesystem::is_regular_file(app_config.imguiFontFile))
                {
                    sFontFile = app_config.imguiFontFile;
                    sNeedRebuildFonts = true;
                }
                static float sSetFontSize = DEFAULT_FONT_SIZE;
                if (sSetFontSize != app_config.imguiFontSize)
                {
                    // Dynamic font scaling has been implemented since https://github.com/ocornut/imgui/releases/tag/v1.92.0
                    sSetFontSize = app_config.imguiFontSize * sCurrentScale;
                    ImGui::GetStyle()._NextFrameFontSizeBase = sSetFontSize;
                    // sNeedRebuildFonts = true;
                }
                // Rebuild fonts if necessary
                if (sNeedRebuildFonts)
                {
                    io.Fonts->Clear();
                    ImFontConfig font_cfg;
                    font_cfg.FontDataOwnedByAtlas = false;
                    if (!sFontFile.empty() && std::filesystem::is_regular_file(sFontFile))
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
                    ImGui_ImplOpenGL3_CreateDeviceObjects();
#else
                    ImGui_ImplMetal_DestroyDeviceObjects();
                    ImGui_ImplMetal_CreateDeviceObjects(device);
#endif
                    sNeedRebuildFonts = false;
                }

                bool shouldDraw = app.OnUpdate();

#if defined(__APPLE__)
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);
                layer.drawableSize = CGSizeMake(width, height);
                id <CAMetalDrawable> drawable = [layer nextDrawable];
                id <MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
                renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 0);
                renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
                renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
                renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
                id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];

                ImGui_ImplMetal_NewFrame(renderPassDescriptor);
#else
                ImGui_ImplOpenGL3_NewFrame();
#endif
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
                app.OnFrame();
                ImGui::Render();
#if defined(__APPLE__)
                ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);
                [renderEncoder endEncoding];
                [commandBuffer presentDrawable:drawable];
                [commandBuffer commit];
#else
                int display_w, display_h;
                glfwGetFramebufferSize(window, &display_w, &display_h);
                glViewport(0, 0, display_w, display_h);
                glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
                glClear(GL_COLOR_BUFFER_BIT);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                glfwSwapBuffers(window);
#endif
            }
        } catch (std::exception &e)
        {
            throw e;
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