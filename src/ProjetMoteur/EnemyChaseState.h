#pragma once
#include "Core/State.h"

class StateMachine;
class ECS;
class Entity;

class EnemyChaseState : public State
{
public:
	virtual void OnEnter(StateMachine& machine) override;
	virtual void OnExit(StateMachine& machine) override;
	virtual void Update(StateMachine& machine, ECS* ecs, float dt) override;

private:
	bool IsPlayerInRange(Entity* enemy, Entity* player);
};

