#pragma once
#include "Engine/ECS/System.h"

class CameraSystem : public System
{
public:
	void Update(ECS& ecs, float deltaTime) override;
};

