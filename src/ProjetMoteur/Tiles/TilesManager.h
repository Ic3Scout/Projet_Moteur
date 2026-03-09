#pragma once
#include <vector>

class Tiles;
class ECS;
class Renderer;

class TilesManager
{
public:
	static TilesManager& Instance();

	void Initialize(ECS* ecs, Renderer* renderer);

	void AddTile(Tiles* tile);

	void Update(float deltaTime);

	void SpawnTile();
	
	void OnEnter();
	void OnExit();

	std::vector<Tiles*> GetActiveTiles();

	void SetScoreValue(int scoreValue) { m_scoreValue = scoreValue; }
	int GetScoreValue() const { return m_scoreValue; }
	float GetSpeed() const { return m_speed; };
private:

	float m_baseSpeed = 5.0f;
	float m_speedPer100 = 0.2f;
	float m_speed = 0.0f;

	std::vector<Tiles*> m_tiles;
	std::vector<Tiles*> m_activeTiles;

	int m_currentConnection = 0;
	int m_scoreValue = 0;
};

