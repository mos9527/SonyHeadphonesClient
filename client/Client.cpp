#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include <imgui.h>

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
               float cycles = 1.0f, float (*easing)(float) = ImEaseLinear)
{
    constexpr ImVec2 kPoints[] = {{-1, 1}, {-1, -1}, {1, -1}, {1, 1}};
    ImVec2 points[std::size(kPoints)];
    if (center)
        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2 - size * sqrt(2) / 2);
    auto [offset, region, draw] = ImWindowDrawOffsetRegionList();
    float t = ImBlinkF(interval), theta = easing(t) * acos(-1) * cycles;
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

bool ImModalButton(const char* label, int lineIndex = 0, int lineTotal = 1)
{
    auto& style = ImGui::GetStyle();
    float padding = style.FramePadding.x;
    float width = ImGui::GetContentRegionAvail().x / lineTotal;
    if (lineIndex)
        ImGui::SameLine();
    return ImGui::Button(label, ImVec2(width - padding, 0));
}

void ImSetNextWindowCentered()
{
    auto& style = ImGui::GetStyle();
    float padding = style.FramePadding.x;
    ImGui::SetNextWindowPos(
        ImGui::GetContentRegionAvail() / 2 + ImVec2{padding * 2, 0} /* <- I have no idea why it's 2 times */,
        0, {0.5f, 0.5f}
        );
    ImGui::SetNextWindowSize({0, 0});
}

