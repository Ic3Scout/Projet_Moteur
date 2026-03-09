#pragma once
#include <dxgi.h>
#include <DirectXMath.h>

using namespace DirectX;

class Transform
{
public:
	Transform();

	//IDENTITY
	void Identity();
	void IdentityMatrix(XMFLOAT4X4& matrix); //needed ?

	//POSITION
	void SetPosition(float x, float y, float z); // set XYZ
	void MoveForward(float dist); // move along the "forward" of the obj
	void MoveUp(float dist); // move along the "up" of the obj
	void MoveRight(float dist); // move along the "right" of the obj

	void CoutPosition(); // DEBUG

	//SCALING
	void SetScale(float scale); // set the scale 
	void Scale(float scale); // multiply current scale
	void SetScaleVector(XMFLOAT3 vectorScale); // set the scale with XYZ Vector
	void ScaleVector(XMFLOAT3 vectorScale); // multiply current scale with XYZ Vector


	void CoutScale(); // DEBUG

	//ROTATION
	void SetRotationX(float angle); // set the rotation
	void SetRotationY(float angle); // set the rotation
	void SetRotationZ(float angle); // set the rotation

	void LookAt(float x, float y, float z); // look at a specific coords

	void SetMatrix(float matrixPos, float value); // manually modifiy a value of the matrix

	void CoutRotation(); // DEBUG
	void CoutMatrix(); // DEBUG

protected:
	//position
	XMFLOAT3 m_position;

	//scaling
	XMFLOAT3 m_scale;
	
	//rotation
	XMFLOAT3 m_forward;
	XMFLOAT3 m_up;
	XMFLOAT3 m_right;

	XMFLOAT4 m_quaternion;
	XMFLOAT4X4 m_matrix;
};


