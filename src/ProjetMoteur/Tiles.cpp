#include "ProjetMoteur/Tiles/Tiles.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ResourceManager.h"
#include "Engine/ECS/Components/MeshRendererComponent.h"
#include <cmath>
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Components/EnemyComponent.h"
#include "Core/Random.h"
#include <iostream>
#include "Engine/ECS/Components/ObstacleComponent.h"
#include "Engine/ECS/Components/ColliderComponent.h"

void Tiles::MoveZ(float dist)
{
	m_position.z += dist;
}

void Tiles::CreateArc(ECS* ecs, Renderer* renderer, XMFLOAT3 center, float radius, float startAngle, float endAngle, int segments)
{
	float angleStep = (endAngle - startAngle) / segments;
	for (int i = 0; i <= segments; ++i)
	{
		float angle = startAngle + i * angleStep;
		float x = center.x + radius * cosf(angle);
		float z = center.z + radius * sinf(angle);

		XMFLOAT3 offset = { x , 0.0f, z  };

		Entity* tileEntity = ecs->CreateEntity<Entity>();
		tileEntity->AddComponent<MeshRendererComponent>()->SetMesh(ResourceManager::Instance().GetMeshPreset(MeshPreset::Cylinder), renderer);
		tileEntity->SetPosition(GetPosition().x + offset.x, GetPosition().y + offset.y, GetPosition().z + offset.z);
		tileEntity->SetScale({ 0.1f, 0.3f, 0.1f });
		tileEntity->RotateX(XM_PIDIV2);

		GetEntities().push_back(tileEntity);
		GetLocalOffset().push_back(offset);
	}
}

void Tiles::SpawnEnemy()
{
	Random random;
	int choosenEnemy = random.RandomInt(1, 3);
	float chance = random.Chance(50.0f);

	if(chance < 0.5f)
	{
		choosenEnemy = 0; // no enemy
	}

	for(Entity* entity : m_entities)
	{
		if (entity->GetComponent<EnemyComponent>())
		{
			if (entity->GetComponent<EnemyComponent>()->GetID() == choosenEnemy)
			{
				entity->SetActive(true);
				entity->GetComponent<EnemyComponent>()->SetScoreValue(0);
				m_activeEnemy = entity;
			}
			else
			{
				entity->SetActive(false);
			}
		}
		if(entity->GetComponent<ObstacleComponent>())
		{
			if (choosenEnemy == 0)
			{
				entity->SetActive(false);
			}
			else
			{
				entity->SetActive(true);
			}
		}
	}
}

void Tiles::CreateTileEntity(ECS* ecs, Renderer* renderer, XMFLOAT3 offset, XMFLOAT3 scale, float Yrotation)
{
	Entity* tileEntity = ecs->CreateEntity<Entity>();
	tileEntity->AddComponent<MeshRendererComponent>()->SetMesh(ResourceManager::Instance().GetMeshPreset(MeshPreset::Cylinder), renderer);
	tileEntity->SetPosition(GetPosition().x + offset.x, GetPosition().y + offset.y, GetPosition().z + offset.z);
	tileEntity->SetScale(scale);
	tileEntity->RotateX(XM_PIDIV2);
	tileEntity->RotateY(XMConvertToRadians(Yrotation));
	tileEntity->GetComponent<MeshRendererComponent>()->SetTexture(m_railTexture);
	GetEntities().push_back(tileEntity);
	GetLocalOffset().push_back(offset);
	GetRails().push_back(tileEntity);
}

void Tiles::CreateEnemyEntity(ECS* ecs, Renderer* renderer, XMFLOAT3 offset, XMFLOAT3 scale, int ID)
{
	Entity* EnemyEntity = ecs->CreateEntity<Entity>();
	EnemyEntity->AddComponent<MeshRendererComponent>()->SetMesh(ResourceManager::Instance().GetMeshPreset(MeshPreset::Cube), renderer);
	EnemyEntity->AddComponent<EnemyComponent>()->SetID(ID);
	EnemyEntity->AddComponent<ColliderComponent>()->SetType(ColliderComponent::Type::Sphere);
	EnemyEntity->SetPosition(GetPosition().x + offset.x, GetPosition().y + offset.y, GetPosition().z + offset.z);
	EnemyEntity->SetScale(scale);
	EnemyEntity->GetComponent<MeshRendererComponent>()->SetTexture(m_targetHandle);
	EnemyEntity->SetActive(false);
	EnemyEntity->GetComponent<ColliderComponent>()->SetRadius(0.5f);
	GetEntities().push_back(EnemyEntity);
	GetLocalOffset().push_back(offset);
}

void Tiles::CreateWallEntity(ECS* ecs, Renderer* renderer, XMFLOAT3 offset, XMFLOAT3 scale)
{
	// Define Walls
	Entity* ObstacleEntity = ecs->CreateEntity<Entity>();
	ObstacleEntity->AddComponent<MeshRendererComponent>()->SetMesh(ResourceManager::Instance().GetMeshPreset(MeshPreset::Cube), renderer);
	ObstacleEntity->AddComponent<ObstacleComponent>();
	ObstacleEntity->AddComponent<ColliderComponent>()->SetType(ColliderComponent::Type::Box);
	ObstacleEntity->SetPosition(GetPosition().x + offset.x, GetPosition().y + offset.y, GetPosition().z + offset.z);
	ObstacleEntity->SetScale(scale);
	ObstacleEntity->GetComponent<MeshRendererComponent>()->SetTexture(m_wallTexture);
	ObstacleEntity->SetActive(false);
	ObstacleEntity->GetComponent<ColliderComponent>()->SetRadius(1.0f);
	m_obstacle = ObstacleEntity;
	GetEntities().push_back(ObstacleEntity);
	GetLocalOffset().push_back(offset);
}

void Tiles::SetActive(bool active)
{
	m_active = active;

	for (Entity* entity : m_entities)
	{
		if (entity->GetComponent<EnemyComponent>()) continue;
		if (entity->GetComponent<ObstacleComponent>()) continue;
		entity->SetActive(active);
	}
}

void Tiles::SetActiveEnemies(bool active)
{
	for (Entity* entity : m_entities)
	{
		if (entity->GetComponent<EnemyComponent>())
		{
			entity->SetActive(active);
		}
		if(entity->GetComponent<ObstacleComponent>())
		{
			entity->SetActive(active);
		}
	}
}
