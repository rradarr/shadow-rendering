#pragma once

#include "Material.hpp"

class ProjectionShadowMaterial : public Material
{
public:
    ProjectionShadowMaterial() = default;
    ProjectionShadowMaterial(const std::string vertexShaderFileName, const std::string pixelShaderFileName) {
        Material(vertexShaderFileName, pixelShaderFileName);
    };

    enum RootSignatureParameters {
        CBV_WVP = 0,
        CBV_LIGHT_PARAMS = 1,
        PARAM_COUNT
    };

private:
    std::vector<D3D12_ROOT_PARAMETER> rootParameters;

    // Inheriting classes can override the following methods to specialize.
    virtual std::vector<D3D12_ROOT_PARAMETER> CreateRootParameters();
    virtual void CustomizePipelineStateObjectDescription(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc);
};