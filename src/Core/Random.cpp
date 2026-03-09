#include "Random.h"

int Random::RandomInt(int min, int max)
{
	return min + rand() % (max - min + 1);
}

float Random::RandomFloat(float min, float max)
{
	return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

float Random::Chance(float percentage)
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX) < (percentage / 100.0f);
}