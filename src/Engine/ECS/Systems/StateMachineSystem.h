#pragma once
#include "Engine/ECS/System.h"

class StateMachineSystem : public System
{
public:
	void Update(ECS& ecs, float deltaTime) override;
};

