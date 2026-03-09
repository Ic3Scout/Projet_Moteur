#pragma once
#include "Engine/Scene/Scene.h"
#include <vector>


class Entity;
class Engine;
class Camera;
class Projectile;
class Renderer;

class SandboxScene : public Scene
{
public:
	~SandboxScene() = default;
	void Initialize(ECS* ecs, Renderer* renderer, Engine* engine, Entity* camera, SceneManager* sceneManager, UIFrame* frame) override;
	void OnEnter() override;
	void OnExit() override;
	void Update(float dt) override;

private:
	float m_deltaTime = 0; // deltaTime

	std::vector<Entity*> m_entities = {};

	Entity* m_cube = nullptr;
	Entity* m_sphere = nullptr;
	Entity* m_cylinder = nullptr;
	Entity* m_moon = nullptr;
	Entity* m_moonMoon = nullptr;
	Entity* m_floor = nullptr;

	Entity* m_rail = nullptr;
	std::vector<Entity*> m_rails = {};

	Entity* m_body = nullptr;
	Entity* m_cam = nullptr;

	float m_maxRails = 5;

	float m_angleOrbit = 0;

	float m_speedPlayer = 1.f;

	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
	bool m_locked = true;

	Projectile* m_projectile = nullptr;

	//TEST ENTITIES
	Entity* CreateSphere();
	Entity* CreateCylinder();
	Entity* CreateCube();
	Entity* CreateMoon();
	Entity* CreateMoonMoon();

	//LOOK WITH MOUSE + WASD/ZQSD
	void MoveCamera();
	void Debug();
	void HandleCursor();
	void UpdateCameraTransform();

	//SHOOTING
	void Shooting();
};

