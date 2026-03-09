#include "RunnerSystem.h"
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Components/RunnerComponent.h"
#include "Engine/ECS/Components/ObstacleComponent.h"

void RunnerSystem::Update(ECS& ecs, float deltaTime)
{
	
}

RunnerComponent* RunnerSystem::GetRunnerComponent(ECS& ecs)
{
	for(Entity* entity : ecs.GetEntities())
	{
		RunnerComponent* runner = entity->GetComponent<RunnerComponent>();
		if(runner != nullptr)
			return runner;
	}
	return nullptr;
}