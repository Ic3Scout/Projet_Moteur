#include "ECS.h"
#include "System.h"

ECS::ECS()
{
	m_Id = 0;
}

ECS::~ECS()
{
	for (Entity* entity : m_Entities)
	{
		delete entity;
	}
}

void ECS::Update(float deltaTime)
{
	for (System* system : m_Systems)
	{
		system->Update(*this, deltaTime);
	}

	ProcessDestroy();
}

void ECS::DestroyEntity(Entity* entity)
{
	m_EntitiesToDestroy.push_back(entity);
}

void ECS::ProcessDestroy()
{
	for (Entity* entity : m_EntitiesToDestroy)
	{
		auto it = std::find(m_Entities.begin(), m_Entities.end(), entity);
		if (it != m_Entities.end())
		{
			delete *it;
			m_Entities.erase(it);
		}
	}
	m_EntitiesToDestroy.clear();
}
