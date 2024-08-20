#include "stdafx.hpp"
#include "ImGuiPerformanceWindow.hpp"

#include "imgui.h"

#include "Timer.hpp"

void ImGuiPerformanceWindow::Display()
{
    if(!isVisible)
        return;

    Timer* timer = Timer::GetInstance();

    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Performance");

    ImGui::Checkbox("Rolling avg.", &rollingAvg);

    double fps = 0.0;
    double deltaTime = 0.0;
    if(rollingAvg) {
        fps = timer->GetRollingAvgFps();
        deltaTime = timer->GetRollingAvgDeltaTime();
    }
    else {
        fps = timer->GetFps();
        deltaTime = timer->GetDeltaTime();
    }
    ImGui::Text("Delta time: %.3f ms", deltaTime * 1000.0);
    ImGui::SameLine();
    ImGui::Text("%.1f FPS", fps);

    if(ImGui::Button("Close")) {
        SetVisibility(false);
    }
    
    ImGui::End();
}
