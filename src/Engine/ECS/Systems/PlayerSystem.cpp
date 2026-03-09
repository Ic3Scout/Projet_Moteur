#include "PlayerSystem.h"
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ECS/Components/PlayerComponent.h"
#include "Engine/ECS/Components/ColliderComponent.h"
#include "Engine/ECS/Components/ObstacleComponent.h"

void PlayerSystem::Update(ECS& ecs, float deltaTime)
{
	std::vector<Entity*> Entities = ecs.GetEntities();

	for(Entity* entity : Entities)
	{
		if (entity->GetComponent<PlayerComponent>())
		{
			ColliderComponent* collider = entity->GetComponent<ColliderComponent>();
			collider->OnCollisionEnter = [this](Entity* self, Entity* other)
			{
				if (other->GetComponent<ObstacleComponent>())
				{
					self->SetActive(false);
				}
			};
		}
	}
}
