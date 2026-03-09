#pragma once
#include <DirectXMath.h>
#include <vector>
#include "Engine/ECS/Entity.h"
#include "Engine/ResourceManager.h"

using namespace DirectX;

class ECS;
class Renderer;

class Tiles
{
public:
	virtual void Initialize(ECS* ecs, Renderer* renderer) = 0;
	void SetActive(bool active);
	void SetActiveEnemies(bool active);
	bool IsActive() const { return m_active; }

	void SetIsTurnR(bool isTurnR) { m_isTurnR = isTurnR; }
	bool IsTurnR() const { return m_isTurnR; }

	void SetIsTurnL(bool isTurnL) { m_isTurnL = isTurnL; }
	bool IsTurnL() const { return m_isTurnL; }

	void SetPosition(const XMFLOAT3& position) { m_position = position; }
	XMFLOAT3 GetPosition() const { return m_position; }

	void SetIn(int in) { m_In = in; }
	int GetIn() const { return m_In; }

	void SetOut(int out) { m_Out = out; }
	int GetOut() const { return m_Out; }

	std::vector<Entity*>& GetEntities() { return m_entities; }
	std::vector<Entity*>& GetRails() { return m_rails; }

	void SetSpeed(float speed) { m_speed = speed; }
	float GetSpeed() const { return m_speed; }

	virtual void Update(float deltaTime) = 0;

	void MoveZ(float dist);

	std::vector<XMFLOAT3>& GetLocalOffset() { return m_localOffset; }

	void CreateArc(ECS* ecs, Renderer* renderer, XMFLOAT3 center, float radius, float startAngle, float endAngle, int segments);

	void SpawnEnemy();

	void SetScoreValue(int scoreValue) { m_scoreValue = scoreValue; }
	int GetScoreValue() const { return m_scoreValue; }

	void CreateTileEntity(ECS* ecs, Renderer* renderer, XMFLOAT3 offset, XMFLOAT3 scale, float Yrotation);
	void CreateEnemyEntity(ECS* ecs, Renderer* renderer, XMFLOAT3 offset, XMFLOAT3 scale, int ID);
	void CreateWallEntity(ECS* ecs, Renderer* renderer, XMFLOAT3 offset, XMFLOAT3 scale);

	Entity* m_activeEnemy = nullptr;
	Entity* m_obstacle = nullptr;

	TextureHandle m_railTexture = ResourceManager::Instance().LoadTexture(L"../../res/RailTex.png");
	TextureHandle m_targetHandle = ResourceManager::Instance().LoadTexture(L"../../res/TargetTex.png");
	TextureHandle m_wallTexture = ResourceManager::Instance().LoadTexture(L"../../res/briqueTex.png");

private:
	bool m_active = false;
	bool m_isTurnR = false;
	bool m_isTurnL = false;
	
	XMFLOAT3 m_position = {};
	int m_In = 0;
	int m_Out = 0;
	
	float m_speed = 0.0f;

	int m_scoreValue = 0;
	


	std::vector<Entity*> m_entities = {};
	std::vector<Entity*> m_rails = {};
	std::vector<XMFLOAT3> m_localOffset = {};
};

