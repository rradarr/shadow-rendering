#include "stdafx.hpp"
#include "ProjectionShadowRenderer.hpp"

#include <cassert>
#include "SceneObject.hpp"

void ProjectionShadowRenderer::Render(ComPtr<ID3D12GraphicsCommandList> commandList, const std::vector<SceneObject> &sceneObjects, UINT currentFrameBufferIndex)
{
#ifdef _DEBUG
    assert(projectionShadowMaterial != nullptr);
#endif

    // Set the render target for the output merger stage (the output of the pipeline).
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissor);
    // const float clearColor[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    // commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    // commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    if(sceneObjects.size() > 0){
        commandList->SetPipelineState(projectionShadowMaterial->GetPSO().Get());
        commandList->SetGraphicsRootSignature(projectionShadowMaterial->GetRootSignature().Get());
        for(const SceneObject& sceneObject : sceneObjects) {
            // set the root constant at index 0 for mvp matix
            commandList->SetGraphicsRootConstantBufferView(
                ProjectionShadowMaterial::CBV_WVP,
                sceneObject.GetWVPBufferAddress(currentFrameBufferIndex));
            commandList->SetGraphicsRootConstantBufferView(
                ProjectionShadowMaterial::CBV_LIGHT_PARAMS,
                lightingParametersBuffer.GetGPUResourceVirtualAddress());

            sceneObject.Draw(commandList);
        }
    }
}

void ProjectionShadowRenderer::SetLightingParametersBuffer(MappedResourceLocation lightingParameters)
{
    lightingParametersBuffer = lightingParameters;
}