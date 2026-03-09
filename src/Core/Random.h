#pragma once
#include <random>

class Random
{
public:
	static int RandomInt(int min, int max);
	static float RandomFloat(float min, float max);
	static float Chance(float percentage);
};

