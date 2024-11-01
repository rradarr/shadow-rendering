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
#include "Tracy.hpp"
// #include "TracyD3D12.hpp" // Note: included in VoyagerEngine.hpp.
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

    // Note: just override this to false if vsync is ok.
    allowsTearing = CheckTearingSupport();
    // allowsTearing = false;
    LoadPipeline();
    LoadAssets();
    LoadScene();

    // Create an instance of the input handler.
    MainInputController::GetInstance();

    imguiController.InitImGui(windowHandle, mc_frameBufferCount, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
    
    // Initialize the Tracy D3D12 context.
    tracyCtx = TracyD3D12Context(DXContext::getDevice().Get(), m_commandQueue.Get());

    std::cout << "Engine initialized." << std::endl;
}

void VoyagerEngine::OnUpdate()
{
    // Mark zone for Tracy.
    ZoneScoped;

    m_frameBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
    WaitForPreviousFrame();

    OnEarlyUpdate();

    double deltaTime = Timer::GetInstance()->GetDeltaTime();

    // Output last deltaTime for Tracy.
    TracyPlot("deltaTime", deltaTime);

    // Update the camera.
    {
        DirectX::XMFLOAT2 mouseDelta;
        DirectX::XMStoreFloat2(&mouseDelta, EngineStateModel::GetInstance()->GetInputState().mouseDelta);
        m_mainCamera.UpdateCamera(
            mouseDelta,
            EngineStateModel::GetInstance()->GetInputState().keyboradMovementInput,
            deltaTime
        );
    }

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
    
    UpdateLightFitting();

    //Update light params
    {
        RenderingState renderState = EngineStateModel::GetInstance()->GetRenderingState();
        lightParams.lightNearFarFrustumSize.x = 1.f;
        lightParams.lightNearFarFrustumSize.y = 50.f;
        lightParams.lightNearFarFrustumSize.z = 7.f;
        lightParams.lightNearFarFrustumSize.w = renderState.worldSpaceLightSize;
        lightParams.lightFilterKernelScaleBias.x = static_cast<float>(renderState.chosenPFCMode);
        lightParams.lightFilterKernelScaleBias.y = static_cast<float>(renderState.manualPCFKernelSize);
        lightParams.lightFilterKernelScaleBias.z = renderState.pcfSampleOffset;
        lightParams.lightFilterKernelScaleBias.w = renderState.inShaderDepthBias;
        lightParams.mapAmbientSampler.x = renderState.ambientStrength;
        lightParams.mapAmbientSampler.y = static_cast<float>(renderState.useBilinearFiltering);
        memcpy(lightingParamsBuffer[m_frameBufferIndex].GetMappedResourceAddress(), &lightParams, sizeof(lightParams));
    }

    // std::cout << "Updated" << std::endl;
}

void VoyagerEngine::OnRender()
{
    // Mark zone for Tracy.
    ZoneScoped;

    // Record all the commands we need to render the scene into the command list.
    PopulateCommandList();

    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    // Set the signall fence command at the end of the command queue (it will set the value of the fence to the passed m_fenceValue).
    m_fenceValue[m_frameBufferIndex]++;
    ThrowIfFailed(m_commandQueue->Signal(m_fence[m_frameBufferIndex].Get(), m_fenceValue[m_frameBufferIndex]));

    // Present the frame.
    // Note: sync interval 0 to allow tearing, otherwise use 1. Also the flag should be removed if vsync on.
    UINT presentFlags = allowsTearing? DXGI_PRESENT_ALLOW_TEARING : 0;
    ThrowIfFailed(m_swapChain->Present(allowsTearing? 0 : 1, presentFlags));

    // std::cout << "Rendered" << std::endl;

    // Mark the frame end for Tracy.
    TracyD3D12Collect(tracyCtx);
    TracyD3D12NewFrame(tracyCtx);
    FrameMark;
}

