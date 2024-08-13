#pragma once

using Microsoft::WRL::ComPtr;

class EngineObject;

class Renderer
{
public:
    Renderer() = default;
    virtual ~Renderer() = 0;

    // Record commands into the commandList that render the scaneObjects.
    virtual void Render(ComPtr<ID3D12GraphicsCommandList> commandList, const std::vector<EngineObject>& sceneObjects, UINT currentFrameBufferIndex) = 0;
};