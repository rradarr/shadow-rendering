#include "stdafx.h"
#include "VoyagerEngine.h"

#include "DXSampleHelper.h"
#include "CbvSrvDescriptorHeapManager.h"
#include "TextureLoader.h"
#include "WindowsApplication.h"
#include "Timer.h"
#include "DXContext.h"
#include "BufferMemoryManager.h"
#include "EngineHelpers.h"

#include "SceneObject.h"
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
    hasFocus = true;
    windowCenter = { static_cast<float>(windowWidth) / 2.f, static_cast<float>(windowHeight) / 2.f };
    keyboradMovementInput = { 0, 0, 0 };
    mouseDelta = { 0, 0 };
}

void VoyagerEngine::OnInit(HWND windowHandle)
{
    this->windowHandle = windowHandle;

    LoadPipeline();
    LoadAssets();
    LoadScene();
    imguiController.InitImGui(windowHandle, mc_frameBufferCount, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
    std::cout << "Engine initialized." << std::endl;
}

void VoyagerEngine::OnUpdate()
{
    m_frameBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
    WaitForPreviousFrame();

    OnEarlyUpdate();

    DirectX::XMFLOAT2 delta;
    DirectX::XMStoreFloat2(&delta, mouseDelta);
    m_mainCamera.UpdateCamera(delta, keyboradMovementInput);
    //keyboradMovementInput = { 0, 0, 0 };

    Timer* timer = Timer::GetInstance();
    double deltaTime = timer->GetDeltaTime();
    // Store the view matrix (for lighting).
    DirectX::XMStoreFloat4x4(&m_wvpPerObject.viewMat, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_mainCamera.viewMat)));
    DirectX::XMStoreFloat4x4(&m_wvpPerObject.projectionMat, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_mainCamera.projMat)));

    // update app logic, such as moving the camera or figuring out what objects are in view
    static float rIncrement = float(0.2 * deltaTime);
    static float gIncrement = float(0.6 * deltaTime);
    static float bIncrement = float(0.9 * deltaTime);

    m_cbData.colorMultiplier.x += rIncrement;
    m_cbData.colorMultiplier.y += gIncrement;
    m_cbData.colorMultiplier.z += bIncrement;

    if (m_cbData.colorMultiplier.x >= 1.0f || m_cbData.colorMultiplier.x <= 0.0f)
    {
        m_cbData.colorMultiplier.x = m_cbData.colorMultiplier.x >= 1.0f ? 1.0f : 0.0f;
        rIncrement = -rIncrement;
    }
    if (m_cbData.colorMultiplier.y >= 1.0f || m_cbData.colorMultiplier.y <= 0.0f)
    {
        m_cbData.colorMultiplier.y = m_cbData.colorMultiplier.y >= 1.0f ? 1.0f : 0.0f;
        gIncrement = -gIncrement;
    }
    if (m_cbData.colorMultiplier.z >= 1.0f || m_cbData.colorMultiplier.z <= 0.0f)
    {
        m_cbData.colorMultiplier.z = m_cbData.colorMultiplier.z >= 1.0f ? 1.0f : 0.0f;
        bIncrement = -bIncrement;
    }

    // copy our ConstantBuffer instance to the mapped constant buffer resource
    memcpy(cbColorMultiplierGPUAddress[m_frameBufferIndex], &m_cbData, sizeof(m_cbData));

    // Update object positions.
    // create rotation matrices
    DirectX::XMMATRIX rotXMat = DirectX::XMMatrixRotationX(float(0.0 * deltaTime));
    DirectX::XMMATRIX rotYMat = DirectX::XMMatrixRotationY(float(0.2 * deltaTime));
    DirectX::XMMATRIX rotZMat = DirectX::XMMatrixRotationZ(float(0.0 * deltaTime));

    DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(&m_mainCamera.viewMat); // load view matrix
    DirectX::XMMATRIX projMat = DirectX::XMLoadFloat4x4(&m_mainCamera.projMat); // load projection matrix

    for (SceneObject &sceneObject : sceneObjects) {

        DirectX::XMMATRIX rotMat = DirectX::XMLoadFloat4x4(&sceneObject.rotation) * sceneObject.delta_rotXMat * sceneObject.delta_rotYMat * sceneObject.delta_rotZMat;

        // create translation matrix for cube 1 from cube 1's position vector
        DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat4(&sceneObject.position));

        DirectX::XMStoreFloat4x4(&sceneObject.rotation, rotMat);

        // we want cube 2 to be half the size of cube 1, so we scale it by .5 in all dimensions

        float scale = 0.5f;
        DirectX::XMMATRIX scaleMat =  DirectX::XMMatrixScaling(scale, scale, scale);

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

    // position the ship
    DirectX::XMVECTOR pos = m_mainCamera.camPosition;
    pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(m_mainCamera.localFront, 0.8f)); // move ship in front of camera
    pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(m_mainCamera.localUp, -0.15f)); // move ship down
    DirectX::XMStoreFloat4(&ship.position, pos);

    DirectX::XMMATRIX rotMat = DirectX::XMLoadFloat4x4(&ship.rotation);
    rotMat = rotMat * m_mainCamera.rotMat;
    DirectX::XMStoreFloat4x4(&ship.rotation, rotMat);
    DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslationFromVector(pos);
    DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(0.05f, 0.05f, 0.05f);

    //DirectX::XMMATRIX worldMat = scaleMat * translationMat * m_mainCamera.rotMat;
    DirectX::XMMATRIX worldMat = scaleMat * rotMat * translationMat;
    DirectX::XMStoreFloat4x4(&ship.worldMat, worldMat);

    DirectX::XMStoreFloat4x4(&m_wvpPerObject.worldMat, DirectX::XMMatrixTranspose(worldMat));
    /*DirectX::XMStoreFloat4x4(&m_wvpPerObject.viewMat, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_mainCamera.viewMat)));
    DirectX::XMStoreFloat4x4(&m_wvpPerObject.projectionMat, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_mainCamera.projMat)));*/
    DirectX::XMMATRIX transposed = DirectX::XMMatrixTranspose(worldMat * viewMat * projMat); // must transpose wvp matrix for the gpu
    DirectX::XMStoreFloat4x4(&m_wvpPerObject.wvpMat, transposed); // store transposed wvp matrix in constant buffer

    ship.UpdateWVPMatrices(&m_wvpPerObject, sizeof(m_wvpPerObject), m_frameBufferIndex);
    //memcpy(m_WVPConstantBuffersGPUAddress[m_frameBufferIndex] + sizeof(m_wvpPerObject) * ship.idx, &m_wvpPerObject, sizeof(m_wvpPerObject));

    imguiController.UpdateImGui();

    // std::cout << "Updated" << std::endl;
}

