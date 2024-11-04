#include "stdafx.hpp"
#include "VarianceShadowMapRenderer.hpp"

#include <cassert>
// #include "SceneObject.hpp"

#include "Tracy.hpp"

void VarianceShadowMapRenderer::SetShadowPassViewport(CD3DX12_VIEWPORT viewport)
{
    shadowPassViewport = viewport;
    shadowPassScissor = CreateFullViewScissor(shadowPassViewport);
}

void VarianceShadowMapRenderer::Render(ComPtr<ID3D12GraphicsCommandList> commandList, const std::vector<SceneObject> &sceneObjects, UINT currentFrameBufferIndex)
{
#ifdef _DEBUG
    assert(mainMaterial != nullptr);
    assert(depthPassMaterial != nullptr);
#endif

    {
        // Start a D3D12 Tracy zone.
        TracyD3D12NamedZone(*tracyCtx, varianceDepthPassVar, commandList.Get(), "Variance shadow map Pass", 1);

        // Transition the shadow map resource to be usable as a depth resource.
        // Also transition the shadow map depth resource to states compatible
        // with regular shadow mapping method.
        std::vector<CD3DX12_RESOURCE_BARRIER> barriers {
            CD3DX12_RESOURCE_BARRIER::Transition(
                shadowMap.Get(),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                D3D12_RESOURCE_STATE_RENDER_TARGET),
            CD3DX12_RESOURCE_BARRIER::Transition(
                shadowMapDepthStencilResource.Get(),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                D3D12_RESOURCE_STATE_DEPTH_WRITE),
        };
        commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

        // Render to the shadow map.
        commandList->OMSetRenderTargets(1, &shadowMapRTVHandle, FALSE, &shadowMapDSVHandle);

        commandList->RSSetViewports(1, &shadowPassViewport);
        commandList->RSSetScissorRects(1, &shadowPassScissor);
        const float clearColor[] = { 1.f, 1.f, 1.f, 1.0f };
        commandList->ClearRenderTargetView(shadowMapRTVHandle, clearColor, 0, nullptr);
        commandList->ClearDepthStencilView(shadowMapDSVHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        if(sceneObjects.size() > 0){
            commandList->SetPipelineState(depthPassMaterial->GetPSO().Get());
            commandList->SetGraphicsRootSignature(depthPassMaterial->GetRootSignature().Get());

            commandList->SetGraphicsRootConstantBufferView(
                    VarianceShadowMapDepthMaterial::CBV_LIGHT_WVP,
                    lightWVPBuffer.GetGPUResourceVirtualAddress());

            for(const SceneObject& sceneObject : sceneObjects) {
                commandList->SetGraphicsRootConstantBufferView(
                    VarianceShadowMapDepthMaterial::CBV_WVP,
                    sceneObject.GetWVPBufferAddress(currentFrameBufferIndex));

                sceneObject.Draw(commandList);
            }
        }

        // Transition the shadow map resource to be usable in the pixel shader.
        barriers = {
            CD3DX12_RESOURCE_BARRIER::Transition(
                shadowMap.Get(),
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_GENERIC_READ),
            CD3DX12_RESOURCE_BARRIER::Transition(
                shadowMapDepthStencilResource.Get(),
                D3D12_RESOURCE_STATE_DEPTH_WRITE,
                D3D12_RESOURCE_STATE_GENERIC_READ)
        };
        commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
    }

    if(engineState->GetRenderingState().useGaussianBlurOnVSM)
    {
        // Transition the resources to be usable here.
        std::vector<CD3DX12_RESOURCE_BARRIER> barriers {
            CD3DX12_RESOURCE_BARRIER::Transition(
                tmpShadowMap.Get(),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                D3D12_RESOURCE_STATE_RENDER_TARGET)
        };
        commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

        // Perform filtering.
        commandList->OMSetRenderTargets(1, &tmpShadowMapRTVHandle, FALSE, nullptr);

        commandList->RSSetViewports(1, &shadowPassViewport);
        commandList->RSSetScissorRects(1, &shadowPassScissor);
        const float clearColor[] = { 1.f, 1.f, 1.f, 1.0f };
        commandList->ClearRenderTargetView(tmpShadowMapRTVHandle, clearColor, 0, nullptr);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        commandList->SetPipelineState(blurPassMaterial->GetPSO().Get());
        commandList->SetGraphicsRootSignature(blurPassMaterial->GetRootSignature().Get());

        commandList->SetGraphicsRoot32BitConstant(
            GaussianBlurMaterial::CONSTANT_FILTER_DIRECTION,
            0U, 0U); // Blur direction on X.
        commandList->SetGraphicsRootConstantBufferView(
                GaussianBlurMaterial::CBV_FILTER_PARAMS,
                blurringParametersBuffer.GetGPUResourceVirtualAddress());
        commandList->SetGraphicsRootDescriptorTable(
            GaussianBlurMaterial::TABLE_TEXTURE_INPUT,
            shadowMapSRVHandle);

        fullScreenQuad.Draw(commandList);

        // Swap the transition.
        barriers = {
            CD3DX12_RESOURCE_BARRIER::Transition(
                shadowMap.Get(),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                D3D12_RESOURCE_STATE_RENDER_TARGET),
            CD3DX12_RESOURCE_BARRIER::Transition(
                tmpShadowMap.Get(),
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_GENERIC_READ)
        };
        commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

        // Render Y blurring into the shadow map.
        commandList->OMSetRenderTargets(1, &shadowMapRTVHandle, FALSE, nullptr);

        commandList->RSSetViewports(1, &shadowPassViewport);
        commandList->RSSetScissorRects(1, &shadowPassScissor);
        // const float clearColor[] = { 1.f, 1.f, 1.f, 1.0f };
        commandList->ClearRenderTargetView(shadowMapRTVHandle, clearColor, 0, nullptr);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        commandList->SetPipelineState(blurPassMaterial->GetPSO().Get());
        commandList->SetGraphicsRootSignature(blurPassMaterial->GetRootSignature().Get());

        commandList->SetGraphicsRoot32BitConstant(
            GaussianBlurMaterial::CONSTANT_FILTER_DIRECTION,
            1U, 0U); // Blur direction on Y.
        commandList->SetGraphicsRootConstantBufferView(
                GaussianBlurMaterial::CBV_FILTER_PARAMS,
                blurringParametersBuffer.GetGPUResourceVirtualAddress());
        commandList->SetGraphicsRootDescriptorTable(
            GaussianBlurMaterial::TABLE_TEXTURE_INPUT,
            tmpShadowMapSRVHandle);

        fullScreenQuad.Draw(commandList);

        // Transision shadow map back to SRV.
        barriers = {
            CD3DX12_RESOURCE_BARRIER::Transition(
                shadowMap.Get(),
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_GENERIC_READ)
        };
        commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
    }

    {
        // Start a D3D12 Tracy zone.
        TracyD3D12NamedZone(*tracyCtx, varianceMainPassVar, commandList.Get(), "Variance main Pass", 1);

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
                VarianceShadowMapMainMaterial::CBV_LIGHT_PARAMS,
                lightingParametersBuffer.GetGPUResourceVirtualAddress());
            commandList->SetGraphicsRootConstantBufferView(
                VarianceShadowMapMainMaterial::CBV_LIGHT_WVP,
                lightWVPBuffer.GetGPUResourceVirtualAddress());
            commandList->SetGraphicsRootDescriptorTable(
                VarianceShadowMapMainMaterial::TABLE_TEXTURE_SHADOW,
                shadowMapSRVHandle);
            // commandList->SetGraphicsRootDescriptorTable(
            //     VarianceShadowMapMainMaterial::TABLE_TEXTURE_OFFSETS,
            //     pcfOffsetsSRVHandle);

            for(const SceneObject& sceneObject : sceneObjects) {
                commandList->SetGraphicsRootConstantBufferView(
                    VarianceShadowMapMainMaterial::CBV_WVP,
                    sceneObject.GetWVPBufferAddress(currentFrameBufferIndex));
                commandList->SetGraphicsRootDescriptorTable(
                    VarianceShadowMapMainMaterial::TABLE_TEXTURE_ALBEDO,
                    sceneObject.GetAlbedoTextureDescriptorHandle());

                sceneObject.Draw(commandList);
            }
        }
    }
}
