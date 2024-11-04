#include "stdafx.hpp"
#include "VarianceShadowMapDepthMaterial.hpp"

std::vector<D3D12_ROOT_PARAMETER> VarianceShadowMapDepthMaterial::CreateRootParameters()
{
    // Create the root descriptor (for wvp matrices)
    D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
    rootCBVDescriptor.ShaderRegister = 0; // b0 in shader
    rootCBVDescriptor.RegisterSpace = 0;

    // Create another root descriptor (for light camera wvp matrix)
    D3D12_ROOT_DESCRIPTOR rootCBVLightWVPDescriptor;
    rootCBVLightWVPDescriptor.ShaderRegister = 1; // b1 in shader
    rootCBVLightWVPDescriptor.RegisterSpace = 0;

    // create a root parameter and fill it out
    rootParameters.resize(PARAM_COUNT);
    rootParameters[CBV_WVP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[CBV_WVP].Descriptor = rootCBVDescriptor;
    rootParameters[CBV_WVP].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    rootParameters[CBV_LIGHT_WVP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[CBV_LIGHT_WVP].Descriptor = rootCBVLightWVPDescriptor;
    rootParameters[CBV_LIGHT_WVP].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    return rootParameters;
}

void VarianceShadowMapDepthMaterial::CustomizePipelineStateObjectDescription(D3D12_GRAPHICS_PIPELINE_STATE_DESC &psoDesc)
{
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32_FLOAT;
    psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // Bias settings
    psoDesc.RasterizerState.DepthBias = 1;
    psoDesc.RasterizerState.SlopeScaledDepthBias = 1.f;
    psoDesc.RasterizerState.DepthBiasClamp = 10.f;

    // Good sponza settings \/
    // psoDesc.RasterizerState.DepthBias = 2;
    // psoDesc.RasterizerState.SlopeScaledDepthBias = 2.5f;
    // psoDesc.RasterizerState.DepthBiasClamp = 10.f;
}
