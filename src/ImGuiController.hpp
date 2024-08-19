#pragma once

#include "ImGuiPerformanceWindow.hpp"

class ImGuiController {
public:
    ImGuiController();
    ~ImGuiController();

    void InitImGui(HWND windowHwnd, UINT framesInFlight, DXGI_FORMAT rtvFormat);
    void UpdateImGui();
    void RenderImGui(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);
    void ShutdownImGui();

private:
    bool initialized = false;

    ImGuiPerformanceWindow perfWindow;
};