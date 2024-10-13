#include "stdafx.hpp"
#include "ShadowMapMainMaterial.hpp"

std::vector<D3D12_ROOT_PARAMETER> ShadowMapMainMaterial::CreateRootParameters()
{
    descriptorTablePixelRanges.resize(1);
    descriptorTablePixelRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorTablePixelRanges[0].NumDescriptors = 1;
    descriptorTablePixelRanges[0].BaseShaderRegister = 0; // t0 in shader
    descriptorTablePixelRanges[0].RegisterSpace = 0;
    descriptorTablePixelRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // this appends the range to the end of the root signature descriptor tables

    D3D12_ROOT_DESCRIPTOR_TABLE descriptorTablePixel;
    descriptorTablePixel.NumDescriptorRanges = static_cast<UINT>(descriptorTablePixelRanges.size());
    descriptorTablePixel.pDescriptorRanges = descriptorTablePixelRanges.data();

    // Shadow map texture descriptor table.
    descriptorTableShadowPixelRanges.resize(1);
    descriptorTableShadowPixelRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorTableShadowPixelRanges[0].NumDescriptors = 1;
    descriptorTableShadowPixelRanges[0].BaseShaderRegister = 1; // t1 in shader
    descriptorTableShadowPixelRanges[0].RegisterSpace = 0;
    descriptorTableShadowPixelRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_ROOT_DESCRIPTOR_TABLE descriptorTablePixelShadow;
    descriptorTablePixelShadow.NumDescriptorRanges = static_cast<UINT>(descriptorTableShadowPixelRanges.size());
    descriptorTablePixelShadow.pDescriptorRanges = descriptorTableShadowPixelRanges.data();

    // Create root descriptor for object wvp matrices.
    D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
    rootCBVDescriptor.ShaderRegister = 0; // b0 in shader
    rootCBVDescriptor.RegisterSpace = 0;

    // Create root descriptor for light parameters.
    D3D12_ROOT_DESCRIPTOR rootCBVLightParamsDescriptor;
    rootCBVLightParamsDescriptor.ShaderRegister = 1; // b1 in shader
    rootCBVLightParamsDescriptor.RegisterSpace = 0;

    // Create root descriptor for light wvp matrices.
    D3D12_ROOT_DESCRIPTOR rootCBVLightWVPDescriptor;
    rootCBVLightWVPDescriptor.ShaderRegister = 2; // b2 in shader
    rootCBVLightWVPDescriptor.RegisterSpace = 0;

    // create a root parameter and fill it out
    rootParameters.resize(PARAM_COUNT);
    rootParameters[CBV_WVP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[CBV_WVP].Descriptor = rootCBVDescriptor;
    rootParameters[CBV_WVP].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    
    rootParameters[CBV_LIGHT_PARAMS].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[CBV_LIGHT_PARAMS].Descriptor = rootCBVLightParamsDescriptor;
    rootParameters[CBV_LIGHT_PARAMS].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;  // TODO: Light params are per vertex in other materials?

    rootParameters[CBV_LIGHT_WVP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[CBV_LIGHT_WVP].Descriptor = rootCBVLightWVPDescriptor;
    rootParameters[CBV_LIGHT_WVP].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    rootParameters[TABLE_TEXTURE_ALBEDO].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[TABLE_TEXTURE_ALBEDO].DescriptorTable = descriptorTablePixel;
    rootParameters[TABLE_TEXTURE_ALBEDO].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    rootParameters[TABLE_TEXTURE_SHADOW].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[TABLE_TEXTURE_SHADOW].DescriptorTable = descriptorTablePixelShadow;
    rootParameters[TABLE_TEXTURE_SHADOW].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    return rootParameters;
}

void ShadowMapMainMaterial::CustomizePipelineStateObjectDescription(D3D12_GRAPHICS_PIPELINE_STATE_DESC &psoDesc)
{

}

std::vector<D3D12_STATIC_SAMPLER_DESC> ShadowMapMainMaterial::CreateSamplers()
{
    // Create the static sampler description for the albedo texture.
    D3D12_STATIC_SAMPLER_DESC samplerAlbedo = {};
    samplerAlbedo.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    samplerAlbedo.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerAlbedo.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerAlbedo.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerAlbedo.MipLODBias = 0;
    samplerAlbedo.MaxAnisotropy = 0;
    samplerAlbedo.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    samplerAlbedo.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    samplerAlbedo.MinLOD = 0.f;
    samplerAlbedo.MaxLOD = D3D12_FLOAT32_MAX;
    samplerAlbedo.ShaderRegister = 0; // s0 in shader
    samplerAlbedo.RegisterSpace = 0;
    samplerAlbedo.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // Create the static sampler description for the depth shadow map.
    D3D12_STATIC_SAMPLER_DESC samplerDepthShadowMap = {};
    samplerDepthShadowMap.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    samplerDepthShadowMap.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    samplerDepthShadowMap.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    samplerDepthShadowMap.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    samplerDepthShadowMap.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
    samplerDepthShadowMap.MipLODBias = 0;
    samplerDepthShadowMap.MaxAnisotropy = 0;
    samplerDepthShadowMap.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    samplerDepthShadowMap.MinLOD = 0.f;
    samplerDepthShadowMap.MaxLOD = D3D12_FLOAT32_MAX;
    samplerDepthShadowMap.ShaderRegister = 1; // s1 in shader
    samplerDepthShadowMap.RegisterSpace = 0;
    samplerDepthShadowMap.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    return std::vector<D3D12_STATIC_SAMPLER_DESC>{samplerAlbedo, samplerDepthShadowMap};
}

D3D12_ROOT_SIGNATURE_FLAGS ShadowMapMainMaterial::CreateRootSignatureFlags()
{
    D3D12_ROOT_SIGNATURE_FLAGS flags = (
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

    return flags;
}
