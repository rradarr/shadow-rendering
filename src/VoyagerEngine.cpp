#include "stdafx.hpp"
#include "VoyagerEngine.hpp"

#include "DXSampleHelper.h"
#include "CbvSrvDescriptorHeapManager.hpp"
#include "TextureLoader.hpp"
#include "WindowsApplication.hpp"
#include "Timer.hpp"
#include "DXContext.hpp"
#include "BufferMemoryManager.hpp"
#include "EngineHelpers.hpp"
#include "DefaultRenderer.hpp"
#include "EngineStateModel.hpp"
#include "MainInputController.hpp"

#include "SceneObject.hpp"
#include <random>
#include <limits>

VoyagerEngine::VoyagerEngine(UINT windowWidth, UINT windowHeight, std::string windowName) :
    Engine(windowWidth, windowHeight, windowName)
{
    m_frameBufferIndex = 0;
    m_frameIndex = 0;
    m_viewport = CD3DX12_VIEWPORT{ 0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight) };
    m_scissorRect = CD3DX12_RECT{ 0, 0, static_cast<LONG>(windowWidth), static_cast<LONG>(windowHeight) };
    m_rtvDescriptorSize = 0;
    m_dsvHeapIncrement = 0;

    EngineStateModel* state = EngineStateModel::GetInstance();
    state->GetWindowState().windowCenter = 
        { static_cast<float>(windowWidth) / 2.f, static_cast<float>(windowHeight) / 2.f };
}

