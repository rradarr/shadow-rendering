#pragma once

#include "Material.hpp"

class VarianceShadowMapMainMaterial : public Material
{
public:
    VarianceShadowMapMainMaterial() = default;
    VarianceShadowMapMainMaterial(const std::string vertexShaderFileName, const std::string pixelShaderFileName) {
        Material(vertexShaderFileName, pixelShaderFileName);
    };

    enum RootSignatureParameters {
        CBV_WVP = 0,
        CBV_LIGHT_PARAMS = 1,
        CBV_LIGHT_WVP = 2,
        TABLE_TEXTURE_ALBEDO = 3,
        TABLE_TEXTURE_SHADOW = 4, // Note: we have to tables bc I can't be asked to keep the descriptors contiguous.
        // TABLE_TEXTURE_VARIANCE = 5,
        PARAM_COUNT
    };

private:
    std::vector<D3D12_DESCRIPTOR_RANGE> descriptorTablePixelRanges;
    std::vector<D3D12_DESCRIPTOR_RANGE> descriptorTableShadowPixelRanges;
    std::vector<D3D12_ROOT_PARAMETER> rootParameters;

    // Inheriting classes should not override these methods.
    virtual std::vector<D3D12_ROOT_PARAMETER> CreateRootParameters();
    virtual void CustomizePipelineStateObjectDescription(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc);
    virtual std::vector<D3D12_STATIC_SAMPLER_DESC> CreateSamplers();
    virtual D3D12_ROOT_SIGNATURE_FLAGS CreateRootSignatureFlags();
};
