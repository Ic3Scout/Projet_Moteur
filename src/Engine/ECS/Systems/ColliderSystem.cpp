#include "ColliderSystem.h"
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ECS/Components/ColliderComponent.h"

void ColliderSystem::Update(ECS& ecs, float deltaTime)
{
	for (Entity* entity : ecs.GetEntities())
	{
		if(entity == nullptr || entity->IsActive() == false)
			continue;

		ColliderComponent* collider = entity->GetComponent<ColliderComponent>();
		if (collider == nullptr)
			continue;
		for (Entity* otherEntity : ecs.GetEntities())
		{
			if(otherEntity == nullptr || !otherEntity->IsActive())
				continue;

			if (entity == otherEntity)
				continue;

			ColliderComponent* otherCollider = otherEntity->GetComponent<ColliderComponent>();
			if (otherCollider == nullptr)
				continue;
			
			if(CheckSphereCollision(entity, otherEntity))
			{
				// Handle sphere collision
				collider->TriggerCollisionEnter(entity, otherEntity);
				otherCollider->TriggerCollisionEnter(otherEntity, entity);
			}
			else if(CheckBoxCollision(entity, otherEntity))
			{
				// Handle box collision
				collider->TriggerCollisionEnter(entity, otherEntity);
				otherCollider->TriggerCollisionEnter(otherEntity, entity);
			}

		}
	}
}

bool ColliderSystem::CheckSphereCollision(Entity* entityA, Entity* entityB)
{
	ColliderComponent* sphereA = entityA->GetComponent<ColliderComponent>();
	ColliderComponent* sphereB = entityB->GetComponent<ColliderComponent>();

	if (sphereA == nullptr || sphereB == nullptr)
		return false;

	if(sphereA->GetType() != ColliderComponent::Type::Sphere || sphereB->GetType() != ColliderComponent::Type::Sphere)
		return false;

	XMFLOAT3 posA = entityA->GetPosition();
	XMFLOAT3 posB = entityB->GetPosition();

	float radiusA = sphereA->GetRadius();
	float radiusB = sphereB->GetRadius();

	float distanceSquared = (posA.x - posB.x) * (posA.x - posB.x) + (posA.y - posB.y) * (posA.y - posB.y) + (posA.z - posB.z) * (posA.z - posB.z);

	float radiusSum = radiusA + radiusB;

	return distanceSquared <= radiusSum * radiusSum;

}

bool ColliderSystem::CheckBoxCollision(Entity* entityA, Entity* entityB)
{

	ColliderComponent* boxA = entityA->GetComponent<ColliderComponent>();
	ColliderComponent* boxB = entityB->GetComponent<ColliderComponent>();

	if (boxA == nullptr || boxB == nullptr)
		return false;
	if (boxA->GetType() != ColliderComponent::Type::Box || boxB->GetType() != ColliderComponent::Type::Box)
		return false;

	XMFLOAT3 posA = entityA->GetPosition();
	XMFLOAT3 posB = entityB->GetPosition();

	XMFLOAT3 sizeA = boxA->GetBoxSize();
	XMFLOAT3 sizeB = boxB->GetBoxSize();

	bool collisionX = posA.x + sizeA.x / 2 >= posB.x - sizeB.x / 2 && posB.x + sizeB.x / 2 >= posA.x - sizeA.x / 2;
	bool collisionY = posA.y + sizeA.y / 2 >= posB.y - sizeB.y / 2 && posB.y + sizeB.y / 2 >= posA.y - sizeA.y / 2;
	bool collisionZ = posA.z + sizeA.z / 2 >= posB.z - sizeB.z / 2 && posB.z + sizeB.z / 2 >= posA.z - sizeA.z / 2;

	return collisionX && collisionY && collisionZ;
}
