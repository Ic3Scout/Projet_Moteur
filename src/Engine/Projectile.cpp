#include "Projectile.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ResourceManager.h"
#include "Render/Renderer.h"
#include "Engine/ECS/ECS.h"
#include "Engine/Engine.h"
#include "Engine/ECS/Components/MeshRendererComponent.h"
#include "Engine/ECS/Components/RigidBodyComponent.h"
#include "Engine/ECS/Components/ProjectileComponent.h"
#include "Engine/ECS/Components/ColliderComponent.h"
#include "Engine/ECS/Components/EnemyComponent.h"

void Projectile::Initialize(float size, Shape shape, int poolSize,float distanceMax,Engine* engine, ECS* ecs, Renderer* renderer)
{
	m_size = size;
	m_shape = shape;
	m_ecs = ecs;
	m_renderer = renderer;
	m_engine = engine;
	m_maxDistance = distanceMax;

	CreatePulling(poolSize);
}

Entity* Projectile::GetAvaibleProjectile()
{
	for (Entity* projectile : m_projectiles)
	{
		if (!projectile->IsActive())
		{
			projectile->SetActive(true);
			return projectile;
		}
	}
	return nullptr;
}

void Projectile::CreatePulling(int poolSize)
{
	for (int i = 0; i < poolSize; i++)
	{
		Entity* projectile = m_ecs->CreateEntity<Entity>();
		projectile->SetActive(false);
		projectile->AddComponent<MeshRendererComponent>()->SetMesh(ResourceManager::Instance().GetMeshPreset(MeshPreset::Sphere), m_renderer);
		projectile->GetComponent<MeshRendererComponent>()->SetTexture(ResourceManager::Instance().LoadTexture(L"../../res/BallTex.png"));
		projectile->SetScale(m_size);
		projectile->AddComponent<RigidBodyComponent>()->SetUseGravity(false);
		projectile->AddComponent<ProjectileComponent>()->SetMaxDistance(m_maxDistance);
		projectile->GetComponent<ProjectileComponent>()->SetDistanceTraveled(0.0f);
		projectile->AddComponent<ColliderComponent>()->SetType(ColliderComponent::Type::Sphere);
		projectile->GetComponent<ColliderComponent>()->SetRadius(m_size);

		m_projectiles.push_back(projectile);
	}
}

void Projectile::UpdateDistance(float dt)
{
	for (Entity* projectile : m_projectiles)
	{
		if (projectile->IsActive())
		{
			XMFLOAT3 velocity = projectile->GetComponent<RigidBodyComponent>()->GetVelocity();
			ProjectileComponent* projectileComp = projectile->GetComponent<ProjectileComponent>();

			float speed = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);

			projectileComp->SetDistanceTraveled(projectileComp->GetDistanceTraveled() + speed * dt);

			if(projectileComp->GetDistanceTraveled() >= projectileComp->GetMaxDistance())
			{
				projectile->SetActive(false);
				projectileComp->SetDistanceTraveled(0.0f);
			}
		}
	}
}

void Projectile::Update(float dt)
{
	UpdateDistance(dt);

	for(Entity* projectile : m_projectiles)
	{
		if (projectile->IsActive())
		{
			ColliderComponent* collider = projectile->GetComponent<ColliderComponent>();
			collider->OnCollisionEnter = [this](Entity* self, Entity* other)
			{
				if(other->GetComponent<EnemyComponent>())
				{
					other->SetActive(false);
					other->GetComponent<EnemyComponent>()->SetScoreValue(100);
					self->SetActive(false);
				}
			};
		}
	}
}
