#pragma once
#include "Renderer.hpp"

#include "ShadowMapDepthMaterial.hpp"
#include "ShadowMapMainMaterial.hpp"
#include "ResourceManager.hpp"

#include "TracyD3D12.hpp"

using Microsoft::WRL::ComPtr;

class ShadowMapRenderer : public Renderer
{
public:
    ShadowMapRenderer() {};
    ~ShadowMapRenderer() {};

    void SetShadowPassViewport(CD3DX12_VIEWPORT viewport);
    void SetMainMaterial(ShadowMapMainMaterial* material) { mainMaterial = material; }
    void SetDepthPassMaterial(ShadowMapDepthMaterial* material) { depthPassMaterial = material; }
    void SetLightWVPBuffer(MappedResourceLocation wvpBuffer) { lightWVPBuffer = wvpBuffer; }
    void SetLightingParametersBuffer(MappedResourceLocation lightingParameters) { lightingParametersBuffer = lightingParameters; }
    void SetShadowMapResource(ComPtr<ID3D12Resource> shadowMapResource) {shadowMap = shadowMapResource; }
    void SetDepthPassDSV(CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle) { shadowMapDSVHandle = dsvHandle; }
    void SetShadowMapSRV(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle) { shadowMapSRVHandle = srvHandle; }
    void SetPCFOffsetsSRV(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle) { pcfOffsetsSRVHandle = srvHandle; }

    void SetTracyContext(TracyD3D12Ctx* tracyContext) { tracyCtx = tracyContext; }

    virtual void Render(ComPtr<ID3D12GraphicsCommandList> commandList, const std::vector<SceneObject>& sceneObjects, UINT currentFrameBufferIndex);

private:
    CD3DX12_VIEWPORT shadowPassViewport;
    CD3DX12_RECT shadowPassScissor;

    ShadowMapMainMaterial* mainMaterial = nullptr;
    ShadowMapDepthMaterial* depthPassMaterial = nullptr;

    MappedResourceLocation lightWVPBuffer;
    MappedResourceLocation lightingParametersBuffer;

    ComPtr<ID3D12Resource> shadowMap;
    CD3DX12_CPU_DESCRIPTOR_HANDLE shadowMapDSVHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE shadowMapSRVHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE pcfOffsetsSRVHandle;

    TracyD3D12Ctx* tracyCtx;
};

/*
As anoter day passes
as the spots of sun begin to climb the walls
I try to remember the morning
and I don't know if there was any.
*/