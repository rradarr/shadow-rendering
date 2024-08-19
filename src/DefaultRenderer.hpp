#pragma once

#include "Renderer.hpp"
#include "ResourceManager.hpp"

/* A default renderer that does not override the SceneObject materials. */
class DefaultRenderer : public Renderer
{
public:
    DefaultRenderer() {};
    ~DefaultRenderer() {};

    virtual void Render(ComPtr<ID3D12GraphicsCommandList> commandList, const std::vector<SceneObject>& sceneObjects, UINT currentFrameBufferIndex);

    /* This pointer will be used in state barrires. */
    void SetRenderTargetResource(ComPtr<ID3D12Resource> renderTargetResource);
    void SetRTV(CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle);
    void SetDSV(CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle);

    /* The scissor will be set to the full extent of this viewport. */
    void SetViewport(CD3DX12_VIEWPORT viewport);

    /* The descriptor heap needs to be already bound by the application. It needs to contain
    all descriptors that will be used for rendering. */
    // void SetDescriptorHeapStartHandle(CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorHeapHandle);

    void SetLightingParametersBuffer(MappedResourceLocation lightingParameters);

private:
    CD3DX12_RECT CreateScissor(CD3DX12_VIEWPORT viewport);

    ComPtr<ID3D12Resource> renderTargetResource;
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle;
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle;
    CD3DX12_VIEWPORT viewport;
    CD3DX12_RECT scissor;
    // CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorHeapHandle;

    MappedResourceLocation lightingParametersBuffer;
};