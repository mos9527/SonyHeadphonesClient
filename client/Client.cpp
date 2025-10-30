#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_sdl2.h>

#include <mdr/Headphones.hpp>
#include "Platform/Platform.hpp"

#include <SDL2/SDL.h>
using namespace mdr;

mdr::MDRHeadphones gDevice;
String gBugcheckMessage;

#pragma region ImGui Extra
constexpr ImGuiWindowFlags kImWindowFlagsTopMost = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoTitleBar;

// Only useful if you're manipulating the DrawList which has positions
// that are _NOT_ window local
Tuple<ImVec2, ImVec2, ImDrawList*> ImWindowDrawOffsetRegionList()
{
    ImVec2 offset = ImGui::GetCursorScreenPos();
    ImVec2 region = ImGui::GetContentRegionAvail();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    return {offset, region, drawList};
}

// Centered text.
void ImTextCentered(const char* text)
{
    ImVec2 size = ImGui::CalcTextSize(text);
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2 - size.x / 2);
    ImGui::Text("%s", text);
}

// Generate linear, monotonous ints of [0, count - 1] at interval of intervalMS
int ImBlink(int intervalMS, int count)
{
    size_t time = ImGui::GetTime() * 1000;
    time = time % (intervalMS * count);
    return time / intervalMS;
}

// Generate linear, monotonous float in range of [0, 1] at interval of intervalMS
float ImBlinkF(float intervalMS)
{
    float time = ImGui::GetTime();
    intervalMS /= 1000.0f;
    time = fmod(time, intervalMS);
    return time / intervalMS;
}

// CSS linear easing function on x of range [0,1]
constexpr float ImEaseLinear(float x)
{
    return x;
}

// CSS easeInOutCubic easing function on x of range [0,1]
constexpr float ImEaseInOutCubic(float x)
{
    return x < 0.5f ? 4 * pow(x, 3.0f) : 1.0f - pow(-2.0f * x + 2.0f, 3.0f) / 2.0f;
}

// Your next favourite spinner
void ImSpinner(float interval, float size, int color, float thickness = 1.0f, bool center = false,
               float (*easing)(float) = ImEaseLinear)
{
    constexpr ImVec2 kPoints[] = {{-1, 1}, {-1, -1}, {1, -1}, {1, 1}};
    ImVec2 points[std::size(kPoints)];
    if (center)
        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2 - size * sqrt(2) / 2);
    auto [offset, region, draw] = ImWindowDrawOffsetRegionList();
    float t = ImBlinkF(interval), theta = easing(t) * acos(-1);
    for (int i = 0; auto p : kPoints)
    {
        auto& pp = points[i++] = {
            p.x * cos(theta) - p.y * sin(theta),
            p.x * sin(theta) + p.y * cos(theta),
        };
        pp *= size, pp += offset, pp.x += size, pp.y += size;
    }
    draw->AddPolyline(points, std::size(kPoints), color, ImDrawFlags_Closed, thickness);
    ImGui::Dummy({sqrt(2.0f) * size, sqrt(2.0f) * size});
}
#pragma endregion

#pragma region States
enum
{
    APP_STATE_RUNNING,
    APP_STATE_BUGCHECK
} appState{APP_STATE_RUNNING};

enum
{
    CONN_STATE_NO_CONNECTION,
    CONN_STATE_CONNECTING,
    CONN_STATE_CONNECTED,
    CONN_STATE_DISCONNECTED
} connState{CONN_STATE_NO_CONNECTION};
#pragma endregion

