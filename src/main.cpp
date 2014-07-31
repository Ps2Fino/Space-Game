#include <SDL.h>
#include <SDL_image.h> // For loading textures from pngs
#include <iostream>
#include "utils.hpp"

#include "Sprite.hpp"
#include "Ship.hpp"

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

	// Create a little ship sprite and animate it
	std::string shipImagePath = getResourcePath() + "ship.png";
	Ship myShip(64, 48, renderer, shipImagePath);
	EVENT myEvent = UP;

	// Create a little game loop. The ship can move up or down
	// and escape will exit the game
	bool running = true;
	SDL_Event event;
	while(running)
	{
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_UP)
					{
						myEvent = UP;
						// std::cout << "Event is now up" << std::endl;
					}
					else if (event.key.keysym.sym == SDLK_DOWN)
					{
						myEvent = DOWN;
						// std::cout << "Event is now down" << std::endl;
					}
					else if (event.key.keysym.sym == SDLK_ESCAPE)
					{
						running = false;
					}
					else
					{
						myEvent = LEFT;
					}
					myShip.update(myEvent);
					break;

				case SDL_QUIT:
					running = false;
					break;

				default:
					break;
			}
		}
		// myShip.update(myEvent); // This gives a cool effect

		// Now draw
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, bg_tex, NULL, NULL);
		myShip.draw();
		SDL_RenderPresent(renderer);
	}

	// for (int i=0; i < 200; ++i)
	// {
	// 	// First update the little guy
	// 	myShip.update(myEvent);

	// 	// Now blitz him
	// 	SDL_RenderClear(renderer);
	// 	SDL_RenderCopy(renderer, bg_tex, NULL, NULL);
	// 	myShip.draw();
	// 	SDL_RenderPresent(renderer);
	// }

	// SDL_Delay(2000);


	// Cleanup
	SDL_DestroyTexture(bg_tex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}