void VoyagerEngine::OnRender()
{
    //m_frameBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
    //WaitForPreviousFrame();

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

    //WaitForPreviousFrame();

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
    switch (keyCode) {
    case 0x57: // W
        keyboradMovementInput.z = 1;
        break;
    case 0x53: // S
        keyboradMovementInput.z = -1;
        break;
    case 0x44: // D
        keyboradMovementInput.x = 1;
        break;
    case 0x41: // A
        keyboradMovementInput.x = -1;
        break;
    case 0x20: // SPACE
        keyboradMovementInput.y = 1;
        break;
    case 0x11: // CTRL
        keyboradMovementInput.y = -1;
        break;
    };
}

void VoyagerEngine::OnKeyUp(UINT8 keyCode)
{
    switch (keyCode) {
    case 0x57: // W
        keyboradMovementInput.z = keyboradMovementInput.z == 1? 0 : keyboradMovementInput.z;
        break;
    case 0x53: // S
        keyboradMovementInput.z = keyboradMovementInput.z == -1? 0 : keyboradMovementInput.z;
        break;
    case 0x44: // D
        keyboradMovementInput.x = keyboradMovementInput.x == 1 ? 0 : keyboradMovementInput.x;
        break;
    case 0x41: // A
        keyboradMovementInput.x = keyboradMovementInput.x == -1 ? 0 : keyboradMovementInput.x;
        break;
    case 0x20: // SPACE
        keyboradMovementInput.y = keyboradMovementInput.y == 1 ? 0 : keyboradMovementInput.y;
        break;
    case 0x11: // CTRL
        keyboradMovementInput.y = keyboradMovementInput.y == -1 ? 0 : keyboradMovementInput.y;
        break;
    case 0x58: // X
        useWireframe = !useWireframe;
    };
}

