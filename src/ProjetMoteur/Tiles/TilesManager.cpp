#include "TilesManager.h"
#include "Tiles.h"
#include "Engine/ECS/ECS.h"
#include "Render/Renderer.h"
#include "Tiles/Tile1.h"
#include "Tiles/Tile2.h"
#include "Tiles/Tile3.h"
#include "Tiles/Tile4.h"
#include "Tiles/Tile5.h"
#include "Tiles/Tile6.h"
#include "Core/Random.h"
#include <iostream>

static TilesManager* s_instance = nullptr;

TilesManager& TilesManager::Instance()
{
	if (s_instance == nullptr)
		s_instance = new TilesManager();
	return *s_instance;
}

void TilesManager::Initialize(ECS* ecs, Renderer* renderer)
{
	m_currentConnection = 0;

	Tile1* tile1 = new Tile1();
	tile1->Initialize(ecs, renderer);
	AddTile(tile1);

	Tile2* tile2 = new Tile2();
	tile2->Initialize(ecs, renderer);
	AddTile(tile2);

	Tile3* tile3 = new Tile3();
	tile3->Initialize(ecs, renderer);
	AddTile(tile3);

    Tile4* tile4 = new Tile4();
    tile4->Initialize(ecs, renderer);
    AddTile(tile4);

	Tile5* tile5 = new Tile5();
	tile5->Initialize(ecs, renderer);
	AddTile(tile5);

	Tile6* tile6 = new Tile6();
	tile6->Initialize(ecs, renderer);
	AddTile(tile6);
}

void TilesManager::AddTile(Tiles* tile)
{
	m_tiles.push_back(tile);
}

void TilesManager::Update(float deltaTime)
{
	m_speed = m_baseSpeed + (GetScoreValue() / 100) * m_speedPer100;
    for (int i = 0; i < m_activeTiles.size(); )
    {
        Tiles* tile = m_activeTiles[i];
		tile->SetSpeed(m_speed);
        SetScoreValue(GetScoreValue() + tile->GetScoreValue());
        tile->SetScoreValue(0);

        if (tile->GetPosition().z + 20.0f < 0.0f)
        {
            tile->SetActive(false);
            tile->SetActiveEnemies(false);

            m_activeTiles.erase(m_activeTiles.begin() + i);
            SpawnTile();
			tile->SpawnEnemy();
        }
        else
        {
            tile->Update(deltaTime);
            i++;
        }
    }
}

void TilesManager::SpawnTile()
{
    std::vector<Tiles*> compatibles;

    for (Tiles* tile : m_tiles)
    {
        if (tile->GetIn() == m_currentConnection && !tile->IsActive())
        {
            compatibles.push_back(tile);
        }
    }

    if (compatibles.empty())
        return;

    int index = Random::RandomInt(0, static_cast<int>(compatibles.size()) - 1);
    Tiles* chosen = compatibles[index];

    chosen->SetActive(true);

    float newZ;

    if (m_activeTiles.size() >= 1)
    {
        newZ = m_activeTiles.back()->GetPosition().z;
        newZ += 18.0f;
    }
    else
    {
		newZ = 0.0f;
    }

    chosen->SetPosition({ 0,0,newZ });

    m_currentConnection = chosen->GetOut();

    m_activeTiles.push_back(chosen);
}

void TilesManager::OnEnter()
{
    m_currentConnection = 0;
    m_activeTiles.clear();

    for (Tiles* tile : m_tiles)
    {
        tile->SetActive(false);
    }

    for (int i = 0; i < 3; i++)
    {
        SpawnTile();
    }
}

void TilesManager::OnExit()
{
    for (Tiles* tile : m_tiles)
    {
        tile->SetActive(false);
        tile->SetActiveEnemies(false);
    }
    m_activeTiles.clear();
}

std::vector<Tiles*> TilesManager::GetActiveTiles() {
    return m_activeTiles;
}