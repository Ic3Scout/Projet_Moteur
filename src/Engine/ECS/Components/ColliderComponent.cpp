#include "ColliderComponent.h"

void ColliderComponent::TriggerCollisionEnter(Entity* self, Entity* other)
{
	if (OnCollisionEnter)
	{
		OnCollisionEnter(self, other);
	}
}
