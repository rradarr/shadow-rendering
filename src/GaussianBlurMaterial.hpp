#pragma once

#include "Material.hpp"

class GaussianBlurMaterial : public Material
{
public:
    GaussianBlurMaterial() = default;
    GaussianBlurMaterial(const std::string vertexShaderFileName, const std::string pixelShaderFileName) {
        Material(vertexShaderFileName, pixelShaderFileName);
    };

    enum RootSignatureParameters {
        CONSTANT_FILTER_DIRECTION = 0,
        CBV_FILTER_PARAMS = 1,
        TABLE_TEXTURE_INPUT = 2,
        PARAM_COUNT
    };

private:
    std::vector<D3D12_DESCRIPTOR_RANGE> descriptorTablePixelRanges;
    std::vector<D3D12_ROOT_PARAMETER> rootParameters;

    // Inheriting classes can override the following methods to specialize.
    virtual std::vector<D3D12_ROOT_PARAMETER> CreateRootParameters();
    virtual void CustomizePipelineStateObjectDescription(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc);
    virtual std::vector<D3D12_STATIC_SAMPLER_DESC> CreateSamplers();
    virtual D3D12_ROOT_SIGNATURE_FLAGS CreateRootSignatureFlags();
};