void VoyagerEngine::OnInit(HWND windowHandle)
{
    this->windowHandle = windowHandle;

    LoadPipeline();
    LoadAssets();
    LoadScene();

    // Create an instance of the input handler.
    MainInputController::GetInstance();

    imguiController.InitImGui(windowHandle, mc_frameBufferCount, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
    std::cout << "Engine initialized." << std::endl;
}

void VoyagerEngine::OnUpdate()
{
    m_frameBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
    WaitForPreviousFrame();

    OnEarlyUpdate();

    // Update the camera.
    {
        DirectX::XMFLOAT2 delta;
        DirectX::XMStoreFloat2(&delta, EngineStateModel::GetInstance()->GetInputState().mouseDelta);
        m_mainCamera.UpdateCamera(delta, EngineStateModel::GetInstance()->GetInputState().keyboradMovementInput);
    }

    double deltaTime = Timer::GetInstance()->GetDeltaTime();

    // Store the view and projection matrices for use in shaders (lighting).
    DirectX::XMStoreFloat4x4(&m_wvpPerObject.viewMat, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_mainCamera.viewMat)));
    DirectX::XMStoreFloat4x4(&m_wvpPerObject.projectionMat, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_mainCamera.projMat)));

    // Load thew view and projection matrices for update calculations.
    DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(&m_mainCamera.viewMat);
    DirectX::XMMATRIX projMat = DirectX::XMLoadFloat4x4(&m_mainCamera.projMat);

    for (SceneObject &sceneObject : sceneObjects) {

        DirectX::XMMATRIX rotMat = DirectX::XMLoadFloat4x4(&sceneObject.rotation);

        // create translation matrix for cube 1 from cube 1's position vector
        DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat4(&sceneObject.position));

        DirectX::XMStoreFloat4x4(&sceneObject.rotation, rotMat);

        // we want cube 2 to be half the size of cube 1, so we scale it by .5 in all dimensions

        DirectX::XMMATRIX scaleMat =  DirectX::XMMatrixScaling(sceneObject.scale.x, sceneObject.scale.y, sceneObject.scale.z);

        // create cube1's world matrix by first rotating the cube, then positioning the rotated cube
        DirectX::XMMATRIX worldMat = scaleMat * translationMat *rotMat;
        DirectX::XMStoreFloat4x4(&sceneObject.worldMat, worldMat);
        // store cube1's world matrix

        DirectX::XMStoreFloat4x4(&m_wvpPerObject.worldMat, DirectX::XMMatrixTranspose(worldMat));

        // Store the view matrix (for lighting).
        /*DirectX::XMStoreFloat4x4(&m_wvpPerObject.viewMat, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_mainCamera.viewMat)));
        DirectX::XMStoreFloat4x4(&m_wvpPerObject.projectionMat, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_mainCamera.projMat)));*/

        // update constant buffer for cube1
        // create the wvp matrix and store in constant buffer

        DirectX::XMMATRIX wvpMat = DirectX::XMLoadFloat4x4(&sceneObject.worldMat) * viewMat * projMat; // create wvp matrix
        DirectX::XMMATRIX transposed = DirectX::XMMatrixTranspose(wvpMat); // must transpose wvp matrix for the gpu
        DirectX::XMStoreFloat4x4(&m_wvpPerObject.wvpMat, transposed); // store transposed wvp matrix in constant buffer
        // copy our ConstantBuffer instance to the mapped constant buffer resource
        sceneObject.UpdateWVPMatrices(&m_wvpPerObject, sizeof(m_wvpPerObject), m_frameBufferIndex);
        // memcpy(m_WVPConstantBuffersGPUAddress[m_frameBufferIndex] + sizeof(m_wvpPerObject)* sceneObject.idx, &m_wvpPerObject, sizeof(m_wvpPerObject));
    }

    // Position suzanne
    {
        DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat4(&suzanne.position));
        DirectX::XMMATRIX rotMat = DirectX::XMLoadFloat4x4(&suzanne.rotation);
        DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(suzanne.scale.x, suzanne.scale.y, suzanne.scale.z);
        DirectX::XMMATRIX worldMat = scaleMat * rotMat * translationMat;
        DirectX::XMStoreFloat4x4(&m_wvpPerObject.worldMat, DirectX::XMMatrixTranspose(worldMat));

        DirectX::XMMATRIX transposed = DirectX::XMMatrixTranspose(worldMat * viewMat * projMat); // must transpose wvp matrix for the gpu
        DirectX::XMStoreFloat4x4(&m_wvpPerObject.wvpMat, transposed); // store transposed wvp matrix in constant buffer
        suzanne.UpdateWVPMatrices(&m_wvpPerObject, sizeof(m_wvpPerObject), m_frameBufferIndex);
    }

    // Position ground plane
    {
        DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat4(&groundPlane.position));
        DirectX::XMMATRIX rotMat = DirectX::XMLoadFloat4x4(&groundPlane.rotation);
        DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(groundPlane.scale.x, groundPlane.scale.y, groundPlane.scale.z);
        DirectX::XMMATRIX worldMat = scaleMat * rotMat * translationMat;
        DirectX::XMStoreFloat4x4(&m_wvpPerObject.worldMat, DirectX::XMMatrixTranspose(worldMat));

        DirectX::XMMATRIX transposed = DirectX::XMMatrixTranspose(worldMat * viewMat * projMat); // must transpose wvp matrix for the gpu
        DirectX::XMStoreFloat4x4(&m_wvpPerObject.wvpMat, transposed); // store transposed wvp matrix in constant buffer
        groundPlane.UpdateWVPMatrices(&m_wvpPerObject, sizeof(m_wvpPerObject), m_frameBufferIndex);
    }

    imguiController.UpdateImGui();

    // std::cout << "Updated" << std::endl;
}

void VoyagerEngine::OnRender()
{
    // Record all the commands we need to render the scene into the command list.
    PopulateCommandList();

    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    // Set the signall fence command at the end of the command queue (it will set the value of the fence to the passed m_fenceValue).
    m_fenceValue[m_frameBufferIndex]++;
    ThrowIfFailed(m_commandQueue->Signal(m_fence[m_frameBufferIndex].Get(), m_fenceValue[m_frameBufferIndex]));

    // Present the frame.
    ThrowIfFailed(m_swapChain->Present(1, 0));

    // std::cout << "Rendered" << std::endl;
}

void VoyagerEngine::OnDestroy()
{
    // Get swapchain out out fullscreen before exiting
    BOOL fs = false;
    if (m_swapChain->GetFullscreenState(&fs, NULL))
        m_swapChain->SetFullscreenState(false, NULL);

    // Wait for the GPU to be done with all work.
    m_fenceValue[m_frameBufferIndex]++;
    m_commandQueue->Signal(m_fence[m_frameBufferIndex].Get(), m_fenceValue[m_frameBufferIndex]);
    WaitForPreviousFrame();

    CloseHandle(m_fenceEvent);

    std::cout << "Engine destroyed." << std::endl;
}

