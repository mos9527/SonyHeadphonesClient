#include <ranges>
#include <algorithm>

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include <imgui.h>
#include <SDL2/SDL.h>

#include <mdr/Headphones.hpp>
#include "Platform/Platform.hpp"
using namespace mdr;

mdr::MDRHeadphones gDevice;
String gBugcheckMessage;

#pragma region Enum Names
const char* FormatEnum(v2::t1::AudioCodec codec)
{
    using enum v2::t1::AudioCodec;
    switch (codec)
    {
    case UNSETTLED:
        return "<unsettled>";
    case SBC:
        return "SBC";
    case AAC:
        return "AAC";
    case LDAC:
        return "LDAC";
    case APT_X:
        return "aptX";
    case APT_X_HD:
        return "aptX HD";
    case LC3:
        return "LC3";
    default:
    case OTHER:
        return "Unknown";
    }
}

const char* FormatEnum(v2::t1::UpscalingType codec)
{
    using enum v2::t1::UpscalingType;
    switch (codec)
    {
    case DSEE_HX:
        return "DSEE HX";
    case DSEE:
        return "DSEE";
    case DSEE_HX_AI:
        return "DSEE HX AI";
    case DSEE_ULTIMATE:
        return "DSEE ULTIMATE";
    default:
        return "DSEE Unknown";
    }
}

const char* FormatEnum(v2::t1::BatteryChargingStatus status)
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
}

const char* FormatEnum(v2::t1::NoiseAdaptiveSensitivity status)
{
    using enum v2::t1::NoiseAdaptiveSensitivity;
    switch (status)
    {
    case STANDARD:
        return "Standard";
    case HIGH:
        return "High";
    case LOW:
        return "Low";
    default:
        return "Unknown";
    }
}

const char* FormatEnum(v2::t1::DetectSensitivity status)
{
    using enum v2::t1::DetectSensitivity;
    switch (status)
    {
    case AUTO:
        return "Auto";
    case HIGH:
        return "High";
    case LOW:
        return "Low";
    default:
        return "Unknown";
    }
}

const char* FormatEnum(v2::t1::ModeOutTime status)
{
    using enum v2::t1::ModeOutTime;
    switch (status)
    {
    case FAST:
        return "Short (~5s)";
    case MID:
        return "Standard (~15s)";
    case SLOW:
        return "Long (~30s)";
    case NONE:
        return "Don't end automatically";
    default:
        return "Unknown";
    }
}

const char* FormatEnum(v2::t1::EqPresetId id)
{
    using enum v2::t1::EqPresetId;
    switch (id)
    {
    case OFF:
        return "Off";
    case ROCK:
        return "Rock";
    case POP:
        return "Pop";
    case JAZZ:
        return "Jazz";
    case DANCE:
        return "Dance";
    case EDM:
        return "EDM";
    case R_AND_B_HIP_HOP:
        return "R&B/Hip-Hop";
    case ACOUSTIC:
        return "Acoustic";
    case BRIGHT:
        return "Bright";
    case EXCITED:
        return "Excited";
    case MELLOW:
        return "Mellow";
    case RELAXED:
        return "Relaxed";
    case VOCAL:
        return "Vocal";
    case TREBLE:
        return "Treble";
    case BASS:
        return "Bass";
    case SPEECH:
        return "Speech";
    case CUSTOM:
        return "Custom";
    case USER_SETTING1:
        return "User Setting 1";
    case USER_SETTING2:
        return "User Setting 2";
    case USER_SETTING3:
        return "User Setting 3";
    case USER_SETTING4:
        return "User Setting 4";
    case USER_SETTING5:
        return "User Setting 5";
    default:
        return "Unknown";
    }
}
#pragma endregion
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
    ImGui::Dummy({sqrt(2.0f) * size, sqrt(2.0f) * size + ImGui::GetStyle().FramePadding.y * 2.0f});
}

// Fill the available horizontal region with lineTotal amount of buttons
// This is used for modal dialogues
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
        ImGui::GetContentRegionAvail() / 2 + ImVec2{padding, 0},
        0, {0.5f, 0.5f}
        );
    ImGui::SetNextWindowSize({0, 0});
}

