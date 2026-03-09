#include "Mesh.h"
#include "UploadContext.h"

bool Mesh::Initialize(UploadContext& uploader, std::vector<Vertex> vertices, std::vector<uint32_t> indices)
{
	if (vertices.empty() || indices.empty())
		return false;

	Release();

	// Stockage CPU
	m_vertices = std::move(vertices);
	m_indices = std::move(indices);

	CalculateNormals();

	m_indexCount = (uint32_t)m_indices.size();

	const uint64_t vbBytes = uint64_t(m_vertices.size()) * sizeof(Vertex);
	const uint64_t ibBytes = uint64_t(m_indices.size()) * sizeof(uint32_t);

	// Upload GPU
	if (uploader.UploadBuffer(m_vertices.data(), vbBytes, m_pVb, m_pVbUpload,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER) == false)
		return false;

	m_vbView.BufferLocation = m_pVb->GetGPUVirtualAddress();
	m_vbView.SizeInBytes = (UINT)vbBytes;
	m_vbView.StrideInBytes = sizeof(Vertex);

	if (uploader.UploadBuffer(m_indices.data(), ibBytes, m_pIb, m_pIbUpload,
		D3D12_RESOURCE_STATE_INDEX_BUFFER) == false)
		return false;

	m_ibView.BufferLocation = m_pIb->GetGPUVirtualAddress();
	m_ibView.SizeInBytes = (UINT)ibBytes;
	m_ibView.Format = DXGI_FORMAT_R32_UINT;

	return true;
}

bool Mesh::CreateTriangle(UploadContext& uploader)
{
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex{ { 0.0f,  0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f } });
	vertices.push_back(Vertex{ { 0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } });
	vertices.push_back(Vertex{ { -0.5f,-0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } });

	std::vector<uint32_t> indices = { 0,1,2 };

	return Initialize(uploader, std::move(vertices), std::move(indices));
}

bool Mesh::CreateQuad(UploadContext& uploader)
{
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex{ { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } });
	vertices.push_back(Vertex{ {  0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } });
	vertices.push_back(Vertex{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } });
	vertices.push_back(Vertex{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } });

	std::vector<uint32_t> indices =
	{
		0, 1, 2,
		0, 2, 3
	};

	return Initialize(uploader, std::move(vertices), std::move(indices));
}

bool Mesh::CreateCube(UploadContext& uploader)
{
	const float s = 0.5f;

	std::vector<Vertex> vertices;
	vertices.reserve(24);

	std::vector<uint32_t> indices;
	indices.reserve(36);

	auto addFace = [&](XMFLOAT3 a, XMFLOAT3 b, XMFLOAT3 c, XMFLOAT3 d)
		{
			// 4 vertices par face
			uint32_t base = (uint32_t)vertices.size();

			vertices.push_back(Vertex{ a, { 0.0f, 0.0f, 0.0f }, {0.0f, 1.0f} }); // bas-gauche
			vertices.push_back(Vertex{ b, { 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f} }); // haut-gauche
			vertices.push_back(Vertex{ c, { 0.0f, 0.0f, 0.0f }, {1.0f, 0.0f} }); // haut-droit
			vertices.push_back(Vertex{ d, { 0.0f, 0.0f, 0.0f }, {1.0f, 1.0f} }); // bas-droit

			// 2 triangles
			indices.push_back(base + 0); indices.push_back(base + 1); indices.push_back(base + 2);
			indices.push_back(base + 0); indices.push_back(base + 2); indices.push_back(base + 3);
		};

	// BACK  (-Z)
	addFace({ -s, -s, -s }, { -s, +s, -s }, { +s, +s, -s }, { +s, -s, -s });

	// RIGHT (+X)
	addFace({ +s, -s, -s }, { +s, +s, -s }, { +s, +s, +s }, { +s, -s, +s });

	// FRONT (+Z)
	addFace({ +s, -s, +s }, { +s, +s, +s }, { -s, +s, +s }, { -s, -s, +s });

	// LEFT  (-X)
	addFace({ -s, -s, +s }, { -s, +s, +s }, { -s, +s, -s }, { -s, -s, -s });

	// TOP   (+Y)
	addFace({ -s, +s, -s }, { -s, +s, +s }, { +s, +s, +s }, { +s, +s, -s });

	// BOTTOM(-Y)
	addFace({ -s, -s, +s }, { -s, -s, -s }, { +s, -s, -s }, { +s, -s, +s });

	return Initialize(uploader, std::move(vertices), std::move(indices));
}

