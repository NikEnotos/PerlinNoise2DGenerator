
#include <iostream>
#include <random> // For std::random_device and std::mt19937
#include "PerlinNoise2DGenerator.h"


PerlinNoise2DGenerator::PerlinNoise2DGenerator(int widthX, int heightY, int frequency, int seedIn, bool seamlessVertically, bool seamlessHorizontally, float minThreshold, float maxThreshold)
{
    freq = frequency;

    // Recalculate width and height of array
    int newWidthX = (widthX - 1) % (freq + 1) == 0 ? widthX : widthX + freq + 1 - ((widthX - 1) % (freq + 1));  // made with the help of HelgSugarcube
    int newHeightY = (heightY - 1) % (freq + 1) == 0 ? heightY : heightY + freq + 1 - ((heightY - 1) % (freq + 1));  // made with the help of HelgSugarcube    

    // Creating array with recalculated width and height 
    noise2DArray.resize(newWidthX, std::vector<float>(newHeightY));
    blackAndWhiteNoise2DArray.resize(newWidthX, std::vector<int>(newHeightY));

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

    this->seamlessVertically = seamlessVertically;
    this->seamlessHorizontally = seamlessHorizontally;

    this->minThreshold = minThreshold;
    this->maxThreshold = maxThreshold;

    // Resizing and setting initial2dNoiseGrid
    setInitial2DNoiseGrid();


    // Get the number of CPU cores for the number of threads
    numCPUs = std::thread::hardware_concurrency();

    //DEBUG
    numCPUs = 8;
    numOfCalculationThreads = 6;
    int numOfTaskParts = numOfCalculationThreads * 10;

    // Setting range for a task and set amount to prepare
    // Finding "greatest common divisor" for Width, Height and number of CPUs
    //praparationRange = std::_Gcd(Width, Height);
    //praparationRange = std::_Gcd(praparationRange, numCPUs);

    //praparationRange = Width >= Height ? std::ceil( Width / numCPUs) : std::ceil(Height / numCPUs);
    //leftForPreparation = Width >= Height ? Width : Height;

    // TODO: find a better way to separate the task
    calculationRange = std::ceil(float(Width) / numOfTaskParts);
    leftForCalculatio = Width;

    // DEBUG
    std::cout << "number of CPUs = " << numCPUs << std::endl;
    std::cout << "number of threads for grid preparation = " << numOfCalculationThreads << std::endl;
    std::cout << "number of threads for grid convertion = " << numCPUs - numOfCalculationThreads << std::endl;
    std::cout << "the entire task is divided into: " << numOfTaskParts << " parts " << std::endl;

    // Creating Ptoducers and Customers threads
    std::vector<std::thread> gridCalculationThreads(numOfCalculationThreads);
    std::vector<std::thread> gridConvertionThreads(numCPUs - numOfCalculationThreads);

    for (int i = 0; i < gridCalculationThreads.size(); ++i)
    {
        // Pass function to creating thread as member of this class 
        gridCalculationThreads[i] = std::thread(&PerlinNoise2DGenerator::gridCalculation, this);
    }
    for (int i = 0; i < gridConvertionThreads.size(); ++i)
    {
        // Pass function to creating thread as member of this class 
        gridConvertionThreads[i] = std::thread(&PerlinNoise2DGenerator::gridConvertion, this);
    }

    // join THREADS
    for (auto& thread : gridCalculationThreads)
    {
        thread.join();
    }
    for (auto& thread : gridConvertionThreads)
    {
        thread.join();
    }
   

    // Rescale ouptut array to propper size relying on the "seamlessness" requirements
    if (!seamlessVertically)
    {
        noise2DArray.resize(widthX);
        blackAndWhiteNoise2DArray.resize(widthX);
        Width = widthX;
    }
    if (!seamlessHorizontally)
    {
        noise2DArray.resize(Width, std::vector<float>(heightY));
        blackAndWhiteNoise2DArray.resize(Width, std::vector<int>(heightY));
        Height = heightY;
    }
}




