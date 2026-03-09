#include "Camera.h"
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

Camera::Camera()
{
	m_fov = XM_PIDIV4; // Field of view

	m_width = 16.0f;
	m_height = 9.0f;

	m_aspectRatio = m_width / m_height; // format 16:9 classique

	m_nearPlane = 1.0f; // distance min to render
	m_farPlane = 100.0f; // distance max to render
}

Camera::~Camera()
{
	// DESTROY CAM IF NEEDED
}

XMFLOAT3 Camera::GetPosition()
{
	// MAYBE HANDLED BY TRANSFORM
	return { 0,0,0 };
}

void Camera::SetPosition(float x, float y, float z)
{
	// MAYBE HANDLED BY TRANSFORM

}

void Camera::Update()
{
	// UPDATE CAMERA
}

void Camera::SetLens(float fov, float aspectRatio, float nearP, float farP)
{
	m_fov = fov;
	m_aspectRatio = aspectRatio;
	m_nearPlane = nearP;
	m_farPlane = farP;
}