void VoyagerEngine::OnMouseMove(int mouseX, int mouseY)
{
    if (hasFocus)
    {
        mousePos = { static_cast<float>(mouseX), static_cast<float>(mouseY) };
    }
}

void VoyagerEngine::OnGotFocus()
{
    hasFocus = true;
}

void VoyagerEngine::OnLostFocus()
{
    hasFocus = false;
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

    CbvSrvDescriptorHeapManager::CreateHeap(mc_frameBufferCount + 2);
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

    // Create the buffers for use with the root signature
    {
        // Create the constant buffer commited resource.
        // We will update the constant buffer one or more times per frame, so we will use only an upload heap
        // unlike previously we used an upload heap to upload the vertex and index data, and then copied over
        // to a default heap. If you plan to use a resource for more than a couple frames, it is usually more
        // efficient to copy to a default heap where it stays on the gpu. In this case, our constant buffer
        // will be modified and uploaded at least once per frame, so we only use an upload heap.

        BufferMemoryManager buffMng;

        // Create the resources as commited resources, and pointers to cbv for each frame
        for (int i = 0; i < mc_frameBufferCount; ++i)
        {
            // Size of the resource heap must be a multiple of 64KB for single-textures and constant buffers
            // Will be data that is read from so we keep it in the generic read state.
            buffMng.AllocateBuffer(m_constantDescriptorTableBuffers[i], 1042 * 64, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
            m_constantDescriptorTableBuffers[i]->SetName(L"Constant Buffer Table Buffer Upload Resource Heap");

            m_cbvPerFrameSize = (sizeof(ColorConstantBuffer) + 255) & ~255;

            // Create the cbviews.
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
            cbvDesc.BufferLocation = m_constantDescriptorTableBuffers[i]->GetGPUVirtualAddress();
            cbvDesc.SizeInBytes = m_cbvPerFrameSize;    // CB size is required to be 256-byte aligned.
            CbvSrvDescriptorHeapManager::AddConstantBufferView(cbvDesc);
            std::cout << CbvSrvDescriptorHeapManager::GetCurrentHeadOffset() << std::endl;

            /*DXContext::getDevice().Get()->CreateConstantBufferView(&cbvDesc, m_shaderAccessHeapHeadHandle);
            m_shaderAccessHeapHeadHandle.Offset(1, m_shaderAccessDescriptorSize);
            std::cout << m_shaderAccessHeapHeadHandle.ptr << std::endl;*/
        }
        ZeroMemory(&m_cbData, sizeof(m_cbData)); // Zero out the memory of our data.

        // Map the GPU memory of the constant buffer to a CPU-accesible region.
        CD3DX12_RANGE readRange(0, 0);    // We do not intend to read from this resource on the CPU. (End is less than or equal to begin)
        for (int i = 0; i < mc_frameBufferCount; ++i)
        {
            m_constantDescriptorTableBuffers[i]->Map(0, &readRange, reinterpret_cast<void**>(&cbColorMultiplierGPUAddress[i]));
            memcpy(cbColorMultiplierGPUAddress[i], &m_cbData, sizeof(m_cbData));
        }
    }
    // Create the buffers for wvp martices
    {
        // create the constant buffer resource heap
        // We will update the constant buffer one or more times per frame, so we will use only an upload heap
        // unlike previously we used an upload heap to upload the vertex and index data, and then copied over
        // to a default heap. If you plan to use a resource for more than a couple frames, it is usually more
        // efficient to copy to a default heap where it stays on the gpu. In this case, our constant buffer
        // will be modified and uploaded at least once per frame, so we only use an upload heap

        // first we will create a resource heap (upload heap) for each frame for the models constant buffers
        // As you can see, we are allocating 64KB for each resource we create. Buffer resource heaps must be
        // an alignment of 64KB. We are creating 3 resources, one for each frame. Each constant buffer is
        // only a 4x4 matrix of floats in this tutorial. So with a float being 4 bytes, we have
        // 16 floats in one constant buffer, and we will store 2 constant buffers in each
        // heap, one for each cube, thats only 64x2 bits, or 128 bits we are using for each
        // resource, and each resource must be at least 64KB (65536 bits)
        // Create our Constant Buffer descriptor heap

        BufferMemoryManager buffMng;

        for (int i = 0; i < mc_frameBufferCount; ++i)
        {
            // create resource for cube 1
            // size of the resource heap. Must be a multiple of 64KB for single-textures and constant buffers
            buffMng.AllocateBuffer(m_WVPConstantBuffers[i], 1024 * 64, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
            m_WVPConstantBuffers[i]->SetName(L"Root Constant Buffer Upload Resource Heap");

            ZeroMemory(&m_wvpPerObject, sizeof(m_wvpPerObject));

            CD3DX12_RANGE readRange(0, 0);    // We do not intend to read from this resource on the CPU. (so end is less than or equal to begin)
            // map the resource heap to get a gpu virtual address to the beginning of the heap
            ThrowIfFailed(m_WVPConstantBuffers[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_WVPConstantBuffersGPUAddress[i])));

            // Because of the constant read alignment requirements, constant buffer views must be 256 bit aligned. Our buffers are smaller than 256 bits,
            // so we need to add spacing between the two buffers, so that the second buffer starts at 256 bits from the beginning of the resource heap.
            memcpy(m_WVPConstantBuffersGPUAddress[i], &m_wvpPerObject, sizeof(m_wvpPerObject)); // cube1's constant buffer data
            memcpy(m_WVPConstantBuffersGPUAddress[i] + sizeof(m_wvpPerObject), &m_wvpPerObject, sizeof(m_wvpPerObject)); // cube2's constant buffer data
        }
    }

    // Create the buffer for lighting parameters.
    {
        ZeroMemory(&lightParams, sizeof(lightParams));
        lightingParamsBuffer = resourceManager.AddMappedUploadResource(&lightParams, sizeof(lightParams));
        
        /*
        BufferMemoryManager buffMng;

        // Size of the resource heap. Must be a multiple of 64KB for single-textures and constant buffers.
        buffMng.AllocateBuffer(m_LigtParamConstantBuffer, 1024 * 64, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
        m_LigtParamConstantBuffer->SetName(L"Root Constant Buffer Upload Resource Heap (lighting params)");

        ZeroMemory(&lightParams, sizeof(lightParams));

        CD3DX12_RANGE readRange(0, 0);    // We do not intend to read from this resource on the CPU. (so end is less than or equal to begin)
        // Map the resource heap to get a gpu virtual address to the beginning of the heap.
        ThrowIfFailed(m_LigtParamConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_LightParamConstantBufferGPUAddres)));

        // Because of the constant read alignment requirements, constant buffer views must be 256 bit aligned.
        memcpy(m_LightParamConstantBufferGPUAddres, &lightParams, sizeof(lightParams.lightPosition));
        */
    }

    // Load the texture
    {
        sampleTexture.CreateFromFile("Sci_fi_Metal_Panel_006_basecolor.jpg"); // Create the texture from file.
    }

    // Create the vertex and index buffers.
    {
        shipMesh.CreateFromFile("ship_v1_normals_test.obj");
        ship = SceneObject(int(sceneObjects.size()), &shipMesh);
        
        // TODO new code
        CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CbvSrvDescriptorHeapManager::GetHeap()->GetGPUDescriptorHandleForHeapStart());
        descriptorHandle = descriptorHandle.Offset(sampleTexture.GetOffsetInHeap(), CbvSrvDescriptorHeapManager::GetDescriptorSize());
        ship.SetAlbedoTexture(descriptorHandle);
        ship.SetMaterial(&materialLit);
        std::vector<MappedResourceLocation> WVPResources;
        for(int i = 0; i < mc_frameBufferCount; i++){
            ZeroMemory(&m_wvpPerObject, sizeof(m_wvpPerObject));
            MappedResourceLocation resource = resourceManager.AddMappedUploadResource(&m_wvpPerObject, sizeof(m_wvpPerObject));
            WVPResources.push_back(resource);
        }
        ship.SetWVPPerFrameBufferLocations(WVPResources);
        // /TODO new code

        //SceneObject sceneObject = SceneObject(sceneObjects.size(), shipMesh);
        ship.position = DirectX::XMFLOAT4(2.f, 0.0f, 0.0f, 0.0f);
        ship.delta_rotXMat = DirectX::XMMatrixRotationX(0.0f);
        ship.delta_rotYMat = DirectX::XMMatrixRotationY(0.01f);
        ship.delta_rotZMat = DirectX::XMMatrixRotationZ(0.0f);


        DirectX::XMVECTOR posVec = DirectX::XMLoadFloat4(&ship.position);
        DirectX::XMMATRIX tmpMat = DirectX::XMMatrixTranslationFromVector(posVec);
        DirectX::XMStoreFloat4x4(&ship.worldMat, tmpMat);
        DirectX::XMStoreFloat4x4(&ship.rotation, DirectX::XMMatrixIdentity());

        // Create the depth/stencil heap and buffer.
        {
            BufferMemoryManager buffMng;

            D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
            dsvHeapDesc.NumDescriptors = 1;
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
    }

    LoadMaterials();
    SetLightPosition();

    std::cout << "Assets loaded." << std::endl;
}

void VoyagerEngine::LoadMaterials()
{
    // TODO these don't compile because of incompatible root signatures and shaders!
    
    // materialTextured.SetShaders("VertexShader.hlsl", "PixelShader.hlsl");
    // materialTextured.CreateMaterial();

    // materialNoTex.SetShaders("VertexShader_noTex.hlsl", "PixelShader_noTex.hlsl");
    // materialNoTex.CreateMaterial();

    // materialWireframe.SetShaders("VertexShader_wireframe.hlsl", "PixelShader_wireframe.hlsl");
    // materialWireframe.CreateMaterial();

    // materialNormalsDebug.SetShaders("VertexShader_normalsDebug.hlsl", "PixelShader_normalsDebug.hlsl");
    // materialNormalsDebug.CreateMaterial();

    materialLit.SetShaders("VertexShader_lit.hlsl", "PixelShader_lit.hlsl");
    materialLit.CreateMaterial();
}

void VoyagerEngine::LoadScene()
{
    m_mainCamera.InitCamera(DirectX::XMFLOAT4(0.f, 0.f, -8.f, 0.f), DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f), aspectRatio);

    //position piramids
    pyramid1Position = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR posVec = DirectX::XMLoadFloat4(&pyramid1Position);
    DirectX::XMMATRIX tmpMat = DirectX::XMMatrixTranslationFromVector(posVec);
    DirectX::XMStoreFloat4x4(&pyramid1WorldMat, tmpMat);
    DirectX::XMStoreFloat4x4(&pyramid1RotMat, DirectX::XMMatrixIdentity());

    pyramid2PositionOffset = DirectX::XMFLOAT4(1.5f, 0.0f, 0.0f, 0.0f);
    posVec = DirectX::XMVectorAdd(DirectX::XMLoadFloat4(&pyramid1Position), DirectX::XMLoadFloat4(&pyramid2PositionOffset));
    tmpMat = DirectX::XMMatrixTranslationFromVector(posVec);
    DirectX::XMStoreFloat4x4(&pyramid2WorldMat, tmpMat);
    DirectX::XMStoreFloat4x4(&pyramid2RotMat, DirectX::XMMatrixIdentity());
}