void PerlinNoise2DGenerator::gridCalculation()
{
    // Set variables of the range of preparation
    int start, end;

    while ( true /* END OF THE RANGE !AND! NOT OUT OF ARRAY */)
    {
        
        // Define preparation range
        {
            std::unique_lock<std::mutex> lock_preparationProgress(preparationProgress_mutex);

            if (leftForCalculatio > 0)
            {
                end = leftForCalculatio;
                start = leftForCalculatio = (leftForCalculatio - calculationRange) >= 0 ? leftForCalculatio - calculationRange : 0;
            }
            else
            {
                std::unique_lock<std::mutex> lock_finishedThreadsNum(finishedThreadsNum_mutex);
                ++finishedThreadsNum;

                std::cout << "[+] gridCalculation: finishedThreadsNum = " << finishedThreadsNum << std::endl;

                return;
            }
        }
        

        
        // Generate Perlin noise value for each element of noise2DArray
        for (int y = 0; y < Height; ++y)
        {
            //for (int y = 0; y < Height; ++y)
            for (int x = start; x < end; ++x)
            {
                // Creating 4 vectors from 4 nearest base noise dots to pixel position
                Point vectorLT;
                vectorLT.x = (x % (freq + 1)) * (1 / float(freq + 1));
                vectorLT.y = (y % (freq + 1)) * (1 / float(freq + 1));

                Point vectorRT;
                vectorRT.x = ((x % (freq + 1)) - freq - 1) * (1 / float(freq + 1));
                vectorRT.y = (y % (freq + 1)) * (1 / float(freq + 1));

                Point vectorLB;
                vectorLB.x = (x % (freq + 1)) * (1 / float(freq + 1));
                vectorLB.y = ((y % (freq + 1)) - freq - 1) * (1 / float(freq + 1));

                Point vectorRB;
                vectorRB.x = ((x % (freq + 1)) - freq - 1) * (1 / float(freq + 1));
                vectorRB.y = ((y % (freq + 1)) - freq - 1) * (1 / float(freq + 1));

                // Doing dot product of 4 nearest base noise dots and previous 4 vectors
                float LTDp = dotProduct(vectorLT, initial2DNoiseGrid[int(x / (freq + 1))][int(y / (freq + 1))]);
                float RTDp = dotProduct(vectorRT, initial2DNoiseGrid[int(x / (freq + 1)) + 1][int(y / (freq + 1))]);
                float LBDp = dotProduct(vectorLB, initial2DNoiseGrid[int(x / (freq + 1))][int(y / (freq + 1)) + 1]);
                float RBDp = dotProduct(vectorRB, initial2DNoiseGrid[int(x / (freq + 1)) + 1][int(y / (freq + 1)) + 1]);


                // Finding interpolation with fade for "top" two dot products  
                float UpLerp = fadeLerp((x % (freq + 1)) * (1 / float(freq + 1)), LTDp, RTDp);
                // Finding interpolation with fade for "bottom" two dot products     
                float DownLerp = fadeLerp((x % (freq + 1)) * (1 / float(freq + 1)), LBDp, RBDp);
                // Finding result value for Perling noise in position x y  
                float finalLerp = fadeLerp((y % (freq + 1)) * (1 / float(freq + 1)), UpLerp, DownLerp);
                

                // Set up thresholds for result value
                finalLerp = thresholdsSetup(finalLerp, minThreshold, maxThreshold);

                // LOCK noise2DArray
                {
                    std::unique_lock<std::mutex> lock_noise2DArray(noise2DArray_mutex);
                    // Set value of Perling noise in the position to array
                    noise2DArray[x][y] = finalLerp;
                }
            }
        }

        // NOTIFY "Customer" thread that chunk is ready
        {
            std::unique_lock<std::mutex> lock_tasksQueue(tasksQueue_mutex);
            tasksQueue.push(end);
            available_task.notify_one();
        }

    } 
}

