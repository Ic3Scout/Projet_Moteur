#pragma once

class State;
class Entity;
class ECS;

class StateMachine
{
public:
	void ChangeState(State* newState);
	void Update(float dt);
	State* GetCurrentState() const { return currentState; }

	void SetOwner(Entity* entity) { owner = entity; }
	Entity* GetOwner() const { return owner; }
	void SetECS(ECS* ecs) { m_ecs = ecs; }

private:
	Entity* owner = nullptr;
	State* currentState = nullptr;
	ECS* m_ecs = nullptr;
};

