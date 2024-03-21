#include <iostream>
#include "PerlinNoise2DGenerator.h"
#include "SFML/Graphics.hpp"
#include <chrono>
#include <random> 

int main(int argc, char* argv[])
{

	if (argc < 4 || argv[1] == "-h" || argv[1] == "--help")
	{
		std::cout << "Input arguments in the following order: \n Width(int),\n Height(int),\n Frequency(int),\n [optional] Seed(int, 0 for rendom),\n [optional] Vertically seamless(bool),\n [optional] Horizontally seamless(bool)" << std::endl << std::endl;
		std::cout << "Example: PerlinNoiseGenerator.exe 1920 1080 50 1234 false true" << std::endl << std::endl;
	}
	else
	{
		int width = atoi(argv[1]);
		int height = atoi(argv[2]);
		int friquency = atoi(argv[3]);

		int seed = 0;
		bool verticallySeamless = false;
		bool horizontallySeamless = false;

		if (argc >= 5) seed = atoi(argv[4]);
		if (argc >= 6) verticallySeamless = argv[5];
		if (argc >= 7) horizontallySeamless = argv[6];

		std::cout << "<<< Perlin noise thread version >>>" << std::endl;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Start the clock
		auto noise_Generation_start = std::chrono::high_resolution_clock::now();

		PerlinNoise2DGenerator noise(width, height, friquency, seed, verticallySeamless, horizontallySeamless);



		// Create an SFML image to store the pixel data
		sf::Image image;
		image.create(noise.Width, noise.Height);

		// Convert the values to black and white and set pixels in the image
		for (int y = 0; y < noise.Height; ++y) {
			for (int x = 0; x < noise.Width; ++x) {

				int grayValue = noise.getBlackAndWhiteValueAtPoint(x, y);

				sf::Color pixelColor(grayValue, grayValue, grayValue);

				image.setPixel(x, y, pixelColor);

			}
		}



		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Stop the clock
		auto noise_Generation_end = std::chrono::high_resolution_clock::now();
		// Calculate the duration
		auto noise_Generation_duration = std::chrono::duration_cast<std::chrono::seconds>(noise_Generation_end - noise_Generation_start);
		// Print the duration
		std::cout << ">>> Noise Generation took: " << noise_Generation_duration.count() << " seconds" << std::endl;



		std::string image_name = std::to_string(width) + "x" + std::to_string(height) + "_PerlinNoise.png";

		if (!image.saveToFile(image_name))
		{
			std::cout << "[-] Image saving has failed :( " << std::endl << std::endl;
		}
		else
		{
			std::cout << "[+] Image saved successfully (" << image_name << ")" << std::endl << std::endl;
		}

	}

	return 0;
}


