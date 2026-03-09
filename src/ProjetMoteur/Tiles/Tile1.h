#pragma once
#include "ProjetMoteur/Tiles/Tiles.h"
#include "Engine/ECS/ECS.h"
#include "Render/Renderer.h"

class Tile1 : public Tiles
{
public:
	void Initialize(ECS* ecs, Renderer* renderer) override;
	void Update(float deltaTime) override;


};

