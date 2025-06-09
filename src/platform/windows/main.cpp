#include "platform/windows/WindowsBluetoothConnector.h"
#include "App.h"

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <dwmapi.h>
#include <windowsx.h>

UINT const WMAPP_NOTIFYCALLBACK = WM_APP + 1;
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static bool g_SystrayEnabled = false;
namespace WindowsGUIInternal
{

    void CreateRenderTarget()
    {
        ID3D11Texture2D* pBackBuffer = nullptr;
        g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (pBackBuffer == nullptr)
        {
            throw std::runtime_error("Unexpected: pBackBuffer is null");
        }
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
        pBackBuffer->Release();
    }

    void CleanupRenderTarget()
    {
        if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
    }

    bool CreateDeviceD3D(HWND hWnd)
    {
        // Setup swap chain
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 2;
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT createDeviceFlags = 0;
        //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
        if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
            return false;

        CreateRenderTarget();
        return true;
    }

    void CleanupDeviceD3D()
    {
        CleanupRenderTarget();
        if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
        if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
        if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    }

    // Win32 message handler
    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        static bool windowShown = true;
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
            case WMAPP_NOTIFYCALLBACK:
            {
                switch (LOWORD(lParam))
                {
                case NIN_BALLOONUSERCLICK:
                case WM_LBUTTONDOWN:
                case WM_RBUTTONDOWN:
                {
                    windowShown = !windowShown;
                    ShowWindow(hWnd, windowShown ? SW_SHOWNORMAL : SW_HIDE);
                    if (windowShown) SetForegroundWindow(hWnd);
                    break;
                }
                }
            }
            case WM_SIZE:
                if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
                    {
                        CleanupRenderTarget();
                        g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                        CreateRenderTarget();                    
                    }
                else if (wParam == SIZE_MINIMIZED && g_SystrayEnabled) {
					ShowWindow(hWnd, SW_HIDE);
					return 0;
                }
            case WM_SYSCOMMAND:
                if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
                break;
            case WM_QUIT:
            case WM_CLOSE:
                PostQuitMessage(0);
                break;
            break;
        }
        return ::DefWindowProc(hWnd, msg, wParam, lParam);
    }
}
void EnterGUIMainLoop(BluetoothWrapper bt)
{
    // Create application window
    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW), CS_CLASSDC, WindowsGUIInternal::WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, APP_NAME_W, NULL };
    ::RegisterClassExW(&wc);
    //TODO: pass window data (size, name, etc) as params and autoscale
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, APP_NAME_W, WS_OVERLAPPEDWINDOW | WS_CAPTION, CW_USEDEFAULT, CW_USEDEFAULT, GUI_WIDTH, GUI_HEIGHT, NULL, NULL, wc.hInstance, NULL);


    // Snippets from https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/Win7Samples/winui/shell/appshellintegration/NotificationIcon
    // Setup the SysTray icon
    // Declare NOTIFYICONDATA details. 
    // Error handling is omitted here for brevity. Do not omit it in your code.
    NOTIFYICONDATA nid = {
		.cbSize = sizeof(nid),
        .hWnd = hwnd,
        .uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_GUID | NIF_INFO,
        .uCallbackMessage = WMAPP_NOTIFYCALLBACK,
		.szTip = APP_NAME_W,
        .szInfo = APP_NAME_W,
		.szInfoTitle = L"Running in System Tray",
        .dwInfoFlags = NIIF_INFO | NIIF_RESPECT_QUIET_TIME,
        .guidItem = { 0x9bd2c97f, 0x083c, 0x428a, {0xb4, 0x66, 0xb9, 0x9c, 0xb3, 0x64, 0x12, 0x27 } }
    };
    LoadIconMetric(NULL, MAKEINTRESOURCE(IDI_WINLOGO), LIM_SMALL, &(nid.hIcon));
    if (FAILED(Shell_NotifyIcon(NIM_ADD, &nid) ? S_OK : E_FAIL))
    {
        // throw std::runtime_error("Failed to create SysTray icon");
        // see https://github.com/mos9527/SonyHeadphonesClient/issues/11 
        g_SystrayEnabled = false;
        MessageBoxA(hwnd, "Failed to create System Tray icon\nYou can still bring up the app from the Taskbar", "Warning", MB_OK | MB_ICONWARNING);
    }
    else
        g_SystrayEnabled = true;
     

    // Initialize Direct3D
    if (!WindowsGUIInternal::CreateDeviceD3D(hwnd))
    {
        WindowsGUIInternal::CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        throw std::runtime_error("Failed to create D3D device");
    }

    // Hide the window by default
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    UINT presentFlags = 0;

    // Main loop
    MSG msg = { 0 };
    {
        App app(std::move(bt));        
        while (msg.message != WM_QUIT)
        {
            // Poll and handle messages (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.

            if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                continue;
            }

            // Start the Dear ImGui frame
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();

            ImGui::NewFrame();
            // Our GUI routine
            app.OnImGui();
            ImGui::EndFrame();

            ImGui::Render();
            g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
            g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&WINDOW_COLOR);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            //We need this because Present doesn't delay when the app is minimized.
            if (g_pSwapChain->Present(1, presentFlags) == DXGI_STATUS_OCCLUDED) {
                presentFlags = DXGI_PRESENT_TEST;
                //Artificial VSYNC when the app is minimized
                Sleep(BACKGROUND_UPDATE_INTERVAL);
            }
            else {
                presentFlags = 0;
            }
        }
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    WindowsGUIInternal::CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    SetProcessDPIAware();    
#ifdef _DEBUG
    AllocConsole(); 
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
#endif // _DEBUG

    try
    {
        EnterGUIMainLoop(BluetoothWrapper(std::make_unique<WindowsBluetoothConnector>()));
    }
    catch (const std::exception& e)
    {
		MessageBoxA(NULL, e.what(), "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    return 0;
}