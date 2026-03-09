#pragma once
#include "Engine/ECS/System.h"

class Entity;

class ColliderSystem : public System
{
public:
	virtual void Update(ECS& ecs, float deltaTime) override;

private:
	bool CheckSphereCollision(Entity* entityA, Entity* entityB);
	bool CheckBoxCollision(Entity* entityA, Entity* entityB);

};