void VoyagerEngine::OnDestroy()
{
    // Destroy the tracy DX12 context.
    TracyD3D12Destroy(tracyCtx);

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
    swapChainDesc.Flags = allowsTearing? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0; // Note: remove this flag if v-sync should be on.

    ComPtr<IDXGISwapChain> swapChain;
    ThrowIfFailed(DXContext::getFactory().Get()->CreateSwapChain(
        m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
        &swapChainDesc,
        &swapChain
    ));

    ThrowIfFailed(swapChain.As(&m_swapChain));

    // This sample does not support fullscreen transitions.
    ThrowIfFailed(DXContext::getFactory().Get()->MakeWindowAssociation(WindowsApplication::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));

    // Ensure fullscreen is not on (for tearing).
    m_swapChain->SetFullscreenState(false, NULL);

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

    // Create the buffers for lighting parameters.
    {
        for(int i = 0; i < mc_frameBufferCount; i++){
            ZeroMemory(&lightParams, sizeof(lightParams));
            MappedResourceLocation resource = resourceManager.AddMappedUploadResource(&lightParams, sizeof(lightParams));
            lightingParamsBuffer.push_back(resource);
        }   
    }

    // Load the textures
    {
        sampleTexture.CreateFromFile("blank_texture.png");
        CreatePCFOffsetTexture();
    }

    // Create the vertex and index buffers.
    {
        CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CbvSrvDescriptorHeapManager::GetHeap()->GetGPUDescriptorHandleForHeapStart());
        descriptorHandle = descriptorHandle.Offset(sampleTexture.GetOffsetInHeap(), CbvSrvDescriptorHeapManager::GetDescriptorSize());

        // Load suzanne
        {
            std::vector<DirectX::XMFLOAT4> boundingB;
            suzanneMesh.CreateFromFile("cube_split.obj", &boundingB);
            suzanne = SceneObject(&suzanneMesh, boundingB);

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
            // Dragon settings \/
            // suzanne.scale = DirectX::XMFLOAT3(1.5f, 1.5f, 1.5f);
            // suzanne.position = DirectX::XMFLOAT4(0.f, 0.43f, 0.f, 1.f);
            // Sponza settings \/
            // suzanne.scale = DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f);
            // suzanne.position = DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f);
            // Power Plant Settings \/
            // suzanne.scale = DirectX::XMFLOAT3(0.0002f, 0.0002f, 0.0002f);
            // suzanne.position = DirectX::XMFLOAT4(-12.f, -1.f, 9.f, 1.f);
            DirectX::XMStoreFloat4x4(&suzanne.rotation, DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(180.f)));

            suzanne.UpdateBoundingBox();
        }

        // Create the ground plane.
        {
            std::vector<Vertex> groundVertices;
            std::vector<DWORD> groundIndices;
            Mesh::CreatePlane(groundVertices, groundIndices);
            groundPlaneMesh = Mesh(groundVertices, groundIndices);
            std::vector<DirectX::XMFLOAT4> boundingB = Mesh::GetBoundingBox(groundVertices);

            groundPlane = SceneObject(&groundPlaneMesh, boundingB);

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

            groundPlane.UpdateBoundingBox();
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
            // Shdaow map resolutions:
            // 512
            // 1024
            // 2048
            // 4096
            const unsigned int shadowMapResolution = 2048U;
            CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R24G8_TYPELESS, shadowMapResolution, shadowMapResolution, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
            shadowMap.CreateEmpty(resourceDesc, srvDesc, D3D12_RESOURCE_STATE_GENERIC_READ, &depthOptimizedClearValue);

            // Set the viewport to be used when rendering into the shadow map, it will be passed to the shadowMapRenderer.
            shadowMapViewport = CD3DX12_VIEWPORT{0.0f, 0.0f, static_cast<float>(shadowMapResolution), static_cast<float>(shadowMapResolution)};

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

            // Create the shadow map WVP constant buffers. (We could use one, but since we fit the map each frame we need per-frame)
            for(int i = 0; i < mc_frameBufferCount; i++){
                ZeroMemory(&m_wvpPerObject, sizeof(m_wvpPerObject));
                MappedResourceLocation resource = resourceManager.AddMappedUploadResource(&m_wvpPerObject, sizeof(m_wvpPerObject));
                shadowMapWVPBuffers.push_back(resource);
            }
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
    shadowMapRenderer.SetDepthPassMaterial(&materialShadowMapDepth);
    shadowMapRenderer.SetDepthPassDSV(shadowMapDSVHeapLocation);
    // shadowMapRenderer.SetLightWVPBuffer(shadowMapWVPBuffer); Needs to be updated each frame.
    shadowMapRenderer.SetShadowMapResource(shadowMap.GetTextureResource());
    CD3DX12_GPU_DESCRIPTOR_HANDLE shadowMapSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(CbvSrvDescriptorHeapManager::GetHeap()->GetGPUDescriptorHandleForHeapStart());
    shadowMapSRV = shadowMapSRV.Offset(shadowMap.GetOffsetInHeap(), CbvSrvDescriptorHeapManager::GetDescriptorSize()); // TODO: Move getting the SRV desc. handle into a Texture method.
    shadowMapRenderer.SetShadowMapSRV(shadowMapSRV);
    shadowMapRenderer.SetShadowPassViewport(shadowMapViewport);
    CD3DX12_GPU_DESCRIPTOR_HANDLE offsetsSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(CbvSrvDescriptorHeapManager::GetHeap()->GetGPUDescriptorHandleForHeapStart());
    offsetsSRV = offsetsSRV.Offset(pcfOffsetsTexture.GetOffsetInHeap(), CbvSrvDescriptorHeapManager::GetDescriptorSize()); // TODO: Move getting the SRV desc. handle into a Texture method.
    shadowMapRenderer.SetPCFOffsetsSRV(offsetsSRV);
    shadowMapRenderer.SetTracyContext(&tracyCtx);
}

