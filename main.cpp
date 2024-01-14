#include <iostream>
#include "PerlinNoise2DGenerator.h"
#include "SFML/Graphics.hpp"

#include <random> 

 int main()
{
    int seed = 123456788;
    int friquency = 10;  //11
    const int width = 120;  //100
    const int height = 80;  //80
    PerlinNoise2DGenerator noise(width, height, friquency, seed, true);
    

    int friquency_Add = 6;
    PerlinNoise2DGenerator sand_noise(width, height, friquency_Add, seed / 2);
    PerlinNoise2DGenerator l1_noise(width, height, friquency_Add, seed / 2 +1);
    PerlinNoise2DGenerator l2_noise(width, height, friquency_Add, seed / 2 +2);
    PerlinNoise2DGenerator l3_noise(width, height, friquency_Add, seed / 2 +3);
    PerlinNoise2DGenerator l4_noise(width, height, friquency_Add, seed / 2 +4);


    const float minValue = -1.0f;
    const float maxValue = 1.0f;

    sf::RenderWindow window(sf::VideoMode(noise.Width, noise.Height), "Black and White Image");

    // Create an SFML image to store the pixel data
    sf::Image image;
    image.create(noise.Width, noise.Height);

    // Set up threshold for each type of tile 
    ////////////////////////////////////////////
    int mountainsLevel = 32 * 8;
    int groundLevel4 = 32 * 7;
    int groundLevel3 = 32 * 6;
    int groundLevel2 = 32 * 5; //110
    int groundLevel1 = 32 * 4; //100
    int sandLevel = 32 * 3;
    int shallowWaterLevel = 32 * 2;
    int deepWaterLevel = 32;
    ///////////////////////////////////////////

    // Convert the values to black and white and set pixels in the image
    for (int y = 0; y < noise.Height; ++y) {
        for (int x = 0; x < noise.Width; ++x) {
            float value = noise.getValueAtPoint(x, y);


            //float S_Value = sand_noise.getValueAtPoint(x, y);
            //float L1_Value = l1_noise.getValueAtPoint(x, y);
            //float L2_Value = l2_noise.getValueAtPoint(x, y);
            //float L3_Value = l3_noise.getValueAtPoint(x, y);
            //float L4_Value = l4_noise.getValueAtPoint(x, y);

            // Map the value from [minValue, maxValue] to [0, 255]
            int grayValue = static_cast<int>((value - minValue) / (maxValue - minValue) * 255);

            //int sValue = static_cast<int>((S_Value - minValue) / (maxValue - minValue) * 255);
            //int l1Value = static_cast<int>((L1_Value - minValue) / (maxValue - minValue) * 255);
            //int l2Value = static_cast<int>((L2_Value - minValue) / (maxValue - minValue) * 255);
            //int l3Value = static_cast<int>((L3_Value - minValue) / (maxValue - minValue) * 255);
            //int l4Value = static_cast<int>((L4_Value - minValue) / (maxValue - minValue) * 255);

            // Set pixel color
            if (grayValue < deepWaterLevel)
            {
                sf::Color pixelColor(17, 60, 130);
                image.setPixel(x, y, pixelColor);
            }
            else if (grayValue < shallowWaterLevel)
            {
                sf::Color pixelColor(100, 150, 232); 
                image.setPixel(x, y, pixelColor);
            }
            else if (grayValue < sandLevel)
            {
                sf::Color pixelColor(252, 248, 0); // default sand

                //if (sValue > 32 && sValue < 64 || sValue > 192 && sValue < 224) { pixelColor = sf::Color(224, 205, 0); } // sand hills 
                //if (sValue > 80 && sValue < 85 || sValue > 150 && sValue < 155) { pixelColor = sf::Color(245, 168, 59); } // sand stones 

                image.setPixel(x, y, pixelColor);
            }
            else if (grayValue < groundLevel1)
            {
                sf::Color pixelColor(22, 201, 70);

                //if (l1Value > 32 && l1Value < 64 || l1Value > 192 && l1Value < 224) { pixelColor = sf::Color(36, 191, 83); } // l1 hills 
                //if (l1Value > 80 && l1Value < 85 || l1Value > 150 && l1Value < 155) { pixelColor = sf::Color(245, 168, 59); } // sand stones 

                image.setPixel(x, y, pixelColor);
            }
            else if (grayValue < groundLevel2)
            {
                sf::Color pixelColor(17, 166, 57);

                //if (l2Value > 32 && l2Value < 64 || l2Value > 192 && l2Value < 224) { pixelColor = sf::Color(32, 171, 106); } // l2 forests 
                //if (l2Value > 80 && l2Value < 85 || l2Value > 150 && l2Value < 155) { pixelColor = sf::Color(28, 163, 64); } // l2 hills
                image.setPixel(x, y, pixelColor);
            }
            else if (grayValue < groundLevel3)
            {
                sf::Color pixelColor(15, 145, 50);

                //if (l2Value > 32 && l2Value < 64 || l2Value > 192 && l2Value < 224) { pixelColor = sf::Color(32, 171, 106); } // l2 forests continue on Level 3

                image.setPixel(x, y, pixelColor);
            }
            else if (grayValue < groundLevel4)
            {
                sf::Color pixelColor(9, 110, 36);
                image.setPixel(x, y, pixelColor);
            }
            else if (grayValue < mountainsLevel)
            {
                sf::Color pixelColor(174, 191, 179);
                image.setPixel(x, y, pixelColor);
            } 
            
            //sf::Color pixelColor(grayValue, grayValue, grayValue);
            //image.setPixel(x, y, pixelColor);

        }
    }

    /////////////////////////////////////////////////////  RIVERS LOGIC  /////////////////////////////////////////////////////
    int numOfRivers = 0;
    int riverSeed = 666;

    std::mt19937 randDir(riverSeed);
    
    for (int riverNum = 0; riverNum < numOfRivers; ++riverNum)
    {

        std::mt19937 rx(riverSeed + riverNum), ry(riverSeed + riverNum);
        int x = rx() % width;
        int y = ry() % height;
        while (!(noise.getValueAtPoint(x, y) <= -0.5))
        {
            ++x;
            if (x == width)
            {
                x = 0;
                ++y;
            }
            if (y == height)
            {
                x = 0;
                y = 0;
            }
        }

        image.setPixel(x, y, sf::Color(242, 7, 19));
        Point possibleLeftUpTiles[]{ {-1, -1}, {0, -1}, {-1, 0} };
        Point possibleLeftDownTiles[]{ {-1, 0}, {-1, 1}, {0, 1} };
        Point possibleRightUpTiles[]{ {0, -1}, {1, -1}, {1, 0} };
        Point possibleRightDownTiles[]{ {1, 0}, {0, 1}, {1, 1} };

        int riverLenght = 40;

        if (noise.getValueAtPoint(x + 1, y + 1) <= -0.5) // check RIGHT BOTTOM neighbour
        {
            for (int i = 0; i < riverLenght; ++i)
            {           
                int index = randDir() % 3;

                    std::cout << "iter:" << i << " x = " << x << " + " << possibleLeftUpTiles[index].x << " y = " << y << " + " << possibleLeftUpTiles[index].y << " grayValue = " << noise.getValueAtPoint(x, y) << "\n";

                    x = x + possibleLeftUpTiles[index].x; // TODO: CHECK OUT OF RANGE
                    y = y + possibleLeftUpTiles[index].y; // TODO: CHECK OUT OF RANGE

                    if (x + 1 >= width || y + 1 >= height || x <= 0 || y <= 0 || noise.getValueAtPoint(x, y) <= -0.5) { std::cout << "----BREAK----" << "\n"; break; }

                    image.setPixel(x, y, sf::Color(252, 3, 236));

                    //std::cout << "iter:" << i << " x = " << x << " y = " << y << "\n";
            }
        }

        else if (noise.getValueAtPoint(x + 1, y - 1) <= -0.5) // check RIGHT TOP neighbour
        {
            for (int i = 0; i < riverLenght; ++i)
            {
                int index = randDir() % 3;

                    std::cout << "iter:" << i << " x = " << x << " + " << possibleLeftDownTiles[index].x << " y = " << y << " + " << possibleLeftDownTiles[index].y << " grayValue = " << noise.getValueAtPoint(x, y) << "\n";

                    x = x + possibleLeftDownTiles[index].x; // TODO: CHECK OUT OF RANGE
                    y = y + possibleLeftDownTiles[index].y; // TODO: CHECK OUT OF RANGE

                    if (x + 1 >= width || y + 1 >= height || x <= 0 || y <= 0 || noise.getValueAtPoint(x, y) <= -0.5) { std::cout << "----BREAK----" << "\n"; break; }

                    image.setPixel(x, y, sf::Color(252, 3, 236));

                    //std::cout << "iter:" << i << " x = " << x << " y = " << y << "\n";
            }
        }

        else if (noise.getValueAtPoint(x - 1, y + 1) <= -0.5) // check LEFT BOTTOM neighbour
        {
            for (int i = 0; i < riverLenght; ++i)
            {
                    int index = randDir() % 3;

                    std::cout << "iter:" << i << " x = " << x  << " + " << possibleRightUpTiles[index].x << " y = " << y << " + " << possibleRightUpTiles[index].y << " grayValue = " << noise.getValueAtPoint(x, y) << "\n";

                    x = x + possibleRightUpTiles[index].x; // TODO: CHECK OUT OF RANGE
                    y = y + possibleRightUpTiles[index].y; // TODO: CHECK OUT OF RANGE

                    if (x + 1 >= width || y + 1 >= height || x <= 0 || y <= 0 || noise.getValueAtPoint(x, y) <= -0.5) { std::cout << "----BREAK----" << "\n"; break; }

                    image.setPixel(x, y, sf::Color(252, 3, 236));

                    
            }
        }

        else if (noise.getValueAtPoint(x - 1, y - 1) <= -0.5) // check LEFT TOP neighbour
        {
            for (int i = 0; i < riverLenght; ++i)
            {
                    int index = randDir() % 3;

                    std::cout << "iter:" << i << " x = " << x << " + " << possibleRightDownTiles[index].x << " y = " << y << " + " << possibleRightDownTiles[index].y << " grayValue = " << noise.getValueAtPoint(x, y) << "\n";

                    x = x + possibleRightDownTiles[index].x;  // TODO: CHECK OUT OF RANGE
                    y = y + possibleRightDownTiles[index].y;  // TODO: CHECK OUT OF RANGE

                    if (x + 1 >= width || y + 1 >= height || x <= 0 || y <= 0 || noise.getValueAtPoint(x, y) <= -0.5) { std::cout << "----BREAK----" << "\n"; break; }

                    image.setPixel(x, y, sf::Color(252, 3, 236));

                    //std::cout << "iter:" << i << " x = " << x << " y = " << y << "\n";
            }
        }

    }
    

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Create an SFML texture and sprite to draw the image on the window
    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite(texture);

    // Main loop
    while (window.isOpen()) {
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear the window
        window.clear();

        // Draw the sprite (image)
        window.draw(sprite);

        // Display the window
        window.display();
    }

}


