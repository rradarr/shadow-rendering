#include "stdafx.h"
#include "CbvSrvDescriptorHeapManager.h"

#include "DXContext.h"
#include "DXSampleHelper.h"

ComPtr<ID3D12DescriptorHeap> CbvSrvDescriptorHeapManager::shaderAccessHeap; // Main descriptor heap for all SRV/CBV/UAV resource descriptors
CD3DX12_CPU_DESCRIPTOR_HANDLE CbvSrvDescriptorHeapManager::shaderAccessHeapHandle;
CD3DX12_CPU_DESCRIPTOR_HANDLE CbvSrvDescriptorHeapManager::shaderAccessHeapHeadHandle;
UINT CbvSrvDescriptorHeapManager::heapDescriptorCapacity = 0;
UINT CbvSrvDescriptorHeapManager::currentHeadOffset = 0;
UINT CbvSrvDescriptorHeapManager::shaderAccessDescriptorSize = 0;

UINT CbvSrvDescriptorHeapManager::AddConstantBufferView(D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc)
{
    DXContext::getDevice().Get()->CreateConstantBufferView(&cbvDesc, shaderAccessHeapHeadHandle);
    shaderAccessHeapHeadHandle.Offset(1, shaderAccessDescriptorSize);

    return currentHeadOffset++;
}

UINT CbvSrvDescriptorHeapManager::AddShaderResourceView(D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc, ComPtr<ID3D12Resource> resourceBuffer)
{
    DXContext::getDevice().Get()->CreateShaderResourceView(resourceBuffer.Get(), &srvDesc, shaderAccessHeapHeadHandle);
    shaderAccessHeapHeadHandle.Offset(1, shaderAccessDescriptorSize);

    return currentHeadOffset++;
}

UINT CbvSrvDescriptorHeapManager::ReserveDescriptorInHeap()
{
    shaderAccessHeapHeadHandle.Offset(1, shaderAccessDescriptorSize);

    return currentHeadOffset++;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE CbvSrvDescriptorHeapManager::GetCpuDescriptorHandle(UINT descriptorIndex)
{
    assert(descriptorIndex <= currentHeadOffset);
    CD3DX12_CPU_DESCRIPTOR_HANDLE offsetHandle = shaderAccessHeapHandle;
    offsetHandle.Offset(descriptorIndex, shaderAccessDescriptorSize);
    return offsetHandle;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE CbvSrvDescriptorHeapManager::GetGpuDescriptorHandle(UINT descriptorIndex)
{
    assert(descriptorIndex <= currentHeadOffset);
    CD3DX12_GPU_DESCRIPTOR_HANDLE offsetHandle{shaderAccessHeap->GetGPUDescriptorHandleForHeapStart()};
    offsetHandle.Offset(descriptorIndex, shaderAccessDescriptorSize);
    return offsetHandle;
}

void CbvSrvDescriptorHeapManager::CreateHeap(UINT numberOfDeaspriptorsInHeap)
{
    heapDescriptorCapacity = numberOfDeaspriptorsInHeap;

    // Create our main SRV/CBV/UAV descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = heapDescriptorCapacity;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    ThrowIfFailed(DXContext::getDevice().Get()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&shaderAccessHeap)));
    shaderAccessHeap->SetName(L"Main Shader Access Descriptor Heap");

    shaderAccessDescriptorSize = DXContext::getDevice().Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    shaderAccessHeapHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(shaderAccessHeap->GetCPUDescriptorHandleForHeapStart());
    shaderAccessHeapHeadHandle = shaderAccessHeapHandle;
    currentHeadOffset = 0;
}
