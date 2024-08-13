#include "stdafx.h"

#include "EngineObject.h"


EngineObject::EngineObject(int index, Mesh* mesh) : mesh(mesh) {
	this->idx = index;
	this->delta_rotXMat = DirectX::XMMatrixRotationX(0.f);
	this->delta_rotYMat = DirectX::XMMatrixRotationX(0.f);
	this->delta_rotZMat = DirectX::XMMatrixRotationX(0.f);
}

void EngineObject::SetMaterial(StandardMaterial *material)
{
	this->material = material;
}

void EngineObject::SetAlbedoTexture(D3D12_GPU_DESCRIPTOR_HANDLE descriptorHandle)
{
	albedoTextureDescriptorHandle = descriptorHandle;
}

void EngineObject::SetWVPPerFrameBufferLocations(std::vector<MappedResourceLocation> WVPResourceLocations)
{
	this->WVPResourceLocations = WVPResourceLocations;
}

void EngineObject::UpdateWVPMatrices(void* data, size_t dataSize, UINT resourceFramebufferIndex)
{
	assert(dataSize == WVPResourceLocations[resourceFramebufferIndex].GetAllocatedResourceSize());
	memcpy(WVPResourceLocations[resourceFramebufferIndex].GetMappedResourceAddress(), data, dataSize);
}

StandardMaterial *const EngineObject::GetMaterial() const
{
    return material;
}

D3D12_GPU_DESCRIPTOR_HANDLE EngineObject::GetAlbedoTextureDescriptorHandle() const
{
    return albedoTextureDescriptorHandle;
}

D3D12_GPU_VIRTUAL_ADDRESS EngineObject::GetWVPBufferAddress(UINT frameBufferIndex) const
{
    return WVPResourceLocations[frameBufferIndex].GetGPUResourceVirtualAddress();
}

void EngineObject::Draw(ComPtr<ID3D12GraphicsCommandList> commandList) const
{
	// TODO(rrzeczko): Bind per-object data here?
	// But how? All we will have is a pointer to base class Material...
	mesh->InsertBufferBind(commandList);
	mesh->InsertDrawIndexed(commandList);
}
