#pragma once

#include "Material.hpp"

class ShadowMapDepthMaterial : public Material
{
public:
    ShadowMapDepthMaterial() = default;
    ShadowMapDepthMaterial(const std::string vertexShaderFileName) {
        Material();
        SetShaders(vertexShaderFileName);
    };

    virtual void SetShaders(const std::string vertexShaderFileName);
    // Final step of material creation, actually compiles shaders, allocates DX resources etc.
    virtual void CreateMaterial();

    enum RootSignatureParameters {
        CBV_WVP = 0,
        CBV_LIGHT_WVP = 1,
        PARAM_COUNT
    };

private:
    std::vector<D3D12_ROOT_PARAMETER> rootParameters;

    // Inheriting classes can override the following methods to specialize.
    virtual std::vector<D3D12_ROOT_PARAMETER> CreateRootParameters();
    virtual void CustomizePipelineStateObjectDescription(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc);
};
