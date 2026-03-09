#pragma once
#include "Engine/ECS/System.h"

class PhysicSystem : public System
{
public:
	virtual void Update(ECS& ecs, float deltaTime) override;
};

