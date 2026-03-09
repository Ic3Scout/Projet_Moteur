#pragma once
#include "Engine/ECS/Component.h"
#include <DirectXMath.h>
#include <functional>

using namespace DirectX;

class ColliderComponent : public Component
{
public:
	enum class Type
	{
		Box,
		Sphere
	};

	void SetType(Type newType) { m_type = newType; }
	void SetRadius(float newRadius) { m_radius = newRadius; }
	void SetBoxSize(const XMFLOAT3& newSize) { m_boxSize = newSize; }
	void SetIsTrigger(bool trigger) { m_isTrigger = trigger; }

	Type GetType() const { return m_type; }
	float GetRadius() const { return m_radius; }
	XMFLOAT3 GetBoxSize() const { return m_boxSize; }
	bool IsTrigger() const { return m_isTrigger; }

	std::function<void(Entity* self, Entity* other)> OnCollisionEnter;
	void TriggerCollisionEnter(Entity* self,Entity* other);

private:
	
	Type m_type = Type::Sphere;

	float m_radius = 1.0f;

	XMFLOAT3 m_boxSize = XMFLOAT3(1.0f, 1.0f, 1.0f);

	bool m_isTrigger = false;
};

