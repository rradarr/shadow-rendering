#include "stdafx.hpp"
#include "WireframeMaterial.hpp"

std::vector<D3D12_ROOT_PARAMETER> WireframeMaterial::CreateRootParameters()
{
    // Create the root descriptor (for wvp matrices)
    D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
    rootCBVDescriptor.ShaderRegister = 0; // b0 in shader
    rootCBVDescriptor.RegisterSpace = 0;

    // create a root parameter and fill it out
    rootParameters.resize(PARAM_COUNT);
    rootParameters[CBV_WVP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[CBV_WVP].Descriptor = rootCBVDescriptor;
    rootParameters[CBV_WVP].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    return rootParameters;
}

void WireframeMaterial::CustomizePipelineStateObjectDescription(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc)
{
    psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
}