void DrawDeviceDiscovery()
{
    MDR_ASSERT(connState == CONN_STATE_NO_CONNECTION);
    MDRConnection* conn = clientPlatformConnectionGet();
    MDR_ASSERT(conn != nullptr);
    if (ImGui::CollapsingHeader("Device Discovery", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static MDRDeviceInfo* pDeviceInfo = nullptr;
        static int nDeviceInfo = 0;
        ImGui::SeparatorText("Select from one of the available devices");
        Span devices{pDeviceInfo, pDeviceInfo + nDeviceInfo};

        static int deviceIndex = -1;
        int btnIndex = 0;
        for (const auto& device : devices)
            ImGui::RadioButton(device.szDeviceName, &deviceIndex, btnIndex++);
        if (ImGui::Button("Refresh") || pDeviceInfo == nullptr)
            mdrConnectionGetDevicesList(conn, &pDeviceInfo, &nDeviceInfo);
        if (ImGui::Button("Connect"))
        {
            // XXX: Other service UUIDs?
            int res = mdrConnectionConnect(conn, devices[deviceIndex].szDeviceMacAddress, MDR_SERVICE_UUID_XM5);
            if (res != MDR_RESULT_OK && res != MDR_RESULT_INPROGRESS)
                throw std::runtime_error(fmt::format("Connection Failure.\nError: {}",
                                                     mdrConnectionGetLastError(conn)));
            connState = CONN_STATE_CONNECTING;
        }
    }
}

void DrawDeviceConnecting()
{
    MDR_ASSERT(connState == CONN_STATE_CONNECTING);
    MDRConnection* conn = clientPlatformConnectionGet();
    int res = mdrConnectionPoll(conn, 0);
    switch (res)
    {
    case MDR_RESULT_OK:
        connState = CONN_STATE_CONNECTED;
        gDevice = mdr::MDRHeadphones(conn);
        // Do an init - this should always be possible now
        MDR_ASSERT(gDevice.Invoke(gDevice.RequestInit()) == MDR_RESULT_OK);
        return;
    case MDR_RESULT_ERROR_TIMEOUT:
    case MDR_RESULT_INPROGRESS:
    {
        ImGui::OpenPopup("Connection");
        ImGui::SetNextWindowPos(ImGui::GetContentRegionAvail() / 2, 0, {0.5, 0.5});
        ImGui::SetNextWindowSize({0,0});
        if (ImGui::BeginPopupModal("Connection", nullptr, kImWindowFlagsTopMost))
        {
            ImGui::NewLine();
            ImTextCentered("Connecting...");
            ImGui::Dummy({0, 16.0f});
            ImSpinner(1000.0f, 24.0f, IM_COL32(255, 255, 255, 255), 2.0f, true, ImEaseInOutCubic);
            ImGui::NewLine();
            ImGui::Text("%s", mdrConnectionGetLastError(conn));
            ImGui::NewLine();
            ImGui::EndPopup();
        }
        return;
    }
    default:
        throw std::runtime_error(fmt::format("Connection Failure.\nError: {}", mdrConnectionGetLastError(conn)));
    }
}

void DrawDeviceControls()
{
    ImGui::Text("You made it.");
    int evt = gDevice.PollEvents();
    ImGui::Text("Evt: %d", evt);
    switch (evt)
    {
    case MDR_HEADPHONES_ERROR:
        // Irrecoverable. Disconnecting now
        mdrConnectionDisconnect(gDevice.mConn);
        connState = CONN_STATE_DISCONNECTED;
        break;
    default:
        break;
    }
}

void DrawDeviceDisconnect()
{
    ImGui::OpenPopup("Disconnected");
    ImGui::SetNextWindowPos(ImGui::GetContentRegionAvail() / 2, 0, {0.5, 0.5});
    ImGui::SetNextWindowSize({0,0});
    if (ImGui::BeginPopupModal("Disconnected", nullptr, kImWindowFlagsTopMost))
    {
        ImGui::NewLine();
        ImTextCentered("Device Disconnected");
        ImGui::Dummy({0, 16.0f});
        ImSpinner(5000.0f, 24.0f, IM_COL32(255, 0, 0, 255), 2.0f, true);
        ImGui::NewLine();
        ImGui::SeparatorText("Messages");
        ImGui::Text("Connection: %s", mdrConnectionGetLastError(gDevice.mConn));
        ImGui::Text("Headphones: %s", gDevice.GetLastError());
        ImGui::NewLine();
        if (ImGui::Button("OK"))
            connState = CONN_STATE_NO_CONNECTION;
        ImGui::EndPopup();
    }
}

void DrawApp()
{
    auto& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("##", nullptr, kImWindowFlagsTopMost);
    try
    {
        switch (connState)
        {
        case CONN_STATE_NO_CONNECTION:
            DrawDeviceDiscovery();
            break;
        case CONN_STATE_CONNECTING:
            DrawDeviceConnecting();
            break;
        case CONN_STATE_CONNECTED:
            DrawDeviceControls();
            break;
        case CONN_STATE_DISCONNECTED:
            DrawDeviceDisconnect();
            break;
        }
    }
    catch (const std::runtime_error& exc)
    {
        gBugcheckMessage = exc.what();
        appState = APP_STATE_BUGCHECK;
    }
    ImGui::End();
}

// You know this one.
void DrawBugcheck()
{
    constexpr float kMargin = 10.0f;
    auto& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 255));
    ImGui::Begin("##", nullptr, kImWindowFlagsTopMost);
    auto [offset, region, draw] = ImWindowDrawOffsetRegionList();
    float fontBase = std::max(ImGui::CalcTextSize(gBugcheckMessage.c_str()).x, region.x);
    fontBase = (region.x - kMargin * 4) / (fontBase + kMargin * 4);
    ImGui::PushFont(ImGui::GetFont(), ImGui::GetStyle().FontSizeBase * fontBase);
    float sizeV = ImGui::CalcTextSize(gBugcheckMessage.c_str()).y + ImGui::GetTextLineHeight();
    ImVec2 tl{kMargin, kMargin}, br{region.x - kMargin, sizeV + kMargin * 8};
    tl += offset, br += offset;
    draw->AddRectFilled(tl, br, IM_COL32(255 * ImBlink(1000u, 2u), 0, 0, 255));
    draw->AddRectFilled(tl + tl, br - tl, IM_COL32(0, 0, 0, 255));
    ImGui::SetCursorPosY(offset.y + kMargin * 4);
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
    ImTextCentered("Guru Meditation. Please screenshot and report.");
    ImTextCentered(gBugcheckMessage.c_str());
    ImGui::PopStyleColor();
    ImGui::SetCursorPosY(br.y + kMargin * 2);
    ImGui::SeparatorText("To Report");
    ImGui::TextWrapped("* Check the Open/Closed Github Issue tickets and see if it's a duplicate.");
    ImGui::TextWrapped("* If not, take a screenshot of this screen and submit a new one");
    ImGui::SeparatorText("Github Issues");
    ImGui::TextWrapped("* https://github.com/mos9527/SonyHeadphonesClient/issues");
    ImGui::PopFont();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::End();
}

bool ShouldClientExit()
{
    switch (appState)
    {
    case APP_STATE_RUNNING:
        DrawApp();
        break;
    case APP_STATE_BUGCHECK:
        DrawBugcheck();
        break;
    }
    return false;
}
