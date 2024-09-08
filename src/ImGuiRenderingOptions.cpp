#include "stdafx.hpp"
#include "ImGuiRenderingOptions.hpp"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

ImGuiRenderingOptions::ImGuiRenderingOptions()
{
}

ImGuiRenderingOptions::~ImGuiRenderingOptions()
{
}

void ImGuiRenderingOptions::Display()
{
    RenderingState::RenderingMode currentRenderingMode = EngineStateModel::GetInstance()->GetRenderingState().chosenRenderingMode;
    int renderingModeId = static_cast<int>(currentRenderingMode);
    
    if(ImGui::Combo("Rendering mode:", &renderingModeId, RenderingState::renderingModeNames, IM_ARRAYSIZE(RenderingState::renderingModeNames))) {
        EngineStateModel::GetInstance()->GetRenderingState().chosenRenderingMode =
            static_cast<RenderingState::RenderingMode>(renderingModeId);
    }
    if(renderingModeId == 0) {
        ImGui::TextWrapped("This rendering mode is the default one.\n"
        "It uses per-object materials that generally implement "
        "simple normal-based shadows and allow the use of an "
        "albedo (color) texture.");
    }
    else if(renderingModeId == 1) {
        ImGui::TextWrapped("This rendering mode visualizes meshes "
        "of the object in the scene using wireframe rendering.\n"
        "TIP: this mode can be toggled with X when not flying.");
    }
    else if(renderingModeId == 2) {
        ImGui::TextWrapped("This rendering mode visualizes normals "
        "of the object meshes in the scene by coloring the mesh with "
        "RGB values that represent normal vector XYZ components.");
    }
}