void PerlinNoise2DGenerator::gridConvertion()
{

    while (true /* !tasksQueue.empty() && threadsFinished == ThreadsNum */)
    {
        int currentTasksEnd;

        tasksQueue_mutex.lock();
        if (!tasksQueue.empty())
        {
            currentTasksEnd = tasksQueue.front();
            tasksQueue.pop();

            tasksQueue_mutex.unlock();
        }
        else
        {
            tasksQueue_mutex.unlock();

            // Checking for any preparations in progress
            {
                std::unique_lock<std::mutex> lock_finishedThreadsNum(finishedThreadsNum_mutex);

                if (finishedThreadsNum < numOfCalculationThreads)
                {
                    std::cout << "[!] gridConvertion WAITING " << std::endl;

                    available_task.wait(lock_finishedThreadsNum, [&] {return !tasksQueue.empty() || finishedThreadsNum == numOfCalculationThreads; });

                    std::cout << "[*] gridConvertion CONTINUE " << std::endl;

                    continue;
                }
                else
                {
                    std::cout << "[+] gridConvertion " << std::endl;
                    return;
                }
            }
        }


        int currentTasksStart = (currentTasksEnd - calculationRange) >= 0 ? currentTasksEnd - calculationRange : 0;

        // Convert the values to black and white and set pixels in the image
        for (int y = 0; y < Height; ++y) {
            for (int x = currentTasksStart; x < currentTasksEnd; ++x) {

                // It is safe to not mute noise2DArray for time of reading
                float value = noise2DArray[x][y];

                // Map the value from [minValue, maxValue] to [0, 255]
                int grayValue = static_cast<int>((value - minThreshold) / (maxThreshold - minThreshold) * 255);

                // LOCK blackAndWhiteNoise2DArray
                {
                    std::unique_lock<std::mutex> lock_blackAndWhiteNoise2DArray(blackAndWhiteNoise2DArray_mutex);
                    // Set value of Perling noise in the position to array
                    blackAndWhiteNoise2DArray[x][y] = grayValue;
                }

            }
        }

    }
}


void PerlinNoise2DGenerator::setInitial2DNoiseGrid()
{
    // All possible vectors for basic noise (1.4142 can be added as well)
    std::vector<Point> possibleVectors { Point{1,1}, Point{-1.0f,1.0f}, Point{1.0f,-1.0f}, Point{-1.0f,-1.0f}, Point{std::sqrt(2.0f),0}, Point{0,std::sqrt(2.0f)}, Point{-std::sqrt(2.0f),0}, Point{0,-std::sqrt(2.0f)} };

    // Set size for basic noise grid array 
    initial2DNoiseGrid.resize((Width / (freq + 1)) + 2, std::vector<Point>((Height / (freq + 1)) + 2));

    // Filling in freq grid with random values from possibleVectors
    for (int y = 0; y < (Height / (freq + 1)) + 2; ++y)
    {
        for (int x = 0; x < (Width / (freq + 1)) + 2; ++x)
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
        for (int i = 0; i <= (Height / (freq + 1)); ++i)
        {
            initial2DNoiseGrid[(Width / (freq + 1))][i] = initial2DNoiseGrid[0][i];
        }
    }
    // Make the noise seamless along X axis
    if (seamlessHorizontally)
    {
        for (int i = 0; i <= (Width / (freq + 1)); ++i)
        {
            initial2DNoiseGrid[i][(Height / (freq + 1))] = initial2DNoiseGrid[i][0];
        }
    }
}

float PerlinNoise2DGenerator::dotProduct(Point A, Point B)
{
    return (A.x * B.x) + (A.y * B.y);
}

float PerlinNoise2DGenerator::fadeLerp(float fract, float A, float B)
{
    return A + fadeFunction(fract) * (B - A);
}

float PerlinNoise2DGenerator::cosLerp(float fract, float A, float B)
{
    float ft = fract * 3.1415927f;
    float weightFactor = (1.0f - std::cos(ft)) * 0.5f;
    float result = A * (1.0f - weightFactor) + B * weightFactor;
    return result;
}

