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
#include "platform/windows/ImmersiveColor.h"
#include "backends/imgui_impl_opengl3.h"
#include <dwmapi.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "uxtheme.lib")
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

#ifdef _WIN32

RTL_OSVERSIONINFOW g_windowsVersionInfo;

typedef LONG NTSTATUS, *PNTSTATUS;
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS (0x00000000)
#endif

typedef NTSTATUS (WINAPI *RtlGetVersion_t)(PRTL_OSVERSIONINFOW);

// https://stackoverflow.com/questions/36543301/detecting-windows-10-version/36543774#36543774
BOOL GetOSVersion(PRTL_OSVERSIONINFOW lpRovi)
{
    HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
    if (hMod)
    {
        RtlGetVersion_t fxPtr = (RtlGetVersion_t)GetProcAddress(hMod, "RtlGetVersion");
        if (fxPtr)
        {
            lpRovi->dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
            if (STATUS_SUCCESS == fxPtr(lpRovi))
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

typedef bool (WINAPI *ShouldAppsUseDarkMode_t)(); // 132
bool ShouldAppsUseDarkMode()
{
    static ShouldAppsUseDarkMode_t fn = (ShouldAppsUseDarkMode_t)-1;
    if (fn == (ShouldAppsUseDarkMode_t)-1)
    {
        HMODULE h = LoadLibraryW(L"uxtheme.dll");
        if (h)
        {
            fn = (ShouldAppsUseDarkMode_t)GetProcAddress(h, MAKEINTRESOURCEA(132));
        }
    }
    return fn ? fn() : false /* Pre-1809 behavior */;
}

bool g_micaSupported;

void CheckMicaSupported()
{
    g_micaSupported = IsCompositionActive() && g_windowsVersionInfo.dwBuildNumber >= 22523;
}

static ImVec4 HueSatShift(const ImVec4& col, float deltaH, float deltaS)
{
    float h, s, v;
    ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, h, s, v);
    h = fmodf(h + deltaH, 1.0f);
    s = std::clamp(s + deltaS, 0.0f, 1.0f);
    float r, g, b;
    ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b);
    return ImVec4(r, g, b, col.w);
}

static void ApplyAccentColorsToImGuiStyle(bool dark)
{
    // Accent colors are only supported on Windows 8+
    if (g_windowsVersionInfo.dwBuildNumber < 9200)
        return;

    ImVec4 accentColor = ImColor(CImmersiveColor::GetColor(IMCLR_SystemAccent));

    // Convert accent color to hue for reference
    float accentH, accentS, accentV;
    ImGui::ColorConvertRGBtoHSV(accentColor.x, accentColor.y, accentColor.z, accentH, accentS, accentV);

    ImGuiStyle& style = ImGui::GetStyle();

    // Get base color hue
    float baseH, baseS, baseV;
    ImGui::ColorConvertRGBtoHSV(
        style.Colors[ImGuiCol_TextLink].x,
        style.Colors[ImGuiCol_TextLink].y,
        style.Colors[ImGuiCol_TextLink].z,
        baseH, baseS, baseV);

    // Compute hue delta to match system accent hue
    float deltaH = accentH - baseH;
    if (deltaH > 0.5f) deltaH -= 1.0f;
    else if (deltaH < -0.5f) deltaH += 1.0f;

    float deltaS = accentS - baseS;

    // Apply hue shift to relevant colors
    if (dark)
    {
        static constexpr ImGuiCol kColorsToShiftDark[] = {
            ImGuiCol_FrameBg,
            ImGuiCol_FrameBgHovered,
            ImGuiCol_FrameBgActive,
            ImGuiCol_TitleBgActive,
            ImGuiCol_CheckMark,
            ImGuiCol_SliderGrab,
            ImGuiCol_SliderGrabActive,
            ImGuiCol_Button,
            ImGuiCol_ButtonHovered,
            ImGuiCol_ButtonActive,
            ImGuiCol_Header,
            ImGuiCol_HeaderHovered,
            ImGuiCol_HeaderActive,
            ImGuiCol_SeparatorHovered,
            ImGuiCol_SeparatorActive,
            ImGuiCol_ResizeGrip,
            ImGuiCol_ResizeGripHovered,
            ImGuiCol_ResizeGripActive,
            ImGuiCol_TabHovered,
            ImGuiCol_Tab,
            ImGuiCol_TabSelected,
            ImGuiCol_TabSelectedOverline,
            ImGuiCol_TabDimmed,
            ImGuiCol_TabDimmedSelected,
            ImGuiCol_TextLink,
        };
        for (auto col : kColorsToShiftDark)
        {
            style.Colors[col] = HueSatShift(style.Colors[col], deltaH, deltaS);
        }
    }
    else
    {
        static constexpr ImGuiCol kColorsToShiftLight[] = {
            ImGuiCol_FrameBgHovered,
            ImGuiCol_FrameBgActive,
            ImGuiCol_TitleBgActive,
            ImGuiCol_CheckMark,
            ImGuiCol_SliderGrab,
            ImGuiCol_SliderGrabActive,
            ImGuiCol_Button,
            ImGuiCol_ButtonHovered,
            ImGuiCol_ButtonActive,
            ImGuiCol_Header,
            ImGuiCol_HeaderHovered,
            ImGuiCol_HeaderActive,
            ImGuiCol_SeparatorHovered,
            ImGuiCol_SeparatorActive,
            ImGuiCol_ResizeGrip,
            ImGuiCol_ResizeGripHovered,
            ImGuiCol_ResizeGripActive,
            ImGuiCol_TabHovered,
            ImGuiCol_Tab,
            ImGuiCol_TabSelected,
            ImGuiCol_TabSelectedOverline,
            ImGuiCol_TabDimmed,
            ImGuiCol_TabDimmedSelected,
            ImGuiCol_TextLink,
        };
        for (auto col : kColorsToShiftLight)
        {
            style.Colors[col] = HueSatShift(style.Colors[col], deltaH, deltaS);
        }
    }
}

void UpdateWindowDwmAttributes(HWND hwnd)
{
    BOOL dark = ShouldAppsUseDarkMode();

    if (IsCompositionActive())
    {
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

        if (g_micaSupported)
        {
            DWM_SYSTEMBACKDROP_TYPE backdrop = DWMSBT_MAINWINDOW;
            DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));
        }
    }

    if (dark)
    {
        ImGui::StyleColorsDark();
    }
    else
    {
        ImGui::StyleColorsLight();
    }
    ApplyAccentColorsToImGuiStyle(dark);
}

