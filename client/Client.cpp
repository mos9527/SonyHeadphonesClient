#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <span>
#include <string>
#include <tuple>
#include <utility>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <mdr-c/Headphones.h>
#include <mdr/Protocol.hpp>
#include "Fonts/PlexSansIcon.h"
#include "MaterialYouTheme.hpp"
#include "PacketObserver.hpp"
#include "Platform/Platform.hpp"
#ifdef MDR_CLIENT_DEBUGGER
#include "Debugger.hpp"
#endif

MDRHeadphones* gDevice;
mdr::String gHeadphonesError;
#ifdef MDR_CLIENT_DEBUGGER
bool gDebuggerOpen{};
bool gDebuggerOnlyMode{};
#endif

template <typename T>
T MDRStruct()
{
    T value{};
    value.struct_size = sizeof(T);
    return value;
}

#pragma region Enum Names
const char* FormatAudioCodec(MDRAudioCodec codec)
{
    switch (codec)
    {
    case MDR_AUDIO_CODEC_UNKNOWN:
        return "<unsettled>";
    case MDR_AUDIO_CODEC_SBC:
        return "SBC";
    case MDR_AUDIO_CODEC_AAC:
        return "AAC";
    case MDR_AUDIO_CODEC_LDAC:
        return "LDAC";
    case MDR_AUDIO_CODEC_APTX:
        return "aptX";
    case MDR_AUDIO_CODEC_APTX_HD:
        return "aptX HD";
    case MDR_AUDIO_CODEC_LC3:
        return "LC3";
    default:
    case MDR_AUDIO_CODEC_OTHER:
        return "Unknown";
    }
}

const char* FormatDseeType(MDRDSEEType type)
{
    switch (type)
    {
    case MDR_DSEE_HX:
        return "DSEE HX";
    case MDR_DSEE_STANDARD:
        return "DSEE";
    case MDR_DSEE_HX_AI:
        return "DSEE HX AI";
    case MDR_DSEE_ULTIMATE:
        return "DSEE ULTIMATE";
    default:
        return "DSEE Unknown";
    }
}

const char* FormatChargingState(MDRChargingState status)
{
    switch (status)
    {
    case MDR_CHARGING_YES:
        return "Charging";
    case MDR_CHARGING_COMPLETE:
        return "Charged";
    case MDR_CHARGING_NO:
        return ""; // Hidden
    default:
    case MDR_CHARGING_UNKNOWN:
        return "Unknown";
    }
}

const char* FormatAdaptiveSensitivity(MDRAdaptiveSensitivity status)
{
    switch (status)
    {
    case MDR_ADAPTIVE_SENSITIVITY_STANDARD:
        return "Standard";
    case MDR_ADAPTIVE_SENSITIVITY_HIGH:
        return "High";
    case MDR_ADAPTIVE_SENSITIVITY_LOW:
        return "Low";
    default:
        return "Unknown";
    }
}

const char* FormatSpeechSensitivity(MDRSpeechSensitivity status)
{
    switch (status)
    {
    case MDR_SPEECH_SENSITIVITY_AUTO:
        return "Auto";
    case MDR_SPEECH_SENSITIVITY_HIGH:
        return "High";
    case MDR_SPEECH_SENSITIVITY_LOW:
        return "Low";
    default:
        return "Unknown";
    }
}

const char* FormatSpeakTimeout(MDRSpeakTimeout status)
{
    switch (status)
    {
    case MDR_SPEAK_TIMEOUT_SHORT:
        return "Short (~5s)";
    case MDR_SPEAK_TIMEOUT_MEDIUM:
        return "Standard (~15s)";
    case MDR_SPEAK_TIMEOUT_LONG:
        return "Long (~30s)";
    case MDR_SPEAK_TIMEOUT_MANUAL:
        return "Don't end automatically";
    default:
        return "Unknown";
    }
}

const char* FormatEqualizerPreset(MDREqualizerPreset id)
{
    switch (id)
    {
    case MDR_EQ_OFF:
        return "Off";
    case MDR_EQ_ROCK:
        return "Rock";
    case MDR_EQ_POP:
        return "Pop";
    case MDR_EQ_JAZZ:
        return "Jazz";
    case MDR_EQ_DANCE:
        return "Dance";
    case MDR_EQ_EDM:
        return "EDM";
    case MDR_EQ_R_AND_B_HIP_HOP:
        return "R&B/Hip-Hop";
    case MDR_EQ_ACOUSTIC:
        return "Acoustic";
    case MDR_EQ_BRIGHT:
        return "Bright";
    case MDR_EQ_EXCITED:
        return "Excited";
    case MDR_EQ_MELLOW:
        return "Mellow";
    case MDR_EQ_RELAXED:
        return "Relaxed";
    case MDR_EQ_VOCAL:
        return "Vocal";
    case MDR_EQ_TREBLE:
        return "Treble";
    case MDR_EQ_BASS:
        return "Bass";
    case MDR_EQ_SPEECH:
        return "Speech";
    case MDR_EQ_HEAVY:
        return "Heavy";
    case MDR_EQ_CLEAR:
        return "Clear";
    case MDR_EQ_HARD:
        return "Hard";
    case MDR_EQ_SOFT:
        return "Soft";
    case MDR_EQ_GAMING:
        return "Gaming";
    case MDR_EQ_FPS_1:
        return "FPS 1";
    case MDR_EQ_FPS_2:
        return "FPS 2";
    case MDR_EQ_FPS_3:
        return "FPS 3";
    case MDR_EQ_CUSTOM:
        return "Custom";
    case MDR_EQ_USER_1:
        return "User Setting 1";
    case MDR_EQ_USER_2:
        return "User Setting 2";
    case MDR_EQ_USER_3:
        return "User Setting 3";
    case MDR_EQ_USER_4:
        return "User Setting 4";
    case MDR_EQ_USER_5:
        return "User Setting 5";
    default:
        return "Unknown";
    }
}

const char* FormatAssignableAction(MDRAssignableAction action)
{
    switch (action)
    {
    case MDR_ASSIGNABLE_NOISE_CONTROL:
        return "Ambient Sound Control";
    case MDR_ASSIGNABLE_PLAYBACK:
        return "Playback Control";
    case MDR_ASSIGNABLE_TRACK_CONTROL:
        return "Track Control";
    case MDR_ASSIGNABLE_VOICE_ASSISTANT:
        return "Voice Assistant";
    case MDR_ASSIGNABLE_NOISE_CONTROL_QUICK_ACCESS:
        return "Ambient Sound Control";
    case MDR_ASSIGNABLE_QUICK_ACCESS:
        return "Quick Access";
    case MDR_ASSIGNABLE_NONE:
        return "No Function";
    default:
        return "Unknown";
    }
}

const char* FormatNoiseButtonMode(MDRNoiseButtonMode function)
{
    switch (function)
    {
    case MDR_NOISE_BUTTON_NONE:
        return "No Function";
    case MDR_NOISE_BUTTON_NOISE_AMBIENT_OFF:
        return "NC-ASM-OFF";
    case MDR_NOISE_BUTTON_NOISE_AMBIENT:
        return "NC-ASM";
    case MDR_NOISE_BUTTON_NOISE_OFF:
        return "NC-OFF";
    case MDR_NOISE_BUTTON_AMBIENT_OFF:
        return "ASM-OFF";
    default:
        return "Unknown";
    }
}

const char* FormatAutoPowerOff(uint32_t minutes)
{
    switch (minutes)
    {
    case 5:
        return "5 minutes of no Bluetooth connection";
    case 15:
        return "15 minutes of no Bluetooth connection";
    case 30:
        return "30 minutes of no Bluetooth connection";
    case 60:
        return "1 hour of no Bluetooth connection";
    case 180:
        return "3 hours of no Bluetooth connection";
    case 0:
        return "Do not turn off";
    default:
        return "Unknown";
    }
}

const char* FormatFeatureAvailability(MDRFeatureAvailability availability)
{
    switch (availability)
    {
    case MDR_FEATURE_AVAILABLE:
        return PSI_OK;
    case MDR_FEATURE_UNAVAILABLE:
        return PSI_REMOVE;
    default:
        return "?";
    }
}
#pragma endregion

bool FeatureAvailable(MDRFeature feature)
{
    MDRFeatureAvailability availability = MDR_FEATURE_UNKNOWN;
    return gDevice && mdrHeadphonesGetFeature(gDevice, feature, &availability) == MDR_RESULT_OK &&
        availability == MDR_FEATURE_AVAILABLE;
}

mdr::String GetText(MDRText text, uint32_t index = 0)
{
    if (!gDevice)
        return {};
    uint32_t size = 0;
    if (mdrHeadphonesGetText(gDevice, text, index, nullptr, &size) != MDR_RESULT_OK || size == 0)
        return {};
    mdr::Vector<char> buffer(size);
    if (mdrHeadphonesGetText(gDevice, text, index, buffer.data(), &size) != MDR_RESULT_OK)
        return {};
    return buffer.data();
}

