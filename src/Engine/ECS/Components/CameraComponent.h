#pragma once
#include "Engine/ECS/Component.h"
#include <DirectXMath.h>

using namespace DirectX;

class Frustum;

class CameraComponent : public Component
{
public:
	void SetFOV(float fov) { m_fov = fov; }
	void SetViewMatrix(const XMFLOAT4X4& viewMatrix) { m_viewMatrix = viewMatrix; }
	void SetProjectionMatrix(const XMFLOAT4X4& projectionMatrix) { m_projectionMatrix = projectionMatrix; }
	void SetAll(float nearPlane, float farPlane, float height, float width, bool perspective);
	void SetWindowSize(float width, float height);

	const XMFLOAT4X4& GetViewMatrix() const { return m_viewMatrix; }
	const XMFLOAT4X4& GetProjectionMatrix() const { return m_projectionMatrix; }
	float GetFOV() const { return m_fov; }
	float GetNearPlane() const { return m_nearPlane; }
	float GetFarPlane() const { return m_farPlane; }
	float GetAspectRatio() const { return m_aspectRatio; }

	void Update();
	void UpdateProjectionMatrix();

private:
	float m_fov = 45.0f;
	float m_nearPlane = 0.1f;
	float m_farPlane = 100.0f;
	float m_height = 16.0f;
	float m_width = 9.0f;
	float m_aspectRatio = m_height / m_width;

	bool m_isPerspective = true;

	XMFLOAT4X4 m_viewMatrix = XMFLOAT4X4();
	XMFLOAT4X4 m_projectionMatrix = XMFLOAT4X4();

	Frustum* m_frustum = nullptr;
};

