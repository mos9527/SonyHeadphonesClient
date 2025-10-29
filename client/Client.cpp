#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_sdl2.h>

#include <mdr/Headphones.hpp>
#include "Platform/Platform.hpp"

#include <SDL2/SDL.h>
using namespace mdr;

template <typename... Args>
using Tuple = std::tuple<Args...>;

Tuple<ImVec2, ImVec2, ImDrawList*> ImWindowDrawOffsetRegionList()
{
    ImVec2 offset = ImGui::GetWindowPos();
    ImVec2 region = ImGui::GetContentRegionAvail();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    return {offset, region, drawList};
}

void ImTextCentered(const char* text)
{
    ImVec2 size = ImGui::CalcTextSize(text);
    auto [offset, region, draw] = ImWindowDrawOffsetRegionList();
    ImGui::SetCursorPosX(offset.x + region.x / 2 - size.x / 2);
    ImGui::Text("%s", text);
}

mdr::MDRHeadphones device;

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
} connState{CONN_STATE_NO_CONNECTION};

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
            connState = CONN_STATE_CONNECTING;
    }
}

String bugcheckMessage;

void DrawApp()
{
    auto& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("##", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    try
    {
        DrawDeviceDiscovery();
    }
    catch (const std::runtime_error& exc)
    {
        bugcheckMessage = exc.what();
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
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0,0,0,255));
    ImGui::Begin("Bugcheck", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    auto [offset, region, draw] = ImWindowDrawOffsetRegionList();
    float fontBase = ImGui::CalcTextSize(bugcheckMessage.c_str()).x;
    fontBase = (region.x - kMargin * 4) / (fontBase + kMargin * 4);
    ImGui::PushFont(io.Fonts->Fonts[0] /* Proggy */, ImGui::GetStyle().FontSizeBase * fontBase);
    float sizeV = ImGui::CalcTextSize(bugcheckMessage.c_str()).y + ImGui::GetTextLineHeight();
    ImVec2 tl{kMargin, kMargin}, br{region.x - kMargin, sizeV + kMargin * 8};
    tl += offset, br += offset;
    draw->AddRectFilled(tl, br, IM_COL32(255 * (static_cast<size_t>(ImGui::GetTime()) & 1), 0, 0, 255));
    draw->AddRectFilled(tl + tl, br - tl, IM_COL32(0, 0, 0, 255));
    ImGui::SetCursorPosY(offset.y + kMargin * 4);
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,0,0,255));
    ImTextCentered("Guru Meditation. Please screenshot and report.");
    ImTextCentered(bugcheckMessage.c_str());
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
