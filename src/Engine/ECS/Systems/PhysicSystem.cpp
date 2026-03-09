#include "PhysicSystem.h"
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ECS/Components/RigidBodyComponent.h"

void PhysicSystem::Update(ECS& ecs, float deltaTime)
{
	for(Entity* entity : ecs.GetEntities())
	{
		RigidBodyComponent* rigidBody = entity->GetComponent<RigidBodyComponent>();
		if(rigidBody)
		{
			XMFLOAT3 pos = entity->GetPosition();
			XMFLOAT3 vel = rigidBody->GetVelocity(); 

			if(rigidBody->IsUsingGravity())
			{
				vel.y -= 10.0f * deltaTime; 
			}
			pos.x += vel.x * deltaTime;
			pos.y += vel.y * deltaTime;
			pos.z += vel.z * deltaTime;
			entity->SetPosition(pos.x, pos.y, pos.z);
			rigidBody->SetVelocity(vel); 
		}
	}
}
