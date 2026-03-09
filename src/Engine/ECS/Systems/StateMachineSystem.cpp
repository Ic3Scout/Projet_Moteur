#include "StateMachineSystem.h"
#include "Engine/ECS/Components/StateMachineComponent.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ECS/ECS.h"

void StateMachineSystem::Update(ECS& ecs, float deltaTime)
{
	for(Entity* entity : ecs.GetEntities())
	{
		StateMachineComponent* smc = entity->GetComponent<StateMachineComponent>();
		if(smc)
		{
			smc->GetStateMachine()->Update(deltaTime);
		}
	}
}
