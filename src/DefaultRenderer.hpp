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

    void SetLightingParametersBuffer(MappedResourceLocation lightingParameters);

private:
    MappedResourceLocation lightingParametersBuffer;
};