void ImTextWithBorder(const char* text, int color, float rounding = 0.0f, float thickness = 1.0f)
{
    auto& style = ImGui::GetStyle();
    float padding = style.FramePadding.x;
    ImVec2 size = ImGui::CalcTextSize(text);
    ImVec2 pad = {padding, padding};
    auto [offset, region, draw] = ImWindowDrawOffsetRegionList();
    draw->AddRect(offset - pad, offset + size + pad, color, rounding, ImDrawFlags_None, thickness);
    ImGui::Text("%s", text);
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

void ExceptionHandler(auto&& func)
{
    try
    {
        func();
    }
    catch (const std::runtime_error& exc)
    {
        gBugcheckMessage = exc.what();
        appState = APP_STATE_BUGCHECK;
    }
}

void DrawDeviceDiscovery()
{
    MDR_CHECK(connState == CONN_STATE_NO_CONNECTION);
    MDRConnection* conn = clientPlatformConnectionGet();
    MDR_CHECK(conn != nullptr);
    ImSetNextWindowCentered();
    ImGui::OpenPopup("DeviceDiscovery");
    if (ImGui::BeginPopupModal("DeviceDiscovery", nullptr, kImWindowFlagsTopMost))
    {
        static MDRDeviceInfo* pDeviceInfo = nullptr;
        static int nDeviceInfo = 0;
        Span devices{pDeviceInfo, pDeviceInfo + nDeviceInfo};

        static int deviceIndex = -1;
        int btnIndex = 0;
        for (const auto& device : devices)
            ImGui::RadioButton(device.szDeviceName, &deviceIndex, btnIndex++);
        if (ImModalButton("Connect", 0, 2))
        {
            if (deviceIndex != -1)
            {
                // XXX: Other service UUIDs?
                int res = mdrConnectionConnect(conn, devices[deviceIndex].szDeviceMacAddress, MDR_SERVICE_UUID_XM5);
                if (res != MDR_RESULT_OK && res != MDR_RESULT_INPROGRESS)
                    throw std::runtime_error(fmt::format("Connection Failure.\nError: {}",
                                                         mdrConnectionGetLastError(conn)));
                connState = CONN_STATE_CONNECTING;
            }
        }
        if (ImModalButton("Refresh", 1, 2) || pDeviceInfo == nullptr)
            mdrConnectionGetDevicesList(conn, &pDeviceInfo, &nDeviceInfo);
        ImGui::EndPopup();
    }
}

void DrawDeviceConnecting()
{
    MDR_CHECK(connState == CONN_STATE_CONNECTING);
    MDRConnection* conn = clientPlatformConnectionGet();
    switch (mdrConnectionPoll(conn, 0))
    {
    case MDR_RESULT_OK:
        connState = CONN_STATE_CONNECTED;
        gDevice = mdr::MDRHeadphones(conn);
        // Do an init - this should always be possible when @ref MDRHeadphones
        // is first created.
        MDR_CHECK(gDevice.Invoke(gDevice.RequestInit()) == MDR_RESULT_OK);
        return;
    case MDR_RESULT_ERROR_TIMEOUT:
    case MDR_RESULT_INPROGRESS:
    {
        ImSetNextWindowCentered();
        ImGui::OpenPopup("Connection");
        if (ImGui::BeginPopupModal("Connection", nullptr, kImWindowFlagsTopMost))
        {
            ImGui::NewLine();
            ImTextCentered("Connecting...");
            ImGui::Dummy({0, 16.0f});
            ImSpinner(1000.0f, 24.0f, IM_COL32(255, 255, 255, 255), 2.0f, true, 2.0f, ImEaseInOutCubic);
            ImGui::NewLine();
            ImGui::Text("%s", mdrConnectionGetLastError(conn));
            ImGui::NewLine();
            if (ImModalButton("Cancel", 1, 1))
            {
                mdrConnectionDisconnect(conn);
                connState = CONN_STATE_NO_CONNECTION;
            }
            ImGui::EndPopup();
        }
        return;
    }
    default:
    {
        connState = CONN_STATE_DISCONNECTED;
        mdrConnectionDisconnect(conn);
        break;
    }
    }
}

void DrawDeviceControlsHeader()
{
    MDRConnection* conn = clientPlatformConnectionGet();
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu(gDevice.mModelName.c_str()))
        {
            if (ImGui::Button("Disconnect"))
            {
                mdrConnectionDisconnect(conn);
                connState = CONN_STATE_NO_CONNECTION;
            }
            if (ImGui::Button("Shutdown"))
            {
                gDevice.mShutdown.desired = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    // Stats
    if (ImGui::BeginTable("##Stats", 2, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        /* Batteries */
        {
            constexpr auto kFormatChargingState = [&](v2::t1::BatteryChargingStatus status)
            {
                using enum v2::t1::BatteryChargingStatus;
                switch (status)
                {
                case CHARGING:
                    return "Charging...";
                case CHARGED:
                    return "Charged";
                case NOT_CHARGING:
                    return "Discharging";
                default:
                case UNKNOWN:
                    return "Unknown";
                }
            };
            bool supportSingle = gDevice.mSupport.
                                         contains(v2::MessageMdrV2FunctionType_Table1::BATTERY_LEVEL_INDICATOR);
            supportSingle |= gDevice.mSupport.contains(
                v2::MessageMdrV2FunctionType_Table1::BATTERY_LEVEL_WITH_THRESHOLD);
            bool supportLR = gDevice.mSupport.contains(
                v2::MessageMdrV2FunctionType_Table1::LEFT_RIGHT_BATTERY_LEVEL_INDICATOR);
            supportLR |= gDevice.mSupport.
                                 contains(v2::MessageMdrV2FunctionType_Table1::LR_BATTERY_LEVEL_WITH_THRESHOLD);
            bool supportCase = gDevice.mSupport.contains(
                v2::MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_INDICATOR);
            supportCase |= gDevice.mSupport.contains(
                v2::MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD);
            if (supportSingle && !supportLR)
            {
                float single = gDevice.mBatteryL.level;
                single /= gDevice.mBatteryL.threshold;
                ImGui::Text("Battery: %.2f", single);
                ImGui::SameLine();
                ImGui::ProgressBar(single, {-1, 0}, kFormatChargingState(gDevice.mBatteryL.charging));
            }
            if (supportLR)
            {
                float single = gDevice.mBatteryL.level;
                single /= gDevice.mBatteryL.threshold;
                ImGui::Text("L: %.2f", single);
                ImGui::SameLine();
                ImGui::ProgressBar(single, {-1, 0}, kFormatChargingState(gDevice.mBatteryL.charging));
                single = gDevice.mBatteryR.level;
                single /= gDevice.mBatteryR.threshold;
                ImGui::Text("R: %.2f", single);
                ImGui::SameLine();
                ImGui::ProgressBar(single, {-1, 0}, kFormatChargingState(gDevice.mBatteryR.charging));
            }
            if (supportCase)
            {
                float single = gDevice.mBatteryCase.level;
                single /= 100.0f; // gDevice.mBatteryCase.threshold <-- Wonky. Got threshold=30 and value=76 pairs
                // Seems like 100.0f is always the case for...case
                ImGui::Text("Case: %.2f", single);
                ImGui::SameLine();
                ImGui::ProgressBar(single, {-1, 0}, kFormatChargingState(gDevice.mBatteryCase.charging));
            }
        }
        ImGui::TableSetColumnIndex(1);
        /* Now Playing & Codec */
        {
            ImGui::Text("Title:  %s", gDevice.mPlayTrackTitle.c_str());
            ImGui::Text("Album:  %s", gDevice.mPlayTrackAlbum.c_str());
            ImGui::Text("Artist: %s", gDevice.mPlayTrackArtist.c_str());
            if (gDevice.mSupport.contains(v2::MessageMdrV2FunctionType_Table1::CODEC_INDICATOR))
            {
                constexpr auto kFormatCodecName = [&](v2::t1::AudioCodec codec)
                {
                    using enum v2::t1::AudioCodec;
                    switch (codec)
                    {
                        case UNSETTLED: return "<unsettled>";
                        case SBC: return "SBC";
                        case AAC: return "AAC";
                        case LDAC: return "LDAC";
                        case APT_X: return "aptX";
                        case APT_X_HD: return "aptX HD";
                        case LC3: return "LC3";
                        default:
                        case OTHER: return "Unknown";
                    }
                };
                ImTextWithBorder(kFormatCodecName(gDevice.mAudioCodec), IM_COL32(255,255,255,255), 1.0f, 2.0f);
            }
        }
        ImGui::EndTable();
    }
}

void DrawDeviceControls()
{
    MDRConnection* conn = clientPlatformConnectionGet();
    int event = gDevice.PollEvents();
    DrawDeviceControlsHeader();
    ImGui::Separator();
    ExceptionHandler([&]
    {
        switch (event)
        {
        case MDR_HEADPHONES_TASK_INIT_OK:
            // Request for a stat update ASAP
            // User may request for this themselves - we don't do periodic checks this time
            MDR_CHECK(gDevice.Invoke(gDevice.RequestSync()) == MDR_RESULT_OK);
            return;
        case MDR_HEADPHONES_IDLE:
            // Commit changes if needed to
            if (gDevice.IsDirty())
                MDR_CHECK(gDevice.Invoke(gDevice.RequestCommit()) == MDR_RESULT_OK);
            return;
        case MDR_HEADPHONES_ERROR:
            // Irrecoverable. Disconnect now.
            mdrConnectionDisconnect(conn);
            connState = CONN_STATE_DISCONNECTED;
        case MDR_HEADPHONES_INPROGRESS:
        default:
            break;
        }
    });
}

void DrawDeviceDisconnect()
{
    MDRConnection* conn = clientPlatformConnectionGet();
    ImGui::OpenPopup("Disconnected");
    ImSetNextWindowCentered();
    if (ImGui::BeginPopupModal("Disconnected", nullptr, kImWindowFlagsTopMost))
    {
        ImGui::NewLine();
        ImTextCentered("Device Disconnected");
        ImGui::NewLine();
        ImSpinner(5000.0f, 24.0f, IM_COL32(255, 0, 0, 255), 2.0f, true);
        ImGui::NewLine();
        ImGui::SeparatorText("Messages");
        ImGui::Text("Connection: %s", mdrConnectionGetLastError(conn));
        ImGui::Text("Headphones: %s", gDevice.GetLastError());
        ImGui::NewLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImModalButton("OK", 1, 1))
            connState = CONN_STATE_NO_CONNECTION;
        ImGui::EndPopup();
    }
}

void DrawApp()
{
    auto& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("##", nullptr, kImWindowFlagsTopMost | ImGuiWindowFlags_MenuBar);
    ExceptionHandler([&]
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
    });
    ImGui::End();
}

// You know this one.
void DrawBugcheck()
{
    auto& style = ImGui::GetStyle();
    float padding = style.FramePadding.x;
    auto& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 255));
    ImGui::Begin("##", nullptr, kImWindowFlagsTopMost);
    auto [offset, region, draw] = ImWindowDrawOffsetRegionList();
    float fontBase = std::max(ImGui::CalcTextSize(gBugcheckMessage.c_str()).x, region.x);
    fontBase = (region.x - padding * 4) / (fontBase + padding * 4);
    ImGui::PushFont(ImGui::GetFont(), ImGui::GetStyle().FontSizeBase * fontBase);
    float sizeV = ImGui::CalcTextSize(gBugcheckMessage.c_str()).y + ImGui::GetTextLineHeight();
    ImVec2 tl{padding, padding}, br{region.x - padding, sizeV + padding * 8};
    tl += offset, br += offset;
    draw->AddRectFilled(tl, br, IM_COL32(255 * ImBlink(1000u, 2u), 0, 0, 255));
    draw->AddRectFilled(tl + tl, br - tl, IM_COL32(0, 0, 0, 255));
    ImGui::SetCursorPosY(offset.y + padding * 4);
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
    ImTextCentered("Guru Meditation. Please screenshot and report.");
    ImTextCentered(gBugcheckMessage.c_str());
    ImGui::PopStyleColor();
    ImGui::SetCursorPosY(br.y + padding * 2);
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
