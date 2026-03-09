#pragma once
#include "Engine/ECS/Component.h"
class ProjectileComponent : public Component
{
public:
	void SetMaxDistance(float maxDistance) { m_maxDistance = maxDistance; }
	float GetMaxDistance() { return m_maxDistance; }
	
	void SetDistanceTraveled(float distance) { m_distanceTraveled = distance; }
	float GetDistanceTraveled() { return m_distanceTraveled; }

private:
	float m_distanceTraveled = 0.0f;
	float m_maxDistance = 50.0f;
};

