#pragma once
#include "Engine/ECS/Component.h"
#include <DirectXMath.h>

using namespace DirectX;

class RailFollowerComponent : public Component
{
public:
	void SetRailPosition(const XMFLOAT3& pos) { m_RailPosition = pos; }
	void SetSpeed(float speed) { m_Speed = speed; }
	void SetLockRotation(bool lock) { m_lockRotation = lock; }

	XMFLOAT3 GetRailPosition() const { return m_RailPosition; }
	float GetSpeed() const { return m_Speed; }
	bool GetLockRotation() const { return m_lockRotation; }

private:
	XMFLOAT3 m_RailPosition;
	float m_Speed = 0.0f;

	bool m_lockRotation = true;

};

