#pragma once
#include "Engine/ECS/Component.h"
class EnemyComponent : public Component
{
public:

	void SetID(int id) { m_id = id; }
	int GetID() const { return m_id; }
	
	void SetScoreValue(int scoreValue) { m_scoreValue = scoreValue; }
	int GetScoreValue() const { return m_scoreValue; }
private:
	int m_id = 0;;
	int m_scoreValue = 0;
};

