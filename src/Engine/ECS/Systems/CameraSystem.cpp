#include "CameraSystem.h"
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Components/CameraComponent.h"

void CameraSystem::Update(ECS& ecs, float deltaTime)
{
	for(Entity* entity : ecs.GetEntities())
	{
		CameraComponent* camera = entity->GetComponent<CameraComponent>();
		if(camera == nullptr)
			continue;
		camera->Update();
		camera->UpdateProjectionMatrix();
	}
}
