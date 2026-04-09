#pragma once
#include <imgui.h>
#include <cstdint>

#include "cpp/cam/hct.h"
#include "cpp/dynamiccolor/material_dynamic_colors.h"
#include "cpp/scheme/scheme_tonal_spot.h"
#include "cpp/utils/utils.h"

namespace MaterialYouTheme {

using namespace material_color_utilities;

inline ImVec4 ArgbToImVec4(Argb argb) {
    return ImVec4(
        static_cast<float>(RedFromInt(argb)) / 255.0f,
        static_cast<float>(GreenFromInt(argb)) / 255.0f,
        static_cast<float>(BlueFromInt(argb)) / 255.0f,
        static_cast<float>(AlphaFromInt(argb)) / 255.0f
    );
}

inline ImVec4 ArgbToImVec4(Argb argb, float alpha) {
    return ImVec4(
        static_cast<float>(RedFromInt(argb)) / 255.0f,
        static_cast<float>(GreenFromInt(argb)) / 255.0f,
        static_cast<float>(BlueFromInt(argb)) / 255.0f,
        alpha
    );
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

// Sony fallback: when device reports a neutral color, use this fixed blue scheme
static constexpr Argb kDefaultSourceColor = 0xFF364FA8; // sca_primary

struct Theme {
    Argb primary;
    Argb onPrimary;
    Argb primaryContainer;
    Argb onPrimaryContainer;
};

inline Theme Generate(Argb sourceArgb) {
    Hct sourceHct(sourceArgb);
    SchemeTonalSpot scheme(sourceHct, /*isDark=*/true);

    Theme t{};
    t.primary             = MaterialDynamicColors::Primary().GetArgb(scheme);
    t.onPrimary           = MaterialDynamicColors::OnPrimary().GetArgb(scheme);
    t.primaryContainer    = MaterialDynamicColors::PrimaryContainer().GetArgb(scheme);
    t.onPrimaryContainer  = MaterialDynamicColors::OnPrimaryContainer().GetArgb(scheme);
    return t;
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

    // Tabs: elevation-based hover
    // Unselected: dark base
    c[ImGuiCol_Tab]                    = ArgbToImVec4(FixedSurfaceColors::surfaceContainerLow);
    // Selected: primaryContainer (dark accent)
    c[ImGuiCol_TabSelected]            = ArgbToImVec4(theme.primaryContainer);
    c[ImGuiCol_TabSelectedOverline]    = ArgbToImVec4(theme.primary);
    // Hovered: primary (bright) — works for both selected and unselected hover
    c[ImGuiCol_TabHovered]             = ArgbToImVec4(theme.primary);
    // Dimmed (unfocused tab bar)
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

    // Plot — Sound Connect uses primary only for accent
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
    Apply(Generate(kDefaultSourceColor));
}

inline void ApplyFromSourceColor(Argb sourceArgb) {
    Apply(Generate(sourceArgb));
}

// Map MDR ModelColor enum to a source color for theme generation.
// Neutral colors fall back to kDefaultSourceColor (Sony's fixed blue scheme).
// Non-neutral values from Sony's cloud API (sca_source_color field).
inline Argb SourceColorFromModelColor(uint8_t modelColor) {
    switch (modelColor) {
    case 4:  return 0xFFF7594E; // RED
    case 5:  return 0xFF52688D; // BLUE
    case 6:  return 0xFFD4B4B8; // PINK
    case 7:  return 0xFFF7F799; // YELLOW
    case 8:  return 0xFFB9DBCE; // GREEN
    case 11: return 0xFFECE2C6; // CREAM
    case 12: return 0xFFFCA48E; // ORANGE
    case 13: return 0xFF504434; // BROWN
    case 14: return 0xFFA9A0D1; // VIOLET
    default: return kDefaultSourceColor; // DEFAULT/BLACK/WHITE/SILVER/GRAY/GOLD
    }
}

} // namespace MaterialYouTheme
