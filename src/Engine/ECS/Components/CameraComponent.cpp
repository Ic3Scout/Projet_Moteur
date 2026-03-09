#include "CameraComponent.h"
#include "Engine/Utils/Frustum.h"
#include "Engine/ECS/Entity.h"

void CameraComponent::SetAll(float nearPlane, float farPlane, float height, float width, bool perspective)
{
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	m_height = height;
	m_width = width;
	m_aspectRatio = m_width / m_height;
	m_isPerspective = perspective;

	m_frustum = new Frustum();
}

void CameraComponent::SetWindowSize(float width, float height)
{
	m_width = width; 
	m_height = height; 
	m_aspectRatio = width / height; 
}

void CameraComponent::Update()
{
	XMFLOAT4X4 transform = GetEntity()->GetMatrix();
	XMMATRIX world = XMLoadFloat4x4(&transform);

	XMMATRIX view = XMMatrixInverse(nullptr, world);
	XMStoreFloat4x4(&m_viewMatrix, view);

	XMMATRIX proj = XMLoadFloat4x4(&m_projectionMatrix);
	XMMATRIX viewProj = view * proj;

	XMFLOAT4X4 vp;
	XMStoreFloat4x4(&vp, viewProj);
	m_frustum->FromViewProjection(vp);
}

void CameraComponent::UpdateProjectionMatrix()
{
	if(m_isPerspective)
		XMStoreFloat4x4(&m_projectionMatrix, XMMatrixPerspectiveFovLH(m_fov, m_aspectRatio, m_nearPlane, m_farPlane));
	else
		XMStoreFloat4x4(&m_projectionMatrix, XMMatrixOrthographicLH(m_width, m_height, m_nearPlane, m_farPlane));
}
