#pragma once
#include "Engine/ECS/System.h"
class ECS;

class PlayerSystem : public System
{
public:
	virtual void Update(ECS& ecs, float deltaTime) override;
};

