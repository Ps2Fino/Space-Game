#include <SDL.h>
#include <SDL_image.h> // For loading textures from pngs
#include <iostream>
#include <vector> // For the std::vector data structure
#include <random> // For random bullet colors
#include <SDL_ttf.h> // For drawing text to the screen

#include "RSConstants.hpp"

#include "boost/shared_ptr.hpp" // For smart pointers and the like
#include "Utils.hpp"
#include "Sprite.hpp"
#include "Ship.hpp"
#include "Background.hpp"
#include "Bullet.hpp"
#include "Asteroid.hpp"
#include "Stats.hpp"
#include "SoundFX.hpp"
#include "MenuScreen.hpp"

///////////////////////////////////////////
//// Core functions for the state machine /
///////////////////////////////////////////

void doMenuCase(int &event, unsigned int &lastAsteroidTime);
void doGameCase(int &event, int &fireEvent, unsigned int &lastAsteroidTime);
void doGameOverCase(int &event);

///////////////////////////////////////////
//// Core functions for main //////////////
///////////////////////////////////////////

int initSDL(SDL_Window **window, SDL_Renderer **renderer, TTF_Font **font);

void handleInput(int &playerEvent, int &fireEvent);

void updateEnemies(std::vector<AsteroidPtr> &asteroids); // Update the vector of asteroids

void handleCollisions(Ship *player, std::vector<AsteroidPtr> &asteroids, 
						Stats *scoreTable); // Simple collision checker for everything

void drawEntities(SDL_Renderer *renderer, Ship *player, 
					Background *bg, std::vector<AsteroidPtr> &asteroids, 
					Stats *scoreTable, MenuScreen *menuScreen);

void activateAsteroid();
int loadLevel(int argc, char **argv);

const char *levels[4] = {
	"lv1-bg.png",
	"lv2-bg.png",
	"lv3-bg.png",
	"lv4-bg.png"
};

// Global variables. This is bad practice, don't do this. My excuse is its 01:32 am
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;

std::vector<AsteroidPtr> asteroids;
ShipPtr ship;
BackgroundPtr background;
StatsPtr scoreTable;
MenuScreenPtr menuScreen;

bool running; // The main program loop variable
GAME_STATE state = MENU; // The state of the game

