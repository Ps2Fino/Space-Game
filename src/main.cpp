#include <SDL.h>
#include <SDL_image.h> // For loading textures from pngs
#include <iostream>
#include "utils.hpp"

using namespace std;

int main (int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	// Create the window that will hold our game
	SDL_Window *window = SDL_CreateWindow("Rocket Shooter!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (window == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	// Next, we need a renderer to render the assets to the screen
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr)
	{
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// Load the first level for the game
	std::string imagePath = getResourcePath() + "lv1-bg.png";
	SDL_Texture *bg_tex = IMG_LoadTexture(renderer, imagePath.c_str());
	if (bg_tex == nullptr)
	{
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// Draw the texture and wait for 2 seconds
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, bg_tex, NULL, NULL);
	SDL_RenderPresent(renderer);

	SDL_Delay(2000);


	// Cleanup
	SDL_DestroyTexture(bg_tex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}