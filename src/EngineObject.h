#pragma once

#include "Mesh.h"
#include "StandardMaterial.h"
#include "ResourceManager.h"

class EngineObject
{
public:
	EngineObject(int index, Mesh* mesh);
	EngineObject() = default;

	void SetMaterial(StandardMaterial* material);
	void SetAlbedoTexture(D3D12_GPU_DESCRIPTOR_HANDLE descriptorHandle);
	/* This method expects to receive a memory address mapped to the buffer resource.
	The first resource if the offset into this memory, where the contigous range of 3
	allocations each of size offsetIncrementSize begins. */
	void SetWVPPerFrameBufferLocations(std::vector<MappedResourceLocation> WVPResourceLocations);

	void UpdateWVPMatrices(void* data, size_t dataSize, UINT resourceFramebufferIndex);

	StandardMaterial* const GetMaterial() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetAlbedoTextureDescriptorHandle() const;
	/* Get the GPU address of the buffer memory that should be bound as the WVP of this object.
	The passed frameBufferIndex determines which in-flight copy of the data will be returned. */
	D3D12_GPU_VIRTUAL_ADDRESS GetWVPBufferAddress(UINT frameBufferIndex) const;
	void Draw(ComPtr<ID3D12GraphicsCommandList> commandList) const;
	
	int idx;

	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4X4 rotation;
	DirectX::XMFLOAT4X4 worldMat;
	DirectX::XMMATRIX delta_rotXMat;
	DirectX::XMMATRIX delta_rotYMat;
	DirectX::XMMATRIX delta_rotZMat;
private:
	Mesh* mesh;
	StandardMaterial* material;
	std::vector<MappedResourceLocation> WVPResourceLocations;

	D3D12_GPU_DESCRIPTOR_HANDLE albedoTextureDescriptorHandle;
};
