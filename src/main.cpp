#include <SDL.h>
#include <SDL_image.h> // For loading textures from pngs
#include <iostream>
#include <vector> // For the std::vector data structure
#include <cstdlib> // For random
#include <ctime> // For time()

#include <SDL_ttf.h> // For drawing text to the screen

#include "RSConstants.hpp"

#include "boost/shared_ptr.hpp" // For smart pointers and the like
#include "Utils.hpp"
#include "Sprite.hpp"
#include "Ship.hpp"
#include "Background.hpp"
#include "Bullet.hpp"
#include "Asteroid.hpp"

///////////////////////////////////////////
//// Core functions for main //////////////
///////////////////////////////////////////

int initSDL(SDL_Window **window, SDL_Renderer **renderer);

void handleInput(int &playerEvent, int &fireEvent);

void updateEnemies(std::vector<AsteroidPtr> &asteroids); // Update the vector of asteroids

void handleCollisions(Ship *player, std::vector<AsteroidPtr> &asteroids); // Simple collision checker for everything

void drawEntities(SDL_Renderer *renderer, Ship *player, 
					Background *bg, std::vector<AsteroidPtr> &asteroids);

void activateAsteroid();
int loadLevel(int argc, char **argv);

///////////////////////////////////////////
//// Global variables for the game ////////
///////////////////////////////////////////

/**
  * This is bad practice. In general, you shouldn't do this.
  */

const char *levels[4] = {
	"lv1-bg.png",
	"lv2-bg.png",
	"lv3-bg.png",
	"lv4-bg.png"
};

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

std::vector<AsteroidPtr> asteroids;
ShipPtr ship;
BackgroundPtr background;

bool running; // The main program loop variable

signed int ASTEROID_INTERVAL; // Just declare it for now

/**
  * This game runs in the main function of the application
  * This is not ideal; we would like it to spawn a seperate
  * thread for the game loop. For this small game, it is OK
  * but try not to do this in your own projects
  */
