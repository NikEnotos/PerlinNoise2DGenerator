#include <iostream>
#include "PerlinNoise2DGenerator.h"
#include "SFML/Graphics.hpp"

 int main()
{
    int seed = 12346788;
    int friquency = 10;
    const int width = 100;
    const int height = 100;
    PerlinNoise2DGenerator noise(width, height, friquency, seed);

    const float minValue = -1.0f;
    const float maxValue = 1.0f;

    sf::RenderWindow window(sf::VideoMode(width, height), "Black and White Image");

    // Create an SFML image to store the pixel data
    sf::Image image;
    image.create(width, height);

    // Set up threshold for each type of tile 
    ////////////////////////////////////////////
    int mountainsLevel = 240;
    int forstLevel = 200;
    int hillsLevel = 160; //110
    int meadowsLevel1 = 130; //100
    int sandLevel = 80;
    int shallowWaterLevel = 60;
    int deepWaterLevel = 20;
    ///////////////////////////////////////////

    // Convert the values to black and white and set pixels in the image
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float value = noise.getValueAtPoint(x, y);

            // Map the value from [minValue, maxValue] to [0, 255]
            int grayValue = static_cast<int>((value - minValue) / (maxValue - minValue) * 255);

            // Set pixel color
            //if (grayValue <= deepWaterLevel)
            //{
            //    sf::Color pixelColor(17, 60, 130);
            //    image.setPixel(x, y, pixelColor);
            //}
            //else if (grayValue <= shallowWaterLevel)
            //{
            //    sf::Color pixelColor(100, 150, 232); 
            //    image.setPixel(x, y, pixelColor);
            //}
            //else if (grayValue <= sandLevel)
            //{
            //    sf::Color pixelColor(252, 248, 0);
            //    image.setPixel(x, y, pixelColor);
            //}
            //else if (grayValue <= meadowsLevel1)
            //{
            //    sf::Color pixelColor(85, 255, 0);
            //    image.setPixel(x, y, pixelColor);
            //}
            //else if (grayValue <= hillsLevel)
            //{
            //    sf::Color pixelColor(6, 150, 11);
            //    image.setPixel(x, y, pixelColor);
            //}
            //else if (grayValue <= forstLevel)
            //{
            //    sf::Color pixelColor(4, 145, 94);
            //    image.setPixel(x, y, pixelColor);
            //}
            //else if (grayValue <= mountainsLevel)
            //{
            //    sf::Color pixelColor(197, 250, 247);
            //    image.setPixel(x, y, pixelColor);
            //}
            //else
            //{
            //    sf::Color pixelColor(169, 176, 176); // Higher than mountains 
            //    image.setPixel(x, y, pixelColor);
            //}

            sf::Color pixelColor(grayValue, grayValue, grayValue);
            image.setPixel(x, y, pixelColor);

        }
    }

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