uint8_t GetModelColor()
{
    MDRModel identity = MDRStruct<MDRModel>();
    return gDevice && mdrHeadphonesGetModel(gDevice, &identity) == MDR_RESULT_OK ? identity.model_color : 0;
}

mdr::Vector<MDRBattery> GetBatteries()
{
    mdr::Vector<MDRBattery> values(4);
    for (auto& value : values)
        value = MDRStruct<MDRBattery>();
    uint32_t count = static_cast<uint32_t>(values.size());
    if (!gDevice || mdrHeadphonesGetBatteries(gDevice, values.data(), &count) != MDR_RESULT_OK)
        return {};
    values.resize(count);
    return values;
}

mdr::Vector<MDRPairedDevice> GetPairedDevices()
{
    mdr::Vector<MDRPairedDevice> values(16);
    for (auto& value : values)
        value = MDRStruct<MDRPairedDevice>();
    uint32_t count = static_cast<uint32_t>(values.size());
    if (!gDevice || mdrHeadphonesGetPairedDevices(gDevice, values.data(), &count) != MDR_RESULT_OK)
        return {};
    values.resize(count);
    return values;
}

mdr::Vector<MDRGeneralSettingInfo> GetGeneralSettings()
{
    mdr::Vector<MDRGeneralSettingInfo> values(4);
    for (auto& value : values)
        value = MDRStruct<MDRGeneralSettingInfo>();
    uint32_t count = static_cast<uint32_t>(values.size());
    if (!gDevice || mdrHeadphonesGetGeneralSettingInfo(gDevice, values.data(), &count) != MDR_RESULT_OK)
        return {};
    values.resize(count);
    return values;
}

mdr::Vector<int> GetEqualizerBands()
{
    mdr::Vector<int8_t> bytes(16);
    uint32_t count = static_cast<uint32_t>(bytes.size());
    if (!gDevice || mdrHeadphonesGetEqualizerBands(gDevice, bytes.data(), &count) != MDR_RESULT_OK)
        return {};
    bytes.resize(count);
    mdr::Vector<int> values;
    values.reserve(count);
    for (const int8_t value : bytes)
        values.push_back(value);
    return values;
}

void SetEqualizerBands(const mdr::Vector<int>& values)
{
    mdr::Vector<int8_t> bytes;
    bytes.reserve(values.size());
    for (const int value : values)
        bytes.push_back(static_cast<int8_t>(value));
    if (!bytes.empty())
        mdrHeadphonesSetEqualizerBands(gDevice, bytes.data(), static_cast<uint32_t>(bytes.size()));
}

void CloseDevice()
{
    if (!gDevice)
        return;
    gHeadphonesError = GetText(MDR_TEXT_LAST_ERROR);
    clientPacketObserverDetach();
    mdrHeadphonesDestroy(gDevice);
    gDevice = nullptr;
}

#pragma region ImGui Extra
constexpr ImGuiWindowFlags kImWindowFlagsTopMost =
    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;

// -- https://github.com/ocornut/imgui/issues/3379#issuecomment-2943903877
void ImScrollWhenDraggingOnVoid(const ImVec2& delta, ImGuiMouseButton mouse_button)
{
    using namespace ImGui;

    ImGuiContext& g = *GetCurrentContext();
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID("##scrolldraggingoverlay");
    KeepAliveID(id);

    // Passing 0 to ItemHoverable means it doesn't set HoveredId, which is what we want.
    if (g.ActiveId == 0 && ItemHoverable(window->Rect(), 0, g.CurrentItemFlags) &&
        IsMouseClicked(mouse_button, ImGuiInputFlags_None, id))
        SetActiveID(id, window);
    if (g.ActiveId == id && !g.IO.MouseDown[mouse_button])
        ClearActiveID();

    // Set keep underlying highlight. However, mouse not necessarily hovering same item creates a weird disconnect.
    // if (g.ActiveId == id)
    //    g.ActiveIdAllowOverlap = true;

    // if (g.ActiveId == id && delta.x != 0.0f)
    //     SetScrollX(window, window->Scroll.x + delta.x);
    if (g.ActiveId == id && delta.y != 0.0f)
        SetScrollY(window, window->Scroll.y - delta.y);
}

void ImScrollWhenDraggingAnywhere(const ImVec2& delta, ImGuiMouseButton mouse_button)
{
    ImGuiContext& g = *ImGui::GetCurrentContext();
    const bool backup_hovered_id_allow_overlap = g.HoveredIdAllowOverlap;
    g.HoveredIdAllowOverlap = true;
    ImScrollWhenDraggingOnVoid(delta, mouse_button);
    g.HoveredIdAllowOverlap = backup_hovered_id_allow_overlap; // As we know ScrollWhenDraggingOnVoid() doesn't changed
                                                               // HoveredId we can unconditionally restore.
}
// --

// Only useful if you're manipulating the DrawList which has positions
// that are _NOT_ window local
std::tuple<ImVec2, ImVec2, ImDrawList*> ImWindowDrawOffsetRegionList()
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
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2 - size.x / 2 + ImGui::GetStyle().FramePadding.x);
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
constexpr float ImEaseLinear(float x) { return x; }

// CSS easeInOutCubic easing function on x of range [0,1]
constexpr float ImEaseInOutCubic(float x)
{
    return x < 0.5f ? 4 * pow(x, 3.0f) : 1.0f - pow(-2.0f * x + 2.0f, 3.0f) / 2.0f;
}

// Your next favourite spinner
void ImSpinner(float interval, float size, int color, float thickness = 1.0f, bool centerX = false,
               bool centerY = false, float cycles = 1.0f, float (*easing)(float) = ImEaseLinear)
{
    constexpr ImVec2 kPoints[] = {{-1, 1}, {-1, -1}, {1, -1}, {1, 1}};
    auto& style = ImGui::GetStyle();
    ImVec2 points[std::size(kPoints)];
    if (centerX)
        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2 - size / 2);
    if (centerY)
        ImGui::SetCursorPosY((ImGui::GetTextLineHeight() + style.FramePadding.y * 2 - size) / 2);
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
    ImGui::Dummy({sqrt(2.0f) * size, sqrt(2.0f) * size + style.FramePadding.y * 2.0f});
}

// Fill the available horizontal region with lineTotal amount of buttons
// This is used for modal dialogues
bool ImModalButton(const char* label, int lineIndex = 0, int lineTotal = 1)
{
    assert(lineIndex < lineTotal);
    auto& style = ImGui::GetStyle();
    float padding = style.FramePadding.x;
    float width = ImGui::GetContentRegionAvail().x / lineTotal;
    if (lineIndex)
        ImGui::SameLine();
    return ImGui::Button(label, lineTotal > 1 ? ImVec2{width - padding, 0} : ImVec2{width, 0});
}

void ImSetNextWindowCentered()
{
    auto& style = ImGui::GetStyle();
    float padding = style.FramePadding.x;
    ImGui::SetNextWindowPos({0.0f, ImGui::GetContentRegionAvail().y / 2 + padding}, 0, {0.0f, 0.5f});
    ImGui::SetNextWindowSize({ImGui::GetIO().DisplaySize.x, 0});
}

void ImTextWithBorder(const char* text, int color, float rounding = 0.0f, float thickness = 1.0f)
{
    auto& style = ImGui::GetStyle();
    ImVec2 size = ImGui::CalcTextSize(text);
    auto [offset, region, draw] = ImWindowDrawOffsetRegionList();
    ImVec2 pad = style.FramePadding / 2;
    ImGui::Text("%s", text);
    offset.y += style.FramePadding.y;
    draw->AddRect(offset - pad, offset + size + pad, color, rounding, ImDrawFlags_None, thickness);
    ImGui::Dummy({pad.x, 0});
}

