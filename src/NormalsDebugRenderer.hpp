#pragma once

#include "Renderer.hpp"
#include "NormalsDebugMaterial.hpp"

class NormalsDebugRenderer : public Renderer {
public:
    NormalsDebugRenderer() {};
    ~NormalsDebugRenderer() {};

    void SetMaterial(NormalsDebugMaterial* material) { normalsMaterial = material; }

    virtual void Render(ComPtr<ID3D12GraphicsCommandList> commandList, const std::vector<SceneObject>& sceneObjects, UINT currentFrameBufferIndex);

private:
    // The overriding material of this renderer.
    NormalsDebugMaterial* normalsMaterial = nullptr;
};