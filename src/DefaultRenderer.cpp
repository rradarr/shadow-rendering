#include "stdafx.hpp"
#include "DefaultRenderer.hpp"

#include "SceneObject.hpp"

void DefaultRenderer::Render(ComPtr<ID3D12GraphicsCommandList> commandList, const std::vector<SceneObject> &sceneObjects, UINT currentFrameBufferIndex)
{
        // Set the render target for the output merger stage (the output of the pipeline).
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissor);
    const float clearColor[] = { 0.005f, 0.005f, 0.005f, 1.0f };
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    if(sceneObjects.size() > 0){
        commandList->SetGraphicsRootSignature(sceneObjects[0].GetMaterial()->GetRootSignature().Get());
        for(const SceneObject& sceneObject : sceneObjects) {
            commandList->SetPipelineState(sceneObject.GetMaterial()->GetPSO().Get());

            commandList->SetGraphicsRootDescriptorTable(StandardMaterial::TABLE_TEXTURE_ALBEDO, sceneObject.GetAlbedoTextureDescriptorHandle());
            commandList->SetGraphicsRootConstantBufferView(StandardMaterial::CBV_LIGHT_PARAMS, lightingParametersBuffer.GetGPUResourceVirtualAddress());
        
            // set the root constant at index 0 for mvp matix
            commandList->SetGraphicsRootConstantBufferView(StandardMaterial::CBV_WVP, sceneObject.GetWVPBufferAddress(currentFrameBufferIndex));

            sceneObject.Draw(commandList);
        }
    }
}

void DefaultRenderer::SetRTV(CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle)
{
    this->rtvHandle = rtvHandle;
}

void DefaultRenderer::SetDSV(CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle)
{
    this->dsvHandle = dsvHandle;
}

void DefaultRenderer::SetViewport(CD3DX12_VIEWPORT viewport)
{
    this->viewport = viewport;
    this->scissor = CreateScissor(viewport);
}

void DefaultRenderer::SetLightingParametersBuffer(MappedResourceLocation lightingParameters)
{
    lightingParametersBuffer = lightingParameters;
}

CD3DX12_RECT DefaultRenderer::CreateScissor(CD3DX12_VIEWPORT viewport)
{
    return CD3DX12_RECT{ 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height) };
}