void VoyagerEngine::LoadScene()
{
    // m_mainCamera.InitCamera(DirectX::XMFLOAT4(-2.f, 3.f, -7.f, 0.f), DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f), aspectRatio);
    // m_mainCamera.InitCamera(DirectX::XMFLOAT4(10.f, 20.f, -10.f, 0.f), DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f), aspectRatio);

    // m_mainCamera.InitCamera(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f), DirectX::XMFLOAT4(0.f, 0.f, -1.f, 1.f), aspectRatio);

    // Dragon camera position.
    // m_mainCamera.InitCamera(DirectX::XMFLOAT4(1.5f, 1.f, 1.5f, 0.f), DirectX::XMFLOAT4(-1.f, 0.f, 0.2f, 0.f), aspectRatio);

    // Cube camera position.
    m_mainCamera.InitCamera(DirectX::XMFLOAT4(-2.f, 2.f, -2.5f, 0.f), DirectX::XMFLOAT4(-0.3f, 0.4f, 0.5f, 0.f), aspectRatio);

    // Power Plant camera position.
    // m_mainCamera.InitCamera(DirectX::XMFLOAT4(18.f, 7.f, -12.f, 0.f), DirectX::XMFLOAT4(0.f, 5.f, 2.f, 0.f), aspectRatio);

    // Sponza camera position.
    // m_mainCamera.InitCamera(DirectX::XMFLOAT4(11.f, 6.7f, -1.f, 0.f), DirectX::XMFLOAT4(0.f, 5.9f, 2.2f, 0.f), aspectRatio);

    UpdateLightFitting();
}

