#pragma once
#include <vector>

struct Point
{
	float x;
	float y;
};

class PerlinNoise2DGenerator
{
public: 

	PerlinNoise2DGenerator(int widthX, int heightY, int friquency, int seedIn = 0, bool useCosLerp = false, float minThreshold = -1.0, float maxThreshold = 1.0);

	inline int getSeed() { return seed; }

	float getValueAtPoint(int x, int y);

private:

	int seed; // The seed is used to initialize the state of the generator and determines the sequence of random numbers it produces.
	int f; // Friquency

	std::vector<std::vector<float>> noise2DArray;

	float fadeFunction(float pos);

	float dotProduct(Point A, Point B);

	float fadeLerp(float fract, float A, float B);
	float cosLerp(float fract, float A, float B, float minResult = -1.0, float maxResult = 1.0);
};