#include "stdafx.hpp"
#include "ProjectionShadowMaterial.hpp"

std::vector<D3D12_ROOT_PARAMETER> ProjectionShadowMaterial::CreateRootParameters()
{
    // Create the root descriptor (for wvp matrices)
    D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
    rootCBVDescriptor.ShaderRegister = 0; // b0 in shader
    rootCBVDescriptor.RegisterSpace = 0;

    // Root descriptor for light parameters
    D3D12_ROOT_DESCRIPTOR rootCBVLightingDescriptor;
    rootCBVLightingDescriptor.ShaderRegister = 1;
    rootCBVLightingDescriptor.RegisterSpace = 0;

    // create a root parameter and fill it out
    rootParameters.resize(PARAM_COUNT);
    rootParameters[CBV_WVP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[CBV_WVP].Descriptor = rootCBVDescriptor;
    rootParameters[CBV_WVP].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    rootParameters[CBV_LIGHT_PARAMS].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[CBV_LIGHT_PARAMS].Descriptor = rootCBVLightingDescriptor;
    rootParameters[CBV_LIGHT_PARAMS].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    return rootParameters;
}

void ProjectionShadowMaterial::CustomizePipelineStateObjectDescription(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc)
{
    psoDesc.RasterizerState.SlopeScaledDepthBias = -0.01f;
    // NOTE: The SlopeScaledDepthBias works much better, even at extreme angles.
    // The regular depth bias has to be quite large (~5) and breaks when looking
    // edge-on.
    // psoDesc.RasterizerState.DepthBias = -5;
    psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
}