void VoyagerEngine::OnKeyDown(UINT8 keyCode)
{
    MainInputController::GetInstance()->HandleKeyDown(keyCode);    
}

void VoyagerEngine::OnKeyUp(UINT8 keyCode)
{
    MainInputController::GetInstance()->HandleKeyUp(keyCode);
}

void VoyagerEngine::OnMouseMove(int mouseX, int mouseY)
{
    MainInputController::GetInstance()->HandleMouseMove(mouseX, mouseY);
}

void VoyagerEngine::LoadPipeline()
{
    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(DXContext::getDevice().Get()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = mc_frameBufferCount;
    swapChainDesc.BufferDesc.Width = windowWidth;
    swapChainDesc.BufferDesc.Height = windowHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.OutputWindow = WindowsApplication::GetHwnd();
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;

    ComPtr<IDXGISwapChain> swapChain;
    ThrowIfFailed(DXContext::getFactory().Get()->CreateSwapChain(
        m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
        &swapChainDesc,
        &swapChain
    ));

    ThrowIfFailed(swapChain.As(&m_swapChain));

    // This sample does not support fullscreen transitions.
    ThrowIfFailed(DXContext::getFactory().Get()->MakeWindowAssociation(WindowsApplication::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));

    m_frameBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

    // Create RTV descriptor heaps.
    {
        // Describe and create a render target view (RTV) descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = mc_frameBufferCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ThrowIfFailed(DXContext::getDevice().Get()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RTVHeap)));

        m_rtvDescriptorSize = DXContext::getDevice().Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    // Create frame resources.
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVHeap->GetCPUDescriptorHandleForHeapStart());

        // Setup RTV descriptor to specify sRGB format.
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        // Create a RTV for each frame.
        for (UINT n = 0; n < mc_frameBufferCount; n++)
        {
            ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n]))); // Store pointer to swapchain buffer in m_renderTargets[n]
            DXContext::getDevice().Get()->CreateRenderTargetView(m_renderTargets[n].Get(), &rtvDesc, rtvHandle); // Create a resource based on the buffer pointer (?) within the heap at rtvHandle
            rtvHandle.Offset(1, m_rtvDescriptorSize);
        }
    }

    for (int i = 0; i < mc_frameBufferCount; i++) {
        ThrowIfFailed(DXContext::getDevice().Get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator[i])));
    }

    std::cout << "Pipeline loaded." << std::endl;

    // Single WVP CBV per frame for one object, plus a texture, light info buffer,
    // shadow map texture and WVP for shadow mapping.
    // TODO: how many do we really need?
    CbvSrvDescriptorHeapManager::CreateHeap(mc_frameBufferCount + 4);

    // Initialize the DepthStencilView heap increment size.
    m_dsvHeapIncrement = DXContext::getDevice().Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void VoyagerEngine::LoadAssets()
{
    // Create the command list.
    ThrowIfFailed(DXContext::getDevice().Get()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator[0].Get(), nullptr, IID_PPV_ARGS(&m_commandList)));

    // Command lists are created in the recording state, but there is nothing
    // to record yet. The main loop expects it to be closed, so close it now.
    ThrowIfFailed(m_commandList->Close());

    // Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        for (int i = 0; i < mc_frameBufferCount; i++) {
            ThrowIfFailed(DXContext::getDevice().Get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence[i])));
            m_fenceValue[i] = 0;
        }

        // Create an event handle to use for frame synchronization.
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    // Create the buffer for lighting parameters.
    {
        ZeroMemory(&lightParams, sizeof(lightParams));
        lightingParamsBuffer = resourceManager.AddMappedUploadResource(&lightParams, sizeof(lightParams));        
    }

    // Load the texture
    {
        sampleTexture.CreateFromFile("blank_texture.png");
    }

    // Create the vertex and index buffers.
    {
        CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CbvSrvDescriptorHeapManager::GetHeap()->GetGPUDescriptorHandleForHeapStart());
        descriptorHandle = descriptorHandle.Offset(sampleTexture.GetOffsetInHeap(), CbvSrvDescriptorHeapManager::GetDescriptorSize());

        // Load suzanne
        {
            suzanneMesh.CreateFromFile("suzanne.obj");
            suzanne = SceneObject(&suzanneMesh);

            suzanne.SetAlbedoTexture(descriptorHandle);
            suzanne.SetMaterial(&materialLit);

            std::vector<MappedResourceLocation> WVPResources;
            for(int i = 0; i < mc_frameBufferCount; i++){
                ZeroMemory(&m_wvpPerObject, sizeof(m_wvpPerObject));
                MappedResourceLocation resource = resourceManager.AddMappedUploadResource(&m_wvpPerObject, sizeof(m_wvpPerObject));
                WVPResources.push_back(resource);
            }
            suzanne.SetWVPPerFrameBufferLocations(WVPResources);
            suzanne.scale = DirectX::XMFLOAT3(0.7f, 0.7f, 0.7f);
            suzanne.position = DirectX::XMFLOAT4(0.f, 0.7f, 0.f, 1.f);
            // Sponza settings \/
            // suzanne.scale = DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f);
            // suzanne.position = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f);
            DirectX::XMStoreFloat4x4(&suzanne.rotation, DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(180.f)));
        }

        // Create the ground plane.
        {
            std::vector<Vertex> groundVertices;
            std::vector<DWORD> groundIndices;
            Mesh::CreatePlane(groundVertices, groundIndices);
            groundPlaneMesh = Mesh(groundVertices, groundIndices);

            groundPlane = SceneObject(&groundPlaneMesh);

            groundPlane.SetMaterial(&materialLit);
            groundPlane.SetAlbedoTexture(descriptorHandle);

            std::vector<MappedResourceLocation> WVPResources;
            for(int i = 0; i < mc_frameBufferCount; i++){
                ZeroMemory(&m_wvpPerObject, sizeof(m_wvpPerObject));
                MappedResourceLocation resource = resourceManager.AddMappedUploadResource(&m_wvpPerObject, sizeof(m_wvpPerObject));
                WVPResources.push_back(resource);
            }
            groundPlane.SetWVPPerFrameBufferLocations(WVPResources);
            groundPlane.scale = DirectX::XMFLOAT3(5.f, 5.f, 5.f);
            groundPlane.position = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f);
            DirectX::XMStoreFloat4x4(&groundPlane.rotation, DirectX::XMMatrixIdentity());
        }

        // Create the depth/stencil heap and buffer.
        {
            BufferMemoryManager buffMng;

            D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
            dsvHeapDesc.NumDescriptors = 2; // One for main render target, one for the shadow map.
            dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            ThrowIfFailed(DXContext::getDevice().Get()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsHeap)));

            D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
            depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
            depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
            depthOptimizedClearValue.DepthStencil.Stencil = 0;

            CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, windowWidth, windowHeight, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
            buffMng.AllocateBuffer(m_depthStencilBuffer, &resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_HEAP_TYPE_DEFAULT, &depthOptimizedClearValue);
            m_dsHeap->SetName(L"Depth/Stencil Resource Heap");

            D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
            depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
            depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

            DXContext::getDevice().Get()->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilDesc, m_dsHeap->GetCPUDescriptorHandleForHeapStart());
        }

        // Create the shadow map depth buffer / depth texture.
        {
            // For the shadow map we need additional space on the DSV and SRV heaps.
            // We need a depth view and a texture view, both for the same texture
            // resource. Using resource barriers we will transition between them.

            // Create the shadow map texture view descriptor (for use in the Texture class).
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;

            D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
            depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
            depthOptimizedClearValue.DepthStencil.Stencil = 0;

            // Create the resource.
            CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R24G8_TYPELESS, 4096U, 4096U, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
            shadowMap.CreateEmpty(resourceDesc, srvDesc, D3D12_RESOURCE_STATE_GENERIC_READ, &depthOptimizedClearValue);

            // Set the viewport to be used when rendering into the shadow map, it will be passed to the shadowMapRenderer.
            // TODO: was 4096
            shadowMapViewport = CD3DX12_VIEWPORT{0.0f, 0.0f, 4096.f, 4096.f};

            // Create the shadow map depth view.
            D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
            depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

            shadowMapDSVHeapLocation = CD3DX12_CPU_DESCRIPTOR_HANDLE{m_dsHeap->GetCPUDescriptorHandleForHeapStart()};
            
            shadowMapDSVHeapLocation.Offset(1, m_dsvHeapIncrement);
            DXContext::getDevice().Get()->CreateDepthStencilView(
                shadowMap.GetTextureResource().Get(),
                &depthStencilDesc,
                shadowMapDSVHeapLocation);

            // Create the shadow map WVP constant buffer.
            ZeroMemory(&m_wvpPerObject, sizeof(m_wvpPerObject));
            shadowMapWVPBuffer = resourceManager.AddMappedUploadResource(&m_wvpPerObject, sizeof(m_wvpPerObject));
        }
    }

    SetLightPosition();
    LoadMaterials();

    std::cout << "Assets loaded." << std::endl;
}