void VoyagerEngine::PopulateCommandList()
{
    // Mark zone for Tracy.
    ZoneScoped;

    // We already waited for the GPU to finish work for this framebuffer (we cannot reset a commandAllocator before it's commands have finished executing!)
    ThrowIfFailed(m_commandAllocator[m_frameBufferIndex]->Reset());

    // However, when ExecuteCommandList() is called on a particular command
    // list, that command list can then be reset at any time and must be before
    // re-recording.
    ThrowIfFailed(m_commandList->Reset(m_commandAllocator[m_frameBufferIndex].Get(), materialLit.GetPSO().Get()));
    
    {
        // Start a D3D12 Tracy zone.
        TracyD3D12NamedZone(tracyCtx, whatVar, m_commandList.Get(), "Main cmdList", 1);

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
            renderer.SetLightingParametersBuffer(lightingParamsBuffer[m_frameBufferIndex]);
            renderer.SetRTV(rtvHandle);
            renderer.SetDSV(dsvHandle);
            renderer.SetViewport(m_viewport);
            renderer.Render(m_commandList, sceneObjects, m_frameBufferIndex);
            
            // Then render the projected shadow.
            sceneObjects = std::vector<SceneObject>{suzanne};
            projectionShadowRenderer.SetRTV(rtvHandle);
            projectionShadowRenderer.SetDSV(dsvHandle);
            projectionShadowRenderer.SetViewport(m_viewport);
            projectionShadowRenderer.SetLightingParametersBuffer(lightingParamsBuffer[m_frameBufferIndex]);
            projectionShadowRenderer.Render(m_commandList, sceneObjects, m_frameBufferIndex);
        }
        else if (chosenRenderingMode == RenderingState::RenderingMode::SHADOW_MAP) {
            // Render scene with simple shadow mapping.
            shadowMapRenderer.SetLightingParametersBuffer(lightingParamsBuffer[m_frameBufferIndex]);
            shadowMapRenderer.SetLightWVPBuffer(shadowMapWVPBuffers[m_frameBufferIndex]);
            shadowMapRenderer.SetRTV(rtvHandle);
            shadowMapRenderer.Render(m_commandList, sceneObjects, m_frameBufferIndex);
        }
        else {
            // Render scene objects with the default renderer.
            DefaultRenderer renderer;
            renderer.SetLightingParametersBuffer(lightingParamsBuffer[m_frameBufferIndex]);
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
    }

    ThrowIfFailed(m_commandList->Close());
}

void VoyagerEngine::WaitForPreviousFrame()
{
    // Mark a wait zone for Tracy.
    ZoneScoped;

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

bool VoyagerEngine::CheckTearingSupport()
{
    BOOL allowTearing = FALSE;

    ComPtr<IDXGIFactory5> factory5;
    if (SUCCEEDED(DXContext::getFactory()->QueryInterface(IID_PPV_ARGS(&factory5))))
    // if (SUCCEEDED(DXContext::getFactory().As(&factory5)))
    {
        if (FAILED(factory5->CheckFeatureSupport(
                DXGI_FEATURE_PRESENT_ALLOW_TEARING, 
                &allowTearing, sizeof(allowTearing))))
        {
            allowTearing = FALSE;
        }
    }

#ifdef _DEBUG
    std::cout << "INFO: Tearing support: " << (allowTearing == TRUE? "Yes" : "No") << std::endl;
#endif
    return allowTearing == TRUE;
}

void VoyagerEngine::SetLightPosition()
{
    lightParams.lightPosition = { 10, 10, -10 };
    // lightParams.lightPosition = { 6, 20, -6 }; // <- Good for sponza.

    // Copy our ConstantBuffer instance to the mapped constant buffer resource.
    memcpy(lightingParamsBuffer[0].GetMappedResourceAddress(), &lightParams.lightPosition, sizeof(lightParams.lightPosition));

    // Set the WVP matrices of the shadow mapping light.
    // Set the projection matrix.
    // DirectX::XMMATRIX tmpMat = DirectX::XMMatrixOrthographicLH(7.f, 7.f, 9.f, 25.f); // TODO: calculate this from the scene.
    DirectX::XMMATRIX tmpMat = DirectX::XMMatrixOrthographicLH(7.f, 7.f, 1.f, 50.f);
    // DirectX::XMMATRIX tmpMat = DirectX::XMMatrixOrthographicLH(50.f, 50.f, 1.f, 80.f); // <- Sponza settings.
    // DirectX::XMMATRIX tmpMat = DirectX::XMMatrixPerspectiveLH(7.f, 7.f, 9.f, 25.f);
    DirectX::XMStoreFloat4x4(&m_shadowMapLightCamera.projMat, tmpMat);
    // Set the view matrix.
    m_shadowMapLightCamera.camPosition = DirectX::XMVECTOR{10.f, 10.f, -10.f, 1.f};
    // m_shadowMapLightCamera.camPosition = DirectX::XMVECTOR{6.f, 20.f, -6.f, 1.f}; // <- Sponza settings.
    m_shadowMapLightCamera.camTarget = DirectX::XMVECTOR{0.f, 0.f, 0.f, 1.f};
    tmpMat = DirectX::XMMatrixLookAtLH(
        m_shadowMapLightCamera.camPosition,
        m_shadowMapLightCamera.camTarget,
        DirectX::XMVECTOR{0.f, 1.f, 0.f}
    );
    DirectX::XMStoreFloat4x4(&m_shadowMapLightCamera.viewMat, tmpMat);

    wvpConstantBuffer shadowMapLightWVP{};
    // The matrices need to be transposed because of column/row major ordering difference between hlsl and the DX implementations.
    DirectX::XMStoreFloat4x4(&shadowMapLightWVP.viewMat, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_shadowMapLightCamera.viewMat)));
    DirectX::XMStoreFloat4x4(&shadowMapLightWVP.projectionMat, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_shadowMapLightCamera.projMat)));
    memcpy(shadowMapWVPBuffers[0].GetMappedResourceAddress(), &shadowMapLightWVP, sizeof(shadowMapLightWVP));
}

