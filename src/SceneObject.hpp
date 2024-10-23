#pragma once

#include "Mesh.hpp"
#include "StandardMaterial.hpp"
#include "ResourceManager.hpp"

class SceneObject
{
public:
	SceneObject(Mesh* mesh, std::vector<DirectX::XMFLOAT4> boundingBox);
	SceneObject() = default;

	void SetMaterial(StandardMaterial* material);
	void SetAlbedoTexture(D3D12_GPU_DESCRIPTOR_HANDLE descriptorHandle);

	void SetWVPPerFrameBufferLocations(std::vector<MappedResourceLocation> WVPResourceLocations);

	void UpdateWVPMatrices(void* data, size_t dataSize, UINT resourceFramebufferIndex);
	void UpdateBoundingBox();

	StandardMaterial* const GetMaterial() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetAlbedoTextureDescriptorHandle() const;
	/* Get the GPU address of the buffer memory that should be bound as the WVP of this object.
	The passed frameBufferIndex determines which in-flight copy of the data will be returned. */
	D3D12_GPU_VIRTUAL_ADDRESS GetWVPBufferAddress(UINT frameBufferIndex) const;
	void Draw(ComPtr<ID3D12GraphicsCommandList> commandList) const;

	std::vector<DirectX::XMFLOAT4> GetBoundingBox() { return boundingBox; }

	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4X4 rotation;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4X4 worldMat;

private:
	Mesh* mesh;
	std::vector<DirectX::XMFLOAT4> boundingBox;
	StandardMaterial* material;
	std::vector<MappedResourceLocation> WVPResourceLocations;

	D3D12_GPU_DESCRIPTOR_HANDLE albedoTextureDescriptorHandle;
};
