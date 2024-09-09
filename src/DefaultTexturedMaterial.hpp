#pragma once
#include "StandardMaterial.hpp"
class DefaultTexturedMaterial : public StandardMaterial
{
public:
    DefaultTexturedMaterial() = default;
    DefaultTexturedMaterial(const std::string vertexShaderFileName, const std::string pixelShaderFileName);

// TODO: Clean this up.
/*
    enum RootSignatureParameters {
        TABLE_LIGHT_PARAMS = 0,
        CBV_WVP = 1,
        TABLE_TEXTURE_ALBEDO = 2,
        PARAM_COUNT
    };

private:
    std::vector<D3D12_DESCRIPTOR_RANGE> descriptorTableVertexRanges;
    std::vector<D3D12_DESCRIPTOR_RANGE> descriptorTablePixelRanges;
    std::vector<D3D12_ROOT_PARAMETER> rootParameters;

    // Inheriting classes can override the following methods to specialize.
    virtual std::vector<D3D12_ROOT_PARAMETER> CreateRootParameters();
    virtual D3D12_ROOT_SIGNATURE_FLAGS CreateRootSignatureFlags();
*/
};
