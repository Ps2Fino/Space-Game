#include <SDL.h>
#include <SDL_image.h> // For loading textures from pngs
#include <iostream>

#include "Utils.hpp"
#include "Sprite.hpp"
#include "Ship.hpp"
#include "Background.hpp"
#include "Bullet.hpp"

int initSDL(SDL_Window **window, SDL_Renderer **renderer);
void handleInput(bool &running, GAME_EVENT &playerEvent);
void updateEnemies(Sprite *enemies); // Update the array of sprites
void handleCollisions(Ship &player, Sprite *enemies); // Simple collision checker for everything
// void drawEntities(SDL_Renderer *renderer, Ship &player, Background &bg, Sprite *enemies);
void drawEntities(SDL_Renderer *renderer, Ship &player, Background &bg, Sprite *enemies, Bullet &bullet);

#define PLAY_LEVEL 0
const char *levels[4] = {
	"lv1-bg.png",
	"lv2-bg.png",
	"lv3-bg.png",
	"lv4-bg.png"
};

/**
  * This game runs in the main function of the application
  * This is not ideal; we would like it to spawn a seperate
  * thread for the game loop. For this small game, it is OK
  * but try not to do this in your own projects
  */
int main (int argc, char **argv)
{
	// Get the level we want to play as
	int level = PLAY_LEVEL;
	if (argc == 2)
	{
		level = atoi(argv[1]) - 1;
		if (level >= 4
			|| level < 0)
		{
			level = 0;
		}
	}

	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	int status = initSDL(&window, &renderer);
	if (status != 0)
		return 1;

	// Create the background and the ship
	std::string bgImagePath = getResourcePath() + "img/" + levels[level];
	std::cout << "Playing level: " << bgImagePath << std::endl;
	Background levelBG(renderer, bgImagePath);

	std::string shipImagePath = getResourcePath() + "img/ship.png";
	std::string bulletImagePath = getResourcePath() + "img/bullet_strip.png";
	Ship player(renderer, shipImagePath, bulletImagePath);

	if (bulletTexture == nullptr)
	{
		std::cout << "The bullet texture is nullptr" << std::endl;
	}

	Bullet bullet(renderer, bulletTexture);

	// Create the enemy array

	// TODO: Populate the array of enemies with some enemies

	// Set the boundary for the ship
	player.setMovementBoundary(0, 480);

	// Jump into the game loop
	bool running = true;
	GAME_EVENT gameEvent; // For controlling the ship
	bool bulletIsActive = false;;
	while (running)
	{
		handleInput(running, gameEvent);

		// Update the player and other entities
		player.update(gameEvent);
		bullet.update();
		updateEnemies(NULL);

		// Check collisions here
		handleCollisions(player, NULL);

		// Draw stuff
		drawEntities(renderer, player, levelBG, NULL, bullet);
	}

	// Cleanup everything. The ship, background and enemies will clean themselves
	SDL_DestroyTexture(bulletTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	renderer = NULL;
	window = NULL;	

	return 0;
}

int initSDL(SDL_Window **window, SDL_Renderer **renderer)
{
	SDL_Window *tempWin = NULL;
	SDL_Renderer *tempRen = NULL;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	// Create the window that will hold our game
	tempWin = SDL_CreateWindow("Rocket Shooter!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (tempWin == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	// Next, we need a renderer to render the assets to the screen
	tempRen = SDL_CreateRenderer(tempWin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (tempRen == nullptr)
	{
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(tempWin);
		SDL_Quit();
		return 1;
	}

	// Pass over the temps
	*window = tempWin;
	*renderer = tempRen;

	return 0;
}

void updateEnemies(Sprite *enemies)
{
	// TODO: Implement this
}

void handleCollisions(Ship &player, Sprite *enemies)
{
	// TODO: Implement this
}

void drawEntities(SDL_Renderer *renderer, Ship &player, Background &bg, Sprite *enemies, Bullet &bullet)
{
	// Now draw the frame
	SDL_RenderClear(renderer);

	bg.draw();
	player.draw();
	bullet.draw();
	// TODO: Implement enemy drawing

	SDL_RenderPresent(renderer);
}

/**
  * This game is very simple. It only accepts
  * 4 buttons as input: Up, down, space bar and escape
  * This function checks which one was pressed and acts accordingly
  */
void handleInput(bool &running, GAME_EVENT &playerEvent)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_UP:
						playerEvent = UP;
						break;

					case SDLK_DOWN:
						playerEvent = DOWN;
						break;

					case SDLK_ESCAPE:
						running = false;
						break;

					case SDLK_SPACE:
						// Fire a bullet
						playerEvent = FIRE;
						break;
				}
				break;

			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
					default:
						playerEvent = NONE;
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
}