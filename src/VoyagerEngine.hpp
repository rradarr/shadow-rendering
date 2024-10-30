#pragma once

#include "Engine.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"
#include "RenderingComponents.hpp"
#include "SceneObject.hpp"
#include "ResourceManager.hpp"
#include "ImGuiController.hpp"
#include "WireframeRenderer.hpp"
#include "NormalsDebugRenderer.hpp"
#include "ProjectionShadowRenderer.hpp"
#include "ShadowMapRenderer.hpp"

#include "TracyD3D12.hpp"

using Microsoft::WRL::ComPtr;

class VoyagerEngine : public Engine
{
public:
    VoyagerEngine(UINT windowWidth, UINT windowHeight, std::string windowName);

    virtual void OnInit(HWND windowHandle);
    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnDestroy();

    virtual void OnKeyDown(UINT8 keyCode);
    virtual void OnKeyUp(UINT8 keyCode);
    virtual void OnMouseMove(int mouseX, int mouseY);
    virtual void OnGotFocus() {};
    virtual void OnLostFocus() {};

private:
    static const UINT mc_frameBufferCount = 3;

    // this is the structure of the world, view, projection constant buffer (passed as a root descriptor).
    struct wvpConstantBuffer {
        //union {
            DirectX::XMFLOAT4X4 wvpMat;
            DirectX::XMFLOAT4X4 worldMat;
            DirectX::XMFLOAT4X4 viewMat;
            DirectX::XMFLOAT4X4 projectionMat;
            // space for one more? Nope: 4*4*32 = 512, 512*4 = 2048, 2048 = 256*8
            //BYTE padding[256]; // Constant buffers must be 256-byte aligned which has to do with constant reads on the GPU.
        //};
    };

    struct lightParamsConstantBuffer {
        DirectX::XMFLOAT3 lightPosition; BYTE pad[4];
        // Here we store the light near and far planes, square frustum size and world size of the light.
        DirectX::XMFLOAT4 lightNearFarFrustumSize;
        // Here we store in a silly way the light algorithm type (eg. for shadow mapping type of PCF), kernel size, offset scale and in-shader bias.
        DirectX::XMFLOAT4 lightFilterKernelScaleBias;
        // Here only the shadow map ambient and sampler kind are stored for now.
        DirectX::XMFLOAT4 mapAmbientSampler;
        BYTE padding[192]; // Constant buffers must be 256-byte aligned which has to do with constant reads on the GPU.
    };
    //int ConstantBufferPerObjectAlignedSize = (sizeof(wvpConstantBuffer) + 255) & ~255;

    // Pipeline objects.
    CD3DX12_VIEWPORT m_viewport; // Area that the view-space (rasterizer outputt, between 0,1) will be streched to (and make up the screen-space).
    CD3DX12_RECT m_scissorRect; // Area in cscreen-space that will be drawn.
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12Resource> m_renderTargets[mc_frameBufferCount];
    ComPtr<ID3D12CommandAllocator> m_commandAllocator[mc_frameBufferCount];
    ComPtr<ID3D12GraphicsCommandList> m_commandList; // As many as therads, so one. It can be reset immidiately after submitting.
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
    UINT m_rtvDescriptorSize;

    ComPtr<ID3D12Resource> m_depthStencilBuffer;
    ComPtr<ID3D12DescriptorHeap> m_dsHeap;
    UINT m_dsvHeapIncrement;
    // For main shader access heap the static CbvSrvDescriptorHeapManager is used.

    bool allowsTearing = false;

    // Textures.
    Texture sampleTexture;
    Texture shadowMap;
    CD3DX12_CPU_DESCRIPTOR_HANDLE shadowMapDSVHeapLocation;
    CD3DX12_VIEWPORT shadowMapViewport;
    Texture pcfOffsetsTexture;
    // Materials.
    DefaultTexturedMaterial materialTextured;
    Material materialNoTex;
    WireframeMaterial materialWireframe;
    NormalsDebugMaterial materialNormalsDebug;
    LitMaterial materialLit;
    ProjectionShadowMaterial materialProjectionShadow;
    ShadowMapDepthMaterial materialShadowMapDepth;
    ShadowMapMainMaterial materialShadowMapMain;
    // Renderers.
    WireframeRenderer wireframeRenderer;
    NormalsDebugRenderer normalsDebugRenderer;
    ProjectionShadowRenderer projectionShadowRenderer;
    ShadowMapRenderer shadowMapRenderer;

    // Meshes and SceneObjects.
    Mesh suzanneMesh;
    Mesh groundPlaneMesh;
    SceneObject suzanne;
    SceneObject groundPlane;

    std::vector<SceneObject> sceneObjects;

    ResourceManager resourceManager;
    
    ImGuiController imguiController;

    // Scene objects
    Camera m_mainCamera;
    Camera m_shadowMapLightCamera;
    wvpConstantBuffer m_wvpPerObject;
    lightParamsConstantBuffer lightParams;
    // They will be influenced by GUI so we need per-buffer data.
    std::vector<MappedResourceLocation> lightingParamsBuffer;
    // Since we update the shadow view each frame we need per-frame buffers.
    // MappedResourceLocation shadowMapWVPBuffer;
    std::vector<MappedResourceLocation> shadowMapWVPBuffers;

    // Synchronization objects.
    UINT m_frameBufferIndex;
    UINT m_frameIndex;
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence[mc_frameBufferCount];
    UINT64 m_fenceValue[mc_frameBufferCount];

    // Tracy objects.
    TracyD3D12Ctx tracyCtx;

    void LoadPipeline();
    void LoadAssets();
    void LoadMaterials();
    void LoadScene();
    void PopulateCommandList();
    void WaitForPreviousFrame();

    void OnEarlyUpdate();

    // Utility methods.
    bool CheckTearingSupport();
    void SetLightPosition();
    std::vector<DirectX::XMFLOAT4> FindSceneExtents();
    void UpdateLightFitting();
    void CreatePCFOffsetTexture();

    DirectX::XMFLOAT3 normalize(DirectX::XMFLOAT3 vec);
    DirectX::XMFLOAT3 scale(DirectX::XMFLOAT3 vec, float scale);
    float VoyagerEngine::randFloat();
};

