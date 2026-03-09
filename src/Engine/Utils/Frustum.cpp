#include "Frustum.h"

Frustum::Frustum()
{
	for(int i = 0; i < 6; i++)
	{
		m_planes[i] = { 0.0f, 0.0f, 0.0f, 0.0f };
	}
}

void Frustum::FromViewProjection(const XMFLOAT4X4& viewProjection)
{
	XMVECTOR left = XMVectorSet(viewProjection._14 + viewProjection._11, viewProjection._24 + viewProjection._21, viewProjection._34 + viewProjection._31, viewProjection._44 + viewProjection._41);
	XMVECTOR right = XMVectorSet(viewProjection._14 - viewProjection._11, viewProjection._24 - viewProjection._21, viewProjection._34 - viewProjection._31, viewProjection._44 - viewProjection._41);
	XMVECTOR top = XMVectorSet(viewProjection._14 - viewProjection._12, viewProjection._24 - viewProjection._22, viewProjection._34 - viewProjection._32, viewProjection._44 - viewProjection._42);
	XMVECTOR bottom = XMVectorSet(viewProjection._14 + viewProjection._12, viewProjection._24 + viewProjection._22, viewProjection._34 + viewProjection._32, viewProjection._44 + viewProjection._42);

	XMVECTOR nearP = XMVectorSet(viewProjection._13, viewProjection._23, viewProjection._33, viewProjection._43);
	XMVECTOR farP = XMVectorSet(viewProjection._14 - viewProjection._13, viewProjection._24 - viewProjection._23, viewProjection._34 - viewProjection._33, viewProjection._44 - viewProjection._43);

	XMStoreFloat4(&m_planes[0], NormalizePlane(left));
	XMStoreFloat4(&m_planes[1], NormalizePlane(right));
	XMStoreFloat4(&m_planes[2], NormalizePlane(top));
	XMStoreFloat4(&m_planes[3], NormalizePlane(bottom));
	XMStoreFloat4(&m_planes[4], NormalizePlane(nearP));
	XMStoreFloat4(&m_planes[5], NormalizePlane(farP));
}

XMVECTOR XM_CALLCONV Frustum::NormalizePlane(FXMVECTOR p)
{
	XMVECTOR n = XMVectorSetW(p, 0.0f);
	XMVECTOR len = XMVector3Length(n);
	return XMVectorDivide(p, len);
}
