#include "EnemyChaseState.h"
#include "Core/StateMachine.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Components/PlayerComponent.h"

void EnemyChaseState::OnEnter(StateMachine& machine)
{
	Entity* entity = machine.GetOwner();

}

void EnemyChaseState::OnExit(StateMachine& machine)
{
	// Code to execute when exiting the chase state
}

void EnemyChaseState::Update(StateMachine& machine, ECS* ecs, float dt)
{
	for (Entity* player : ecs->GetEntities())
	{
		if (player->GetComponent<PlayerComponent>() == nullptr) continue;

		if (IsPlayerInRange(machine.GetOwner(), player))
		{
			Entity* enemy = machine.GetOwner();
			enemy->SetPosition(2.0f, 0.0f, 10.0f);
		}
	}
}

bool EnemyChaseState::IsPlayerInRange(Entity* enemy, Entity* player)
{
	XMFLOAT3 enemyPos = enemy->GetPosition();
	XMFLOAT3 playerPos = player->GetPosition();
	float distanceSquared = (enemyPos.x - playerPos.x) * (enemyPos.x - playerPos.x) +
		(enemyPos.y - playerPos.y) * (enemyPos.y - playerPos.y) +
		(enemyPos.z - playerPos.z) * (enemyPos.z - playerPos.z);
	const float detectionRange = 5.0f; // Example range
	return distanceSquared <= detectionRange * detectionRange;
}