std::vector<DirectX::XMFLOAT4> VoyagerEngine::FindSceneExtents()
{
    // For all scene objects, or for suzanne and ground plane...
    std::vector<SceneObject> objects {suzanne, groundPlane};
    
    // Gather all bounding vertices.
    std::vector<DirectX::XMFLOAT4> boundingVerts;
    for(SceneObject& object : objects) {
        std::vector<DirectX::XMFLOAT4> objectBoundingVerts = object.GetBoundingBox();
        boundingVerts.insert(
            std::end(boundingVerts),
            std::begin(objectBoundingVerts),
            std::end(objectBoundingVerts));
    }

    return Mesh::GetBoundingBox(boundingVerts);
}

void VoyagerEngine::UpdateLightFitting()
{
    // Test camera frustum calculation.
    // The frustum cube before perspective division?
    std::vector<DirectX::XMFLOAT4> ndcFrustum{
        DirectX::XMFLOAT4(-1, 1, 0, 1), // Near Plane
        DirectX::XMFLOAT4(-1, -1, 0, 1),
        DirectX::XMFLOAT4(1, -1, 0, 1),
        DirectX::XMFLOAT4(1, 1, 0, 1),
        DirectX::XMFLOAT4(-1, 1, 1, 1), // Far plane
        DirectX::XMFLOAT4(-1, -1, 1, 1),
        DirectX::XMFLOAT4(1, -1, 1, 1),
        DirectX::XMFLOAT4(1, 1, 1, 1)
    };

    // Multiply the frustum by inverse VP camera matrix.
    // Note: perspective divsion cannot be brought back! Or can it...?
    DirectX::XMMATRIX projectionMat = DirectX::XMLoadFloat4x4(&m_mainCamera.projMat);
    DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(&m_mainCamera.viewMat);

    DirectX::XMMATRIX VPMat = DirectX::XMMatrixMultiply(viewMat, projectionMat);
    DirectX::XMMATRIX VPMatInv = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(VPMat), VPMat);

    // Transform from observer clip into world space.
    std::vector<DirectX::XMFLOAT4> worldSpaceNdcFrustum;
    worldSpaceNdcFrustum.resize(ndcFrustum.size());
    DirectX::XMVector4TransformStream(
        worldSpaceNdcFrustum.data(),
        sizeof(DirectX::XMFLOAT4),
        ndcFrustum.data(),
        sizeof(DirectX::XMFLOAT4),
        ndcFrustum.size(),
        VPMatInv
    );

    // Get light space post-perspective coordinates.
    m_shadowMapLightCamera.GetPostProjectionPositions(worldSpaceNdcFrustum, false);
    std::vector<DirectX::XMFLOAT4> lightSpaceViewerFrustum = worldSpaceNdcFrustum;

    // Get a bounding box in light clip space of the viewer frustum, clamp to NDC range.
    // Note: without clipping the light frustum will follow the viewer outside of
    // the original light view range (so, if set correctly, outside of the scene).
    std::vector<DirectX::XMFLOAT4> clipSpaceFrustumBounds = Mesh::GetBoundingBox(lightSpaceViewerFrustum);
    for(auto& position : clipSpaceFrustumBounds) {
        DirectX::XMVECTOR point = DirectX::XMLoadFloat4(&position);
        point = DirectX::XMVectorClamp(
            point,
            DirectX::XMVectorSet(-1.f, -1.f, -std::numeric_limits<float>::infinity(), -10),
            DirectX::XMVectorSet(1.f, 1.f, std::numeric_limits<float>::infinity(), 10)
        );
        DirectX::XMStoreFloat4(&position, point);
    }
    // Now, with the frustum bounding box coordinates in light clip space,
    // clamped by max light XY settings, we can use x and y ob box for fitting.

    // Fit XY extent to the lightSpaceViewerFrustum.
    std::vector<DirectX::XMFLOAT4> minMaxCoordVals = Mesh::GetMinMaxPoints(clipSpaceFrustumBounds);
    float sx = 2.f / (minMaxCoordVals[1].x - minMaxCoordVals[0].x);
    float sy = 2.f / (minMaxCoordVals[1].y - minMaxCoordVals[0].y);
    float ox = (minMaxCoordVals[1].x + minMaxCoordVals[0].x) * -sx / 2.f;
    float oy = (minMaxCoordVals[1].y + minMaxCoordVals[0].y) * -sy / 2.f;

    DirectX::XMFLOAT4X4 fittingMatrix {
        sx, 0.f, 0.f, ox,
        0.f, sy, 0.f, oy,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f
    };
    // Store this matrix into the light camera.
    // Utilize the proj matrix slot for that, no need to modify the shader.
    DirectX::XMMATRIX cameraProjection = DirectX::XMLoadFloat4x4(&m_shadowMapLightCamera.projMat);
    DirectX::XMMATRIX fit = DirectX::XMLoadFloat4x4(&fittingMatrix);
    // We need to transpose either matrix: the fit matrix is normal and will
    // work as is in shaders too. The DX Math projection matrix needs to be
    // transposed to work in hlsl.
    fit = DirectX::XMMatrixTranspose(fit);
    cameraProjection = cameraProjection * fit;

    // Store the fitted matrix into camera WVP buffer.
    wvpConstantBuffer shadowMapLightWVP{};
    // The matrices need to be transposed because of column/row major ordering difference between hlsl and the DX implementations.
    DirectX::XMStoreFloat4x4(&shadowMapLightWVP.viewMat, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_shadowMapLightCamera.viewMat)));
    DirectX::XMStoreFloat4x4(&shadowMapLightWVP.projectionMat, DirectX::XMMatrixTranspose(cameraProjection));
    memcpy(shadowMapWVPBuffers[m_frameBufferIndex].GetMappedResourceAddress(), &shadowMapLightWVP, sizeof(shadowMapLightWVP));

    // Find z scene extents in light space.
    // TODO: god, can't be boothered with this, we will use hand-adjusted
    // near and far planes...
    // std::vector<DirectX::XMFLOAT4> sceneBoungingBox = FindSceneExtents();

    {
        // Find orientation matrix L.
        // For this, first move the light view vector into light view space.
        DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(&m_mainCamera.viewMat);
        DirectX::XMMATRIX invViewMat = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(viewMat), viewMat);
        DirectX::XMVECTOR viewVecWorld = DirectX::XMVector4Transform(DirectX::XMVectorSet(0.f, 0.f, -1.f, 0.f), invViewMat);
        DirectX::XMFLOAT4 viewVecWorldFloat4;
        DirectX::XMStoreFloat4(&viewVecWorldFloat4, viewVecWorld);
        std::vector<DirectX::XMFLOAT4> pos {viewVecWorldFloat4};
        m_shadowMapLightCamera.GetPostViewPositions(pos);
        DirectX::XMVECTOR viewVecLightSpace = DirectX::XMLoadFloat4(&pos[0]);
        viewVecLightSpace = DirectX::XMVectorSetZ(viewVecLightSpace, 0.f); // Make the view vec perpendicular to the light view.
        // Create the rotation matrix.
        DirectX::XMMATRIX matrixL = DirectX::XMMatrixLookAtLH(
            DirectX::XMVectorSet(0.f, 0.f, 0.f, 1.f),
            viewVecLightSpace,
            DirectX::XMVectorSet(0.f, 0.f, 1.f, 0.f));

        // Find the warp view and projection matrices.
        // PSR convex body should be used for this, we will use the clipped bounding
        // box of the view frustum. Move PSR into LPV of the light and find n and f.
        // worldSpaceNdcFrustum - This is already in light post-perspective (post projection divide, should it?)!
        std::vector<DirectX::XMFLOAT4> frustumRotatedInLightSpace;
        frustumRotatedInLightSpace.resize(worldSpaceNdcFrustum.size());
        DirectX::XMVector4TransformStream(
            frustumRotatedInLightSpace.data(),
            sizeof(DirectX::XMFLOAT4),
            worldSpaceNdcFrustum.data(),
            sizeof(DirectX::XMFLOAT4),
            worldSpaceNdcFrustum.size(),
            matrixL
        );
        std::vector<DirectX::XMFLOAT4> bounds = Mesh::GetMinMaxPoints(frustumRotatedInLightSpace);
        float minZ = bounds[0].z;
        float maxZ = bounds[1].z;
        // Find center of projection.
        float pX = (bounds[1].x - bounds[0].x) / 2.f;
        float pY = (bounds[1].y - bounds[0].y) / 2.f;

        // Now, give up.
    }

}