void VoyagerEngine::LoadMaterials()
{
    // TODO these don't compile because of incompatible root signatures and shaders!
    
    // materialTextured.SetShaders("VertexShader.hlsl", "PixelShader.hlsl");
    // materialTextured.CreateMaterial();

    // materialNoTex.SetShaders("VertexShader_noTex.hlsl", "PixelShader_noTex.hlsl");
    // materialNoTex.CreateMaterial();

    materialWireframe.SetShaders("VertexShader_wireframe.hlsl", "PixelShader_wireframe.hlsl");
    materialWireframe.CreateMaterial();
    wireframeRenderer.SetMaterial(&materialWireframe);

    materialNormalsDebug.SetShaders("VertexShader_normalsDebug.hlsl", "PixelShader_normalsDebug.hlsl");
    materialNormalsDebug.CreateMaterial();
    normalsDebugRenderer.SetMaterial(&materialNormalsDebug);

    materialLit.SetShaders("VertexShader_lit.hlsl", "PixelShader_lit.hlsl");
    materialLit.CreateMaterial();

    materialProjectionShadow.SetShaders("VertexShader_shadow_projection.hlsl", "PixelShader_shadow_projection.hlsl");
    materialProjectionShadow.CreateMaterial();
    projectionShadowRenderer.SetMaterial(&materialProjectionShadow);

    materialShadowMapDepth.SetShaders("VertexShader_shadow_mapping_depth.hlsl");
    materialShadowMapDepth.CreateMaterial();
    materialShadowMapMain.SetShaders("VertexShader_shadow_mapping.hlsl", "PixelShader_shadow_mapping.hlsl");
    materialShadowMapMain.CreateMaterial();
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsHeap->GetCPUDescriptorHandleForHeapStart());
    shadowMapRenderer.SetDSV(dsvHandle); // Note: RTV has to be set per frame, as we have multiple backbuffers.
    shadowMapRenderer.SetViewport(m_viewport);
    shadowMapRenderer.SetMainMaterial(&materialShadowMapMain);
    shadowMapRenderer.SetLightingParametersBuffer(lightingParamsBuffer);
    shadowMapRenderer.SetDepthPassMaterial(&materialShadowMapDepth);
    shadowMapRenderer.SetDepthPassDSV(shadowMapDSVHeapLocation);
    shadowMapRenderer.SetLightWVPBuffer(shadowMapWVPBuffer);
    shadowMapRenderer.SetShadowMapResource(shadowMap.GetTextureResource());
    CD3DX12_GPU_DESCRIPTOR_HANDLE shadowMapSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(CbvSrvDescriptorHeapManager::GetHeap()->GetGPUDescriptorHandleForHeapStart());
    shadowMapSRV = shadowMapSRV.Offset(shadowMap.GetOffsetInHeap(), CbvSrvDescriptorHeapManager::GetDescriptorSize()); // TODO: Move getting the SRV desc. handle into a Texture method.
    shadowMapRenderer.SetShadowMapSRV(shadowMapSRV);
    shadowMapRenderer.SetShadowPassViewport(shadowMapViewport);
}

