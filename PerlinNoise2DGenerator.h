#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>

struct Point
{
	float x;
	float y;
};



struct Task
{
	Task(int x, int y, float newLTDp, float newRTDp, float newLBDp, float newRBDp)
	{
		xPosotion = x;
		yPosition = y;
		LTDp = newLTDp;
		RTDp = newRTDp;
		LBDp = newLBDp;
		RBDp = newRBDp;
	}

	int xPosotion;
	int yPosition;
	float LTDp;
	float RTDp;
	float LBDp;
	float RBDp;
};



class PerlinNoise2DGenerator
{
public: 

	PerlinNoise2DGenerator(int widthX, int heightY, int frequency, int seedIn = 0, bool seamlessVertically = false, bool seamlessHorizontally = false, bool useCosLerp = false, float minThreshold = -1.0, float maxThreshold = 1.0);

	inline int getSeed() { return seed; }

	float getValueAtPoint(int x, int y);

	// TODO: Create getWidth() and getHeight()
	int Width;
	int Height;

private:

	// munex for progress of preparation
	std::mutex preparationProgress_mutex;
	// mutex for tasksQueue
	std::mutex tasksQueue_mutex;
	// mutex for finishedThreadsNum
	std::mutex finishedThreadsNum_mutex;
	// mutex for noise2DArray
	std::mutex noise2DArray_mutex;

	std::condition_variable available_task;


	// number of CPUs used for calculations
	int numCPUs;
	// value of the range deducated for a task
	int praparationRange;
	// value to trask progress of preparation
	int leftForPreparation;
	// queue of prepared blocks of tasks
	std::queue<Task> tasksQueue;
	// to control finished threads
	int finishedThreadsNum = 0;

	// DEBUG
	int numOfPreparationThreads;


	// All possible vectors for basic noise (1.4142 can be added as well)
	//const Point possibleVectors[8]{ {1,1}, {-1.0f,1.0f}, {1.0f,-1.0f}, {-1.0f,-1.0f}, {std::sqrt(2),0}, {0,std::sqrt(2)}, {-std::sqrt(2),0}, {0,-std::sqrt(2)} };

	// Array with basic noise grid
	std::vector<std::vector<Point>> initial2DNoiseGrid;


	int seed; // The seed is used to initialize the state of the generator and determines the sequence of random numbers it produces.
	int freq; // Noise grid frequency
	bool seamlessVertically;
	bool seamlessHorizontally;
	bool useCosLerp;
	float minThreshold;
	float maxThreshold;

	std::vector<std::vector<float>> noise2DArray;

	// Function for grid preparation using threading
	void gridPreparation();
	// Function for grid interpolation using threading 
	void GridInterpolation();


	float fadeFunction(float pos);

	float dotProduct(Point A, Point B);

	float thresholdsSetup(float value, float minThreshold, float maxThreshold);

	float fadeLerp(float fract, float A, float B);
	float cosLerp(float fract, float A, float B);

	void setInitial2DNoiseGrid();
};