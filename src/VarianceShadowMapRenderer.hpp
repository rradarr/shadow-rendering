#pragma once
#include "Renderer.hpp"

#include "VarianceShadowMapMainMaterial.hpp"
#include "VarianceShadowMapDepthMaterial.hpp"
#include "GaussianBlurMaterial.hpp"
#include "ResourceManager.hpp"
#include "SceneObject.hpp"
#include "EngineStateModel.hpp"

#include "TracyD3D12.hpp"

using Microsoft::WRL::ComPtr;

class VarianceShadowMapRenderer : public Renderer
{
public:
    VarianceShadowMapRenderer() { engineState = EngineStateModel::GetInstance(); };
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
    void SetBlurMaterial(GaussianBlurMaterial* blurMaterial) { blurPassMaterial = blurMaterial; }
    void SetBlurResource(ComPtr<ID3D12Resource> blurTmpResource) { tmpShadowMap = blurTmpResource; }
    void SetBlurSRV(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle) { tmpShadowMapSRVHandle = srvHandle; }
    void SetBlurRTV(CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle) { tmpShadowMapRTVHandle = rtvHandle; }
    void SetBlurParametersBufer(MappedResourceLocation blurParameters) { blurringParametersBuffer = blurParameters; }
    void SetFullScreenQuad(SceneObject fullscreenQuad) { fullScreenQuad = fullscreenQuad; }
    // void SetPCFOffsetsSRV(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle) { pcfOffsetsSRVHandle = srvHandle; }

    void SetTracyContext(TracyD3D12Ctx* tracyContext) { tracyCtx = tracyContext; }

    virtual void Render(ComPtr<ID3D12GraphicsCommandList> commandList, const std::vector<SceneObject>& sceneObjects, UINT currentFrameBufferIndex);

private:
    CD3DX12_VIEWPORT shadowPassViewport;
    CD3DX12_RECT shadowPassScissor;

    VarianceShadowMapMainMaterial* mainMaterial = nullptr;
    VarianceShadowMapDepthMaterial* depthPassMaterial = nullptr;
    GaussianBlurMaterial* blurPassMaterial = nullptr;

    MappedResourceLocation lightWVPBuffer;
    MappedResourceLocation lightingParametersBuffer;

    ComPtr<ID3D12Resource> shadowMapDepthStencilResource;
    CD3DX12_CPU_DESCRIPTOR_HANDLE shadowMapDSVHandle;
    ComPtr<ID3D12Resource> shadowMap;
    CD3DX12_CPU_DESCRIPTOR_HANDLE shadowMapRTVHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE shadowMapSRVHandle;
    ComPtr<ID3D12Resource> tmpShadowMap; // Used for filtering
    D3D12_GPU_DESCRIPTOR_HANDLE tmpShadowMapSRVHandle;
    CD3DX12_CPU_DESCRIPTOR_HANDLE tmpShadowMapRTVHandle;
    MappedResourceLocation blurringParametersBuffer;
    SceneObject fullScreenQuad;
    // D3D12_GPU_DESCRIPTOR_HANDLE pcfOffsetsSRVHandle;

    EngineStateModel* engineState = nullptr;

    TracyD3D12Ctx* tracyCtx;
};