template <typename T, size_t Extent, typename Formatter>
bool ImComboBoxItems(const char* label, std::span<const T, Extent> items, T& selection, Formatter format)
{
    bool changed = false;
    if (ImGui::BeginCombo(label, format(selection)))
    {
        for (T const& i : items)
        {
            bool selected = i == selection;
            if (ImGui::Selectable(format(i), selected))
                selection = i, changed = true;
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    return changed;
}

bool ImEqualizer(std::span<int> bands)
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
    {
        ImGui::Text("EQ Unavailable (bands=%d)", numBands);
        return false;
    }
    bool changed = false;
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
        changed |= ImGui::VSliderInt("##v", ImVec2{bandWidth, bandHeight}, &bands[i], mn, mx);
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
    return changed;
}
struct ImStylesRAII
{
    size_t numVars = 0, numColors = 0, numFonts = 0;
    template <typename... Args>
    void PushVar(ImGuiStyleVar idx, Args&&... args)
    {
        ImGui::PushStyleVar(idx, args...), numVars++;
    }
    template <typename... Args>
    void PushCol(ImGuiCol idx, Args&&... args)
    {
        ImGui::PushStyleColor(idx, args...), numColors++;
    }
    template <typename... Args>
    void PushFont(ImFont* font, Args&&... args)
    {
        ImGui::PushFont(font, args...), numFonts++;
    }
    ~ImStylesRAII()
    {
        ImGui::PopStyleVar(numVars);
        ImGui::PopStyleColor(numColors);
        while (numFonts--)
            ImGui::PopFont();
    }
};
#pragma endregion

#pragma region States
enum CONN_STATE
{
    CONN_STATE_NO_CONNECTION,
    CONN_STATE_CONNECTING,
    CONN_STATE_CONNECTED,
    CONN_STATE_DISCONNECTED // Passive, or from errors
} connState{};

enum DEVICE_TYPE
{
    DEVICE_TYPE_AUTO,
    DEVICE_TYPE_V2,
    DEVICE_TYPE_V1
};

struct ConnectionAttemptState
{
    static constexpr uint64_t kAttemptTimeoutMs = 10'000;

    std::string address;
    std::array<const char*, 2> services{};
    std::string lastError;
    size_t serviceCount{};
    size_t serviceIndex{};
    uint64_t deadlineMs{};
    bool ble{};
};

ConnectionAttemptState connectionAttempt;

const char* ConnectionAttemptName()
{
    if (connectionAttempt.ble)
        return "BLE";
    if (connectionAttempt.serviceCount == 1)
        return std::strcmp(connectionAttempt.services[0], MDR_SERVICE_UUID_LEGACY) == 0 ? "V1" : "V2";
    return connectionAttempt.serviceIndex == 0 ? "V2" : "V1";
}

void CaptureConnectionError(MDRConnection* conn, MDRResult result)
{
    const char* error = mdrConnectionGetLastError(conn);
    connectionAttempt.lastError = error && *error ? error : mdrResultString(result);
}

MDRResult TryConnectionAttempt(MDRConnection* conn)
{
    while (connectionAttempt.serviceIndex < connectionAttempt.serviceCount)
    {
        const MDRResult result =
            mdrConnectionConnect(conn, connectionAttempt.address.c_str(),
                                 connectionAttempt.services[connectionAttempt.serviceIndex]);
        if (result == MDR_RESULT_OK || result == MDR_RESULT_INPROGRESS)
        {
            connectionAttempt.deadlineMs = SDL_GetTicks() + ConnectionAttemptState::kAttemptTimeoutMs;
            return result;
        }

        CaptureConnectionError(conn, result);
        mdrConnectionDisconnect(conn);
        ++connectionAttempt.serviceIndex;
    }
    return MDR_RESULT_ERROR_NO_CONNECTION;
}

MDRResult AdvanceConnectionAttempt(MDRConnection* conn, MDRResult reason)
{
    CaptureConnectionError(conn, reason);
    mdrConnectionDisconnect(conn);
    ++connectionAttempt.serviceIndex;
    return TryConnectionAttempt(conn);
}

MDRResult StartConnection(
    MDRConnection* conn,
    const char* address,
    bool usingBLE,
    DEVICE_TYPE deviceType)
{
    connectionAttempt = {};
    connectionAttempt.address = address;
    connectionAttempt.ble = usingBLE;
    if (usingBLE)
    {
        connectionAttempt.services[0] = MDR_BLE_SERVICE_UUID_TANDEM_OVER_BLE_HPC;
        connectionAttempt.serviceCount = 1;
    }
    else if (deviceType == DEVICE_TYPE_AUTO)
    {
        connectionAttempt.services = {MDR_SERVICE_UUID_XM5, MDR_SERVICE_UUID_LEGACY};
        connectionAttempt.serviceCount = 2;
    }
    else
    {
        connectionAttempt.services[0] =
            deviceType == DEVICE_TYPE_V2 ? MDR_SERVICE_UUID_XM5 : MDR_SERVICE_UUID_LEGACY;
        connectionAttempt.serviceCount = 1;
    }
    return TryConnectionAttempt(conn);
}
#pragma endregion

void DrawDeviceDiscovery()
{
    assert(connState == CONN_STATE_NO_CONNECTION);
    ImSetNextWindowCentered();
    static bool popup = false;
    if (!popup)
        ImGui::OpenPopup("DeviceDiscovery"), popup = true;
    if (ImGui::BeginPopupModal("DeviceDiscovery", nullptr, kImWindowFlagsTopMost))
    {
        static MDRDeviceInfo* pDeviceInfo = nullptr;
        static int nDeviceInfo = 0;
        ImGui::PushFont(nullptr, ImGui::GetContentRegionAvail().x * 0.05f);
        ImTextCentered("SonyHeadphonesClient");
        ImGui::PopFont();
        ImTextCentered(mdr::Format("Version: {}, Branch: {}, Commit: {}, On {}", CLIENT_VERSION, MDR_GIT_BRANCH_NAME,
                                   MDR_GIT_COMMIT_HASH, MDR_PLATFORM_OS)
                           .c_str());
        // Chose, and have the GATT backend active
        static bool usingBLE = false;
        static DEVICE_TYPE deviceType = DEVICE_TYPE_AUTO;
        static int connInitResult = MDR_RESULT_INPROGRESS;
        // BLE / Classic toggle
        bool needSwitchClientPlatform = clientPlatformConnectionGet() == nullptr;
        {
            ImStylesRAII styles;
            styles.PushFont(nullptr, 12.0f);
            styles.PushVar(ImGuiStyleVar_FramePadding, ImVec2{});
            styles.PushVar(ImGuiStyleVar_FrameRounding, 0.0f);
            {
                ImStylesRAII styles;
                if (usingBLE)
                    styles.PushCol(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
                if (ImModalButton(PSI_BLUETOOTH " Classic", 0, 2))
                    usingBLE = false, needSwitchClientPlatform = true;
            }
            {
                ImStylesRAII styles;
                if (!usingBLE)
                    styles.PushCol(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
                if (ImModalButton(PSI_BLUETOOTH_ALT " BLE (GATT)", 1, 2))
                    usingBLE = true, needSwitchClientPlatform = true;
            }
        }
        ImGui::BeginDisabled(usingBLE);
        {
            ImStylesRAII styles;
            styles.PushFont(nullptr, 12.0f);
            styles.PushVar(ImGuiStyleVar_FramePadding, ImVec2{});
            styles.PushVar(ImGuiStyleVar_FrameRounding, 0.0f);
            constexpr std::array labels{PSI_PLUS_SIGN " Auto", PSI_FAST_FORWARD " V2", PSI_FORWARD " V1"};
            for (int i = 0; i < static_cast<int>(labels.size()); ++i)
            {
                ImStylesRAII buttonStyles;
                if (deviceType != i)
                    buttonStyles.PushCol(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
                if (ImModalButton(labels[i], i, static_cast<int>(labels.size())))
                    deviceType = static_cast<DEVICE_TYPE>(i);
            }
        }
        ImGui::EndDisabled();
        auto RefreshDeviceList = [&]()
        {
            MDRConnection* conn = clientPlatformConnectionGet();
            if (conn) // TODO: Error modals
                mdrConnectionGetDevicesList(conn, &pDeviceInfo, &nDeviceInfo);
        };
        if (needSwitchClientPlatform)
        {
            int flags = 0;
            if (usingBLE)
                flags |= MDR_INIT_BT_BLE;
            MDRConnection* conn = clientPlatformConnectionGet();
            if (conn && pDeviceInfo)
                mdrConnectionFreeDevicesList(conn, &pDeviceInfo), pDeviceInfo = nullptr, nDeviceInfo = 0;
            CloseDevice();
            clientPlatformConnectionDestroy();
            connInitResult = clientPlatformConnectionInit(flags);
            RefreshDeviceList();
        }
        auto DrawDeviceList = [&]()
        {
            ImGui::SeparatorText("Available Devices");
            static int deviceIndex = 0;
            std::span<MDRDeviceInfo> devices{pDeviceInfo, static_cast<size_t>(nDeviceInfo)};
            if (!devices.empty())
            {
                int btnIndex = 0;
                for (const auto& device : devices)
                {
                    ImGui::PushID(device.szDeviceMacAddress);
                    ImGui::RadioButton(device.szDeviceName, &deviceIndex, btnIndex++);
                    ImGui::PopID();
                }
            }
            else
            {
                ImGui::TextWrapped(PSI_WARNING_SIGN " No devices available. Make sure your Bluetooth radio is turned "
                                                    "on, and a compatible device is connected.");
            }
            ImGui::BeginDisabled(devices.empty());
            if (ImModalButton(PSI_LINK " Connect", 0, 2))
            {
                const int res =
                    StartConnection(clientPlatformConnectionGet(), devices[deviceIndex].szDeviceMacAddress,
                                    usingBLE, deviceType);
                if (res != MDR_RESULT_OK && res != MDR_RESULT_INPROGRESS)
                    connState = CONN_STATE_DISCONNECTED;
                else
                    connState = CONN_STATE_CONNECTING;
            }
            ImGui::EndDisabled();
            if (ImModalButton(PSI_REFRESH " Refresh", 1, 2))
                RefreshDeviceList();
        };
        if (connInitResult != MDR_RESULT_OK && connInitResult != MDR_RESULT_INPROGRESS)
        {
            ImTextCentered(mdr::Format(PSI_EXCLAMATION_SIGN " Failed to initialize connection: {}",
                                       mdrResultString(connInitResult))
                               .c_str());
        }
        DrawDeviceList();
        ImGui::SeparatorText(PSI_INFO_SIGN_ALT " Select BLE (GATT) if your device is connected via LE Audio, and "
                                               "Classic if you don't know what that means or otherwise.");
        ImTextCentered(PSI_WARNING_SIGN
                       " This product is not affiliated with Sony. Use at your own risk. " PSI_WARNING_SIGN);
#ifdef MDR_CLIENT_DEBUGGER
        ImGui::Separator();
        if (ImModalButton("Protocol Debugger"))
            gDebuggerOpen = true;
#endif
        ImGui::EndPopup();
    }
    else
        popup = false;
}

// NOTE: Only CONN_STATE_DISCONNECTED state shows the modal
void DisconnectWithModal()
{
    MDRConnection* conn = clientPlatformConnectionGet();
    connState = CONN_STATE_DISCONNECTED;
    CloseDevice();
    mdrConnectionDisconnect(conn);
}

void DrawDeviceConnecting()
{
    assert(connState == CONN_STATE_CONNECTING);
    MDRConnection* conn = clientPlatformConnectionGet();
    MDRResult pollResult = mdrConnectionPoll(conn, 0);
    if (pollResult != MDR_RESULT_OK)
    {
        const bool attemptTimedOut =
            (pollResult == MDR_RESULT_INPROGRESS || pollResult == MDR_RESULT_ERROR_TIMEOUT) &&
            SDL_GetTicks() >= connectionAttempt.deadlineMs;
        const bool attemptFailed =
            pollResult != MDR_RESULT_INPROGRESS && pollResult != MDR_RESULT_ERROR_TIMEOUT;
        if (attemptTimedOut || attemptFailed)
        {
            pollResult =
                AdvanceConnectionAttempt(conn, attemptTimedOut ? MDR_RESULT_ERROR_TIMEOUT : pollResult);
            if (pollResult != MDR_RESULT_OK && pollResult != MDR_RESULT_INPROGRESS)
            {
                connState = CONN_STATE_DISCONNECTED;
                CloseDevice();
                MaterialYouTheme::ApplyDefault();
                return;
            }
        }
    }
    switch (pollResult)
    {
    case MDR_RESULT_OK:
        connState = CONN_STATE_CONNECTED;
        connectionAttempt.lastError.clear();
        CloseDevice();
        if (mdrHeadphonesCreate(conn, &gDevice) != MDR_RESULT_OK)
        {
            DisconnectWithModal();
            return;
        }
        clientPacketObserverAttach(gDevice);
        if (mdrHeadphonesRequestInit(gDevice) != MDR_RESULT_OK)
            DisconnectWithModal();

        return;
    case MDR_RESULT_ERROR_TIMEOUT:
    case MDR_RESULT_INPROGRESS:
        {
            ImSetNextWindowCentered();
            static bool popup = false;
            if (!popup)
                ImGui::OpenPopup("Connection"), popup = true;
            if (ImGui::BeginPopupModal("Connection", nullptr, kImWindowFlagsTopMost))
            {
                ImGui::NewLine();
                ImTextCentered("Connecting...");
                ImTextCentered(mdr::Format("Device type: {}", ConnectionAttemptName()).c_str());
                ImGui::Dummy({0, 16.0f});
                ImSpinner(1000.0f, 24.0f,
                          MaterialYouTheme::ArgbToImU32(MaterialYouTheme::FixedSurfaceColors::onSurface), 2.0f, true,
                          false, 2.0f, ImEaseInOutCubic);
                ImGui::NewLine();
                ImTextCentered(mdrConnectionGetLastError(conn));
                ImGui::NewLine();
                if (ImModalButton(PSI_REMOVE " Cancel"))
                {
                    CloseDevice();
                    mdrConnectionDisconnect(conn);
                    connectionAttempt = {};
                    connState = CONN_STATE_NO_CONNECTION;
                }
                ImGui::EndPopup();
            }
            else
                popup = false;
            return;
        }
    default:
        {
            CaptureConnectionError(conn, pollResult);
            connState = CONN_STATE_DISCONNECTED;
            CloseDevice();
            mdrConnectionDisconnect(conn);
            MaterialYouTheme::ApplyDefault();
            break;
        }
    }
}

void DrawDeviceControlsHeader()
{
    MDRConnection* conn = clientPlatformConnectionGet();
    const mdr::String modelName = GetText(MDR_TEXT_MODEL_NAME);
    if (ImGui::BeginMenuBar())
    {
        auto& style = ImGui::GetStyle();
        /* Disconnect & Shutdown */
        if (ImGui::BeginMenu(mdr::Format(PSI_CHEVRON_DOWN " {}", modelName).c_str()))
        {
            if (ImGui::MenuItem(PSI_UNLINK " Disconnect"))
            {
                CloseDevice();
                mdrConnectionDisconnect(conn);
                connState = CONN_STATE_NO_CONNECTION;
            }
            if (FeatureAvailable(MDR_FEATURE_SHUTDOWN))
            {
                if (ImGui::MenuItem(PSI_OFF " Shutdown"))
                {
                    MDRPower power = MDRStruct<MDRPower>();
                    if (mdrHeadphonesGetPower(gDevice, &power) == MDR_RESULT_OK)
                    {
                        power.shutdown_requested = MDR_TRUE;
                        mdrHeadphonesSetPower(gDevice, &power);
                    }
                }
            }
#ifdef MDR_CLIENT_DEBUGGER
            ImGui::Separator();
            ImGui::MenuItem("Protocol Debugger", nullptr, &gDebuggerOpen);
#endif
            ImGui::EndMenu();
        }
        if (!gDevice)
        {
            ImGui::EndMenuBar();
            return;
        }
        if (!mdrHeadphonesIsReady(gDevice))
            ImSpinner(1000, style.FontSizeBase * 0.5f,
                      MaterialYouTheme::ArgbToImU32(MaterialYouTheme::FixedSurfaceColors::onSurface, 0.5f), 2.0f, false,
                      true, 1.0f, ImEaseInOutCubic);
        /* Cool Badges */
        // Title, Border Color, Text Color
        using Badge = std::tuple<const char*, int, int>;
        std::array<Badge, 4> badges4;
        Badge *badgeFirst = &badges4[0], *badgeLast = &badges4[0];
        /* Codec */
        MDRModel identity = MDRStruct<MDRModel>();
        if (mdrHeadphonesGetModel(gDevice, &identity) == MDR_RESULT_OK &&
            identity.audio_codec != MDR_AUDIO_CODEC_UNKNOWN)
        {
            *(badgeLast++) = {FormatAudioCodec(identity.audio_codec), ~0u, ~0u};
        }
        /* DSEE */
        MDREqualizer equalizer = MDRStruct<MDREqualizer>();
        if (FeatureAvailable(MDR_FEATURE_DSEE) &&
            mdrHeadphonesGetEqualizer(gDevice, &equalizer) == MDR_RESULT_OK &&
            equalizer.dsee_enabled)
        {
            *(badgeLast++) = {FormatDseeType(equalizer.dsee_type), ~0u, ~0u};
        }
        std::span<Badge> badges{badgeFirst, static_cast<size_t>(badgeLast - badgeFirst)};
        // Right-align and draw them
        // XXX: This is surprisingly painful to do.
        ImVec2 padding = style.FramePadding;
        float badgeRegionX = 0, badgeRegionY = 0;
        ImGui::PushFont(ImGui::GetFont(), style.FontSizeBase - padding.y / 2);
        for (auto& [s, border, text] : badges)
        {
            ImVec2 size = ImGui::CalcTextSize(s);
            badgeRegionX += size.x + padding.x * 2, badgeRegionY = std::max(badgeRegionY, size.y);
        }
        ImGui::SameLine(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - badgeRegionX - padding.x * 2);
        float rounding = style.FrameRounding;
        float offsetY = padding.y / 2;
        for (auto& [s, border, text] : badges)
        {
            ImGui::SetCursorPosY(offsetY);
            ImTextWithBorder(s, border, rounding, 2.0f);
        }
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
            if (ImGui::BeginTable("##Battery", 2, ImGuiTableFlags_SizingStretchProp))
            {
                for (const MDRBattery& battery : GetBatteries())
                {
                    if (!battery.present || !battery.update_threshold_percent)
                        continue;
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    const char* label = battery.part == MDR_BATTERY_LEFT ? "L" :
                        battery.part == MDR_BATTERY_RIGHT ? "R" :
                        battery.part == MDR_BATTERY_CASE ? "Case" : "Battery";
                    ImGui::Text("%s: %u%%", label, static_cast<unsigned>(battery.level_percent));
                    ImGui::TableSetColumnIndex(1);
                    ImGui::ProgressBar(
                        battery.level_percent / 100.0f, {-1, 0}, FormatChargingState(battery.charging));
                }
                ImGui::EndTable();
            }
        }
        ImGui::TableSetColumnIndex(1);
        /* Now Playing */
        {
            ImGui::Text(PSI_VOLUME_UP " Now Playing");
            if (ImGui::BeginTable("##NowPlaying", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerH))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Title");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", GetText(MDR_TEXT_TRACK_TITLE).c_str());
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Album");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", GetText(MDR_TEXT_TRACK_ALBUM).c_str());
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Artist");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", GetText(MDR_TEXT_TRACK_ARTIST).c_str());
                ImGui::EndTable();
            }
        }
        ImGui::EndTable();
    }
}

