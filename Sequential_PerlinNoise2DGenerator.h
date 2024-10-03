#pragma once
#include <vector>
#include "PerlinNoise2DGenerator.h"

class Sequential_PerlinNoise2DGenerator
{
public:

	Sequential_PerlinNoise2DGenerator(int widthX, int heightY, int frequency, int seedIn = 0, bool seamlessVertically = false, bool seamlessHorizontally = false, float minThreshold = -1.0, float maxThreshold = 1.0);

	inline int getSeed() { return seed; }

	float getValueAtPoint(int x, int y);

	// TODO: Create getWidth() and getHeight()
	int Width;
	int Height;

private:

	// All possible vectors for basic noise (1.4142 can be added as well)
	//const Point possibleVectors[8]{ {1,1}, {-1.0f,1.0f}, {1.0f,-1.0f}, {-1.0f,-1.0f}, {std::sqrt(2),0}, {0,std::sqrt(2)}, {-std::sqrt(2),0}, {0,-std::sqrt(2)} };

	// Array with basic noise grid
	std::vector<std::vector<Point>> initial2DNoiseGrid;

	int seed; // The seed is used to initialize the state of the generator and determines the sequence of random numbers it produces.
	int f; // Noise grid frequency

	std::vector<std::vector<float>> noise2DArray;

	std::vector<std::vector<int>> blackAndWhiteNoise2DArray;

	float fadeFunction(float pos);

	float dotProduct(Point A, Point B);

	float thresholdsSetup(float value, float minThreshold, float maxThreshold);

	float fadeLerp(float fract, float A, float B);
};