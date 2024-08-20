#include "stdafx.hpp"
#include "Renderer.hpp"

#include <vector>
#include "SceneObject.hpp"

Renderer::~Renderer()
{
}

void Renderer::SetRTV(CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle)
{
    this->rtvHandle = rtvHandle;
}

void Renderer::SetDSV(CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle)
{
    this->dsvHandle = dsvHandle;
}

void Renderer::SetViewport(CD3DX12_VIEWPORT viewport)
{
    this->viewport = viewport;
    this->scissor = CreateFullViewScissor(viewport);
}

CD3DX12_RECT Renderer::CreateFullViewScissor(CD3DX12_VIEWPORT viewport)
{
    return CD3DX12_RECT{ 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height) };
}
