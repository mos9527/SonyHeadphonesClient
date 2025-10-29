#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_sdl2.h>

void DrawControls()
{
    ImGui::Text("Scaling: %.3f", ImGui_ImplSDL2_GetContentScaleForDisplay(0));
    auto& io = ImGui::GetIO();
    ImGui::Text("Window: %.3f, %.3f", io.DisplaySize.x, io.DisplaySize.y);
    ImGui::Text("FB: %.3f, %.3f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
}

bool DrawWindow()
{
    auto& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize(io.DisplaySize);
    static bool open = true;
    ImGui::Begin("##", &open,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    DrawControls();
    ImGui::End();
    return open;
}

bool ShouldClientExit()
{
    ImGui::SetCurrentContext(ImGui::GetCurrentContext());
    DrawWindow();
    return false;
}
