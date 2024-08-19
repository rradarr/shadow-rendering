#include "stdafx.hpp"

#include "SceneObject.hpp"


SceneObject::SceneObject(int index, Mesh* mesh) : mesh(mesh) {
	this->idx = index;
	this->delta_rotXMat = DirectX::XMMatrixRotationX(0.f);
	this->delta_rotYMat = DirectX::XMMatrixRotationX(0.f);
	this->delta_rotZMat = DirectX::XMMatrixRotationX(0.f);
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
	// TODO(rrzeczko): Bind per-object data here?
	// But how? All we will have is a pointer to base class Material...
	mesh->InsertBufferBind(commandList);
	mesh->InsertDrawIndexed(commandList);
}
