#pragma once

#include "Material.hpp"

/* This class should be used as a base for all non-overwriting
standard scene object materials. The root parameters should not be changed
to allow for a uniform binding scheme across multiple materials (meaning
a shared Root Signature between many PSOs)
*/
class StandardMaterial : public Material
{
public:
    StandardMaterial() = default;
    StandardMaterial(const std::string vertexShaderFileName, const std::string pixelShaderFileName);

    enum RootSignatureParameters {
        CBV_LIGHT_PARAMS = 0,
        CBV_WVP = 1,
        TABLE_TEXTURE_ALBEDO = 2,
        PARAM_COUNT
    };

private:
    std::vector<D3D12_DESCRIPTOR_RANGE> descriptorTableVertexRanges;
    std::vector<D3D12_DESCRIPTOR_RANGE> descriptorTablePixelRanges;
    std::vector<D3D12_ROOT_PARAMETER> rootParameters;

    // Inheriting classes should not override these methods.
    virtual std::vector<D3D12_ROOT_PARAMETER> CreateRootParameters() final;
    virtual D3D12_ROOT_SIGNATURE_FLAGS CreateRootSignatureFlags() final;
    virtual std::vector<D3D12_STATIC_SAMPLER_DESC> CreateSamplers() final { return Material::CreateSamplers(); };
};
