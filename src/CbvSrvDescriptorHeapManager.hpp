#pragma once

using Microsoft::WRL::ComPtr;

class CbvSrvDescriptorHeapManager
{
public:
    static UINT AddConstantBufferView(D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc);
    static UINT AddShaderResourceView(D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc, ComPtr<ID3D12Resource> resourceBuffer);
    /* This method is meant for using the heap for externally managed descriptors, eg. ImGui font textures.
    All it does is advance the internal heap head. */
    static UINT ReserveDescriptorInHeap();

    static CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorHandle(UINT descriptorIndex);
    static CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(UINT descriptorIndex);

    static void CreateHeap(UINT numberOfDeaspriptorsInHeap);
    static ComPtr<ID3D12DescriptorHeap> GetHeap() { return shaderAccessHeap; }
    static CD3DX12_CPU_DESCRIPTOR_HANDLE GetHeapBase() { return shaderAccessHeapHandle; }
    static CD3DX12_CPU_DESCRIPTOR_HANDLE GetHeapHead() { return shaderAccessHeapHeadHandle; }
    static UINT GetDescriptorSize() { return shaderAccessDescriptorSize; }
    static UINT GetCurrentHeadOffset() { return currentHeadOffset; }

private:
    static ComPtr<ID3D12DescriptorHeap> shaderAccessHeap; // Main descriptor heap for all SRV/CBV/UAV resource descriptors
    static CD3DX12_CPU_DESCRIPTOR_HANDLE shaderAccessHeapHandle;
    static CD3DX12_CPU_DESCRIPTOR_HANDLE shaderAccessHeapHeadHandle;
    static UINT heapDescriptorCapacity;
    static UINT currentHeadOffset;
    static UINT shaderAccessDescriptorSize;
};
