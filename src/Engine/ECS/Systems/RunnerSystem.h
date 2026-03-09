#pragma once
#include "Engine/ECS/System.h"

class RunnerComponent;

class RunnerSystem : public System
{
public:
	virtual void Update(ECS& ecs, float deltaTime) override;

private:
	RunnerComponent* GetRunnerComponent(ECS& ecs);
};

