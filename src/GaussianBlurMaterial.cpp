#include "stdafx.hpp"
#include "GaussianBlurMaterial.hpp"

std::vector<D3D12_ROOT_PARAMETER> GaussianBlurMaterial::CreateRootParameters()
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

    D3D12_ROOT_DESCRIPTOR rootCBVFilterParamsDescriptor;
    rootCBVFilterParamsDescriptor.ShaderRegister = 0; // b0 in shader
    rootCBVFilterParamsDescriptor.RegisterSpace = 0;

    D3D12_ROOT_CONSTANTS rootConstantDirectionDescriptor;
    rootConstantDirectionDescriptor.Num32BitValues = 1;
    rootConstantDirectionDescriptor.ShaderRegister = 1; // b1 in shader
    rootConstantDirectionDescriptor.RegisterSpace = 0;

    rootParameters.resize(PARAM_COUNT);
    rootParameters[CONSTANT_FILTER_DIRECTION].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    rootParameters[CONSTANT_FILTER_DIRECTION].Constants = rootConstantDirectionDescriptor;
    rootParameters[CONSTANT_FILTER_DIRECTION].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    rootParameters[CBV_FILTER_PARAMS].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[CBV_FILTER_PARAMS].Descriptor = rootCBVFilterParamsDescriptor;
    rootParameters[CBV_FILTER_PARAMS].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    
    rootParameters[TABLE_TEXTURE_INPUT].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[TABLE_TEXTURE_INPUT].DescriptorTable = descriptorTablePixel;
    rootParameters[TABLE_TEXTURE_INPUT].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    return rootParameters;
}

void GaussianBlurMaterial::CustomizePipelineStateObjectDescription(D3D12_GRAPHICS_PIPELINE_STATE_DESC &psoDesc)
{
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32_FLOAT;
    psoDesc.DepthStencilState.DepthEnable = false;
    psoDesc.DepthStencilState.StencilEnable = false;
}

std::vector<D3D12_STATIC_SAMPLER_DESC> GaussianBlurMaterial::CreateSamplers()
{
    std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;
    D3D12_STATIC_SAMPLER_DESC samplerAlbedo = {};
    samplerAlbedo.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    samplerAlbedo.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    samplerAlbedo.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    samplerAlbedo.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    samplerAlbedo.MipLODBias = 0;
    samplerAlbedo.MaxAnisotropy = 0;
    samplerAlbedo.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    samplerAlbedo.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
    samplerAlbedo.MinLOD = 0.f;
    samplerAlbedo.MaxLOD = D3D12_FLOAT32_MAX;
    samplerAlbedo.ShaderRegister = 0; // s0 in shader
    samplerAlbedo.RegisterSpace = 0;
    samplerAlbedo.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    samplers.push_back(samplerAlbedo);

    return samplers;
}

D3D12_ROOT_SIGNATURE_FLAGS GaussianBlurMaterial::CreateRootSignatureFlags()
{
    D3D12_ROOT_SIGNATURE_FLAGS flags = (
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
        D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

    return flags;
}
