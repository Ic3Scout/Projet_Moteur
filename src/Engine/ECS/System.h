#pragma once
class ECS;

class System
{
public:
	virtual ~System() = default;
	virtual void Update(ECS& ecs, float deltaTime) = 0;
};