WNDPROC g_pfnOldWndProc;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SETTINGCHANGE:
        {
            if (lParam && wcscmp((LPCWSTR)lParam, L"ImmersiveColorSet") == 0)
            {
                UpdateWindowDwmAttributes(hwnd);
            }
            break;
        }
    case WM_DWMCOMPOSITIONCHANGED:
        {
            CheckMicaSupported();
            break;
        }
    }

    return CallWindowProcW(g_pfnOldWndProc, hwnd, uMsg, wParam, lParam);
}

#endif

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
#ifdef _WIN32
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
#endif
    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(GUI_DEFAULT_WIDTH, GUI_DEFAULT_HEIGHT, APP_NAME, NULL, NULL);
    if (window == NULL)
        return;

    // Init GUI
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

#ifdef _WIN32
    HWND hwnd = glfwGetWin32Window(window);
    g_pfnOldWndProc = (WNDPROC)SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)MainWndProc);
    CheckMicaSupported();
    UpdateWindowDwmAttributes(hwnd);
#else
    ImGui::StyleColorsDark();
#endif

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
#ifdef _WIN32
                if (app_config.mica && g_micaSupported)
                {
                    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                }
                else
#endif
                {
                    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
                }
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
        GetOSVersion(&g_windowsVersionInfo);
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