#pragma once

using Microsoft::WRL::ComPtr;

class Texture
{
public:
    Texture() = default;
    Texture(const std::string fileName);
    Texture(CD3DX12_RESOURCE_DESC textureDescriptor, D3D12_SHADER_RESOURCE_VIEW_DESC textureViewDescriptor, D3D12_SUBRESOURCE_DATA textureData);
    bool CreateFromFile(const std::string fileName);
    bool CreateFromData(CD3DX12_RESOURCE_DESC textureDescriptor, D3D12_SHADER_RESOURCE_VIEW_DESC textureViewDescriptor, D3D12_SUBRESOURCE_DATA textureData);
    /* Create an empty texture resource in the default heap. */
    void CreateEmpty(
        D3D12_RESOURCE_DESC resourceDescriptor,
        D3D12_SHADER_RESOURCE_VIEW_DESC textureViewDescriptor,
        D3D12_RESOURCE_STATES initialState,
        D3D12_CLEAR_VALUE* clearValue = nullptr);
    UINT GetOffsetInHeap() { return viewOffsetInHeap; }
    ComPtr<ID3D12Resource> GetTextureResource() { return textureBuffer; }

    // Creates a view/descriptor of the texture in the heap provided.
    void CreateTextureView();
private:
    ComPtr<ID3D12Resource> textureBuffer;
    D3D12_RESOURCE_DESC textureDesc;
    UINT viewOffsetInHeap;
};
