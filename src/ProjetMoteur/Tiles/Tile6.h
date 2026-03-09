#pragma once
#include "Tiles.h"
class Tile6 : public Tiles
{
public:
	void Initialize(ECS* ecs, Renderer* renderer) override;
	void Update(float deltaTime) override;
};

