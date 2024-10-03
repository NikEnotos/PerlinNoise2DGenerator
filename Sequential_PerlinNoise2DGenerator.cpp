
#include <iostream>
#include <random> // For std::random_device and std::mt19937
#include "Sequential_PerlinNoise2DGenerator.h"


Sequential_PerlinNoise2DGenerator::Sequential_PerlinNoise2DGenerator(int widthX, int heightY, int frequency, int seedIn, bool seamlessVertically, bool seamlessHorizontally, float minThreshold, float maxThreshold)
{
	f = frequency;

	// Recalculate width and height of array
	int newWidthX = (widthX - 1) % (f + 1) == 0 ? widthX : widthX + f + 1 - ((widthX - 1) % (f + 1));  // made with the help of HelgSugarcube
	int newHeightY = (heightY - 1) % (f + 1) == 0 ? heightY : heightY + f + 1 - ((heightY - 1) % (f + 1));  // made with the help of HelgSugarcube    

	// Creating array with recalculated width and height 
	noise2DArray.resize(newWidthX, std::vector<float>(newHeightY));

	// Save current width and height to variables
	Width = newWidthX;
	Height = newHeightY;

	// Setting seed in the object
	if (seedIn == 0)
	{
		std::random_device rd;
		seed = rd();
	}
	else seed = seedIn;

	// All possible vectors for basic noise
	Point possibleVectors[]{ {1.0,1.0}, {-1.0,1.0}, {1.0,-1.0}, {-1.0,-1.0}, {std::sqrt(2),0}, {0,std::sqrt(2)}, {-std::sqrt(2),0}, {0,-std::sqrt(2)} };  //1.4142

	// Initialization of array with basic noise grid
	std::vector<std::vector<Point>> initial2DNoiseGrid((newWidthX / (f + 1)) + 2, std::vector<Point>((newHeightY / (f + 1)) + 2));
	for (int y = 0; y < (newHeightY / (f + 1)) + 2; ++y)
	{
		for (int x = 0; x < (newWidthX / (f + 1)) + 2; ++x)
		{
			// Random selection of Point valuse based on seed value
			std::mt19937 firstRandomization(seed * (x + 1) + y);
			std::mt19937 secondRandomization(firstRandomization() * (y + 1) + x);
			std::mt19937 randChoise(secondRandomization());
			initial2DNoiseGrid[x][y] = possibleVectors[randChoise() % std::size(possibleVectors)];
		}
	}

	// Make the noise seamless along Y axis
	if (seamlessVertically)
	{
		for (int i = 0; i <= (newHeightY / (f + 1)); ++i)
		{
			initial2DNoiseGrid[(newWidthX / (f + 1))][i] = initial2DNoiseGrid[0][i];
		}
	}

	// Make the noise seamless along X axis
	if (seamlessHorizontally)
	{
		for (int i = 0; i <= (newWidthX / (f + 1)); ++i)
		{
			initial2DNoiseGrid[i][(newHeightY / (f + 1))] = initial2DNoiseGrid[i][0];
		}
	}

	// Generate Perlin noise value for each element of noise2DArray
	for (int y = 0; y < newHeightY; ++y) //noise2DArray[0].size()
	{
		for (int x = 0; x < newWidthX; ++x)
		{
			// Creating 4 vectors from 4 nearest base noise dots to pixel position
			Point vectorLT;
			vectorLT.x = (x % (f + 1)) * (1 / float(f + 1));
			vectorLT.y = (y % (f + 1)) * (1 / float(f + 1));

			Point vectorRT;
			vectorRT.x = ((x % (f + 1)) - f - 1) * (1 / float(f + 1));
			vectorRT.y = (y % (f + 1)) * (1 / float(f + 1));

			Point vectorLB;
			vectorLB.x = (x % (f + 1)) * (1 / float(f + 1));
			vectorLB.y = ((y % (f + 1)) - f - 1) * (1 / float(f + 1));

			Point vectorRB;
			vectorRB.x = ((x % (f + 1)) - f - 1) * (1 / float(f + 1));
			vectorRB.y = ((y % (f + 1)) - f - 1) * (1 / float(f + 1));

			// Doing dot product of 4 nearest base noise dots and previous 4 vectors
			float LTDp = dotProduct(vectorLT, initial2DNoiseGrid[int(x / (f + 1))][int(y / (f + 1))]);
			float RTDp = dotProduct(vectorRT, initial2DNoiseGrid[int(x / (f + 1)) + 1][int(y / (f + 1))]);
			float LBDp = dotProduct(vectorLB, initial2DNoiseGrid[int(x / (f + 1))][int(y / (f + 1)) + 1]);
			float RBDp = dotProduct(vectorRB, initial2DNoiseGrid[int(x / (f + 1)) + 1][int(y / (f + 1)) + 1]);


			// Finding interpolation with fade for "top" two dot products
			float UpLerp = fadeLerp((x % (f + 1)) * (1 / float(f + 1)), LTDp, RTDp);
			// Finding interpolation with fade for "bottom" two dot products
			float DownLerp = fadeLerp((x % (f + 1)) * (1 / float(f + 1)), LBDp, RBDp);
			// Finding result value for Perling noise in position x y
			float finalLerp = fadeLerp((y % (f + 1)) * (1 / float(f + 1)), UpLerp, DownLerp);

			//finalLerp = cosLerp(finalLerp, -1.0, 1.0); // for islands map

			// Set up thresholds for result value
			finalLerp = thresholdsSetup(finalLerp, minThreshold, maxThreshold);
			// Set value of Perling noise in position to array
			noise2DArray[x][y] = finalLerp;

		}
	}

	// Rescale ouptut array to propper size relying on the "seamlessness" requirements
	if (!seamlessVertically)
	{
		noise2DArray.resize(widthX);
		Width = widthX;
	}
	if (!seamlessHorizontally)
	{
		noise2DArray.resize(Width, std::vector<float>(heightY));
		Height = heightY;
	}
}

float Sequential_PerlinNoise2DGenerator::dotProduct(Point A, Point B)
{
	return (A.x * B.x) + (A.y * B.y);
}

float Sequential_PerlinNoise2DGenerator::fadeLerp(float fract, float A, float B)
{
	return A + fadeFunction(fract) * (B - A);
}

float Sequential_PerlinNoise2DGenerator::fadeFunction(float pos)
{
	if (pos < 0 || pos > 1)
	{
		std::cout << "ERROR: pos = " << pos << ". It MUST be 0 < pos < 1!";
		return -1.0;
	}

	return 6 * std::pow(pos, 5) - 15 * std::pow(pos, 4) + 10 * std::pow(pos, 3);
}

float Sequential_PerlinNoise2DGenerator::thresholdsSetup(float value, float minThreshold, float maxThreshold)
{
	return minThreshold + (value + 1.0f) * 0.5f * (maxThreshold - minThreshold);
}

float Sequential_PerlinNoise2DGenerator::getValueAtPoint(int x, int y)
{
	return noise2DArray[x][y];
}
