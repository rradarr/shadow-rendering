#include "stdafx.hpp"
#include "ShadowMapRenderer.hpp"

#include <cassert>
#include "SceneObject.hpp"

#include "Tracy.hpp"

void ShadowMapRenderer::SetShadowPassViewport(CD3DX12_VIEWPORT viewport)
{
    shadowPassViewport = viewport;
    shadowPassScissor = CreateFullViewScissor(shadowPassViewport);
}

void ShadowMapRenderer::Render(ComPtr<ID3D12GraphicsCommandList> commandList, const std::vector<SceneObject> &sceneObjects, UINT currentFrameBufferIndex)
{
#ifdef _DEBUG
    assert(mainMaterial != nullptr);
    assert(depthPassMaterial != nullptr);
#endif

    {
        // Start a D3D12 Tracy zone.
        TracyD3D12NamedZone(*tracyCtx, anotherVar, commandList.Get(), "Shadow map Pass", 1);

        // Transition the shadow map resource to be usable as a depth resource.
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            shadowMap.Get(),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            D3D12_RESOURCE_STATE_DEPTH_WRITE);
        commandList->ResourceBarrier(1, &barrier);

        // Render to the shadow map.
        commandList->OMSetRenderTargets(0, nullptr, FALSE, &shadowMapDSVHandle);

        commandList->RSSetViewports(1, &shadowPassViewport);
        commandList->RSSetScissorRects(1, &shadowPassScissor);
        commandList->ClearDepthStencilView(shadowMapDSVHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        if(sceneObjects.size() > 0){
            commandList->SetPipelineState(depthPassMaterial->GetPSO().Get());
            commandList->SetGraphicsRootSignature(depthPassMaterial->GetRootSignature().Get());

            commandList->SetGraphicsRootConstantBufferView(
                    ShadowMapDepthMaterial::CBV_LIGHT_WVP,
                    lightWVPBuffer.GetGPUResourceVirtualAddress());

            for(const SceneObject& sceneObject : sceneObjects) {
                commandList->SetGraphicsRootConstantBufferView(
                    ShadowMapDepthMaterial::CBV_WVP,
                    sceneObject.GetWVPBufferAddress(currentFrameBufferIndex));

                sceneObject.Draw(commandList);
            }
        }

        // Transition the shadow map resource to be usable in the pixel shader.
        barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            shadowMap.Get(),
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            D3D12_RESOURCE_STATE_GENERIC_READ);
        commandList->ResourceBarrier(1, &barrier);
    }

    {
        // Start a D3D12 Tracy zone.
        TracyD3D12NamedZone(*tracyCtx, otherVar, commandList.Get(), "Main Pass", 1);

        // Render the actual view, using the shadow map as a texture.
        commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

        commandList->RSSetViewports(1, &viewport);
        commandList->RSSetScissorRects(1, &scissor);
        const float clearColor[] = { 0.01f, 0.01f, 0.015f, 1.0f };
        commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        if(sceneObjects.size() > 0){
            commandList->SetPipelineState(mainMaterial->GetPSO().Get());
            commandList->SetGraphicsRootSignature(mainMaterial->GetRootSignature().Get());

            commandList->SetGraphicsRootConstantBufferView(
                ShadowMapMainMaterial::CBV_LIGHT_PARAMS,
                lightingParametersBuffer.GetGPUResourceVirtualAddress());
            commandList->SetGraphicsRootConstantBufferView(
                ShadowMapMainMaterial::CBV_LIGHT_WVP,
                lightWVPBuffer.GetGPUResourceVirtualAddress());
            commandList->SetGraphicsRootDescriptorTable(
                ShadowMapMainMaterial::TABLE_TEXTURE_SHADOW,
                shadowMapSRVHandle);
            commandList->SetGraphicsRootDescriptorTable(
                ShadowMapMainMaterial::TABLE_TEXTURE_OFFSETS,
                pcfOffsetsSRVHandle);

            for(const SceneObject& sceneObject : sceneObjects) {
                commandList->SetGraphicsRootConstantBufferView(
                    ShadowMapMainMaterial::CBV_WVP,
                    sceneObject.GetWVPBufferAddress(currentFrameBufferIndex));
                commandList->SetGraphicsRootDescriptorTable(
                    ShadowMapMainMaterial::TABLE_TEXTURE_ALBEDO,
                    sceneObject.GetAlbedoTextureDescriptorHandle());

                sceneObject.Draw(commandList);
            }
        }
    }
}
