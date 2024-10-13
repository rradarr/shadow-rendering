#pragma once
#include "Renderer.hpp"

#include "ShadowMapDepthMaterial.hpp"
#include "ShadowMapMainMaterial.hpp"
#include "ResourceManager.hpp"

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
};

/*
As anoter day passes
as the spots of sun begin to climb the walls
I try to remember the morning
and I don't know if there was any.
*/