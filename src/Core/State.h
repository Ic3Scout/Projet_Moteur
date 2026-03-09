#pragma once
class StateMachine;
class ECS;

class State
{
public:
	virtual ~State() = default;
	virtual void OnEnter(StateMachine& machine) = 0;
	virtual void OnExit(StateMachine& machine) = 0;

	virtual void Update(StateMachine& machine,ECS* ecs, float dt) = 0;
};

