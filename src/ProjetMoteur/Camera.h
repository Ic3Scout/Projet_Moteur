#pragma once
#include "mftransform.h"
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

class Camera
{
public:
	float m_fov;
	float m_aspectRatio; // Should be : m_width / m_height
	float m_width;
	float m_height;
	float m_nearPlane; // minimal distance to show
	float m_farPlane; // maximal distance to show


public:
	Camera(); //init
	~Camera(); //destroy
	
	void Update();

	// HANDLED BY TRANSFORM ?
	XMFLOAT3 GetPosition();
	void SetPosition(float x, float y, float z);

	// Set frustum -> the "Lens" of the camera, dictates how the view behaves
	void SetLens(float fov, float aspectRatio, float nearP, float farP);
};

