#include "stdafx.hpp"
#include "ResourceManager.hpp"

#include <sstream>

#include "DXSampleHelper.h"
#include "BufferMemoryManager.hpp"

UINT8 *MappedResourceLocation::GetMappedResourceAddress() const
{
    return mappedResourceAddress;
}

size_t MappedResourceLocation::GetAllocatedResourceSize() const
{
    return allocatedResourceSize;
}

D3D12_GPU_VIRTUAL_ADDRESS MappedResourceLocation::GetGPUResourceVirtualAddress() const
{
    return gpuResourceVirtualAddress;
}

ResourceManager::~ResourceManager()
{
    // TODO: unmap the memory?
}

MappedResourceLocation ResourceManager::AddMappedUploadResource(void *data, size_t dataSize)
{
    int bufferWithSpace = -1;

    // Find buffer with space for the resource. If not availible, create new one and use it.
    for(int i = 0; i < mappedBuffers.size(); i++) {
        if(mappedBuffers[i].availableSpace >= dataSize) {
            bufferWithSpace = i;
        }
    }
    if(bufferWithSpace < 0) {
        CreateMappedUploadBuffer();
        bufferWithSpace = static_cast<int>(mappedBuffers.size()) - 1;
    }

    // Ready the MappedResourceLocation struct for return.
    MappedResourceLocation newResource {
        mappedMemory[bufferWithSpace] + mappedBuffers[bufferWithSpace].headOffset,
        dataSize,
        mappedBuffers[bufferWithSpace].bufferResource->GetGPUVirtualAddress() + mappedBuffers[bufferWithSpace].headOffset
    };
    // Copy the resource data into mapped memory.
    memcpy(newResource.GetMappedResourceAddress(), data, newResource.GetAllocatedResourceSize());

    // Update the ManagedBuffer that was just used.
    size_t usedAlignedSpace =  (dataSize + 255) & ~255;
    mappedBuffers[bufferWithSpace].availableSpace -= usedAlignedSpace;
    mappedBuffers[bufferWithSpace].headOffset += usedAlignedSpace;

    return newResource;
}

void ResourceManager::CreateMappedUploadBuffer()
{
    BufferMemoryManager buffMng;

    ManagedBuffer newBuffer{};

    // Size of the resource heap must be a multiple of 64KB for single-textures and constant buffers
    // Will be data that is read from so we keep it in the generic read state.
    buffMng.AllocateBuffer(newBuffer.bufferResource, newBuffer.availableSpace, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
    std::wstringstream bufferNameStream;
    bufferNameStream << "Mapped Upload Buffer: Size=" << newBuffer.availableSpace << " ID=" << mappedBuffers.size();
    newBuffer.bufferResource->SetName(bufferNameStream.str().c_str());
    mappedBuffers.push_back(newBuffer);

    // Map the memory of the new buffer.
    mappedMemory.push_back(nullptr);
    CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU. (so end is less than or equal to begin).
    ThrowIfFailed(newBuffer.bufferResource->Map(0, &readRange, reinterpret_cast<void**>(&mappedMemory.back())));
}
