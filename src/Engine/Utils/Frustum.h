#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Frustum
{
public:
	XMFLOAT4 m_planes[6];

	Frustum();

	void FromViewProjection(const XMFLOAT4X4& viewProjection);
	XMVECTOR XM_CALLCONV NormalizePlane(FXMVECTOR p);
};

