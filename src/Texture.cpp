#include "stdafx.hpp"
#include "Texture.hpp"

#include "DXContext.hpp"
#include "EngineHelpers.hpp"
#include "BufferMemoryManager.hpp"
#include "TextureLoader.hpp"
#include "CbvSrvDescriptorHeapManager.hpp"

Texture::Texture(const std::string fileName)
{
    CreateFromFile(fileName);
}

Texture::Texture(CD3DX12_RESOURCE_DESC textureDescriptor, D3D12_SHADER_RESOURCE_VIEW_DESC textureViewDescriptor, D3D12_SUBRESOURCE_DATA textureData)
{
    CreateFromData(textureDescriptor, textureViewDescriptor, textureData);
}

bool Texture::CreateFromFile(const std::string fileName)
{
    std::wstring tmpName(fileName.begin(), fileName.end());
    LPCWSTR wideFileName = tmpName.c_str();

    BufferMemoryManager buffMng;
    TextureLoader textureLoader;
    int imageBytesPerRow;
    BYTE* imageData;
    
    int imageSize = textureLoader.LoadTextureFromFile(&imageData, textureDesc, EngineHelpers::GetAssetFullPath(fileName), imageBytesPerRow);

    CD3DX12_RESOURCE_DESC desc(textureDesc);
    UINT64 textureUploadBufferSize = 0;
    DXContext::getDevice().Get()->GetCopyableFootprints(&textureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &textureUploadBufferSize);
    ComPtr<ID3D12Resource> textureUploadBuffer;
    buffMng.AllocateBuffer(textureBuffer, &desc, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT);
    buffMng.AllocateBuffer(textureUploadBuffer, static_cast<UINT>(textureUploadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
    D3D12_SUBRESOURCE_DATA textureData = {};
    textureData.pData = &imageData[0];
    textureData.RowPitch = imageBytesPerRow;
    textureData.SlicePitch = imageBytesPerRow * textureDesc.Height;
    buffMng.FillBuffer(textureBuffer, textureData, textureUploadBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    textureBuffer->SetName(L"Texture buffer resource (from image)");

    // Create the SRV Descriptor Heap
    /*D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 1;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_SRVHeap)));
    m_SRVHeap->SetName(L"SRV Heap");*/

    CreateTextureView();

    return true;
}

bool Texture::CreateFromData(CD3DX12_RESOURCE_DESC textureDescriptor, D3D12_SHADER_RESOURCE_VIEW_DESC textureViewDescriptor, D3D12_SUBRESOURCE_DATA textureData)
{
    BufferMemoryManager buffMng;

    textureDesc = textureDescriptor;

    UINT64 textureUploadBufferSize = 0;
    DXContext::getDevice().Get()->GetCopyableFootprints(&textureDescriptor, 0, 1, 0, nullptr, nullptr, nullptr, &textureUploadBufferSize);
    ComPtr<ID3D12Resource> textureUploadBuffer;
    buffMng.AllocateBuffer(textureBuffer, &textureDescriptor, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT);
    buffMng.AllocateBuffer(textureUploadBuffer, static_cast<UINT>(textureUploadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);

    buffMng.FillBuffer(textureBuffer, textureData, textureUploadBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    textureBuffer->SetName(L"Texture buffer resource (from data)");

    viewOffsetInHeap = CbvSrvDescriptorHeapManager::AddShaderResourceView(textureViewDescriptor, textureBuffer);

    return true;
}

void Texture::CreateEmpty(D3D12_RESOURCE_DESC resourceDescriptor, D3D12_SHADER_RESOURCE_VIEW_DESC textureViewDescriptor, D3D12_RESOURCE_STATES initialState, D3D12_CLEAR_VALUE* clearValue)
{
    BufferMemoryManager buffMng;

    textureDesc = resourceDescriptor;

    CD3DX12_RESOURCE_DESC desc(textureDesc);
    buffMng.AllocateBuffer(textureBuffer, &desc, initialState, D3D12_HEAP_TYPE_DEFAULT, clearValue);
    textureBuffer->SetName(L"Texture buffer resource (created empty)");

    // Create the SRV for the texture buffer, using provided ViewDescriptor.
    viewOffsetInHeap = CbvSrvDescriptorHeapManager::AddShaderResourceView(textureViewDescriptor, textureBuffer);
}

void Texture::CreateTextureView()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    viewOffsetInHeap = CbvSrvDescriptorHeapManager::AddShaderResourceView(srvDesc, textureBuffer);
}
