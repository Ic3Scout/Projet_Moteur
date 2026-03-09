#pragma once

class Renderer; 
class ECS;
class Engine;
class Entity;
class SceneManager;
struct UIFrame;

class Scene
{
public:
	virtual ~Scene() = default;

	virtual void Initialize(ECS* ecs, Renderer* renderer, Engine* engine, Entity* camera, SceneManager* sceneManager, UIFrame* frame) = 0;
	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;

	virtual void Update(float dt) = 0;

protected:
	Renderer* m_renderer = nullptr; 
	ECS* m_ecs = nullptr;
	Engine* m_engine = nullptr;
	Entity* m_cam = nullptr;
	SceneManager* m_sceneManager = nullptr;
	UIFrame* m_frame = nullptr;
};