float PerlinNoise2DGenerator::fadeFunction(float pos)
{
    if (pos < 0 || pos > 1)
    {
        std::cout << "ERROR: pos = " << pos << ". It MUST be 0 < pos < 1!";
        return -1.0;
    }

    return 6 * std::pow(pos, 5) - 15 * std::pow(pos, 4) + 10 * std::pow(pos, 3);
}

float PerlinNoise2DGenerator::thresholdsSetup(float value, float minThreshold, float maxThreshold)
{
    return minThreshold + (value + 1.0f) * 0.5f * (maxThreshold - minThreshold);
}

float PerlinNoise2DGenerator::getValueAtPoint(int x, int y)
{
    return noise2DArray[x][y];
}

int PerlinNoise2DGenerator::getBlackAndWhiteValueAtPoint(int x, int y)
{
    return blackAndWhiteNoise2DArray[x][y];
}



// OLD VERSION
/* PerlinNoise2DGenerator::PerlinNoise2DGenerator(int widthX, int heightY, int frequency, int seedIn, bool useCosLerp, float minThreshold, float maxThreshold)
{
    std::vector<float> row(widthX, 0);
    std::vector<std::vector<float>> tempArray(heightY, row);
    noise2DArray = tempArray; // Creating of resulted noise array

    f = frequency;

    // Setting seed in the object
    if (seedIn == 0)
    {
        std::random_device rd;
        seed = rd();
    }
    else seed = seedIn;


    const int gridStepX = widthX / f;
    const int gridStepY = heightY / f;

    // All possible vectors for basic noise
    Point possibleVectors[]{ {1.0,1.0}, {-1.0,1.0}, {1.0,-1.0}, {-1.0,-1.0}, {std::sqrt(2),0}, {0,std::sqrt(2)}, {-std::sqrt(2),0}, {0,-std::sqrt(2)} };  //1.4142

    // Initialization of array with basic noise grid
    std::vector<std::vector<Point>> initial2DNoiseGrid(gridStepY + 1, std::vector<Point>(gridStepX + 1));
    for (int y = 0; y < gridStepY + 1; y++) //initial2DNoiseGrid[0].size()
    {
        for (int x = 0; x < gridStepX + 1; x++)
        {
            std::mt19937 firstRandomization(seed * (x + 1) + y);
            std::mt19937 secondRandomization(firstRandomization() * (y + 1) + x);
            std::mt19937 randChoise(secondRandomization());
            initial2DNoiseGrid[x][y] = possibleVectors[randChoise() % std::size(possibleVectors)];


            // DEBUG
            //std::cout << initial2DNoiseGrid[x][y].x << " " << initial2DNoiseGrid[x][y].y << "\t";
            //if (x == initial2DNoiseGrid.size() - 1) {std::cout << " initial2DNoiseGrid.size() = " << initial2DNoiseGrid.size()<< " initial2DNoiseGrid[0].size() = " << initial2DNoiseGrid[0].size(); std::cout << "\n"; }
        }
    }

    // Generate Perlin noise value for each element of noise2DArray
    for (int y = 0; y < heightY; y++) //noise2DArray[0].size()
    {
        for (int x = 0; x < widthX; x++)
        {
            // Creating 4 vectors from 4 nearest base noise dots to pixel position
            //Point vectorLT;
            //vectorLT.x = (x % f) * float(1 / float(f - 1)); //if (x % f < f / 2) vectorLT.x += 0.01; else vectorLT.x -= 0.01;
            //vectorLT.y = (y % f) * float(1 / float(f - 1)); //if (y % f < f / 2) vectorLT.y += 0.01; else vectorLT.y -= 0.01;

            //Point vectorRT;
            //vectorRT.x = ((x % f) - f + 1) * float(1 / float(f - 1)); //if (x % f < f / 2) vectorRT.x += 0.01; else vectorRT.x -= 0.01;
            //vectorRT.y = (y % f) * float(1 / float(f - 1)); //if (y % f < f / 2) vectorRT.y += 0.01; else vectorRT.y -= 0.01;

            //Point vectorLB;
            //vectorLB.x = (x % f) * float(1 / float(f - 1)); //if (x % f < f / 2) vectorLB.x += 0.01; else vectorLB.x -= 0.01;
            //vectorLB.y = ((y % f) - f + 1) * float(1 / float(f - 1)); //if (y % f < f / 2) vectorLB.y += 0.01; else vectorLB.y -= 0.01;

            //Point vectorRB;
            //vectorRB.x = ((x % f) - f + 1) * float(1 / float(f - 1)); //if (x % f < f / 2) vectorRB.x += 0.01; else vectorRB.x -= 0.01;
            //vectorRB.y = ((y % f) - f + 1) * float(1 / float(f - 1)); //if (y % f < f / 2) vectorRB.y += 0.01; else vectorRB.y -= 0.01;

            Point vectorLT;
            vectorLT.x = (x % f) * (1.0 / f);
            vectorLT.y = (y % f) * (1.0 / f); //std::cout << vectorLT.x << " " << vectorLT.y << "\t";

            Point vectorRT;
            vectorRT.x = ((x % f) - f + 1) * (1.0 / f);
            vectorRT.y = (y % f) * (1.0 / f); //std::cout << vectorRT.x << " " << vectorRT.y << "\t";

            Point vectorLB;
            vectorLB.x = (x % f) * (1.0 / f);
            vectorLB.y = ((y % f) - f + 1) * (1.0 / f); //std::cout << vectorLB.x << " " << vectorLB.y << "\t";

            Point vectorRB;
            vectorRB.x = ((x % f) - f + 1) * (1.0 / f);
            vectorRB.y = ((y % f) - f + 1) * (1.0 / f); //std::cout << vectorRB.x << " " << vectorRB.y << "\t";

            //std::cout << "\n";


            // Doing dot product of 4 nearest base noise dots and previous 4 vectors
            float LTDp = dotProduct(vectorLT, initial2DNoiseGrid[int(x / f)][int(y / f)]);
            float RTDp = dotProduct(vectorRT, initial2DNoiseGrid[int(x / f) + 1][int(y / f)]);
            float LBDp = dotProduct(vectorLB, initial2DNoiseGrid[int(x / f)][int(y / f) + 1]);
            float RBDp = dotProduct(vectorRB, initial2DNoiseGrid[int(x / f) + 1][int(y / f) + 1]);

            // Choose method of interpolation
            if (useCosLerp) {
                // Finding interpolation with fade for "top" two dot products  
                float UpLerp = cosLerp((x % f) * float(1 / float(f - 1)), LTDp, RTDp);
                // Finding interpolation with fade for "bottom" two dot products     
                float DownLerp = cosLerp((x % f) * float(1 / float(f - 1)), LBDp, RBDp);
                // Finding result value for Perling noise in position x y  
                float finalLerp = cosLerp((y % f) * float(1 / float(f - 1)), UpLerp, DownLerp, minThreshold, maxThreshold);

                // Set value of Perling noise in position to array
                //finalLerp = cosLerp(finalLerp, -1.0, 1.0, minThreshold, maxThreshold); // for islands map
                noise2DArray[x][y] = finalLerp;
            }
            else {
                // Finding interpolation with fade for "top" two dot products
                float UpLerp = fadeLerp((x % f) * float(1 / float(f - 1)), LTDp, RTDp);
                // Finding interpolation with fade for "bottom" two dot products
                float DownLerp = fadeLerp((x % f) * float(1 / float(f - 1)), LBDp, RBDp);
                // Finding result value for Perling noise in position x y
                float finalLerp = fadeLerp((y % f) * float(1 / float(f - 1)), UpLerp, DownLerp);

                // Set value of Perling noise in position to array
                //finalLerp = cosLerp(finalLerp, -1.0, 1.0, minThreshold, maxThreshold); // for islands map
                noise2DArray[x][y] = finalLerp;
            }
        }
    }
} */


