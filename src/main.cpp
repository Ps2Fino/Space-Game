#include <SDL.h>
#include <SDL_image.h> // For loading textures from pngs
#include <iostream>

#include "Utils.hpp"
#include "Sprite.hpp"
#include "Ship.hpp"
#include "Background.hpp"

int main (int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
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

	// Create the background and the ship
	std::cout << "About to load the background" << std::endl;
	std::string bgImagePath = getResourcePath() + "lv1-bg.png";
	Background levelBG(renderer, bgImagePath);
	std::string shipImagePath = getResourcePath() + "ship.png";
	Ship player(renderer, shipImagePath);

	// Jump into the game loop
	bool running = true;
	SDL_Event event;
	EVENT myEvent; // For controlling the ship
	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_UP:
							myEvent = UP;
							break;

						case SDLK_DOWN:
							myEvent = DOWN;
							break;

						case SDLK_ESCAPE:
							running = false;
							break;
					}
					break;

				case SDL_KEYUP:
					switch (event.key.keysym.sym)
					{
						default:
							myEvent = NONE;
							break;
					}
					break;

				case SDL_QUIT:
					running = false;
					break;

				default:
					break;
			}
		}

		// Update the player and other entities
		player.update(myEvent);
		// TODO: Update other entities here

		// Now draw the frame
		SDL_RenderClear(renderer);

		levelBG.draw();
		player.draw();

		// TODO: Draw other game objects here

		SDL_RenderPresent(renderer);
	}

	// Cleanup everything
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	renderer = NULL;
	window = NULL;	

	return 0;
}