void VoyagerEngine::LoadScene()
{
    m_mainCamera.InitCamera(DirectX::XMFLOAT4(-2.f, 3.f, -7.f, 0.f), DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f), aspectRatio);
    // m_mainCamera.InitCamera(DirectX::XMFLOAT4(10.f, 20.f, -10.f, 0.f), DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f), aspectRatio);
}

void VoyagerEngine::PopulateCommandList()
{
    // We already waited for the GPU to finish work for this framebuffer (we cannot reset a commandAllocator before it's commands have finished executing!)
    ThrowIfFailed(m_commandAllocator[m_frameBufferIndex]->Reset());

    // However, when ExecuteCommandList() is called on a particular command
    // list, that command list can then be reset at any time and must be before
    // re-recording.
    ThrowIfFailed(m_commandList->Reset(m_commandAllocator[m_frameBufferIndex].Get(), materialLit.GetPSO().Get()));

    // Indicate that the back buffer will be used as a render target.
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandList->ResourceBarrier(1, &barrier);

    ID3D12DescriptorHeap* descriptorHeaps[] = { CbvSrvDescriptorHeapManager::GetHeap().Get() };
    m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVHeap->GetCPUDescriptorHandleForHeapStart(), m_frameBufferIndex, m_rtvDescriptorSize);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsHeap->GetCPUDescriptorHandleForHeapStart());
    std::vector<SceneObject> sceneObjects{groundPlane, suzanne};
    RenderingState::RenderingMode chosenRenderingMode = EngineStateModel::GetInstance()->GetRenderingState().chosenRenderingMode;
    if (chosenRenderingMode == RenderingState::RenderingMode::WIREFRAME) {
        wireframeRenderer.SetRTV(rtvHandle);
        wireframeRenderer.SetDSV(dsvHandle);
        wireframeRenderer.SetViewport(m_viewport);
        wireframeRenderer.Render(m_commandList, sceneObjects, m_frameBufferIndex);
    }
    else if (chosenRenderingMode == RenderingState::RenderingMode::NORMALS_DEBUG) {
        normalsDebugRenderer.SetRTV(rtvHandle);
        normalsDebugRenderer.SetDSV(dsvHandle);
        normalsDebugRenderer.SetViewport(m_viewport);
        normalsDebugRenderer.Render(m_commandList, sceneObjects, m_frameBufferIndex);
    }
    else if(chosenRenderingMode == RenderingState::RenderingMode::SHADOW_PROJECTION) {
        // Render scene objects with the default renderer.
        DefaultRenderer renderer;
        renderer.SetLightingParametersBuffer(lightingParamsBuffer);
        renderer.SetRTV(rtvHandle);
        renderer.SetDSV(dsvHandle);
        renderer.SetViewport(m_viewport);
        renderer.Render(m_commandList, sceneObjects, m_frameBufferIndex);
        
        // Then render the projected shadow.
        sceneObjects = std::vector<SceneObject>{suzanne};
        projectionShadowRenderer.SetRTV(rtvHandle);
        projectionShadowRenderer.SetDSV(dsvHandle);
        projectionShadowRenderer.SetViewport(m_viewport);
        projectionShadowRenderer.SetLightingParametersBuffer(lightingParamsBuffer);
        projectionShadowRenderer.Render(m_commandList, sceneObjects, m_frameBufferIndex);
    }
    else if (chosenRenderingMode == RenderingState::RenderingMode::SHADOW_MAP) {
        // Render scene with simple shadow mapping.
        shadowMapRenderer.SetRTV(rtvHandle);
        shadowMapRenderer.Render(m_commandList, sceneObjects, m_frameBufferIndex);
    }
    else {
        // Render scene objects with the default renderer.
        DefaultRenderer renderer;
        renderer.SetLightingParametersBuffer(lightingParamsBuffer);
        renderer.SetRTV(rtvHandle);
        renderer.SetDSV(dsvHandle);
        renderer.SetViewport(m_viewport);
        renderer.Render(m_commandList, sceneObjects, m_frameBufferIndex);
    }

    // ImGui doesn't have its own renderer as rendering ImGui is very simple.
    imguiController.RenderImGui(m_commandList);

    // Indicate that the back buffer will now be used to present.
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandList->ResourceBarrier(1, &barrier);

    ThrowIfFailed(m_commandList->Close());
}

