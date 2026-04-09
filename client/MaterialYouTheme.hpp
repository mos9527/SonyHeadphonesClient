#pragma once
#include <imgui.h>
#include <cstdint>
#include <iterator>

namespace MaterialYouTheme {

using Argb = uint32_t;

inline ImVec4 ArgbToImVec4(Argb argb) {
    return ImVec4(
        static_cast<float>((argb >> 16) & 0xFF) / 255.0f,
        static_cast<float>((argb >> 8) & 0xFF) / 255.0f,
        static_cast<float>(argb & 0xFF) / 255.0f,
        static_cast<float>((argb >> 24) & 0xFF) / 255.0f
    );
}

inline ImVec4 ArgbToImVec4(Argb argb, float alpha) {
    return ImVec4(
        static_cast<float>((argb >> 16) & 0xFF) / 255.0f,
        static_cast<float>((argb >> 8) & 0xFF) / 255.0f,
        static_cast<float>(argb & 0xFF) / 255.0f,
        alpha
    );
}

inline ImU32 ArgbToImU32(Argb argb) {
    return ImGui::ColorConvertFloat4ToU32(ArgbToImVec4(argb));
}

inline ImU32 ArgbToImU32(Argb argb, float alpha) {
    return ImGui::ColorConvertFloat4ToU32(ArgbToImVec4(argb, alpha));
}

// Sony's fixed dark surface values (from Sound Connect APK ir/b.smali)
struct FixedSurfaceColors {
    static constexpr Argb surface             = 0xFF191C1D;
    static constexpr Argb surfaceContainerLow = 0xFF1C2021;
    static constexpr Argb surfaceContainerHigh = 0xFF282D2F;
    static constexpr Argb surfaceContainerHighest = 0xFF36393A;
    static constexpr Argb onSurface           = 0xFFE1E3E3;
    static constexpr Argb onSurfaceVariant    = 0xFFC4C7C7;
    static constexpr Argb outline             = 0xFF8E9192;
    static constexpr Argb outlineVariant      = 0xFF444748;
    static constexpr Argb inverseSurface      = 0xFFE1E3E3;
    static constexpr Argb inverseOnSurface    = 0xFF2E3132;
    static constexpr Argb error               = 0xFFF2B8B5;
    static constexpr Argb errorContainer      = 0xFF8C1D18;
};

struct Theme {
    Argb primary;
    Argb onPrimary;
    Argb primaryContainer;
    Argb onPrimaryContainer;
};

// Precomputed Material You dark theme palettes per ModelColor.
// Regenerate: cd tooling/theme-generator && npm install && npm run generate
#include "MaterialYouThemeTable.inc"

inline const Theme& ThemeForModelColor(uint8_t modelColor) {
    if (modelColor < std::size(kThemeTable)) return kThemeTable[modelColor];
    return kThemeTable[0]; // DEFAULT
}

inline void Apply(const Theme& theme) {
    auto& style = ImGui::GetStyle();
    ImVec4* c = style.Colors;

    // Fixed surface colors (Sony standard)
    c[ImGuiCol_WindowBg]        = ArgbToImVec4(FixedSurfaceColors::surface);
    c[ImGuiCol_ChildBg]         = ArgbToImVec4(FixedSurfaceColors::surface, 0.0f);
    c[ImGuiCol_PopupBg]         = ArgbToImVec4(FixedSurfaceColors::surfaceContainerLow);
    c[ImGuiCol_MenuBarBg]       = ArgbToImVec4(FixedSurfaceColors::surfaceContainerHigh);
    c[ImGuiCol_ScrollbarBg]     = ArgbToImVec4(FixedSurfaceColors::surface, 0.5f);
    c[ImGuiCol_TableRowBg]      = ArgbToImVec4(FixedSurfaceColors::surface, 0.0f);
    c[ImGuiCol_TableRowBgAlt]   = ArgbToImVec4(FixedSurfaceColors::surfaceContainerLow, 0.5f);

    // Title bar
    c[ImGuiCol_TitleBg]          = ArgbToImVec4(FixedSurfaceColors::surfaceContainerLow);
    c[ImGuiCol_TitleBgActive]    = ArgbToImVec4(FixedSurfaceColors::inverseSurface);
    c[ImGuiCol_TitleBgCollapsed] = ArgbToImVec4(FixedSurfaceColors::surface);

    // Text (fixed)
    c[ImGuiCol_Text]             = ArgbToImVec4(FixedSurfaceColors::onSurface);
    c[ImGuiCol_TextDisabled]     = ArgbToImVec4(FixedSurfaceColors::onSurfaceVariant);

    // Borders (fixed)
    c[ImGuiCol_Border]           = ArgbToImVec4(FixedSurfaceColors::outline);
    c[ImGuiCol_BorderShadow]     = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_Separator]        = ArgbToImVec4(FixedSurfaceColors::outlineVariant);
    c[ImGuiCol_SeparatorHovered] = ArgbToImVec4(theme.primary, 0.78f);
    c[ImGuiCol_SeparatorActive]  = ArgbToImVec4(theme.primary);
    c[ImGuiCol_TableHeaderBg]    = ArgbToImVec4(FixedSurfaceColors::surfaceContainerHighest);
    c[ImGuiCol_TableBorderStrong]= ArgbToImVec4(FixedSurfaceColors::outline);
    c[ImGuiCol_TableBorderLight] = ArgbToImVec4(FixedSurfaceColors::outlineVariant);

