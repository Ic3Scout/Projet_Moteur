
#include "Engine.h"
#include "Core/GameTimer.h"


void Engine::Initialize()
{
	m_isRunning = true;
	m_timer = new GameTimer();

}

void Engine::Run()
{
	if(m_InitCallback)
		m_InitCallback();

	m_timer->Reset();
	while (m_isRunning)
	{

		m_timer->Tick();
		float deltaTime = m_timer->DeltaTime();
		m_ecs.Update(deltaTime);

		if (m_updateCallback)
			m_updateCallback();
	}

	if(m_ShutdownCallback)
		m_ShutdownCallback();
}

void Engine::Shutdown()
{
	m_isRunning = false;
}

float Engine::GetDeltaTime()
{
	return m_timer->DeltaTime();
}

float Engine::GetTotalTime()
{
	return m_timer->TotalTime();
}
