#include <iostream>
#include "PerlinNoise2DGenerator.h"
#include "Sequential_PerlinNoise2DGenerator.h"
#include "SFML/Graphics.hpp"
#include <chrono>
#include <random> 

int main(int argc, char* argv[])
{
	int width;
	int height;
	int friquency;

	int seed;
	bool verticallySeamless;
	bool horizontallySeamless;
	int numOfChunksForAThread;

	std::cout << "Input next arguments:\n\n   Width(int) [Example: 1920]: "; 	std::cin >> width;
	std::cout << "\n   Height(int) [Example: 1080]: "; 	std::cin >> height;
	std::cout << "\n   Frequency(int) <range[>0]> [Example: 50]: "; 	std::cin >> friquency;
	std::cout << "\n   Seed(int) <0 for rendom> [Example: 42]: "; 	std::cin >> seed;
	std::cout << "\n   Vertically seamless(bool) [Example: 1 for 'true' / 0 for 'false']: "; 	std::cin >> verticallySeamless;
	std::cout << "\n   Horizontally seamless(bool) [Example: 1 for 'true' / 0 for 'false']: "; 	std::cin >> horizontallySeamless;
	std::cout << "\n   Number of chunks for a thread(int) <range[1-300]> [Example: 50]: "; 	std::cin >> numOfChunksForAThread;

	{

		std::cout << "\n\n<<< Perlin noise thread version >>> \n" << std::endl;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Start the clock
		auto noise_Generation_start = std::chrono::high_resolution_clock::now();

		PerlinNoise2DGenerator noise(width, height, friquency, seed, verticallySeamless, horizontallySeamless, numOfChunksForAThread);



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



		std::string image_name = std::to_string(width) + "x" + std::to_string(height) + "_Parallel_PerlinNoise.png";

		if (!image.saveToFile(image_name))
		{
			std::cout << "[-] Image saving has failed :( " << std::endl << std::endl;
		}
		else
		{
			std::cout << "[+] Image saved successfully (" << image_name << ")" << std::endl << std::endl;
		}

	}

	{

		////////////////////////////////////////////////////////////////////// SEQUENTIAL PART //////////////////////////////////////////////////////////////////////////
		std::cout << "\n\n<<< Perlin noise Sequential version >>>\n" << std::endl;
		std::cout << "  Processing... " << std::endl;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Stop the clock
		auto seq_noise_Generation_start = std::chrono::high_resolution_clock::now();

		Sequential_PerlinNoise2DGenerator seq_noise(width, height, friquency, seed, verticallySeamless, horizontallySeamless);

		// Create an SFML image to store the pixel data
		sf::Image seq_image;
		seq_image.create(seq_noise.Width, seq_noise.Height);


		// Convert the values to black and white and set pixels in the image
		for (int y = 0; y < seq_noise.Height; ++y) {
			for (int x = 0; x < seq_noise.Width; ++x) {
				float value = seq_noise.getValueAtPoint(x, y);

				// Map the value from [minValue, maxValue] to [0, 255]
				int grayValue = static_cast<int>((value + 1.0) / 2.0 * 255);

				sf::Color pixelColor(grayValue, grayValue, grayValue);
				seq_image.setPixel(x, y, pixelColor);

			}
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Stop the clock
		auto seq_noise_Generation_end = std::chrono::high_resolution_clock::now();
		// Calculate the duration
		auto seq_noise_Generation_duration = std::chrono::duration_cast<std::chrono::seconds>(seq_noise_Generation_end - seq_noise_Generation_start);
		// Print the duration
		std::cout << ">>> Sequential Noise Generation took: " << seq_noise_Generation_duration.count() << " seconds" << std::endl;

		std::string seq_image_name = std::to_string(width) + "x" + std::to_string(height) + "_Sequential_PerlinNoise.png";

		if (!seq_image.saveToFile(seq_image_name))
		{
			std::cout << "[-] Image saving has failed :( " << std::endl << std::endl;
		}
		else
		{
			std::cout << "[+] Image saved successfully (" << seq_image_name << ")" << std::endl << std::endl;
		}

	}

	std::system("pause");

	return 0;
}


