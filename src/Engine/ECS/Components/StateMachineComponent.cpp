#include "StateMachineComponent.h"
#include "Engine/ECS/ECS.h"

void StateMachineComponent::SetStateMachine(Entity* entity, ECS* ecs)
{
	m_stateMachine = new StateMachine();
	m_stateMachine->SetECS(ecs);
	m_stateMachine->SetOwner(entity);
}
