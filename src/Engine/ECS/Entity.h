#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;

class Component;

class Entity
{
public:
	Entity();
	virtual ~Entity() = default;

	void SetId(uint32_t id) { m_Id = id; }
	uint32_t GetId() const { return m_Id; }

	template<typename T>
	T* AddComponent();

	template<typename T>
	T* GetComponent();

	void IdentityMatrix(XMFLOAT4X4& matrix); //needed ?

	//POSITION
	void SetPosition(float x, float y, float z); // set XYZ
	void SetPosition(XMFLOAT3 vector); // set XYZ with Vector

	void SetPositionX(float x); // set XYZ
	void SetPositionY(float y); // set Y
	void SetPositionZ(float z); // set Z

	void MoveForward(float dist); // move along the "forward" of the obj
	void MoveUp(float dist); // move along the "up" of the obj
	void MoveRight(float dist); // move along the "right" of the obj

	void Translate(float x, float y, float z);

	XMFLOAT3 GetPosition() const { return m_position; } // get the position of the obj

	void CoutPosition(); // DEBUG

	//SCALING
	void SetScale(float scale); // set the scale 
	void Scale(float scale); // multiply current scale
	void SetScale(XMFLOAT3 vectorScale); // set the scale with XYZ Vector
	void Scale(XMFLOAT3 vectorScale); // multiply current scale with XYZ Vector
	void SetScale(float scaleX, float scaleY, float scaleZ); // set the scale with XYZ
	void Scale(float scaleX, float scaleY, float scaleZ); // multiply current scale with XYZ
	XMFLOAT3 GetScale() const { return m_scale; } // get the scale of the obj

	void CoutScale(); // DEBUG

	//ROTATION
	void RotateX(float angle); // Rotate in world
	void RotateLocalX(float angle); // Rotate locally
	void RotateY(float angle); // Rotate in world
	void RotateLocalY(float angle); // Rotate locally
	void RotateZ(float angle); // Rotate in world
	void RotateLocalZ(float angle); // Rotate locally

	void SetRotation(const XMFLOAT4& quaternion);
	void SetRotationX(float angle);
	void SetRotationY(float angle);
	void SetRotationZ(float angle);

	void SetForward(XMFLOAT3 vector);
	void SetRight(XMFLOAT3 vector);
	void SetUp(XMFLOAT3 vector);

	XMFLOAT3 GetForward();
	XMFLOAT3 GetRight();
	XMFLOAT3 GetUp();


	void LookAt(float x, float y, float z); // look at a specific coords
	void OrbitAround(XMFLOAT3 pivot, XMFLOAT3 axis, float angle, float radius);

	XMFLOAT4 GetRotation();

	XMFLOAT4X4 GetMatrix() const { return m_matrix; } // get the matrix of the obj

	void CoutRotation(); // DEBUG
	void CoutMatrix(); // DEBUG

	void SetActive(bool active) { m_isActive = active; } 
	bool IsActive() const { return m_isActive; }

public:
	void UpdateBasisFromQuaternion();
	void UpdateWorldMatrix();

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
	uint32_t m_Id = 0;
	std::vector<Component*> m_Components;

	bool m_isActive = true;
};

#include "Entity.inl"
