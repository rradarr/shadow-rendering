#pragma once

using Microsoft::WRL::ComPtr;

class SceneObject;

class Renderer
{
public:
    Renderer() = default;
    virtual ~Renderer() = 0;

    // Record commands into the commandList that render the scaneObjects.
    virtual void Render(ComPtr<ID3D12GraphicsCommandList> commandList, const std::vector<SceneObject>& sceneObjects, UINT currentFrameBufferIndex) = 0;

    void SetRTV(CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle);
    void SetDSV(CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle);
    /* The scissor will be set to the full extent of this viewport. */
    virtual void SetViewport(CD3DX12_VIEWPORT viewport);

protected:
    CD3DX12_RECT CreateFullViewScissor(CD3DX12_VIEWPORT viewport);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle;
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle;
    CD3DX12_VIEWPORT viewport;
    CD3DX12_RECT scissor;
};