int main (int argc, char **argv)
{
	srand(time(NULL)); // Set the seed
	int randomNum = rand() % ASTEROID_INTERVAL_RANGE_MIN + (ASTEROID_INTERVAL_RANGE_MAX + 1);
	ASTEROID_INTERVAL = randomNum;

	// Get the level we want to play as
	int level = PLAY_LEVEL;
	level = loadLevel(argc, argv);

	// establish a window, a rendering context and a font structure for text
	int status = initSDL(&window, &renderer);
	if (status != 0)
		return 1;

	// Create the background and the ship
	std::string bgImagePath = getResourcePath() + "img/" + levels[level];
	background = BackgroundPtr(new Background(renderer, bgImagePath));

	if (background.get()->isTextureNull())
	{
		std::cout << "The background texture is nullptr" << std::endl;
	}

	std::string shipImagePath = getResourcePath() + "img/ship.png";
	std::string bulletImagePath = getResourcePath() + "img/bullet_strip.png";
	ship = ShipPtr(new Ship(renderer, shipImagePath, bulletImagePath));

	// Create the enemy array
	std::string asteroidImagePath = getResourcePath() + "img/asteroid.png";
	SDL_Texture *asteroidTexture = IMG_LoadTexture(renderer, asteroidImagePath.c_str());
	Asteroid::asteroidTexture = asteroidTexture; // Set the asteroid texture

	for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
	{
		asteroids.push_back(AsteroidPtr(new Asteroid(renderer)));
	}

#ifdef LOGGING_FPS
	int countedFrames = 0;
	Uint32 fpsStartTime = SDL_GetTicks();
#endif

	int gameEvent = NONE;
	int fireEvent = NONE;
	running = true;
	unsigned int lastAsteroidTime = 0;

	/**
	  * This is the main game loop. It implements a state machine
	  * consisting of 3 states: The menu screen, the game screen
	  * and the game over screen
	  */
	while (running)
	{
		Uint32 frameStartTime = SDL_GetTicks();

		handleInput(gameEvent, fireEvent);

		// Special input. Regardless of what state we are in, this should exit the game
		if (gameEvent == KEY_ESCAPE_PRESSED
			|| gameEvent == GAME_QUIT)
		{
			running = false;
			continue; // Short circuit the rest of the function
		}

		// Fire an asteroid if enough time has passed
		if (lastAsteroidTime + ASTEROID_INTERVAL <= SDL_GetTicks())
		{
			int randomNum = rand() % ASTEROID_INTERVAL_RANGE_MIN + (ASTEROID_INTERVAL_RANGE_MAX + 1);
			ASTEROID_INTERVAL = randomNum;
			lastAsteroidTime = SDL_GetTicks();
			activateAsteroid();
		}

		// Update the player ship and the asteroid belt
		ship.get()->update(gameEvent, fireEvent);
		updateEnemies(asteroids);

		/*
		 * TODO: Handle collisions between the bullet and the asteroids
		 */

		// Draw stuff
		drawEntities(renderer, ship.get(), background.get(), asteroids);


#ifdef LOGGING_FPS
		// Before we draw, calculate how fast that fram took to compute
		float avgFps = countedFrames / ((SDL_GetTicks() - fpsStartTime) / 1000.0f);
		if (avgFps > 2000000)
		{
			avgFps = 0;
		}

		std::cout << "Frames per second: " << avgFps << std::endl;
#endif

#ifdef LOGGING_FPS
		// Increase the frame count
		++countedFrames;
#endif

		// Delay if we're going too fast
		int frameTicks = SDL_GetTicks() - frameStartTime;
		if (frameTicks < SCREEN_TICKS_PER_FRAME)
		{
			SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
		}
	}

	// Cleanup everything. The ship, background and enemies will clean themselves
	SDL_DestroyTexture(asteroidTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	renderer = NULL;
	window = NULL;

	return 0;
}

int loadLevel(int argc, char **argv)
{
	int level = 0;
	if (argc == 2)
	{
		level = atoi(argv[1]) - 1;
		if (level >= 4
			|| level < 0)
		{
			level = 0;
		}
	}
	return level;
}

void activateAsteroid()
{
	for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
	{
		Asteroid *currAsteroid = asteroids[i].get();
		if (!currAsteroid->checkIsActivated())
		{
			currAsteroid->activate();
			break;
		}
	}
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
	tempWin = SDL_CreateWindow("Rocket Shooter!", 100, 100,
									GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT, 0); // SDL_WINDOW_SHOWN is ignored
	if (tempWin == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		TTF_Quit();
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

	tempWin = NULL;
	tempRen = NULL;

	return 0;
}

void updateEnemies(std::vector<AsteroidPtr> &asteroids)
{
	for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
		asteroids[i].get()->update();
}

void handleCollisions(Ship *player, std::vector<AsteroidPtr> &asteroids)
{
	/**
	  * TODO: Implement the collision handling code here
	  */
}

void drawEntities(SDL_Renderer *renderer, Ship *player, 
					Background *bg, std::vector<AsteroidPtr> &asteroids)
{
	// Now draw the frame
	SDL_RenderClear(renderer);

	if (bg != nullptr)
	{
		bg->draw();
	}
	
	if (!asteroids.empty())
	{
		for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
			asteroids[i].get()->draw();
	}

	if (player != nullptr)
	{
		player->draw();
	}

	// Present everything to be rendered
	SDL_RenderPresent(renderer);
}

/**
  * This game is very simple. It only accepts
  * 4 buttons as input: Up, down, space bar and escape
  * This function checks which one was pressed and acts accordingly
  */
void handleInput(int &gameEvent, int &fireEvent)
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
						gameEvent = KEY_UP_PRESSED;
						break;

					case SDLK_DOWN:
						gameEvent = KEY_DOWN_PRESSED;
						break;

					case SDLK_ESCAPE:
						gameEvent = KEY_ESCAPE_PRESSED;
						break;

					case SDLK_SPACE:
						fireEvent = KEY_SPACE_PRESSED;
						break;

					case SDLK_p:
						gameEvent = KEY_P_PRESSED;
						break;

					default:
						break;
				}
				break;

			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
					case SDLK_UP:
						gameEvent = KEY_RELEASED;
						break;

					case SDLK_DOWN:
						gameEvent = KEY_RELEASED;
						break;

					case SDLK_ESCAPE:
						gameEvent = KEY_RELEASED;
						break;

					case SDLK_SPACE:
						fireEvent = KEY_RELEASED;
						break;

					default:
						break;
				}
				break;

			case SDL_QUIT:
				gameEvent = GAME_QUIT;
				break;

			default:
				break;
		}
	}
}
