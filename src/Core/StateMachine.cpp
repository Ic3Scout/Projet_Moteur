#include "StateMachine.h"
#include "State.h"
#include "Engine/ECS/Entity.h"

void StateMachine::ChangeState(State* newState)
{
	if (currentState)
		currentState->OnExit(*this);
	currentState = newState;
	if (currentState)
		currentState->OnEnter(*this);
}

void StateMachine::Update(float dt)
{
	if (currentState)
		currentState->Update(*this,m_ecs, dt);
}