void VoyagerEngine::CreatePCFOffsetTexture()
{
    // Prepare CPU storage for the data.
    const int windowSize = 16;
    const int filterSize = 8;
    // Will be stored into a 3D texture of windowSize x windowSize x (filterSize * filterSize * 2).
    // This means the sample patterns will repeat every windowSize x windowSize in screen space.
    // The filter will have filterSize * filterSize float2 offsets, stored in half the depth texels in xy and zw.
    std::vector<float> data(windowSize * windowSize * filterSize * filterSize * 2, 0.5f);

    // Generate the data.
    int index = 0;
    for(int texY = 0; texY < windowSize; texY++) {
        for(int texX = 0; texX < windowSize; texX++) {
            for(int v = filterSize - 1; v >= 0; v--) {
                for(int u = 0; u < filterSize; u++) {
                    // randFloat() has range [0:1], so (randFloat() - 0.5f) has range [-0.5:0.5].
                    // Divide by filter size to get all samples in normalized range [0:1] in x and y.
                    float x = ((float)u + 0.5f + (randFloat() - 0.5f)) / (float)filterSize;
                    float y = ((float)v + 0.5f + (randFloat() - 0.5f)) / (float)filterSize;

                    // The sqrtf(y) works as a distance to center. Since we first iterate through
                    // all large y values, the generated outer samples are first in the filter.
                    data[index] = sqrtf(y) * cosf(2 * DirectX::XM_PI * x);
                    data[index + 1] = sqrtf(y) * sinf(2 * DirectX::XM_PI * x);

                    // std::cout << sqrtf(powf((data[index] - 0.f), 2) + powf(data[index + 1] - 0.f, 2)) << std::endl;

                    index += 2;
                }
            }
        }
    }

    // Store the data into a texture.
    CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex3D(
        DXGI_FORMAT_R32G32B32A32_FLOAT, // TODO: Maybe 16 bits per-component?
        filterSize * filterSize / 2, windowSize, windowSize, 1U
    );
    D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc{};
    viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    viewDesc.Format = desc.Format;
    viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
    viewDesc.Texture3D.MipLevels = 1;

    D3D12_SUBRESOURCE_DATA subData{};
    subData.pData = data.data();
    // subData.RowPitch = filterSize * filterSize / 2;
    // subData.SlicePitch = filterSize * filterSize / 2 * windowSize;
    subData.RowPitch = (filterSize * filterSize / 2) * 128 / 8;
    subData.SlicePitch = subData.RowPitch * windowSize;
    
    pcfOffsetsTexture.CreateFromData(desc, viewDesc, subData);
}

DirectX::XMFLOAT3 VoyagerEngine:: normalize(DirectX::XMFLOAT3 vec) {
    float length = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    return DirectX::XMFLOAT3(vec.x / length, vec.y / length, vec.z / length);
}

DirectX::XMFLOAT3 VoyagerEngine::scale(DirectX::XMFLOAT3 vec, float scale) {
    return DirectX::XMFLOAT3(vec.x * scale, vec.y * scale, vec.z * scale);
}

float VoyagerEngine::randFloat() {
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
