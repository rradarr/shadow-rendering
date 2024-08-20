#pragma once

#include "Renderer.hpp"
#include "WireframeMaterial.hpp"

class WireframeRenderer : public Renderer {
public:
    WireframeRenderer() {};
    ~WireframeRenderer() {};

    void SetMaterial(WireframeMaterial* material) { wireframeMaterial = material; }

    virtual void Render(ComPtr<ID3D12GraphicsCommandList> commandList, const std::vector<SceneObject>& sceneObjects, UINT currentFrameBufferIndex);

private:
    // The overriding material of this renderer.
    WireframeMaterial* wireframeMaterial = nullptr;
};