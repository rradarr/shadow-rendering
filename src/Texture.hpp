#pragma once

using Microsoft::WRL::ComPtr;

class Texture
{
public:
    Texture() = default;
    Texture(const std::string fileName);
    bool CreateFromFile(const std::string fileName);
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