void ImTextWithBorder(const char* text, int color, float rounding = 0.0f, float thickness = 1.0f)
{
    auto& style = ImGui::GetStyle();
    float padding = style.FramePadding.x;
    ImVec2 size = ImGui::CalcTextSize(text);
    ImVec2 pad = {padding, padding / 2};
    float cursorY = ImGui::GetCursorPosY();
    ImGui::SetCursorPosY(cursorY + padding * 2);
    auto [offset, region, draw] = ImWindowDrawOffsetRegionList();
    draw->AddRect(offset - pad, offset + size + pad, color, rounding, ImDrawFlags_None, thickness);
    ImGui::SetCursorPosY(cursorY + padding);
    ImGui::Text("%s", text);
    ImGui::Dummy({padding, 0});
}

template <typename T>
void ImComboBoxItems(const char* label, Span<const T> items, T& selection)
{
    if (ImGui::BeginCombo(label, FormatEnum(selection)))
    {
        for (T const& i : items)
        {
            bool selected = i == selection;
            if (ImGui::Selectable(FormatEnum(i), selected))
                selection = i;
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

void ImEqualizer(Span<int> bands)
{
    constexpr const char* kBand5[] = {"400", "1k", "2.5k", "6.3k", "16k"};
    constexpr const char* kBand10[] = {"31", "63", "125", "250", "500", "1k", "2k", "4k", "8k", "16k"};
    const char* const* kBands = nullptr;
    int numBands = static_cast<int>(bands.size());
    int mn = 0, mx = 0;
    if (numBands == 10)
        kBands = kBand10, mn = -6, mx = 6;
    if (numBands == 5)
        kBands = kBand5, mn = -10, mx = 10;
    if (!kBands)
        return ImGui::Text("EQ Unavailable (bands=%d)", numBands);
    auto& style = ImGui::GetStyle();
    float padding = style.FramePadding.x;
    auto [offset, region, draw] = ImWindowDrawOffsetRegionList();
    float bandWidth = region.x / numBands - padding;
    float bandHeight = std::max(region.y, 160.0f);
    if (numBands == 5)
        ImGui::SeparatorText("5-Band EQ");
    if (numBands == 10)
        ImGui::SeparatorText("10-Band EQ");
    for (int i = 0; i < numBands; ++i)
    {
        ImGui::BeginGroup();
        ImGui::PushID(i);
        ImGui::VSliderInt("##v", ImVec2{bandWidth, bandHeight}, &bands[i], mn, mx);
        ImGui::PopID();

        float textWidth = ImGui::CalcTextSize(kBands[i]).x;
        float textOffset = (bandWidth - textWidth) * 0.5f;
        if (textOffset > 0.0f)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffset);
        ImGui::TextUnformatted(kBands[i]);

        ImGui::EndGroup();
        if (i != numBands - 1)
            ImGui::SameLine(0.0f, padding);
    }
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
        /* Disconnect & Shutdown */
        if (ImGui::BeginMenu(gDevice.mModelName.c_str()))
        {
            if (ImGui::MenuItem("Disconnect"))
            {
                mdrConnectionDisconnect(conn);
                connState = CONN_STATE_NO_CONNECTION;
            }
            if (gDevice.mSupport.contains(v2::MessageMdrV2FunctionType_Table1::POWER_OFF))
            {
                if (ImGui::MenuItem("Shutdown"))
                    gDevice.mShutdown.desired = true;
            }
            ImGui::EndMenu();
        }
        ImGui::Dummy({0, 0});
        /* Cool Badges */
        // Title, Border Color, Text Color
        using Badge = Tuple<const char*, int, int>;
        Array<Badge, 4> badges4;
        Badge *badgeFirst = &badges4[0], *badgeLast = &badges4[0];
        /* Codec */
        if (gDevice.mSupport.contains(v2::MessageMdrV2FunctionType_Table1::CODEC_INDICATOR))
        {
            *(badgeLast++) = {FormatEnum(gDevice.mAudioCodec), ~0u, ~0u};
        }
        /* DSEE */
        if (gDevice.mUpscalingEnabled.current)
        {
            *(badgeLast++) = {FormatEnum(gDevice.mUpscalingType), ~0u, ~0u};
        }
        Span badges{badgeFirst, badgeLast};
        // Right-align and draw them
        // XXX: This is surprisingly painful to do.
        auto& style = ImGui::GetStyle();
        float padding = style.FramePadding.x;
        float badgeRegionX = 0;
        ImGui::PushFont(ImGui::GetFont(), style.FontSizeBase - padding / 2);
        for (auto& [s, border, text] : badges)
            badgeRegionX += ImGui::CalcTextSize(s).x + padding * 2;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - badgeRegionX - padding * 2);
        ImGui::Dummy({0, 0});
        float rounding = style.FrameRounding;
        for (auto& [s, border, text] : badges)
            ImTextWithBorder(s, border, rounding, 2.0f);
        ImGui::PopFont();
        ImGui::EndMenuBar();
    }
    // Stats
    if (ImGui::BeginTable("##Stats", 2, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        /* Batteries */
        {
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
                ImGui::Text("Battery: %.2f%%", single * 100);
                ImGui::SameLine();
                ImGui::ProgressBar(single, {-1, 0}, FormatEnum(gDevice.mBatteryL.charging));
            }
            if (supportLR)
            {
                float single = gDevice.mBatteryL.level;
                single /= gDevice.mBatteryL.threshold;
                ImGui::Text("L: %.2f%%", single * 100);
                ImGui::SameLine();
                ImGui::ProgressBar(single, {-1, 0}, FormatEnum(gDevice.mBatteryL.charging));
                single = gDevice.mBatteryR.level;
                single /= gDevice.mBatteryR.threshold;
                ImGui::Text("R: %.2f%%", single * 100);
                ImGui::SameLine();
                ImGui::ProgressBar(single, {-1, 0}, FormatEnum(gDevice.mBatteryR.charging));
            }
            if (supportCase)
            {
                float single = gDevice.mBatteryCase.level;
                single /= 100.0f; // gDevice.mBatteryCase.threshold <-- Wonky. Got threshold=30 and value=76 pairs
                // Seems like 100.0f is always the case for...case
                ImGui::Text("Case: %.2f%%", single * 100);
                ImGui::SameLine();
                ImGui::ProgressBar(single, {-1, 0}, FormatEnum(gDevice.mBatteryCase.charging));
            }
        }
        ImGui::TableSetColumnIndex(1);
        /* Now Playing */
        {
            ImGui::Text("Title:  %s", gDevice.mPlayTrackTitle.c_str());
            ImGui::Text("Album:  %s", gDevice.mPlayTrackAlbum.c_str());
            ImGui::Text("Artist: %s", gDevice.mPlayTrackArtist.c_str());
        }
        ImGui::EndTable();
    }
}

