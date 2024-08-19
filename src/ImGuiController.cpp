#include "stdafx.hpp"
#include "ImGuiController.hpp"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "DXContext.hpp"
#include "CbvSrvDescriptorHeapManager.hpp"

ImGuiController::ImGuiController()
{
    initialized = false;
}

ImGuiController::~ImGuiController()
{
    ShutdownImGui();
}

void ImGuiController::InitImGui(HWND windowHwnd, UINT framesInFlight, DXGI_FORMAT rtvFormat)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    UINT imguiDescriptorId = CbvSrvDescriptorHeapManager::ReserveDescriptorInHeap();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(windowHwnd);
    ImGui_ImplDX12_Init(DXContext::getDevice().Get(), framesInFlight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 
    CbvSrvDescriptorHeapManager::GetHeap().Get(),
    // You'll need to designate a descriptor from your descriptor heap for
    // Dear ImGui to use internally for its font texture's SRV.
    CbvSrvDescriptorHeapManager::GetCpuDescriptorHandle(imguiDescriptorId),
    CbvSrvDescriptorHeapManager::GetGpuDescriptorHandle(imguiDescriptorId));
    
    initialized = true;
}

void ImGuiController::UpdateImGui()
{
    assert(initialized);

    // Start the Dear ImGui frame
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    // ImGui::ShowDemoWindow();

    {
        ImGui::Begin("App settings");

        bool perfVisible = perfWindow.GetVisibility();
        ImGui::Checkbox("Performance info window", &perfVisible);
        perfWindow.SetVisibility(perfVisible);

        ImGui::End();
    }

    perfWindow.Display();
}

void ImGuiController::RenderImGui(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList)
{
    assert(initialized);

    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());
}

void ImGuiController::ShutdownImGui()
{
    if(!initialized)
        return;

    // std::cout << "ImGui shutdown." <<std::endl;

    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
