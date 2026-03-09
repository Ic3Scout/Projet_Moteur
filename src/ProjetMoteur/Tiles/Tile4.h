#pragma once
#include "ProjetMoteur/Tiles/Tiles.h"

class Tile4 : public Tiles
{
public:
	void Initialize(ECS* ecs, Renderer* renderer) override;
	void Update(float deltaTime) override;
};