void DrawDeviceControlsPlayback()
{
    using enum v2::t1::PlaybackControl;
    ImGui::SeparatorText("Volume");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::SliderInt("##Volume", &gDevice.mPlayVolume.desired, 0, 30);
    ImGui::SeparatorText("Controls");
    if (ImModalButton("Prev", 0, 3))
        gDevice.mPlayControl.desired = TRACK_DOWN;
    if (gDevice.mPlayPause == v2::t1::PlaybackStatus::PLAY)
    {
        if (ImModalButton("Pause", 1, 3))
            gDevice.mPlayControl.desired = PAUSE;
    }
    else
    {
        if (ImModalButton("Play", 1, 3))
            gDevice.mPlayControl.desired = PLAY;
    }
    if (ImModalButton("Next", 2, 3))
        gDevice.mPlayControl.desired = TRACK_UP;
}

void DrawDeviceControlsSound()
{
    using F1 = v2::MessageMdrV2FunctionType_Table1;
    constexpr auto kSupports = [](auto x) { return gDevice.mSupport.contains(x); };
    bool supportNC = kSupports(F1::NOISE_CANCELLING_ONOFF)
        || kSupports(F1::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_ONOFF)
        || kSupports(F1::NOISE_CANCELLING_DUAL_SINGLE_OFF_AND_AMBIENT_SOUND_MODE_ONOFF)
        || kSupports(F1::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || kSupports(F1::NOISE_CANCELLING_DUAL_SINGLE_OFF_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || kSupports(F1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AUTO_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || kSupports(F1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_SINGLE_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || kSupports(F1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || kSupports(F1::MODE_NC_NCSS_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_WITH_TEST_MODE)
        || kSupports(F1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION);
    bool supportASM = kSupports(F1::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_ONOFF)
        || kSupports(F1::NOISE_CANCELLING_DUAL_SINGLE_OFF_AND_AMBIENT_SOUND_MODE_ONOFF)
        || kSupports(F1::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || kSupports(F1::NOISE_CANCELLING_DUAL_SINGLE_OFF_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || kSupports(F1::AMBIENT_SOUND_MODE_ONOFF)
        || kSupports(F1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || kSupports(F1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AUTO_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || kSupports(F1::AMBIENT_SOUND_CONTROL_MODE_SELECT)
        || kSupports(F1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_SINGLE_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || kSupports(F1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || kSupports(F1::MODE_NC_NCSS_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_WITH_TEST_MODE)
        || kSupports(F1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION);
    bool supportAutoASM = kSupports(
        F1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION);
    using enum v2::t1::NcAsmMode;
    /* NC/ASM */
    if (supportASM || supportNC)
    {
        if (ImGui::TreeNodeEx("Ambient Sound", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (supportNC)
            {
                if (ImGui::RadioButton(
                    "Noise Cancelling",
                    gDevice.mNcAsmEnabled.current && (!supportASM || gDevice.mNcAsmMode.desired == NC))
                )
                {
                    gDevice.mNcAsmEnabled.desired = true;
                    gDevice.mNcAsmMode.desired = NC;
                }
                ImGui::SameLine();
            }
            if (supportASM)
            {
                if (ImGui::RadioButton(
                    "Ambient Sound",
                    gDevice.mNcAsmEnabled.current && (!supportNC || gDevice.mNcAsmMode.desired == ASM))
                )
                {
                    gDevice.mNcAsmEnabled.desired = true;
                    gDevice.mNcAsmMode.desired = ASM;
                    if (gDevice.mNcAsmAmbientLevel.desired == 0)
                        gDevice.mNcAsmAmbientLevel.desired = 20;
                }
                ImGui::SameLine();
            }
            if (ImGui::RadioButton("Off", !gDevice.mNcAsmEnabled.desired))
                gDevice.mNcAsmEnabled.desired = false;
            ImGui::SeparatorText("Ambient Strength");
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::SliderInt("##AmbStrength", &gDevice.mNcAsmAmbientLevel.desired, 1, 20);
            if (supportAutoASM)
            {
                ImGui::Checkbox("Auto Ambient Sound", &gDevice.mNcAsmAutoAsmEnabled.desired);
                ImGui::BeginDisabled(!gDevice.mNcAsmAutoAsmEnabled.desired);
                using enum v2::t1::NoiseAdaptiveSensitivity;
                auto& desired = gDevice.mNcAsmNoiseAdaptiveSensitivity.desired;
                constexpr v2::t1::NoiseAdaptiveSensitivity kSelections[] = {STANDARD, HIGH, LOW};
                ImComboBoxItems<v2::t1::NoiseAdaptiveSensitivity>("Sensitivity", kSelections, desired);
                ImGui::EndDisabled();
            }
            ImGui::Checkbox("Voice Passthrough", &gDevice.mNcAsmFocusOnVoice.desired);
            ImGui::TreePop();
        }
    }
    /* STC */
    if (kSupports(F1::SMART_TALKING_MODE_TYPE2))
    {
        if (ImGui::TreeNodeEx("Speak To Chat", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("Enabled", &gDevice.mSpeakToChatEnabled.desired);
            ImGui::BeginDisabled(!gDevice.mSpeakToChatEnabled.desired);
            {
                using enum v2::t1::DetectSensitivity;
                constexpr v2::t1::DetectSensitivity kSelections[] = {AUTO, HIGH, LOW};
                ImComboBoxItems<v2::t1::DetectSensitivity>("Sensitivity", kSelections,
                                                           gDevice.mSpeakToChatDetectSensitivity.desired);
            }
            {
                using enum v2::t1::ModeOutTime;
                constexpr v2::t1::ModeOutTime kSelections[] = {FAST, MID, SLOW, NONE};
                ImComboBoxItems<v2::t1::ModeOutTime>("Mode Duration", kSelections,
                                                     gDevice.mSpeakToModeOutTime.desired);
            }
            ImGui::EndDisabled();
            ImGui::TreePop();
        }
    }
    /* Listening Mode */
    // TODO: NOT IMPLEMENTED. Need XM6s to test
    if (kSupports(F1::LISTENING_OPTION))
    {
        if (ImGui::TreeNodeEx("Listening Mode", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::TreePop();
        }
    }
    /* EQ & DSEE */
    if (ImGui::TreeNodeEx("Equalizer & DSEE", ImGuiTreeNodeFlags_DefaultOpen))
    {
        using enum v2::t1::EqPresetId;
        constexpr v2::t1::EqPresetId kSelections[] = {
            OFF, ROCK, POP, JAZZ, DANCE, EDM, R_AND_B_HIP_HOP, ACOUSTIC, BRIGHT, EXCITED,
            MELLOW, RELAXED, VOCAL, TREBLE, BASS, SPEECH,
            CUSTOM, USER_SETTING1, USER_SETTING2, USER_SETTING3, USER_SETTING4, USER_SETTING5
        };
        ImComboBoxItems<v2::t1::EqPresetId>("Preset", kSelections, gDevice.mEqPresetId.desired);
        ImEqualizer(gDevice.mEqConfig.desired);
        if (gDevice.mEqConfig.desired.size() == 5)
        {
            ImGui::SeparatorText("Clear Bass");
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::SliderInt("##", &gDevice.mEqClearBass.desired, -10, 10);
        }
        ImGui::SeparatorText("DSEE Upscaling");
        ImGui::BeginDisabled(!gDevice.mUpscalingAvailable);
        if (ImGui::RadioButton("Off", gDevice.mUpscalingEnabled.desired == false))
            gDevice.mUpscalingEnabled.desired = false;
        if (ImGui::RadioButton("On (Auto)", gDevice.mUpscalingEnabled.desired == true))
            gDevice.mUpscalingEnabled.desired = true;
        ImGui::EndDisabled();
        ImGui::TreePop();
    }
}

void DrawDeviceControlsDevices()
{
    using F2 = v2::MessageMdrV2FunctionType_Table2;
    constexpr auto kSupports = [](auto x) { return gDevice.mSupport.contains(x); };
    bool supportDeviceMgmt = kSupports(F2::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT)
        || kSupports(F2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT)
        || kSupports(F2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE);
    if (!supportDeviceMgmt)
        ImGui::Text("Please enable \"Connect to 2 devices simultaneously\" in System settings to manage devices.");
    ImGui::BeginDisabled(!supportDeviceMgmt);
    auto DrawDeviceElement = [&](const mdr::MDRHeadphones::PeripheralDevice& device, bool selected) -> bool
    {
        ImGui::BeginGroup();
        bool res = ImGui::Selectable(device.name.c_str(), selected);
        if (selected)
        {
            ImGui::Separator();
            if (device.connected)
            {
                if (ImModalButton("Disconnect", 0, 2))
                    gDevice.mPairedDeviceDisconnectMac.desired = device.macAddress;
            }
            else
            {
                if (ImModalButton("Connect", 0, 2))
                    gDevice.mPairedDeviceConnectMac.desired = device.macAddress;
            }
            if (ImModalButton("Unpair", 1, 2))
                gDevice.mPairedDeviceUnpairMac.desired = device.macAddress;
        }
        ImGui::EndGroup();
        return res;
    };
    auto devices = std::views::all(gDevice.mPairedDevices);
    auto connectedDevices = devices | std::views::filter([](auto const& x) { return x.connected; });
    auto unconncetedDevices = devices | std::views::filter([](auto const& x) { return !x.connected; });
    if (ImGui::TreeNodeEx("Connected", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static String connectSelectedMac;
        for (auto& device : connectedDevices)
            if (DrawDeviceElement(device, connectSelectedMac == device.macAddress))
                connectSelectedMac = device.macAddress;
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Paired", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static String pairedSelectedMac;
        for (auto& device : unconncetedDevices)
            if (DrawDeviceElement(device, pairedSelectedMac == device.macAddress))
                pairedSelectedMac = device.macAddress;
        ImGui::TreePop();
    }
    if (gDevice.mPairingMode.desired)
    {
        ImTextCentered("Pairing...");
        ImSpinner(1000.0f, 16.0f, IM_COL32(0, 255, 0, 255), 2.0f, true, 1.0f, ImEaseInOutCubic);
        if (ImModalButton("Stop"))
            gDevice.mPairingMode.desired = false;
    }
    else
    {
        if (ImModalButton("Connect to New Device"))
            gDevice.mPairingMode.desired = true;
        ImGui::TextWrapped("NOTE: For TWS (Earbuds) devices, you may need to take both of your headphones out from your case.");
    }
    ImGui::EndDisabled();
}

void DrawDeviceControlsTabs()
{
    if (ImGui::BeginTabBar("##Controls"))
    {
        if (ImGui::BeginTabItem("Playback"))
        {
            DrawDeviceControlsPlayback();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Sound"))
        {
            DrawDeviceControlsSound();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Devices"))
        {
            DrawDeviceControlsDevices();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void DrawDeviceControls()
{
    MDRConnection* conn = clientPlatformConnectionGet();
    int event = gDevice.PollEvents();
    DrawDeviceControlsHeader();
    ImGui::Separator();
    DrawDeviceControlsTabs();
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
        case MDR_HEADPHONES_TASK_ERR_TIMEOUT:
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
    auto& style = ImGui::GetStyle();
    style.FrameRounding = 8.0f;
    style.FramePadding = ImVec2(8.0f, 8.0f);
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
