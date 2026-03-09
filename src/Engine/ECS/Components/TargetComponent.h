#pragma once
#include "Engine/ECS/Component.h"

class TargetComponent : public Component
{
public:
	void SetTarget(Entity* target) { m_Target = target; }
	Entity* GetTarget() const { return m_Target; }

	void SetHasTarget(bool hasTarget) { m_HasTarget = hasTarget; }
	bool HasTarget() const { return m_HasTarget; }

private:
	Entity* m_Target = nullptr;
	bool m_HasTarget = false;
};

