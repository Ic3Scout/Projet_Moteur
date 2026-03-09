#pragma once
#include "Engine/ECS/System.h"

class ObstacleSystem : public System
{
public:
	virtual void Update(ECS& ecs, float deltaTime) override;
};