void DrawDeviceControlsPlayback()
{
    MDRPlayback playback = MDRStruct<MDRPlayback>();
    if (mdrHeadphonesGetPlayback(gDevice, &playback) != MDR_RESULT_OK)
        return;
    ImGui::SeparatorText("Volume");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    int volume = playback.volume;
    if (ImGui::SliderInt("##Volume", &volume, 0, 30))
    {
        playback.volume = static_cast<uint8_t>(volume);
        mdrHeadphonesSetPlayback(gDevice, &playback);
    }
    ImGui::SeparatorText("Controls");
    if (ImModalButton(PSI_STEP_BACKWARD " Prev", 0, 3))
    {
        MDRPlaybackCommand command = MDRStruct<MDRPlaybackCommand>();
        command.action = MDR_PLAYBACK_PREVIOUS;
        mdrHeadphonesPlayback(gDevice, &command);
    }
    if (playback.status == MDR_PLAYBACK_PLAYING)
    {
        if (ImModalButton(PSI_PAUSE " Pause", 1, 3))
        {
            MDRPlaybackCommand command = MDRStruct<MDRPlaybackCommand>();
            command.action = MDR_PLAYBACK_PAUSE;
            mdrHeadphonesPlayback(gDevice, &command);
        }
    }
    else
    {
        if (ImModalButton(PSI_PLAY " Play", 1, 3))
        {
            MDRPlaybackCommand command = MDRStruct<MDRPlaybackCommand>();
            command.action = MDR_PLAYBACK_PLAY;
            mdrHeadphonesPlayback(gDevice, &command);
        }
    }
    if (ImModalButton(PSI_STEP_FORWARD "Next", 2, 3))
    {
        MDRPlaybackCommand command = MDRStruct<MDRPlaybackCommand>();
        command.action = MDR_PLAYBACK_NEXT;
        mdrHeadphonesPlayback(gDevice, &command);
    }
}