    // Frame backgrounds: primary tint with increasing opacity
    c[ImGuiCol_FrameBg]          = ArgbToImVec4(theme.primary, 0.20f);
    c[ImGuiCol_FrameBgHovered]   = ArgbToImVec4(theme.primary, 0.40f);
    c[ImGuiCol_FrameBgActive]    = ArgbToImVec4(theme.primary, 0.67f);

    // Scrollbar
    c[ImGuiCol_ScrollbarGrab]        = ArgbToImVec4(FixedSurfaceColors::outline);
    c[ImGuiCol_ScrollbarGrabHovered] = ArgbToImVec4(FixedSurfaceColors::onSurfaceVariant);
    c[ImGuiCol_ScrollbarGrabActive]  = ArgbToImVec4(theme.primary);

    // Dynamic accent colors
    c[ImGuiCol_CheckMark]        = ArgbToImVec4(theme.primary);
    c[ImGuiCol_SliderGrab]       = ArgbToImVec4(theme.primary, 0.80f);
    c[ImGuiCol_SliderGrabActive] = ArgbToImVec4(theme.primary);

    // Buttons: primary tint, brighter on hover/active
    c[ImGuiCol_Button]           = ArgbToImVec4(theme.primary, 0.25f);
    c[ImGuiCol_ButtonHovered]    = ArgbToImVec4(theme.primary, 0.50f);
    c[ImGuiCol_ButtonActive]     = ArgbToImVec4(theme.primary, 0.75f);

    // Headers: primary tint with increasing opacity
    c[ImGuiCol_Header]           = ArgbToImVec4(theme.primary, 0.22f);
    c[ImGuiCol_HeaderHovered]    = ArgbToImVec4(theme.primary, 0.50f);
    c[ImGuiCol_HeaderActive]     = ArgbToImVec4(theme.primary, 0.75f);

    // Tabs
    c[ImGuiCol_Tab]                    = ArgbToImVec4(FixedSurfaceColors::surfaceContainerLow);
    c[ImGuiCol_TabSelected]            = ArgbToImVec4(theme.primaryContainer);
    c[ImGuiCol_TabSelectedOverline]    = ArgbToImVec4(theme.primary);
    c[ImGuiCol_TabHovered]             = ArgbToImVec4(theme.primary);
    c[ImGuiCol_TabDimmed]              = ArgbToImVec4(FixedSurfaceColors::surface);
    c[ImGuiCol_TabDimmedSelected]      = ArgbToImVec4(FixedSurfaceColors::surfaceContainerHigh);
    c[ImGuiCol_TabDimmedSelectedOverline] = ArgbToImVec4(FixedSurfaceColors::outline);

    // Misc
    c[ImGuiCol_TextLink]         = ArgbToImVec4(theme.primary);
    c[ImGuiCol_TextSelectedBg]   = ArgbToImVec4(theme.primaryContainer, 0.4f);
    c[ImGuiCol_NavCursor]        = ArgbToImVec4(theme.primary);
    c[ImGuiCol_DragDropTarget]   = ArgbToImVec4(theme.primary);

    // Resize grip: primary tint
    c[ImGuiCol_ResizeGrip]        = ArgbToImVec4(theme.primary, 0.20f);
    c[ImGuiCol_ResizeGripHovered] = ArgbToImVec4(theme.primary, 0.67f);
    c[ImGuiCol_ResizeGripActive]  = ArgbToImVec4(theme.primary, 0.95f);

    // Plot
    c[ImGuiCol_PlotLines]         = ArgbToImVec4(theme.primary);
    c[ImGuiCol_PlotLinesHovered]  = ArgbToImVec4(theme.primary);
    c[ImGuiCol_PlotHistogram]     = ArgbToImVec4(theme.primary);
    c[ImGuiCol_PlotHistogramHovered] = ArgbToImVec4(theme.primary);

    // Modal dim
    c[ImGuiCol_ModalWindowDimBg]      = ImVec4(0, 0, 0, 0.5f);
    c[ImGuiCol_NavWindowingHighlight] = ArgbToImVec4(theme.primary, 0.7f);
    c[ImGuiCol_NavWindowingDimBg]     = ImVec4(0, 0, 0, 0.2f);

    // Input cursor
    c[ImGuiCol_InputTextCursor]  = ArgbToImVec4(theme.primary);
}

inline void ApplyDefault() {
    Apply(kThemeTable[0]);
}

inline void ApplyForModelColor(uint8_t modelColor) {
    Apply(ThemeForModelColor(modelColor));
}

} // namespace MaterialYouTheme
