#include "stdafx.hpp"

#include "SceneObject.hpp"

SceneObject::SceneObject(Mesh* mesh, std::vector<DirectX::XMFLOAT4> boundingBox) : mesh(mesh), boundingBox(boundingBox) {
	position = DirectX::XMFLOAT4{0.f, 0.f, 0.f, 0.f};
	scale = DirectX::XMFLOAT3{1.f, 1.f, 1.f};
	DirectX::XMStoreFloat4x4(&rotation, DirectX::XMMatrixIdentity());
}

void SceneObject::SetMaterial(StandardMaterial *material)
{
	this->material = material;
}

void SceneObject::SetAlbedoTexture(D3D12_GPU_DESCRIPTOR_HANDLE descriptorHandle)
{
	albedoTextureDescriptorHandle = descriptorHandle;
}

void SceneObject::SetWVPPerFrameBufferLocations(std::vector<MappedResourceLocation> WVPResourceLocations)
{
	this->WVPResourceLocations = WVPResourceLocations;
}

void SceneObject::UpdateWVPMatrices(void* data, size_t dataSize, UINT resourceFramebufferIndex)
{
	assert(dataSize == WVPResourceLocations[resourceFramebufferIndex].GetAllocatedResourceSize());
	memcpy(WVPResourceLocations[resourceFramebufferIndex].GetMappedResourceAddress(), data, dataSize);
}

void SceneObject::UpdateBoundingBox()
{
	DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat4(&this->position));
	DirectX::XMMATRIX rotMat = DirectX::XMLoadFloat4x4(&this->rotation);
	DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
	DirectX::XMMATRIX worldMat = scaleMat * rotMat * translationMat;

	std::vector<DirectX::XMFLOAT4> transformedBoundingBox;
	transformedBoundingBox.resize(8);

	DirectX::XMVector4TransformStream(
		transformedBoundingBox.data(),
		sizeof(DirectX::XMFLOAT4),
		boundingBox.data(),
		sizeof(DirectX::XMFLOAT4),
		boundingBox.size(),
		worldMat
	);

	boundingBox = transformedBoundingBox;
}

StandardMaterial *const SceneObject::GetMaterial() const
{
    return material;
}

D3D12_GPU_DESCRIPTOR_HANDLE SceneObject::GetAlbedoTextureDescriptorHandle() const
{
    return albedoTextureDescriptorHandle;
}

D3D12_GPU_VIRTUAL_ADDRESS SceneObject::GetWVPBufferAddress(UINT frameBufferIndex) const
{
    return WVPResourceLocations[frameBufferIndex].GetGPUResourceVirtualAddress();
}

void SceneObject::Draw(ComPtr<ID3D12GraphicsCommandList> commandList) const
{
	mesh->InsertBufferBind(commandList);
	mesh->InsertDrawIndexed(commandList);
}
