#include "stdafx.hpp"
#include "LitMaterial.hpp"

LitMaterial::LitMaterial(const std::string vertexShaderFileName, const std::string pixelShaderFileName)
{
    Material(vertexShaderFileName, pixelShaderFileName);
}
/*
std::vector<D3D12_ROOT_PARAMETER> LitMaterial::CreateRootParameters()
{
    // Create the root descriptor (for wvp matrices)
    D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
    rootCBVDescriptor.ShaderRegister = 0; // b0 in shader
    rootCBVDescriptor.RegisterSpace = 0;

    // Create another root descriptor (for lighting parameters)
    D3D12_ROOT_DESCRIPTOR lightingCBVDescriptor;
    lightingCBVDescriptor.ShaderRegister = 1; // b1 in shader
    lightingCBVDescriptor.RegisterSpace = 0;

    // Create the descriptor table for the texture resource.
    descriptorTablePixelRanges.resize(1);
    descriptorTablePixelRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorTablePixelRanges[0].NumDescriptors = 1;
    descriptorTablePixelRanges[0].BaseShaderRegister = 0; // t0 in shader
    descriptorTablePixelRanges[0].RegisterSpace = 0;
    descriptorTablePixelRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // create a descriptor table for the pixel stage
    D3D12_ROOT_DESCRIPTOR_TABLE descriptorTablePixel;
    descriptorTablePixel.NumDescriptorRanges = static_cast<UINT>(descriptorTablePixelRanges.size());
    descriptorTablePixel.pDescriptorRanges = descriptorTablePixelRanges.data(); // the pointer to the beginning of our ranges array

    // create a root parameter and fill it out
    rootParameters.resize(PARAM_COUNT);
    // WVP matrix.
    rootParameters[CBV_WVP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[CBV_WVP].Descriptor = rootCBVDescriptor;
    rootParameters[CBV_WVP].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    // Lighting parameters.
    rootParameters[CBV_LIGHT_PARAMS].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[CBV_LIGHT_PARAMS].Descriptor = lightingCBVDescriptor;
    rootParameters[CBV_LIGHT_PARAMS].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    // Texture.
    rootParameters[TABLE_TEXTURE_ALBEDO].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // this is a descriptor table
    rootParameters[TABLE_TEXTURE_ALBEDO].DescriptorTable = descriptorTablePixel; // this is our descriptor table for this root parameter
    rootParameters[TABLE_TEXTURE_ALBEDO].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // pixel will access srv (texture)

    return rootParameters;
}

D3D12_ROOT_SIGNATURE_FLAGS LitMaterial::CreateRootSignatureFlags()
{
    D3D12_ROOT_SIGNATURE_FLAGS flags = (
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

    return flags;
}
*/