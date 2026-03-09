#include "EnemyIdleState.h"
#include "Core/StateMachine.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Components/PlayerComponent.h"
#include "EnemyChaseState.h"

void EnemyIdleState::OnEnter(StateMachine& machine)
{
	Entity* entity = machine.GetOwner();
	
	entity->SetPosition(0.0f, 0.0f, 10.0f);
}

void EnemyIdleState::Update(StateMachine& machine,ECS* ecs, float dt)
{
	for (Entity* player : ecs->GetEntities())
	{
		if (player->GetComponent<PlayerComponent>() == nullptr) continue;

		if (IsPlayerInRange(machine.GetOwner(), player))
		{
			machine.ChangeState(new EnemyChaseState());
		}
	}
}

void EnemyIdleState::OnExit(StateMachine& machine)
{

}

bool EnemyIdleState::IsPlayerInRange(Entity* enemy, Entity* player)
{
	XMFLOAT3 enemyPos = enemy->GetPosition();
	XMFLOAT3 playerPos = player->GetPosition();

	float distanceSquared = (enemyPos.x - playerPos.x) * (enemyPos.x - playerPos.x) +
		(enemyPos.y - playerPos.y) * (enemyPos.y - playerPos.y) +
		(enemyPos.z - playerPos.z) * (enemyPos.z - playerPos.z);
	const float detectionRange = 5.0f; // Example range

	return distanceSquared <= detectionRange * detectionRange;
}

