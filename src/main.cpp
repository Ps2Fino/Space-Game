#include <SDL.h>
#include <SDL_image.h> // For loading textures from pngs
#include <iostream>
#include <vector> // For the std::vector data structure

#ifdef USE_CPP_RANDOM
	#include <random> // For random bullet colors
#else
	#include <cstdlib> // For random
	#include <ctime> // For time()
#endif


#include <SDL_ttf.h> // For drawing text to the screen

#include "RSConstants.hpp"

#include "boost/shared_ptr.hpp" // For smart pointers and the like
#include "Utils.hpp"
#include "Sprite.hpp"
#include "Ship.hpp"
#include "Background.hpp"
#include "Bullet.hpp"
#include "Asteroid.hpp"
#include "AsteroidManager.hpp"

void doGameCase(int &event, int &fireEvent, unsigned int &lastAsteroidTime);

///////////////////////////////////////////
//// Core functions for main //////////////
///////////////////////////////////////////

int initSDL(SDL_Window **window, SDL_Renderer **renderer);

void handleInput(int &playerEvent, int &fireEvent);

// Simple collision checker for everything
void handleCollisions(Ship *player, AsteroidManager *asteroidManager);

void drawEntities(SDL_Renderer *renderer, Ship *player, 
					Background *bg, AsteroidManager *asteroidManager);

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

ShipPtr ship;
BackgroundPtr background;
AsteroidManagerPtr asteroidManager;

bool running; // The main program loop variable

GAME_STATE state = MENU; // The state of the game

#ifdef USE_CPP_RANDOM
// Have a variable delay for the asteroids
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 eng(rd()); // seed the generator
	std::uniform_int_distribution<> distr(ASTEROID_INTERVAL_RANGE_MIN, ASTEROID_INTERVAL_RANGE_MAX); // define the range
	signed int ASTEROID_INTERVAL = distr(eng); // Random time interval
#else
	signed int ASTEROID_INTERVAL; // Just declare it for now
#endif

/**
  * This game runs in the main function of the application
  * This is not ideal; we would like it to spawn a seperate
  * thread for the game loop. For this small game, it is OK
  * but try not to do this in your own projects
  */
int main (int argc, char **argv)
{
#ifndef USE_CPP_RANDOM
	srand(time(NULL)); // Set the seed
	int randomNum = rand() % ASTEROID_INTERVAL_RANGE_MIN + (ASTEROID_INTERVAL_RANGE_MAX + 1);
	ASTEROID_INTERVAL = randomNum;
#endif

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

	// Create the asteroid manager
	asteroidManager = AsteroidManagerPtr(new AsteroidManager(renderer));

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

		doGameCase(gameEvent, fireEvent, lastAsteroidTime);

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
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	renderer = NULL;
	window = NULL;

	return 0;
}

void doGameCase(int &event, int &fireEvent, unsigned int &lastAsteroidTime)
{
	// Update the player ship and the asteroid belt
	ship.get()->update(event, fireEvent);
	asteroidManager.get()->update();

	// Check collisions here
	handleCollisions(ship.get(), asteroidManager.get());

	// Draw stuff
	drawEntities(renderer, ship.get(), background.get(), asteroidManager.get());
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

void handleCollisions(Ship *player, AsteroidManager *asteroidManager)
{
	//////////////////////////////////////////////////////////////////////
	////// Detect the asteroid hits //////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	std::vector<AsteroidPtr> asteroids = asteroidManager->getAsteroids();

	for (std::vector<AsteroidPtr>::iterator asteroidIt = asteroids.begin(); asteroidIt != asteroids.end(); ++asteroidIt)
	{
		AsteroidPtr currAsteroid = *asteroidIt;

		if (!currAsteroid.get()->checkIsActivated())
			continue; // An inactive asteroid can't be hit anyways

		SDL_Rect asteroidRect = currAsteroid.get()->getSize();
		std::vector<BulletPtr> bullets = player->getBullets();
		for (std::vector<BulletPtr>::iterator bulletIt = bullets.begin(); bulletIt != bullets.end(); ++bulletIt)
		{
			BulletPtr currBullet = *bulletIt;
			if (!currBullet.get()->checkIsActivated())
				continue; // An inactive bullet can't be used to shoot an asteroid

			SDL_Rect bulletRect = currBullet.get()->getSize();

			// Check for collision with an asteroid
			if (asteroidRect.x >= bulletRect.x
					&& asteroidRect.x <= bulletRect.x + BULLET_COLLISION_WIDTH)
			{
				if (asteroidRect.y >= bulletRect.y - ASTEROID_OFFSET
						&& asteroidRect.y <= bulletRect.y + BULLET_COLLISION_HEIGHT)
				{
					currAsteroid.get()->deactivate();
					currBullet.get()->deactivate();
					break;
				}
			}
		}
	}
}

void drawEntities(SDL_Renderer *renderer, Ship *player, 
					Background *bg, AsteroidManager *asteroidManager)
{
	// Now draw the frame
	SDL_RenderClear(renderer);

	if (bg != nullptr)
	{
		bg->draw();
	}
	
	if (asteroidManager != NULL)
	{
		asteroidManager->draw();
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