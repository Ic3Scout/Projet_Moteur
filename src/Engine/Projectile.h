#pragma once
#include <vector>
#include <DirectXMath.h>
#include "Engine/Utils/Shape.h"

using namespace DirectX;

class Entity;
class Renderer;
class ECS;
class Engine;

class Projectile
{
public:
	~Projectile() = default;

	void Initialize(float size, Shape type, int poolSize,float distanceMax,Engine* engine, ECS* ecs, Renderer* renderer);

	Entity* GetAvaibleProjectile();
	void CreatePulling(int poolSize);

	void UpdateDistance(float dt);
	void Update(float dt);

protected:
	std::vector<Entity*> m_projectiles = {};

	float m_size = 0.0f;
	Shape m_shape = Shape::SPHERE;
	ECS* m_ecs = nullptr;
	Renderer* m_renderer = nullptr;
	Engine* m_engine = nullptr;

	float m_maxDistance = 50.0f;
};

