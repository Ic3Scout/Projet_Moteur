#pragma once
#include "Engine/ECS/Component.h"

class RunnerComponent : public Component
{
public:
	void SetSpeed(float speed) { m_Speed = speed; }
	
	float GetSpeed() const { return m_Speed; }
private:
	float m_Speed = 0.0f;
};

