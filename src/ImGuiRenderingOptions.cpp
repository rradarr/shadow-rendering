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
    else if(renderingModeId == 3) {
        ImGui::TextWrapped("This rendering mode renders the scene "
        "with projection shadows. To do that, shadow casters are projected "
        "onto a ground plane Y=0 with a special matrix and rendered with a "
        "black material on top of the rest of the normally rendered scene.");
    }
    else if(renderingModeId == 4) {
        ImGui::TextWrapped("This rendering mode uses shadow mapping "
        "to render shadows.");

        DisplayShadowMapOptions();
    }
    else if(renderingModeId == static_cast<int>(RenderingState::RenderingMode::VARIANCE_SHADOW_MAP)) {
        ImGui::TextWrapped("This rendering mode uses variance shadow mapping "
        "to render shadows.");

        DisplayVarianceShadowMapOptions();
    }
}

void ImGuiRenderingOptions::DisplayShadowMapOptions()
{
    EngineStateModel* engineState = EngineStateModel::GetInstance();

    // General settings.
    ImGui::SliderFloat("Shadow map ambient", &engineState->GetRenderingState().ambientStrength, 0.f, 1.f);
    ImGui::SliderFloat("Shadow map in-shader depth bias", &engineState->GetRenderingState().inShaderDepthBias, -1.f, 1.f);
    bool useBilinear = static_cast<bool>(engineState->GetRenderingState().useBilinearFiltering);
    if(ImGui::Checkbox("Use hardware bilinear filtering", &useBilinear)) {
        engineState->GetRenderingState().useBilinearFiltering = static_cast<int>(useBilinear);
    }
    
    int chosenPCFMode = static_cast<int>(engineState->GetRenderingState().chosenPFCMode);
    static_cast<int>(RenderingState::PCFMode::SIMPLE);

    // PFC mode selection.
    for(int i = 0; i < static_cast<int>(RenderingState::PCFMode::NUM_PCF_MODES); i++) {
        // ImGui::PushID(i);
        if(ImGui::RadioButton(RenderingState::pcfModeNames[i], &chosenPCFMode, i)) {
            engineState->GetRenderingState().chosenPFCMode = static_cast<RenderingState::PCFMode>(i);
        }
        // ImGui::PopID();
    }

    // PCF mode dependednt explanation and mode settings.
    if(chosenPCFMode == static_cast<int>(RenderingState::PCFMode::SIMPLE)) {
        ImGui::TextWrapped("This filtering mode performs no filtering of the "
        "shadows. It evaluates the shadow exactly once per screen pixel.");
    }
    if(chosenPCFMode == static_cast<int>(RenderingState::PCFMode::MANUAL_FILTER)) {
        ImGui::TextWrapped("This filtering mode performs filtering of the shadows "
        "using a filter kernel created in-shader. The kernel contains offsets "
        "into the shadow map with regard to the current point. Kernel sizes should "
        "be odd numbers. The offset scale can be applied to spread out the kernel "
        "samples.");

        int kernelSize = engineState->GetRenderingState().manualPCFKernelSize;
        if(ImGui::InputInt("Kernel size (odd)", &kernelSize, 2, 4, ImGuiSliderFlags_AlwaysClamp)) {
            // If even just hack it and just dont update...
            if(kernelSize % 2 != 0 && kernelSize > 0 && kernelSize < 12)
                engineState->GetRenderingState().manualPCFKernelSize = kernelSize;
        }
        
        ImGui::SliderFloat("Kernel sample offset scale", &engineState->GetRenderingState().pcfSampleOffset, 0.f, 50.f, "%.3f");
    }
    if(chosenPCFMode == static_cast<int>(RenderingState::PCFMode::RANDOM_OFFESTS_FILTER)) {
        ImGui::TextWrapped("This filtering mode performs filtering of the shadows "
        "using a filter kernel created offline, on application startup. This kernel "
        "contains randomly jittered offsets for PCF samples. It is stored in a "
        "3D texture. The offset scale can be adjusted to control the spread of samples.");
        
        ImGui::SliderFloat("Kernel sample offset scale", &engineState->GetRenderingState().pcfSampleOffset, 0.f, 50.f, "%.3f");
    }
    if(chosenPCFMode == static_cast<int>(RenderingState::PCFMode::PCSS_RANDOM_OFFSETS)) {
        ImGui::TextWrapped("This technique approximates soft shadows with the "
        "penumbra size being dependant on the distance of the occluder to the "
        "light source. It uses PCF with random offset texture.");
        
        ImGui::SliderFloat("Kernel sample offset scale", &engineState->GetRenderingState().pcfSampleOffset, 0.f, 5.f, "%.3f");
    
        ImGui::SliderFloat("Light size in world space", &engineState->GetRenderingState().worldSpaceLightSize, 0.f, 0.5f, "%.3f");
    }
    
}

void ImGuiRenderingOptions::DisplayVarianceShadowMapOptions()
{
    EngineStateModel* engineState = EngineStateModel::GetInstance();

    // General settings.
    ImGui::SliderFloat("Shadow map ambient", &engineState->GetRenderingState().ambientStrength, 0.f, 1.f);
    ImGui::SliderFloat("VSM light leeding reduction", &engineState->GetRenderingState().vsmBleedingReduction, 0.f, 1.f);
    bool useBilinear = static_cast<bool>(engineState->GetRenderingState().useBilinearFiltering);
    if(ImGui::Checkbox("Use hardware bilinear filtering", &useBilinear)) {
        engineState->GetRenderingState().useBilinearFiltering = static_cast<int>(useBilinear);
    }

    bool useBlur = static_cast<bool>(engineState->GetRenderingState().useGaussianBlurOnVSM);
    if(ImGui::Checkbox("Filter VSM with Gaussian blur", &useBlur)) {
        engineState->GetRenderingState().useGaussianBlurOnVSM = static_cast<int>(useBlur);
    }

    int filterSize = engineState->GetRenderingState().gaussianFilterSize;
    if(ImGui::InputInt("Kernel size (odd)", &filterSize, 2, 4, ImGuiSliderFlags_AlwaysClamp)) {
        // If even just hack it and just dont update...
        if(filterSize % 2 != 0 && filterSize > 0 && filterSize < 24)
            engineState->GetRenderingState().gaussianFilterSize = filterSize;
    }

    ImGui::SliderFloat("VSM box blur offset", &engineState->GetRenderingState().gaussianBlurSampleOffset, 0.f, 30.f);
    // ImGui::SliderFloat("VSM Gaussian blur variance", &engineState->GetRenderingState().gaussianBlurVariance, 0.f, 10.f);
}
