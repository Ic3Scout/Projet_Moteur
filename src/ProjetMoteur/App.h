#pragma once

#include "Engine/Engine.h"

class Window;
class Renderer;
class SceneManager;
class Entity;
class ECS;
class Mesh;
class ResourceManager;

class App
{
public:
	App(Engine& engine);

	void Initialize();
	void Update();
	void Shutdown();
	void UpdateWindow();

	void HandleInput();

	void CreateCamera();

private:
	Engine& m_engine;
	Window* m_window = nullptr;
	Renderer* m_renderer = nullptr;
	SceneManager* m_sceneManager = nullptr;
	ResourceManager* m_resourceManager = nullptr;
	Entity* m_cam = nullptr;
	ECS* m_ecs = nullptr;
};