bool Mesh::CreateCylinder(UploadContext& uploader)
{
	const uint32_t slices = 16;
	const float radius = 1.0f;
	const float y0 = 0.0f;
	const float y1 = 1.0f;

	std::vector<Vertex> vertices;
	vertices.reserve((slices + 1) * 2 + 2);

	// Anneaux (duplication du dernier slice pour la couture UV)
	for (uint32_t s = 0; s <= slices; ++s)
	{
		float u = (float)s / (float)slices; // 0..1
		float ang = u * XM_2PI;
		float x = cosf(ang) * radius;
		float z = sinf(ang) * radius;

		vertices.push_back(Vertex{ { x, y0, z }, { 0.0f, 0.0f, 0.0f }, { u, 1.0f } }); // bas
		vertices.push_back(Vertex{ { x, y1, z }, { 0.0f, 0.0f, 0.0f }, { u, 0.0f } }); // haut
	}

	uint32_t centerBottom = (uint32_t)vertices.size();
	vertices.push_back(Vertex{ { 0.0f, y0, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f } });

	uint32_t centerTop = (uint32_t)vertices.size();
	vertices.push_back(Vertex{ { 0.0f, y1, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f } });

	std::vector<uint32_t> indices;
	indices.reserve(slices * 12);

	for (uint32_t s = 0; s < slices; ++s)
	{
		uint32_t b0 = s * 2;
		uint32_t t0 = s * 2 + 1;
		uint32_t b1 = (s + 1) * 2;
		uint32_t t1 = (s + 1) * 2 + 1;

		// Flancs (WINDING CW)
		indices.push_back(b0); indices.push_back(t1); indices.push_back(b1);
		indices.push_back(b0); indices.push_back(t0); indices.push_back(t1);

		// Disque du bas (face visible depuis l'extérieur => vers -Y)
		indices.push_back(centerBottom); indices.push_back(b0); indices.push_back(b1);

		// Disque du haut (face visible depuis l'extérieur => vers +Y)
		indices.push_back(centerTop); indices.push_back(t1); indices.push_back(t0);
	}

	return Initialize(uploader, std::move(vertices), std::move(indices));
}

bool Mesh::CreateSphere(UploadContext& uploader)
{
	uint32_t slices = 10;
	uint32_t stacks = 9;

	uint32_t vertCount = (stacks + 1) * (slices + 1);

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	vertices.reserve(vertCount);
	indices.reserve(stacks * slices * 6);

	for (uint32_t stack = 0; stack <= stacks; ++stack)
	{
		float v = (float)stack / (float)stacks;      // 0..1
		float phi = v * XM_PI;                       // 0..PI

		float y = cosf(phi);
		float r = sinf(phi);

		for (uint32_t slice = 0; slice <= slices; ++slice)
		{
			float u = (float)slice / (float)slices;  // 0..1
			float theta = u * XM_2PI;                // 0..2PI

			float x = r * cosf(theta);
			float z = r * sinf(theta);

			XMFLOAT3 pos = { x , y , z };

			vertices.push_back(Vertex{ pos, { 0.0f, 0.0f, 0.0f }, { u, 1.0f - v } });
		}
	}

	// Indices (winding inversé)
	const uint32_t ring = slices + 1;
	for (uint32_t stack = 0; stack < stacks; ++stack)
	{
		for (uint32_t slice = 0; slice < slices; ++slice)
		{
			uint32_t i0 = stack * ring + slice;
			uint32_t i1 = (stack + 1) * ring + slice;
			uint32_t i2 = (stack + 1) * ring + (slice + 1);
			uint32_t i3 = stack * ring + (slice + 1);

			// Inverse chaque triangle : (a,b,c) -> (a,c,b)
			indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);
			indices.push_back(i0); indices.push_back(i3); indices.push_back(i2);
		}
	}

	return Initialize(uploader, std::move(vertices), std::move(indices));
}

void Mesh::Release()
{
	SafeRelease(m_pVb);
	SafeRelease(m_pVbUpload);
	SafeRelease(m_pIb);
	SafeRelease(m_pIbUpload);

	m_indexCount = 0;
	m_vertices.clear();
	m_indices.clear();
}

void Mesh::CalculateNormals()
{
	for (size_t i = 0; i < m_indices.size(); i += 3)
	{
		uint32_t i0 = m_indices[i];
		uint32_t i1 = m_indices[i + 1];
		uint32_t i2 = m_indices[i + 2];

		XMVECTOR p0 = XMLoadFloat3(&m_vertices[i0].Pos);
		XMVECTOR p1 = XMLoadFloat3(&m_vertices[i1].Pos);
		XMVECTOR p2 = XMLoadFloat3(&m_vertices[i2].Pos);

		XMVECTOR e1 = XMVectorSubtract(p1, p0);
		XMVECTOR e2 = XMVectorSubtract(p2, p0);

		// Normale du triangle
		XMVECTOR n = XMVector3Cross(e1, e2);

		// Accumule sur chaque vertex
		XMStoreFloat3(&m_vertices[i0].Normal,
			XMVectorAdd(XMLoadFloat3(&m_vertices[i0].Normal), n));
		XMStoreFloat3(&m_vertices[i1].Normal,
			XMVectorAdd(XMLoadFloat3(&m_vertices[i1].Normal), n));
		XMStoreFloat3(&m_vertices[i2].Normal,
			XMVectorAdd(XMLoadFloat3(&m_vertices[i2].Normal), n));
	}

	for (Vertex& v : m_vertices)
	{
		XMVECTOR n = XMLoadFloat3(&v.Normal);
		n = XMVector3Normalize(n);
		XMStoreFloat3(&v.Normal, n);
	}
}

void Mesh::FinalizeUpload()
{
	SafeRelease(m_pVbUpload);
	SafeRelease(m_pIbUpload);
}

void Mesh::Draw(ID3D12GraphicsCommandList* cmdList) const
{
	cmdList->IASetVertexBuffers(0, 1, &m_vbView);
	cmdList->IASetIndexBuffer(&m_ibView);
	cmdList->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);
}
