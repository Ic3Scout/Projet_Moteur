#pragma once
#include "Engine/ECS/Component.h"

class DestructibleComponent : public Component
{
public:
	void Destroy() { m_isDestroyed = true; }
	bool IsDestroyed() const { return m_isDestroyed; }

private:
	bool m_isDestroyed = false;
};

