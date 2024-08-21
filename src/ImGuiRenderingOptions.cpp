#include "stdafx.hpp"
#include "ImGuiRenderingOptions.hpp"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "GlobalEngineState.hpp"

ImGuiRenderingOptions::ImGuiRenderingOptions()
{
    // TODO the rendering modes should be specified outside the ImGuiController...
    renderingModes[0] = "Textured & Lit";
    renderingModes[1] = "Wireframe";

    currentRenderingMode = GlobalEngineState::GetInstance()->GetRenderingState().chosenRenderingMode;
}

ImGuiRenderingOptions::~ImGuiRenderingOptions()
{

}

void ImGuiRenderingOptions::Display()
{
    currentRenderingMode = GlobalEngineState::GetInstance()->GetRenderingState().chosenRenderingMode;
    if(ImGui::Combo("Rendering mode:", &currentRenderingMode, renderingModes, IM_ARRAYSIZE(renderingModes))) {
        GlobalEngineState::GetInstance()->GetRenderingState().chosenRenderingMode = currentRenderingMode;
    }
    if(currentRenderingMode == 0) {
        ImGui::TextWrapped("This rendering mode is the default one.\n"
        "It uses per-object materials that generally implement "
        "simple normal-based shadows and allow the use of an "
        "albedo (color) texture.");
    }
    else if(currentRenderingMode == 1) {
        ImGui::TextWrapped("This rendering mode visualizes meshes "
        "of the object in the scene using wireframe rendering.\n"
        "TIP: this mode can be toggled with X when not flying.");
    }
}