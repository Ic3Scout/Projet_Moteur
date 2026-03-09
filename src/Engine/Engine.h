#pragma once
#include <functional>
#include "Engine/ECS/ECS.h"


class GameTimer;

class Engine
{
public:
	using Callback = std::function<void()>;

	void Initialize();
	void Run();
	void Shutdown();

	ECS& GetECS() { return m_ecs; }
	float GetDeltaTime();
	float GetTotalTime();

	void SetUpdateCallback(Callback callback) { m_updateCallback = callback; }
	void SetInitCallback(Callback callback) { m_InitCallback = callback; }
	void SetShutdownCallback(Callback callback) { m_ShutdownCallback = callback; }

private:

	ECS m_ecs;
	GameTimer* m_timer;

	Callback m_updateCallback;
	Callback m_InitCallback;
	Callback m_ShutdownCallback;

	bool m_isRunning = true;
};