void DrawDeviceControlsSound()
{
    const bool supportNC = FeatureAvailable(MDR_FEATURE_NOISE_CANCELLING);
    const bool supportASM = FeatureAvailable(MDR_FEATURE_AMBIENT_SOUND);
    const bool supportAutoASM = FeatureAvailable(MDR_FEATURE_ADAPTIVE_AMBIENT_SOUND);
    /* NC/ASM */
    if (supportASM || supportNC)
    {
        MDRNoiseControl noise = MDRStruct<MDRNoiseControl>();
        const bool haveNoise =
            mdrHeadphonesGetNoiseControl(gDevice, &noise) == MDR_RESULT_OK;
        if (ImGui::TreeNodeEx("Ambient Sound", ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool changed = false;
            if (supportNC)
            {
                if (ImGui::RadioButton("Noise Cancelling", noise.mode == MDR_NOISE_MODE_CANCELLING))
                {
                    noise.mode = MDR_NOISE_MODE_CANCELLING;
                    changed = true;
                }
                ImGui::SameLine();
            }
            if (supportASM)
            {
                if (ImGui::RadioButton("Ambient Sound", noise.mode == MDR_NOISE_MODE_AMBIENT))
                {
                    noise.mode = MDR_NOISE_MODE_AMBIENT;
                    if (noise.ambient_level == 0)
                        noise.ambient_level = 20;
                    changed = true;
                }
                ImGui::SameLine();
            }
            if (ImGui::RadioButton("Off", noise.mode == MDR_NOISE_MODE_OFF))
                noise.mode = MDR_NOISE_MODE_OFF, changed = true;
            ImGui::SeparatorText("Ambient Strength");
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            int ambientLevel = noise.ambient_level;
            if (ImGui::SliderInt("##AmbStrength", &ambientLevel, 1, 20))
                noise.ambient_level = static_cast<uint8_t>(ambientLevel), changed = true;
            if (supportAutoASM)
            {
                bool adaptive = noise.adaptive_ambient != MDR_FALSE;
                if (ImGui::Checkbox("Auto Ambient Sound", &adaptive))
                    noise.adaptive_ambient = adaptive ? MDR_TRUE : MDR_FALSE, changed = true;
                ImGui::BeginDisabled(!adaptive);
                constexpr MDRAdaptiveSensitivity kSelections[] = {
                    MDR_ADAPTIVE_SENSITIVITY_STANDARD, MDR_ADAPTIVE_SENSITIVITY_HIGH,
                    MDR_ADAPTIVE_SENSITIVITY_LOW};
                changed |= ImComboBoxItems(
                    "Sensitivity", std::span{kSelections}, noise.adaptive_sensitivity, FormatAdaptiveSensitivity);
                ImGui::EndDisabled();
            }
            bool focusOnVoice = noise.focus_on_voice != MDR_FALSE;
            if (ImGui::Checkbox("Voice Passthrough", &focusOnVoice))
                noise.focus_on_voice = focusOnVoice ? MDR_TRUE : MDR_FALSE, changed = true;
            if (changed && haveNoise)
                mdrHeadphonesSetNoiseControl(gDevice, &noise);
            ImGui::TreePop();
        }
    }
    /* STC */
    if (FeatureAvailable(MDR_FEATURE_SPEAK_TO_CHAT))
    {
        MDRSpeakToChat speak = MDRStruct<MDRSpeakToChat>();
        const bool haveSpeak =
            mdrHeadphonesGetSpeakToChat(gDevice, &speak) == MDR_RESULT_OK;
        if (ImGui::TreeNodeEx("Speak To Chat", ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool changed = false;
            bool enabled = speak.enabled != MDR_FALSE;
            if (ImGui::Checkbox("Enabled", &enabled))
                speak.enabled = enabled ? MDR_TRUE : MDR_FALSE, changed = true;
            ImGui::BeginDisabled(!enabled);
            constexpr MDRSpeechSensitivity kSensitivity[] = {
                MDR_SPEECH_SENSITIVITY_AUTO, MDR_SPEECH_SENSITIVITY_HIGH, MDR_SPEECH_SENSITIVITY_LOW};
            changed |= ImComboBoxItems(
                "Sensitivity", std::span{kSensitivity}, speak.sensitivity, FormatSpeechSensitivity);
            constexpr MDRSpeakTimeout kTimeout[] = {
                MDR_SPEAK_TIMEOUT_SHORT, MDR_SPEAK_TIMEOUT_MEDIUM, MDR_SPEAK_TIMEOUT_LONG,
                MDR_SPEAK_TIMEOUT_MANUAL};
            changed |= ImComboBoxItems(
                "Mode Duration", std::span{kTimeout}, speak.timeout, FormatSpeakTimeout);
            ImGui::EndDisabled();
            if (changed && haveSpeak)
                mdrHeadphonesSetSpeakToChat(gDevice, &speak);
            ImGui::TreePop();
        }
    }
    /* Listening Mode */
    if (FeatureAvailable(MDR_FEATURE_LISTENING_MODE))
    {
        MDRListening listening = MDRStruct<MDRListening>();
        const bool haveListening =
            mdrHeadphonesGetListening(gDevice, &listening) == MDR_RESULT_OK;
        if (ImGui::TreeNodeEx("Listening Mode", ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool changed = false;
            if (ImGui::RadioButton("Standard", listening.mode == MDR_LISTENING_STANDARD))
                listening.mode = MDR_LISTENING_STANDARD, changed = true;
            if (ImGui::RadioButton("BGM", listening.mode == MDR_LISTENING_BACKGROUND_MUSIC))
                listening.mode = MDR_LISTENING_BACKGROUND_MUSIC, changed = true;

            ImGui::Indent();
            ImGui::BeginDisabled(listening.mode != MDR_LISTENING_BACKGROUND_MUSIC);
            static const std::pair<MDRRoomSize, const char*> kBGMDistanceModes[] = {
                {MDR_ROOM_SMALL, "My Room"},
                {MDR_ROOM_MEDIUM, "Living Room"},
                {MDR_ROOM_LARGE, "Cafe"},
            };
            const char* currentDistStr = "Unknown";
            for (auto const& [k, v] : kBGMDistanceModes)
                if (k == listening.background_room)
                    currentDistStr = v;
            if (ImGui::BeginCombo("Distance", currentDistStr))
            {
                for (auto const& [k, v] : kBGMDistanceModes)
                {
                    bool is_selected = k == listening.background_room;
                    if (ImGui::Selectable(v, is_selected))
                        listening.background_room = k, changed = true;
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::EndDisabled();
            ImGui::Unindent();

            if (ImGui::RadioButton("Cinema", listening.mode == MDR_LISTENING_CINEMA))
                listening.mode = MDR_LISTENING_CINEMA, changed = true;

            if (changed && haveListening)
                mdrHeadphonesSetListening(gDevice, &listening);
            ImGui::TreePop();
        }
    }
    /* EQ & DSEE */
    if (ImGui::TreeNodeEx("Equalizer & DSEE", ImGuiTreeNodeFlags_DefaultOpen))
    {
        MDREqualizer equalizer = MDRStruct<MDREqualizer>();
        const bool haveEqualizer =
            mdrHeadphonesGetEqualizer(gDevice, &equalizer) == MDR_RESULT_OK;
        bool changed = false;
        constexpr MDREqualizerPreset kSelections[] = {
            MDR_EQ_OFF, MDR_EQ_ROCK, MDR_EQ_POP, MDR_EQ_JAZZ, MDR_EQ_DANCE, MDR_EQ_EDM,
            MDR_EQ_R_AND_B_HIP_HOP, MDR_EQ_ACOUSTIC, MDR_EQ_BRIGHT, MDR_EQ_EXCITED, MDR_EQ_MELLOW,
            MDR_EQ_RELAXED, MDR_EQ_VOCAL, MDR_EQ_TREBLE, MDR_EQ_BASS, MDR_EQ_SPEECH, MDR_EQ_HEAVY,
            MDR_EQ_CLEAR, MDR_EQ_HARD, MDR_EQ_SOFT, MDR_EQ_GAMING, MDR_EQ_FPS_1, MDR_EQ_FPS_2,
            MDR_EQ_FPS_3, MDR_EQ_CUSTOM, MDR_EQ_USER_1, MDR_EQ_USER_2, MDR_EQ_USER_3, MDR_EQ_USER_4,
            MDR_EQ_USER_5};
        changed |= ImComboBoxItems(
            "Preset", std::span{kSelections}, equalizer.preset, FormatEqualizerPreset);
        mdr::Vector<int> bands = GetEqualizerBands();
        if (ImEqualizer(bands))
            SetEqualizerBands(bands);
        if (bands.size() == 5)
        {
            ImGui::SeparatorText("Clear Bass");
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            int clearBass = equalizer.clear_bass;
            if (ImGui::SliderInt("##", &clearBass, -10, 10))
                equalizer.clear_bass = static_cast<int8_t>(clearBass), changed = true;
        }
        ImGui::SeparatorText("DSEE");
        ImGui::BeginDisabled(!FeatureAvailable(MDR_FEATURE_DSEE));
        if (ImGui::RadioButton("Off", equalizer.dsee_enabled == MDR_FALSE))
            equalizer.dsee_enabled = MDR_FALSE, changed = true;
        if (ImGui::RadioButton("On (Auto)", equalizer.dsee_enabled != MDR_FALSE))
            equalizer.dsee_enabled = MDR_TRUE, changed = true;
        ImGui::EndDisabled();
        if (changed && haveEqualizer)
            mdrHeadphonesSetEqualizer(gDevice, &equalizer);
        ImGui::TreePop();
    }
}

void DrawDeviceControlsDevices()
{
    const bool supportDeviceMgmt = FeatureAvailable(MDR_FEATURE_PAIRED_DEVICE_MANAGEMENT);
    if (!supportDeviceMgmt)
        ImGui::Text("Please enable \"Connect to 2 devices simultaneously\" in System settings to manage devices.");
    ImGui::BeginDisabled(!supportDeviceMgmt);
    struct DeviceView
    {
        MDRPairedDevice state;
        mdr::String id;
        mdr::String name;
    };
    mdr::Vector<DeviceView> devices;
    for (const MDRPairedDevice& state : GetPairedDevices())
        devices.push_back({state, GetText(MDR_TEXT_PAIRED_DEVICE_ID, state.index),
                           GetText(MDR_TEXT_PAIRED_DEVICE_NAME, state.index)});
    auto StageDeviceAction = [](MDRPairedDeviceCommand command, const mdr::String& id)
    {
        MDRPairedDeviceAction action = MDRStruct<MDRPairedDeviceAction>();
        action.command = command;
        action.device_id = id.c_str();
        action.device_id_size = static_cast<uint32_t>(id.size() + 1);
        mdrHeadphonesSetPairedDevice(gDevice, &action);
    };
    auto DrawDeviceElement = [&](const DeviceView& device, bool selected) -> bool
    {
        ImGui::BeginGroup();
        if (device.state.playback_device)
            ImGui::Text(PSI_VOLUME_DOWN " "), ImGui::SameLine();
        bool res = ImGui::Selectable(device.name.c_str(), selected);
        if (selected)
        {
            ImGui::Separator();
            if (device.state.connected)
            {
                if (ImModalButton(PSI_UNLINK " Disconnect", 0, 2))
                    StageDeviceAction(MDR_PAIRED_DEVICE_DISCONNECT, device.id);
                if (res)
                    StageDeviceAction(MDR_PAIRED_DEVICE_SELECT_PLAYBACK, device.id);
            }
            else
            {
                if (ImModalButton(PSI_LINK " Connect", 0, 2))
                    StageDeviceAction(MDR_PAIRED_DEVICE_CONNECT, device.id);
            }
            if (ImModalButton(PSI_BLUETOOTH_ALT " Unpair", 1, 2))
                StageDeviceAction(MDR_PAIRED_DEVICE_UNPAIR, device.id);
        }
        ImGui::EndGroup();
        return res;
    };
    static mdr::String connectSelectedMac;
    if (ImGui::TreeNodeEx("Connected", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto& device : devices)
            if (device.state.connected && DrawDeviceElement(device, connectSelectedMac == device.id))
                connectSelectedMac = connectSelectedMac == device.id ? "" : device.id;
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Paired", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto& device : devices)
            if (!device.state.connected && DrawDeviceElement(device, connectSelectedMac == device.id))
                connectSelectedMac = connectSelectedMac == device.id ? "" : device.id;
        ImGui::TreePop();
    }
    MDRPairing pairing = MDRStruct<MDRPairing>();
    const bool havePairing =
        mdrHeadphonesGetPairing(gDevice, &pairing) == MDR_RESULT_OK;
    if (pairing.enabled)
    {
        ImTextCentered("Pairing...");
        ImSpinner(1000.0f, 16.0f,
                  MaterialYouTheme::ArgbToImU32(MaterialYouTheme::ThemeForModelColor(GetModelColor()).primary),
                  2.0f, true, false, 1.0f, ImEaseInOutCubic);
        if (ImModalButton("Stop"))
        {
            pairing.enabled = MDR_FALSE;
            if (havePairing)
                mdrHeadphonesSetPairing(gDevice, &pairing);
        }
    }
    else
    {
        if (ImModalButton(PSI_BLUETOOTH " Enter Pairing Mode"))
        {
            pairing.enabled = MDR_TRUE;
            if (havePairing)
                mdrHeadphonesSetPairing(gDevice, &pairing);
        }
        ImGui::TextWrapped(PSI_INFO_SIGN_ALT " For TWS (Earbuds) devices, you may need to take both of your headphones "
                                             "out from your case to enter Pairing Mode.");
    }
    ImGui::EndDisabled();
}

void DrawDeviceControlsSystem()
{
    /* General Settings */
    if (ImGui::TreeNodeEx("General Setting", ImGuiTreeNodeFlags_DefaultOpen))
    {
        using StringPair = std::pair<const char*, const char*>;
        constexpr auto kFormatGSString = [](const char* key, std::span<const StringPair> strings) -> const char*
        {
            auto it = std::lower_bound(strings.begin(), strings.end(), key, [](const StringPair& lhs, const char* rhs)
                                       { return strcmp(lhs.first, rhs) < 0; });
            if (it == strings.end() || strcmp(it->first, key) != 0)
                return "<Unknown>";
            return it->second;
        };
        constexpr StringPair kGSSubjectStrings[] = {{"MULTIPOINT_SETTING", "Connect to 2 devices simultaneously"},
                                                    {"SIDETONE_SETTING", "Capture Voice During a Phone Call"},
                                                    {"TOUCH_PANEL_SETTING", "Touch sensor control panel"}};
        constexpr StringPair kGSSummaryStrings[] = {
            {"MULTIPOINT_SETTING_SUMMARY",
             "For example, when using the audio device with both a PC and a smartphone, you can use it comfortably "
             "without needing to switch connections. During simultaneous connections, playback with the LDAC codec "
             "is not possible even if Prioritize Sound Quality is selected."},
            {"MULTIPOINT_SETTING_SUMMARY_LDAC_AVAILABLE",
             "For example, when using the audio device with both a PC and a smartphone, you can use it comfortably "
             "without needing to switch connections."},
            {"SIDETONE_SETTING_SUMMARY",
             "Your own voice will be easier to hear during calls. If your voice sounds too loud or background "
             "noise is distracting, please turn off this feature."},
        };
        for (const MDRGeneralSettingInfo& info : GetGeneralSettings())
        {
            if (info.type != MDR_GENERAL_SETTING_BOOLEAN)
                continue;
            MDRGeneralSetting setting = MDRStruct<MDRGeneralSetting>();
            if (mdrHeadphonesGetGeneralSetting(gDevice, info.index, &setting) != MDR_RESULT_OK)
                continue;
            const mdr::String subjectKey = GetText(MDR_TEXT_GENERAL_SETTING_SUBJECT, info.index);
            const mdr::String summaryKey = GetText(MDR_TEXT_GENERAL_SETTING_SUMMARY, info.index);
            const char* subject = kFormatGSString(subjectKey.c_str(), kGSSubjectStrings);
            const char* summary = kFormatGSString(summaryKey.c_str(), kGSSummaryStrings);
            bool value = setting.boolean_value != MDR_FALSE;
            ImGui::BeginDisabled(subjectKey.empty() || !info.writable);
            if (ImGui::Checkbox(subject, &value))
            {
                setting.boolean_value = value ? MDR_TRUE : MDR_FALSE;
                mdrHeadphonesSetGeneralSetting(gDevice, &setting);
            }
            if (!summaryKey.empty())
            {
                ImGui::Bullet();
                ImGui::SameLine();
                ImGui::TextWrapped("%s", summary);
            }
            ImGui::EndDisabled();
        }
        ImGui::TreePop();
    }
    /* Assignable Settings */
    if (FeatureAvailable(MDR_FEATURE_ASSIGNABLE_CONTROLS) &&
        ImGui::TreeNodeEx("Touch Preset", ImGuiTreeNodeFlags_DefaultOpen))
    {
        MDRAssignableControls controls = MDRStruct<MDRAssignableControls>();
        if (mdrHeadphonesGetAssignableControls(gDevice, &controls) == MDR_RESULT_OK)
        {
            constexpr MDRAssignableAction kSelections[] = {
                MDR_ASSIGNABLE_PLAYBACK, MDR_ASSIGNABLE_NOISE_CONTROL_QUICK_ACCESS, MDR_ASSIGNABLE_NONE};
            bool changed = ImComboBoxItems(
                "Left Touch", std::span{kSelections}, controls.left, FormatAssignableAction);
            changed |= ImComboBoxItems(
                "Right Touch", std::span{kSelections}, controls.right, FormatAssignableAction);
            if (changed)
                mdrHeadphonesSetAssignableControls(gDevice, &controls);
        }
        ImGui::TreePop();
    }
    /* NC/ASM Button Settings */
    if (FeatureAvailable(MDR_FEATURE_NOISE_CONTROL_BUTTON) &&
        ImGui::TreeNodeEx("NC/AMB Button Function", ImGuiTreeNodeFlags_DefaultOpen))
    {
        MDRNoiseControl noise = MDRStruct<MDRNoiseControl>();
        if (mdrHeadphonesGetNoiseControl(gDevice, &noise) == MDR_RESULT_OK)
        {
            constexpr MDRNoiseButtonMode kSelections[] = {
                MDR_NOISE_BUTTON_NONE, MDR_NOISE_BUTTON_NOISE_AMBIENT_OFF, MDR_NOISE_BUTTON_NOISE_AMBIENT,
                MDR_NOISE_BUTTON_NOISE_OFF, MDR_NOISE_BUTTON_AMBIENT_OFF};
            if (ImComboBoxItems(
                    "Function", std::span{kSelections}, noise.button_mode, FormatNoiseButtonMode))
                mdrHeadphonesSetNoiseControl(gDevice, &noise);
        }
        ImGui::TreePop();
    }
    MDRPower power = MDRStruct<MDRPower>();
    const bool havePower = mdrHeadphonesGetPower(gDevice, &power) == MDR_RESULT_OK;
    /* Head Gesture */
    if (FeatureAvailable(MDR_FEATURE_HEAD_GESTURE) &&
        ImGui::TreeNodeEx("Head Gesture", ImGuiTreeNodeFlags_DefaultOpen))
    {
        bool enabled = power.head_gesture != MDR_FALSE;
        if (ImGui::Checkbox("Enabled", &enabled) && havePower)
        {
            power.head_gesture = enabled ? MDR_TRUE : MDR_FALSE;
            mdrHeadphonesSetPower(gDevice, &power);
        }
        ImGui::TreePop();
    }
    /* Auto Power Off */
    if (FeatureAvailable(MDR_FEATURE_AUTO_POWER_OFF) &&
        ImGui::TreeNodeEx("Auto Power Off", ImGuiTreeNodeFlags_DefaultOpen))
    {
        constexpr uint32_t kSelections[] = {0, 5, 15, 30, 60, 180};
        bool changed = ImComboBoxItems(
            "Time", std::span{kSelections}, power.auto_power_off_minutes, FormatAutoPowerOff);
        if (FeatureAvailable(MDR_FEATURE_WEARING_DETECTION) &&
            power.wearing_power != MDR_WEARING_POWER_UNAVAILABLE)
        {
            bool whenRemoved = power.wearing_power == MDR_WEARING_POWER_WHEN_REMOVED;
            if (ImGui::Checkbox("Power off when removed", &whenRemoved))
                power.wearing_power =
                    whenRemoved ? MDR_WEARING_POWER_WHEN_REMOVED : MDR_WEARING_POWER_DISABLED, changed = true;
        }
        if (changed && havePower)
            mdrHeadphonesSetPower(gDevice, &power);
        ImGui::TreePop();
    }
    /* Auto Pause */
    if (FeatureAvailable(MDR_FEATURE_AUTO_PAUSE) &&
        ImGui::TreeNodeEx("Pause when removed", ImGuiTreeNodeFlags_DefaultOpen))
    {
        bool enabled = power.auto_pause != MDR_FALSE;
        if (ImGui::Checkbox("Enabled", &enabled) && havePower)
        {
            power.auto_pause = enabled ? MDR_TRUE : MDR_FALSE;
            mdrHeadphonesSetPower(gDevice, &power);
        }
        ImGui::TreePop();
    }
    /* Voice Guidance */
    if (FeatureAvailable(MDR_FEATURE_VOICE_GUIDANCE) &&
        ImGui::TreeNodeEx("Voice Guidance", ImGuiTreeNodeFlags_DefaultOpen))
    {
        MDRVoiceGuidance voice = MDRStruct<MDRVoiceGuidance>();
        if (mdrHeadphonesGetVoiceGuidance(gDevice, &voice) == MDR_RESULT_OK)
        {
            bool changed = false;
            bool enabled = voice.enabled != MDR_FALSE;
            if (ImGui::Checkbox("Enabled", &enabled))
                voice.enabled = enabled ? MDR_TRUE : MDR_FALSE, changed = true;
            ImGui::SeparatorText("Volume");
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (FeatureAvailable(MDR_FEATURE_VOICE_GUIDANCE_VOLUME))
            {
                int volume = voice.volume;
                if (ImGui::SliderInt("##Volume", &volume, -2, 2))
                    voice.volume = static_cast<int8_t>(volume), changed = true;
            }
            if (changed)
                mdrHeadphonesSetVoiceGuidance(gDevice, &voice);
        }
        ImGui::TreePop();
    }
}
void DrawDeviceControlsAbout()
{
    if (ImGui::TreeNodeEx("Model", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::BeginTable("##ModelTable", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Model:");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", GetText(MDR_TEXT_MODEL_NAME).c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("MAC:");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", GetText(MDR_TEXT_UNIQUE_ID).c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Firmware Version:");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", GetText(MDR_TEXT_FIRMWARE_VERSION).c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Series:");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", GetText(MDR_TEXT_MODEL_SERIES).c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Color:");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", GetText(MDR_TEXT_MODEL_COLOR).c_str());

            ImGui::EndTable();
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Features", ImGuiTreeNodeFlags_DefaultOpen))
    {
        struct FeatureRow
        {
            const char* name;
            MDRFeature feature;
        };
        constexpr FeatureRow kFeatures[] = {
            {"Identity", MDR_FEATURE_IDENTITY},
            {"Single battery", MDR_FEATURE_BATTERY_SINGLE},
            {"Left/right battery", MDR_FEATURE_BATTERY_LEFT_RIGHT},
            {"Charging case battery", MDR_FEATURE_BATTERY_CASE},
            {"Playback metadata", MDR_FEATURE_PLAYBACK_METADATA},
            {"Playback control", MDR_FEATURE_PLAYBACK_CONTROL},
            {"Playback volume", MDR_FEATURE_PLAYBACK_VOLUME},
            {"Noise cancelling", MDR_FEATURE_NOISE_CANCELLING},
            {"Ambient sound", MDR_FEATURE_AMBIENT_SOUND},
            {"Adaptive ambient sound", MDR_FEATURE_ADAPTIVE_AMBIENT_SOUND},
            {"Speak to Chat", MDR_FEATURE_SPEAK_TO_CHAT},
            {"Listening mode", MDR_FEATURE_LISTENING_MODE},
            {"Equalizer", MDR_FEATURE_EQUALIZER},
            {"DSEE", MDR_FEATURE_DSEE},
            {"Paired device management", MDR_FEATURE_PAIRED_DEVICE_MANAGEMENT},
            {"Pairing mode", MDR_FEATURE_PAIRING_MODE},
            {"General settings", MDR_FEATURE_GENERAL_SETTINGS},
            {"Assignable controls", MDR_FEATURE_ASSIGNABLE_CONTROLS},
            {"Noise control button", MDR_FEATURE_NOISE_CONTROL_BUTTON},
            {"Auto power off", MDR_FEATURE_AUTO_POWER_OFF},
            {"Wearing detection", MDR_FEATURE_WEARING_DETECTION},
            {"Auto pause", MDR_FEATURE_AUTO_PAUSE},
            {"Head gesture", MDR_FEATURE_HEAD_GESTURE},
            {"Voice guidance", MDR_FEATURE_VOICE_GUIDANCE},
            {"Voice guidance volume", MDR_FEATURE_VOICE_GUIDANCE_VOLUME},
            {"Shutdown", MDR_FEATURE_SHUTDOWN},
            {"Connection mode", MDR_FEATURE_CONNECTION_MODE},
            {"Safe listening", MDR_FEATURE_SAFE_LISTENING},
        };
        if (ImGui::BeginTable("##Features", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
        {
            for (const FeatureRow& row : kFeatures)
            {
                MDRFeatureAvailability availability = MDR_FEATURE_UNKNOWN;
                mdrHeadphonesGetFeature(gDevice, row.feature, &availability);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", row.name);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", FormatFeatureAvailability(availability));
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }
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
        if (ImGui::BeginTabItem("System"))
        {
            DrawDeviceControlsSystem();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("About"))
        {
            DrawDeviceControlsAbout();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void DrawDeviceControls()
{
    MDREvent event = MDR_EVENT_NONE;
    const MDRResult pollResult = mdrHeadphonesPoll(gDevice, &event);
    if (pollResult != MDR_RESULT_OK)
    {
        DisconnectWithModal();
        return;
    }
    if (event == MDR_EVENT_INITIALIZE_COMPLETE)
    {
        if (mdrHeadphonesRequestFetch(gDevice) != MDR_RESULT_OK)
        {
            DisconnectWithModal();
            return;
        }
    }
    if (event == MDR_EVENT_IDENTITY_CHANGED || event == MDR_EVENT_SYNC_COMPLETE)
        MaterialYouTheme::ApplyForModelColor(GetModelColor());

    DrawDeviceControlsHeader();
    if (!gDevice)
        return;
    ImGui::Separator();
    ImGui::BeginChild("##ControlTabs");
    DrawDeviceControlsTabs();
    ImScrollWhenDraggingAnywhere(ImGui::GetIO().MouseDelta, ImGuiMouseButton_Left);
    ImGui::EndChild();

    if (mdrHeadphonesIsReady(gDevice) && mdrHeadphonesIsDirty(gDevice) &&
        mdrHeadphonesRequestCommit(gDevice) != MDR_RESULT_OK)
        DisconnectWithModal();
}

void DrawDeviceDisconnect()
{
    MDRConnection* conn = clientPlatformConnectionGet();
    static bool popup = false;
    if (!popup)
    {
        std::fprintf(stderr, "[Client] Device disconnected\n");
        if (!connectionAttempt.lastError.empty())
            std::fprintf(stderr, "[Client] Connection: %s\n", connectionAttempt.lastError.c_str());
        else if (conn)
            std::fprintf(stderr, "[Client] Connection: %s\n", mdrConnectionGetLastError(conn));
        if (!gHeadphonesError.empty())
            std::fprintf(stderr, "[Client] Headphones: %s\n", gHeadphonesError.c_str());
        ImGui::OpenPopup("Disconnected"), popup = true;
    }
    ImSetNextWindowCentered();

    if (ImGui::BeginPopupModal("Disconnected", nullptr, kImWindowFlagsTopMost))
    {
        ImGui::NewLine();
        ImTextCentered("Device Disconnected");
        ImGui::NewLine();
        ImSpinner(5000.0f, 24.0f, MaterialYouTheme::ArgbToImU32(MaterialYouTheme::FixedSurfaceColors::error), 4.0f,
                  true, false);
        ImGui::NewLine();
        ImGui::SeparatorText("Messages");
        if (!connectionAttempt.lastError.empty())
            ImGui::TextWrapped("Connection: %s", connectionAttempt.lastError.c_str());
        else if (conn)
            ImGui::TextWrapped("Connection: %s", mdrConnectionGetLastError(conn));
        if (!gHeadphonesError.empty())
            ImGui::TextWrapped("Headphones: %s", gHeadphonesError.c_str());
#ifdef MDR_CLIENT_DEBUGGER
        ImGui::Separator();
        ImTextCentered(PSI_INFO_SIGN " NOTE: Use the Protocol Debugger for more info. This is available from the Device Selection menu");
#endif
        ImGui::NewLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImModalButton(PSI_LINK " Reconnect"))
        {
            CloseDevice();
            mdrConnectionDisconnect(conn);
            connectionAttempt = {};
            connState = CONN_STATE_NO_CONNECTION;
        }

        ImGui::EndPopup();
    }
    else
        popup = false;
}

void DrawApp()
{
    auto& io = ImGui::GetIO();
    auto& g = *ImGui::GetCurrentContext();
#ifdef MDR_CLIENT_DEBUGGER
    if (gDebuggerOnlyMode)
    {
        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize(io.DisplaySize);
        if (ImGui::Begin("SonyHeadphonesClient", nullptr, kImWindowFlagsTopMost))
            ImGui::TextDisabled("Packet replay mode");
        ImGui::End();
        clientDebuggerDraw(nullptr);
        return;
    }
#endif
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGuiWindowFlags flags = kImWindowFlagsTopMost;
    switch (connState)
    {
    case CONN_STATE_CONNECTED:
        flags |= ImGuiWindowFlags_MenuBar;
        break;
    default:
        break;
    }
    if (ImGui::Begin("SonyHeadphonesClient", nullptr, flags))
    {
        switch (connState)
        {
        case CONN_STATE_NO_CONNECTION:
#ifdef MDR_CLIENT_DEBUGGER
            if (!gDebuggerOpen)
#endif
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
    ImGui::End();
#ifdef MDR_CLIENT_DEBUGGER
    // Error modals replace the debugger popup while preserving its open state.
    // Once the error is dismissed, the debugger reopens with its packet history intact.
    if (connState != CONN_STATE_DISCONNECTED &&
        (gDebuggerOpen || ImGui::IsPopupOpen("Debugger")))
        clientDebuggerDraw(&gDebuggerOpen);
#endif
}

#ifdef MDR_CLIENT_DEBUGGER
void clientEnterDebuggerReplayMode()
{
    CloseDevice();
    clientPlatformConnectionDestroy();
    connectionAttempt = {};
    connState = CONN_STATE_NO_CONNECTION;
    gDebuggerOnlyMode = true;
    gDebuggerOpen = true;
}
#endif

bool clientShouldExit()
{
    // Defines like IMGUI_DISABLE_OBSOLETE_FUNCTIONS changes ImGui struct sizes
    // and can lead to very, very bad results. Check them here too to ensure than this TU got the correct ones.
    IMGUI_CHECKVERSION();
    DrawApp();
    return false;
}