void VoyagerEngine::PopulateCommandList()
{
    // We already waited for the GPU to finish work for this framebuffer (we cannot reset a commandAllocator before it's commands have finished executing!)
    ThrowIfFailed(m_commandAllocator[m_frameBufferIndex]->Reset());

    // However, when ExecuteCommandList() is called on a particular command
    // list, that command list can then be reset at any time and must be before
    // re-recording.
    ThrowIfFailed(m_commandList->Reset(m_commandAllocator[m_frameBufferIndex].Get(), materialLit.GetPSO().Get()));

    ID3D12DescriptorHeap* descriptorHeaps[] = { CbvSrvDescriptorHeapManager::GetHeap().Get() };
    m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    DefaultRenderer renderer;

    // CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorHeapStartHandle{CbvSrvDescriptorHeapManager::GetHeap()->GetGPUDescriptorHandleForHeapStart()};
    // renderer.SetDescriptorHeapStartHandle(descriptorHeapStartHandle);
    
    renderer.SetRenderTargetResource(m_renderTargets[m_frameBufferIndex].Get());

    renderer.SetLightingParametersBuffer(lightingParamsBuffer);
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVHeap->GetCPUDescriptorHandleForHeapStart(), m_frameBufferIndex, m_rtvDescriptorSize);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsHeap->GetCPUDescriptorHandleForHeapStart());
    renderer.SetRTV(rtvHandle);
    renderer.SetDSV(dsvHandle);
    
    renderer.SetViewport(m_viewport);

    std::vector<SceneObject> sceneObjects{ship};
    renderer.Render(m_commandList, sceneObjects, m_frameBufferIndex);

    //TODO: refactor ImGui rendering - the issue is with the render target transition barrier. It can't be within the renderer or imgui has bo be in the renderer...
    // RenderImGui(m_commandList);

    /*
    // Inside defaultRenderer:
    {
        // Indicate that the back buffer will be used as a render target.
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_commandList->ResourceBarrier(1, &barrier);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVHeap->GetCPUDescriptorHandleForHeapStart(), m_frameBufferIndex, m_rtvDescriptorSize);
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsHeap->GetCPUDescriptorHandleForHeapStart());
        // set the render target for the output merger stage (the output of the pipeline)
        m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

        // Set necessary state.
        m_commandList->SetGraphicsRootSignature(materialLit.GetRootSignature().Get());
        m_commandList->RSSetViewports(1, &m_viewport);
        m_commandList->RSSetScissorRects(1, &m_scissorRect);

        if (useWireframe) {
            m_commandList->SetPipelineState(materialWireframe.GetPSO().Get());
            m_commandList->SetGraphicsRootSignature(materialWireframe.GetRootSignature().Get());
        }

        // Record commands.
        const float clearColor[] = { 0.005f, 0.005f, 0.005f, 1.0f };
        m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        m_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
        m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // set constant buffer descriptor table heap and srv descriptor table heap
        ID3D12DescriptorHeap* descriptorHeaps[] = { CbvSrvDescriptorHeapManager::GetHeap().Get() };
        m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
        // Set the root table at index 2 to the texture.
        CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CbvSrvDescriptorHeapManager::GetHeap()->GetGPUDescriptorHandleForHeapStart());
        if (!useWireframe) {
            m_commandList->SetGraphicsRootDescriptorTable(DefaultTexturedMaterial::TABLE_TEXTURE_ALBEDO, descriptorHandle.Offset(sampleTexture.GetOffsetInHeap(), CbvSrvDescriptorHeapManager::GetDescriptorSize()));
            m_commandList->SetGraphicsRootConstantBufferView(DefaultTexturedMaterial::TABLE_LIGHT_PARAMS, m_LigtParamConstantBuffer->GetGPUVirtualAddress());
        }
        //CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorHandle(CbvSrvDescriptorHeapManager::GetHeap()->GetGPUDescriptorHandleForHeapStart());
        //m_commandList->SetGraphicsRootDescriptorTable(0, descriptorHandle.Offset(m_frameBufferIndex, CbvSrvDescriptorHeapManager::GetDescriptorSize()));

        // set the root descriptor table 2 to the srv
        //descriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CbvSrvDescriptorHeapManager::GetHeap()->GetGPUDescriptorHandleForHeapStart());
        //m_commandList->SetGraphicsRootDescriptorTable(2, descriptorHandle.Offset(sampleTexture.GetOffsetInHeap(), CbvSrvDescriptorHeapManager::GetDescriptorSize()));

        //CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CbvSrvDescriptorHeapManager::GetHeap()->GetGPUDescriptorHandleForHeapStart());
        //m_commandList->SetGraphicsRootDescriptorTable(0, descriptorHandle.Offset(m_frameBufferIndex, CbvSrvDescriptorHeapManager::GetDescriptorSize()));

        // draw ball
        int idx = 0;
        for (int i = 0; i < sceneObjects.size(); i++) {
            sceneObjects[i].mesh.InsertBufferBind(m_commandList);
            // set the root constant at index 0 for mvp matix
            m_commandList->SetGraphicsRootConstantBufferView(0, m_WVPConstantBuffers[m_frameBufferIndex]->GetGPUVirtualAddress() + sizeof(wvpConstantBuffer) * sceneObjects[i].idx);
            sceneObjects[i].mesh.InsertDrawIndexed(m_commandList);
        }

        // draw ship
        ship.mesh.InsertBufferBind(m_commandList);
        m_commandList->SetGraphicsRootConstantBufferView(0, m_WVPConstantBuffers[m_frameBufferIndex]->GetGPUVirtualAddress() + sizeof(wvpConstantBuffer) * ship.idx);
        ship.mesh.InsertDrawIndexed(m_commandList);

        //
        //m_commandList->SetPipelineState(materialWireframe.GetPSO().Get());
        //m_commandList->SetGraphicsRootSignature(materialWireframe.GetRootSignature().Get());


        //shipMesh.InsertBufferBind(m_commandList);
        //m_commandList->SetGraphicsRootConstantBufferView(0, m_WVPConstantBuffers[m_frameBufferIndex]->GetGPUVirtualAddress() + sizeof(wvpConstantBuffer) * planets.size());
        //shipMesh.InsertDrawIndexed(m_commandList);

        // Indicate that the back buffer will now be used to present.
        barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_commandList->ResourceBarrier(1, &barrier);
    }
    */

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
    lightParams.lightPosition = { 0, 0, 0 };

    // copy our ConstantBuffer instance to the mapped constant buffer resource
    memcpy(lightingParamsBuffer.GetMappedResourceAddress(), &lightParams.lightPosition, sizeof(lightParams.lightPosition));

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

    // Update FPS display every half a second.
    static double timeTillFpsDisplayUpdate = 0.0;
    timeTillFpsDisplayUpdate += timer->GetDeltaTime();
    if (timeTillFpsDisplayUpdate > 0.5)
    {
        timeTillFpsDisplayUpdate = 0.0;
        std::cout << timer->GetFps() << "\r";
    }

    GetMouseDelta();
}

void VoyagerEngine::GetMouseDelta()
{
    mouseDelta = DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&mousePos), DirectX::XMLoadFloat2(&windowCenter));
    DirectX::XMFLOAT2 delta;
    DirectX::XMStoreFloat2(&delta, mouseDelta);

    if (hasFocus)
    {
        POINT center = { LONG(windowCenter.x), LONG(windowCenter.y) };
        ClientToScreen(windowHandle, &center);
        SetCursorPos(center.x, center.y);
    }
}