// Have a variable delay for the asteroids
std::random_device rd; // obtain a random number from hardware
std::mt19937 eng(rd()); // seed the generator
std::uniform_int_distribution<> distr(ASTEROID_INTERVAL_RANGE_MIN, ASTEROID_INTERVAL_RANGE_MAX); // define the range
signed int ASTEROID_INTERVAL = distr(eng); // Random time interval

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
	level = loadLevel(argc, argv);

	// establish a window, a rendering context and a font structure for text
	int status = initSDL(&window, &renderer, &font);
	if (status != 0)
		return 1;

	// Init the sound library
	status = SoundFX::initMixerLibrary();
	if (status != 0)
		return 1;

	status = SoundFX::loadLaserSound();
	if (status != 0)
		return 1;	

	// Create the background and the ship
	std::string bgImagePath = getResourcePath() + "img/" + levels[level];
	std::cout << "Playing level: " << bgImagePath << std::endl;
	background = BackgroundPtr(new Background(renderer, bgImagePath));

	if (background.get()->isTextureNull())
	{
		std::cout << "The background texture is nullptr" << std::endl;
	}

	// Create the menuscreen
	menuScreen = MenuScreenPtr(new MenuScreen(renderer, font));

	std::string shipImagePath = getResourcePath() + "img/ship.png";
	std::string bulletImagePath = getResourcePath() + "img/bullet_strip.png";
	ship = ShipPtr(new Ship(renderer, shipImagePath, bulletImagePath));

	// Create the score table
	scoreTable = StatsPtr(new Stats(renderer, font));

	// Create the enemy array
	std::string asteroidImagePath = getResourcePath() + "img/asteroid.png";
	SDL_Texture *asteroidTexture = IMG_LoadTexture(renderer, asteroidImagePath.c_str());
	Asteroid::asteroidTexture = asteroidTexture; // Set the asteroid texture

	for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
	{
		asteroids.push_back(AsteroidPtr(new Asteroid(renderer)));
		asteroids.back().get()->setStats(scoreTable.get()); // Add the stats table to the asteroid
		asteroids.back().get()->activate();
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

		// Switch on the game state
		switch (state)
		{
			case MENU:
				doMenuCase(gameEvent, lastAsteroidTime);
				break;

			case GAME:
				doGameCase(gameEvent, fireEvent, lastAsteroidTime);
				break;

			case GAME_OVER:
				doGameOverCase(gameEvent);
				break;

			default:
				break;
		}

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

	// Deactivate the score table
	scoreTable.get()->deactivate();

	// TODO: Show the player a game over screen or something

	if (scoreTable.get()->playerWon())
	{
		std::cout << "YOU WON!!" << std::endl;
	}
	else
	{
		std::cout << "YOU LOST" << std::endl;
	}

	// Cleanup everything. The ship, background and enemies will clean themselves
	TTF_CloseFont(font);
	SDL_DestroyTexture(asteroidTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SoundFX::shutDownMixerLibrary();
	TTF_Quit();
	SDL_Quit();

	renderer = NULL;
	window = NULL;
	font = NULL;

	return 0;
}

void doMenuCase(int &event, unsigned int &lastAsteroidTime)
{
	if (event == GAME_QUIT
			|| event == KEY_ESCAPE_PRESSED)
	{
		running = false;
		return;
	}

	if (event == KEY_P_PRESSED)
	{
		for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
			asteroids[i].get()->resetPosition();
		state = GAME;
		return; // Move to the game state
	}

	// Fire an asteroid if enough time has passed
	if (lastAsteroidTime + ASTEROID_INTERVAL <= SDL_GetTicks())
	{
		ASTEROID_INTERVAL = distr(eng);
		lastAsteroidTime = SDL_GetTicks();
		activateAsteroid();
	}

	// In the menu case, we just display the asteroids moving across the screen
	// and display the "press space to play button"

	updateEnemies(asteroids);
	drawEntities(renderer, NULL, background.get(), asteroids, NULL, menuScreen.get());
}

void doGameCase(int &event, int &fireEvent, unsigned int &lastAsteroidTime)
{
	if (event == KEY_ESCAPE_PRESSED
			|| event == GAME_QUIT)
	{
		running = false;
		std::cout << "Set running to false because gameEvent = " << event << std::endl;
		return; // Short circuit the rest of the function
	}

	// Check the stats class to see if the game has ended
	if (scoreTable.get()->checkIsGameOver())
	{
		running = false;
		std::cout << "Set running to false because scoreTable said game was over" << std::endl;
	}

	// Fire an asteroid if enough time has passed
	if (lastAsteroidTime + ASTEROID_INTERVAL <= SDL_GetTicks())
	{
		ASTEROID_INTERVAL = distr(eng);
		lastAsteroidTime = SDL_GetTicks();
		activateAsteroid();
	}

	// Update the player ship and the asteroid belt
	ship.get()->update(event, fireEvent);
	updateEnemies(asteroids);

	// Check collisions here
	handleCollisions(ship.get(), asteroids, scoreTable.get());

	// Draw stuff
	drawEntities(renderer, ship.get(), background.get(), asteroids, scoreTable.get(), NULL);

	// std::cout << "doGameCase was called successfully" << std::endl;
}

void doGameOverCase(int &event)
{
	// TODO: Implement this
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

int initSDL(SDL_Window **window, SDL_Renderer **renderer, TTF_Font **font)
{
	SDL_Window *tempWin = NULL;
	SDL_Renderer *tempRen = NULL;
	TTF_Font *tempFont = NULL;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	if (TTF_Init() != 0)
	{
		std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
	}

	// Create the window that will hold our game
	tempWin = SDL_CreateWindow("Rocket Shooter!", 100, 100, GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
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
		TTF_Quit();
		SDL_Quit();
		return 1;
	}

	// Load  the font
	std::string fontPath = getResourcePath() + "fonts/tiptoe.ttf";
	tempFont = TTF_OpenFont(fontPath.c_str(), TEXT_FONT_SIZE);
	if (tempFont == nullptr)
	{
		std::cout << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
		SDL_DestroyWindow(tempWin);
		SDL_DestroyRenderer(tempRen);
		TTF_Quit();
		SDL_Quit();
		return 1;
	}

	// Pass over the temps
	*window = tempWin;
	*renderer = tempRen;
	*font = tempFont;

	tempWin = NULL;
	tempRen = NULL;
	tempFont = NULL;

	return 0;
}

void updateEnemies(std::vector<AsteroidPtr> &asteroids)
{
	for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
		asteroids[i].get()->update();

	// std::cout << "Update enemies called successfully" << std::endl;
}

void handleCollisions(Ship *player, std::vector<AsteroidPtr> &asteroids, Stats *scoreTable)
{
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

			if (asteroidRect.x >= bulletRect.x
					&& asteroidRect.x <= bulletRect.x + BULLET_COLLISION_WIDTH)
			{
				if (asteroidRect.y >= bulletRect.y - ASTEROID_OFFSET
						&& asteroidRect.y <= bulletRect.y + BULLET_COLLISION_HEIGHT)
				{
					// std::cout << "Deactivating an asteroid at X: "
					// 		  << asteroidRect.x << "\tY: " << asteroidRect.y
					// 		  << "\tW: " << asteroidRect.w << "\tH: " << asteroidRect.h << "\nwith a bullet at X: "
					// 		  << bulletRect.x << "\tY: " << bulletRect.y << "\tW: " << bulletRect.w << "\tH: " << bulletRect.h << std::endl;
					currAsteroid.get()->deactivate();
					currBullet.get()->deactivate();

					// record the score
					scoreTable->shootAsteroid();
					break;
				}
			}
		}
	}

	// std::cout << "Handle collisions called successfully" << std::endl;
}

void drawEntities(SDL_Renderer *renderer, Ship *player, 
					Background *bg, std::vector<AsteroidPtr> &asteroids, 
					Stats *scoreTable, MenuScreen *menuScreen)
{
	// Now draw the frame
	SDL_RenderClear(renderer);

	// std::cout << "ABout to draw the bg" << std::endl;
	if (bg != nullptr)
	{
		bg->draw();
	}
	// std::cout << "Background was drawn successfully" << std::endl;
	
	if (!asteroids.empty())
	{
		for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
			asteroids[i].get()->draw();
	}
	// std::cout << "Asteroids were drawn successfully" << std::endl;

	if (player != nullptr)
	{
		player->draw();
	}
	// std::cout << "Ship was drawn successfully" << std::endl;

	if (scoreTable != nullptr)
	{
		scoreTable->draw();
	}

	if (menuScreen != nullptr)
	{
		menuScreen->draw();
	}
	// std::cout << "Score table was drawn successfully" << std::endl;

	// Present everything to be rendered
	SDL_RenderPresent(renderer);

	// std::cout << "drawEntities called successfully" << std::endl;
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