void VoyagerEngine::WaitForPreviousFrame()
{
    // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
    // This is code implemented as such for simplicity. More advanced samples
    // illustrate how to use fences for efficient resource usage.                   - So says Microsoft ~_~

    // Wait until the previous frame is finished. If the value of the fence associated with the current framebuffer is
    // less than the current m_fenceValue for the framebuffer, we know that the queue where the fence is has not finished
    // (at the end of the queue the fence will be set to the m_fenceValue++).
    if (m_fence[m_frameBufferIndex]->GetCompletedValue() < m_fenceValue[m_frameBufferIndex])
    {
        // The m_fenceEvent will trigger when the fence for the current frame buffer reaches the specified value.
        ThrowIfFailed(m_fence[m_frameBufferIndex]->SetEventOnCompletion(m_fenceValue[m_frameBufferIndex], m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

void VoyagerEngine::SetLightPosition()
{
    lightParams.lightPosition = { 10, 10, -10 };

    // Copy our ConstantBuffer instance to the mapped constant buffer resource.
    memcpy(lightingParamsBuffer.GetMappedResourceAddress(), &lightParams.lightPosition, sizeof(lightParams.lightPosition));

    // Set the WVP matrices of the shadow mapping light.
    Camera shadowMapLightCamera;
    // Set the projection matrix.
    DirectX::XMMATRIX tmpMat = DirectX::XMMatrixOrthographicLH(7.f, 7.f, 9.f, 25.f); // TODO: calculate this from the scene.
    // DirectX::XMMATRIX tmpMat = DirectX::XMMatrixOrthographicLH(50.f, 50.f, 1.f, 80.f); // <- Sponza settings.
    // DirectX::XMMATRIX tmpMat = DirectX::XMMatrixPerspectiveLH(7.f, 7.f, 9.f, 25.f);
    DirectX::XMStoreFloat4x4(&shadowMapLightCamera.projMat, tmpMat);
    // Set the view matrix.
    shadowMapLightCamera.camPosition = DirectX::XMVECTOR{10.f, 10.f, -10.f, 1.f};
    // shadowMapLightCamera.camPosition = DirectX::XMVECTOR{6.f, 20.f, -6.f, 1.f}; // <- Sponza settings.
    shadowMapLightCamera.camTarget = DirectX::XMVECTOR{0.f, 0.f, 0.f, 1.f};
    tmpMat = DirectX::XMMatrixLookAtLH(
        shadowMapLightCamera.camPosition,
        shadowMapLightCamera.camTarget,
        DirectX::XMVECTOR{0.f, 1.f, 0.f}
    );
    DirectX::XMStoreFloat4x4(&shadowMapLightCamera.viewMat, tmpMat);

    wvpConstantBuffer shadowMapLightWVP{};
    // The matrices need to be transposed because of column/row major ordering difference between hlsl and the DX implementations.
    DirectX::XMStoreFloat4x4(&shadowMapLightWVP.viewMat, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&shadowMapLightCamera.viewMat)));
    DirectX::XMStoreFloat4x4(&shadowMapLightWVP.projectionMat, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&shadowMapLightCamera.projMat)));
    memcpy(shadowMapWVPBuffer.GetMappedResourceAddress(), &shadowMapLightWVP, sizeof(shadowMapLightWVP));
}

DirectX::XMFLOAT3 VoyagerEngine:: normalize(DirectX::XMFLOAT3 vec) {
    float length = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    return DirectX::XMFLOAT3(vec.x / length, vec.y / length, vec.z / length);
}

DirectX::XMFLOAT3 VoyagerEngine::scale(DirectX::XMFLOAT3 vec, float scale) {
    return DirectX::XMFLOAT3(vec.x * scale, vec.y * scale, vec.z * scale);
}

float randFloat() {
    return static_cast<float>(rand()) / RAND_MAX;
}

void VoyagerEngine::OnEarlyUpdate()
{
    m_frameIndex++;

    // Update the timer values (deltaTime, FPS).
    Timer* timer = Timer::GetInstance();
    timer->Update();

    // Update the input states.
    MainInputController::GetInstance()->EarlyUpdate(windowHandle);
}
