#pragma once
#include "Core/State.h"

class StateMachine;
class ECS;
class Entity;

class EnemyIdleState : public State
{
public:
	virtual void OnEnter(StateMachine& machine) override;
	virtual void Update(StateMachine& machine,ECS* ecs, float dt) override;
	virtual void OnExit(StateMachine& machine) override;

private:
	bool IsPlayerInRange(Entity* enemy, Entity* player);

};

