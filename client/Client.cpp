#include "imgui.h"

void DrawControls()
{
    ImGui::Text("TBD");
}

bool DrawWindow()
{
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
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
