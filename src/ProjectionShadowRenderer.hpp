#pragma once

#include "Renderer.hpp"
#include "ProjectionShadowMaterial.hpp"
#include "ResourceManager.hpp"

class ProjectionShadowRenderer : public Renderer {
public:
    ProjectionShadowRenderer() {};
    ~ProjectionShadowRenderer() {};

    void SetMaterial(ProjectionShadowMaterial* material) { projectionShadowMaterial = material; }
    void SetLightingParametersBuffer(MappedResourceLocation lightingParameters);

    virtual void Render(ComPtr<ID3D12GraphicsCommandList> commandList, const std::vector<SceneObject>& sceneObjects, UINT currentFrameBufferIndex);

private:
    // The overriding material of this renderer.
    ProjectionShadowMaterial* projectionShadowMaterial = nullptr;
    MappedResourceLocation lightingParametersBuffer;
};
