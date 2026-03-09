#ifndef MESH_RENDERER_COMPONENT_H_INCLUDED
#define MESH_RENDERER_COMPONENT_H_INCLUDED

#include "Engine/ECS/Component.h"
#include "Engine/ResourceManager.h"
#include "Render/Helpers/d3dUtil.h" 

class Renderer;

using namespace DirectX;

class MeshRendererComponent : public Component
{
public:
    void SetMesh(MeshHandle h, Renderer* renderer);
    void SetTexture(TextureHandle h) { m_texture = h; }

    MeshHandle GetMeshHandle() const { return m_mesh; }
    TextureHandle GetTextureHandle() const { return m_texture; }

    // constant buffer
    bool CreateConstantBuffer(ID3D12Device* device);
    void UpdateConstants(XMFLOAT4X4& worldMatrixT, XMFLOAT4X4& worldViewProjMatrixT);
    D3D12_GPU_VIRTUAL_ADDRESS GetCbAddress() const { return m_pCb ? m_pCb->GetGPUVirtualAddress() : 0; }

private:
	XMFLOAT4X4 BuildWorldInvTranspose(XMFLOAT4X4& worldMatrixT);

    MeshHandle m_mesh{};
    TextureHandle m_texture{};

    ID3D12Resource* m_pCb = nullptr;
    UINT8* m_pCbMapped = nullptr;
};

#endif