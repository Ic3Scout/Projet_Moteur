#pragma once
#include "DirectXMath.h"
#include <d3d12.h>
#include <vector>

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 UV;
};

struct ObjectConstants
{
	XMFLOAT4X4 World;
	XMFLOAT4X4 WorldInvTranspose;
	XMFLOAT4X4 WorldViewProj;
};

class UploadContext;

class Mesh
{
public:
	Mesh() = default;
	~Mesh() { Release(); }

	bool Initialize(UploadContext& uploader, std::vector<Vertex> vertices, std::vector<uint32_t> indices);

	bool CreateTriangle(UploadContext& uploader);
	bool CreateQuad(UploadContext& uploader);
	bool CreateCube(UploadContext& uploader);
	bool CreateCylinder(UploadContext& uploader);
	bool CreateSphere(UploadContext& uploader);

	void FinalizeUpload();
	void Draw(ID3D12GraphicsCommandList* cmdList) const;

	uint32_t IndexCount() const { return m_indexCount; }

	const std::vector<Vertex>& Vertices() const { return m_vertices; }
	const std::vector<uint32_t>& Indices() const { return m_indices; }

	void Release();

private:
	void CalculateNormals();

	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_indices;

	ID3D12Resource* m_pVb = nullptr;
	ID3D12Resource* m_pVbUpload = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_vbView{};

	ID3D12Resource* m_pIb = nullptr;
	ID3D12Resource* m_pIbUpload = nullptr;
	D3D12_INDEX_BUFFER_VIEW m_ibView{};

	uint32_t m_indexCount = 0;
};