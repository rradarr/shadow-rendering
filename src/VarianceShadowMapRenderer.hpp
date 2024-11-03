#pragma once
#include "Renderer.hpp"

#include "VarianceShadowMapMainMaterial.hpp"
#include "VarianceShadowMapDepthMaterial.hpp"
#include "ResourceManager.hpp"

#include "TracyD3D12.hpp"

using Microsoft::WRL::ComPtr;

class VarianceShadowMapRenderer : public Renderer
{
public:
    VarianceShadowMapRenderer() {};
    ~VarianceShadowMapRenderer() {};

    void SetShadowPassViewport(CD3DX12_VIEWPORT viewport);
    void SetMainMaterial(VarianceShadowMapMainMaterial* material) { mainMaterial = material; }
    void SetDepthPassMaterial(VarianceShadowMapDepthMaterial* material) { depthPassMaterial = material; }
    void SetLightWVPBuffer(MappedResourceLocation wvpBuffer) { lightWVPBuffer = wvpBuffer; }
    void SetLightingParametersBuffer(MappedResourceLocation lightingParameters) { lightingParametersBuffer = lightingParameters; }
    void SetShadowMapResource(ComPtr<ID3D12Resource> shadowMapResource) {shadowMap = shadowMapResource; }
    void SetDepthPassDSV(CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle) { shadowMapDSVHandle = dsvHandle; }
    void SetDepthPassDepthResource(ComPtr<ID3D12Resource> shadowMapDepthResource) { shadowMapDepthStencilResource = shadowMapDepthResource; }
    void SetShadowMapSRV(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle) { shadowMapSRVHandle = srvHandle; }
    void SetShadowMapRTV(CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle) { shadowMapRTVHandle = rtvHandle; }
    // void SetPCFOffsetsSRV(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle) { pcfOffsetsSRVHandle = srvHandle; }

    void SetTracyContext(TracyD3D12Ctx* tracyContext) { tracyCtx = tracyContext; }

    virtual void Render(ComPtr<ID3D12GraphicsCommandList> commandList, const std::vector<SceneObject>& sceneObjects, UINT currentFrameBufferIndex);

private:
    CD3DX12_VIEWPORT shadowPassViewport;
    CD3DX12_RECT shadowPassScissor;

    VarianceShadowMapMainMaterial* mainMaterial = nullptr;
    VarianceShadowMapDepthMaterial* depthPassMaterial = nullptr;
    // TODO VarianceShadowMapBlurMaterial* blurPassMaterial = nullptr;

    MappedResourceLocation lightWVPBuffer;
    MappedResourceLocation lightingParametersBuffer;

    ComPtr<ID3D12Resource> shadowMapDepthStencilResource;
    CD3DX12_CPU_DESCRIPTOR_HANDLE shadowMapDSVHandle;
    ComPtr<ID3D12Resource> shadowMap;
    CD3DX12_CPU_DESCRIPTOR_HANDLE shadowMapRTVHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE shadowMapSRVHandle;
    ComPtr<ID3D12Resource> tmpShadowMap; // Used for filtering
    D3D12_GPU_DESCRIPTOR_HANDLE tmpShadowMapSRVHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE pcfOffsetsSRVHandle;

    TracyD3D12Ctx* tracyCtx;
};