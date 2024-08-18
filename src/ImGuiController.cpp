#include "stdafx.h"
#include "ImGuiController.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "DXContext.h"
#include "CbvSrvDescriptorHeapManager.h"
#include "Timer.h"

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

    ImGuiIO& io = ImGui::GetIO();
    float framerate = io.Framerate;

    Timer* timer = Timer::GetInstance();

    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Framerate");                              // Create a window called "Framerate" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter); // TODO make this into a nice lil graph.

        ImGui::Text("ImGui-measured average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("Application-measured average %.3f ms/frame (%.1f FPS)", timer->GetDeltaTime() * 1000.0, timer->GetRollingAvgFps());
        ImGui::End();
    }
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
