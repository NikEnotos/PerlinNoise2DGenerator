#include <iostream>
#include "PerlinNoise2DGenerator.h"
#include "SFML/Graphics.hpp"
#include <chrono>
#include <random> 

 int main()
{
    int seed = 123456788;
    int friquency = 7;
    const int width = 1900;
    const int height = 1060;




    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Start the clock
    auto noise_Generation_start = std::chrono::high_resolution_clock::now();

    PerlinNoise2DGenerator noise(width, height, friquency, seed, true);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Stop the clock
    auto noise_Generation_end = std::chrono::high_resolution_clock::now();
    // Calculate the duration
    auto noise_Generation_duration = std::chrono::duration_cast<std::chrono::milliseconds>(noise_Generation_end - noise_Generation_start);
    // Print the duration
    std::cout << "Noise Generation: " << noise_Generation_duration.count() << " milliseconds" << std::endl;




    const float minValue = -1.0f;
    const float maxValue = 1.0f;

    //sf::RenderWindow window(sf::VideoMode(noise.Width/4, noise.Height/4), "Black and White Image");
    sf::RenderWindow window(sf::VideoMode(0.2 * 1900, 0.2 * 1060), "Black and White Image");

    // Create an SFML image to store the pixel data
    sf::Image image;
    image.create(noise.Width, noise.Height);


    // Convert the values to black and white and set pixels in the image
    for (int y = 0; y < noise.Height; ++y) {
        for (int x = 0; x < noise.Width; ++x) {
            float value = noise.getValueAtPoint(x, y);

            // Map the value from [minValue, maxValue] to [0, 255]
            int grayValue = static_cast<int>((value - minValue) / (maxValue - minValue) * 255);

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


