#include "Tile6.h"
#include "Engine/ECS/ECS.h"
#include "Render/Renderer.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ResourceManager.h"
#include "Engine/ECS/Components/MeshRendererComponent.h"
#include "Engine/ECS/Components/EnemyComponent.h"
#include "Engine/ECS/Components/ColliderComponent.h"
#include "Engine/ECS/Components/ObstacleComponent.h"

void Tile6::Initialize(ECS* ecs, Renderer* renderer)
{

	// Define Rails Positions //
	CreateTileEntity(ecs, renderer, { -2.0f, 0.0f, 0.0f }, { 0.1f, 6.0f, 0.1f }, 0);
	CreateTileEntity(ecs, renderer, { -2.0f, 0.0f, 6.0f }, { 0.1f, 2.8f, 0.1f }, 45);
	CreateTileEntity(ecs, renderer, { 0.0f, 0.0f, 8.0f }, { 0.1f, 6.0f, 0.1f }, 0);
	CreateTileEntity(ecs, renderer, { 0.0f, 0.0f, 14.0f }, { 0.1f, 4.0f, 0.1f }, 0);


	// Define Enemies Spawn Point //
	CreateEnemyEntity(ecs, renderer, { 2.0f, 1.0f, 6.0f }, { 0.5f, 0.5f, 0.5f }, 1);
	CreateEnemyEntity(ecs, renderer, { -2.0f, 1.0f, 6.0f }, { 0.5f, 0.5f, 0.5f }, 2);
	CreateEnemyEntity(ecs, renderer, { 0.0f, 1.0f, 14.0f }, { 0.5f, 0.5f, 0.5f }, 3);

	// Define Walls
	CreateWallEntity(ecs, renderer, { 0.0f, 0.0f, 16.0f }, { 1.0f, 1.0f, 1.0f });

	SetActive(false);
	SetIsTurnL(false);
	SetIsTurnR(false);
	SetIn(-2);
	SetOut(0);
}

void Tile6::Update(float deltaTime)
{
	MoveZ(-(GetSpeed() * deltaTime));

	for (int i = 0; i < GetEntities().size(); i++)
	{
		Entity* entity = GetEntities()[i];
		XMFLOAT3 offset = GetLocalOffset()[i];

		entity->SetPosition(GetPosition().x + offset.x, GetPosition().y + offset.y, GetPosition().z + offset.z);

		if (m_activeEnemy != nullptr)
		{
			if (m_activeEnemy->IsActive() == false)
			{
				int score = m_activeEnemy->GetComponent<EnemyComponent>()->GetScoreValue() + GetScoreValue();
				SetScoreValue(score);
				m_obstacle->SetActive(false);
				m_activeEnemy = nullptr;
			}
		}
	}
}
