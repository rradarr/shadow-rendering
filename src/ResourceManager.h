#pragma once

using Microsoft::WRL::ComPtr;

class MappedResourceLocation {
public:
    MappedResourceLocation() = default;
    MappedResourceLocation(UINT8* mappedResourceAddress, size_t allocatedResourceSize, D3D12_GPU_VIRTUAL_ADDRESS gpuResourceVirtualAddress) :
        mappedResourceAddress(mappedResourceAddress),
        allocatedResourceSize(allocatedResourceSize),
        gpuResourceVirtualAddress(gpuResourceVirtualAddress) {};

    UINT8* GetMappedResourceAddress() const;
    size_t GetAllocatedResourceSize() const;
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUResourceVirtualAddress() const;

private:
    UINT8* mappedResourceAddress;
    size_t allocatedResourceSize;
    D3D12_GPU_VIRTUAL_ADDRESS gpuResourceVirtualAddress;
};

class ResourceManager {
public:
    ResourceManager() = default;
    ~ResourceManager();

    MappedResourceLocation AddMappedUploadResource(void* data, size_t dataSize);

private:
    void CreateMappedUploadBuffer();

    struct ManagedBuffer {
        ComPtr<ID3D12Resource> bufferResource;
        size_t availableSpace = 64 * 1024;
        size_t headOffset = 0;
    };

    std::vector<ManagedBuffer> residentBuffers;
    std::vector<ManagedBuffer> mappedBuffers;
    std::vector<UINT8 *> mappedMemory;
};