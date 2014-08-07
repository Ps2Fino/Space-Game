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

#include "RSConstants.hpp"

#include "boost/shared_ptr.hpp" // For smart pointers and the like
#include "Utils.hpp"
#include "Sprite.hpp"
#include "Ship.hpp"
#include "Background.hpp"
#include "Bullet.hpp"
#include "Asteroid.hpp"
#include "SoundFX.hpp"

// I couldn't build the SDL_ttf library for the psp so I'm removing this for a test build
#ifndef BUILDING_PSP_EBOOT
	#include <SDL_ttf.h> // For drawing text to the screen
	#include "Stats.hpp"
	#include "MenuScreen.hpp"
#else
	// Define a simple struct to store the game score
	typedef struct score {
		int lives;
		int asteroids;
	} GameScore;

	GameScore currentScore;
#endif

///////////////////////////////////////////
//// Core functions for the state machine /
///////////////////////////////////////////

void doMenuCase(int &event, unsigned int &lastAsteroidTime);
void doGameCase(int &event, int &fireEvent, unsigned int &lastAsteroidTime);
void doGameOverCase(int &event, unsigned int &lastAsteroidTime);

///////////////////////////////////////////
//// Core functions for main //////////////
///////////////////////////////////////////

#ifndef BUILDING_PSP_EBOOT
	int initSDL(SDL_Window **window, SDL_Renderer **renderer, TTF_Font **font);
#else
	int initSDL(SDL_Window **window, SDL_Renderer **renderer);
#endif

// These functions don't change
void handleInput(int &playerEvent, int &fireEvent);
void updateEnemies(std::vector<AsteroidPtr> &asteroids); // Update the vector of asteroids

#ifndef BUILDING_PSP_EBOOT
	void handleCollisions(Ship *player, std::vector<AsteroidPtr> &asteroids, 
							Stats *scoreTable); // Simple collision checker for everything
#else
	void handleCollisions(Ship *player, std::vector<AsteroidPtr> &asteroids);
#endif

#ifndef BUILDING_PSP_EBOOT
	void drawEntities(SDL_Renderer *renderer, Ship *player, 
						Background *bg, std::vector<AsteroidPtr> &asteroids, 
						Stats *scoreTable, MenuScreen *menuScreen);
#else
	void drawEntities(SDL_Renderer *renderer, Ship *player, 
						Background *bg, std::vector<AsteroidPtr> &asteroids);
#endif

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

#ifndef BUILDING_PSP_EBOOT
	TTF_Font *font = NULL;
#endif

std::vector<AsteroidPtr> asteroids;
ShipPtr ship;
BackgroundPtr background;

#ifndef BUILDING_PSP_EBOOT
	StatsPtr scoreTable;
	MenuScreenPtr menuScreen;
#endif

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
#ifdef PSP_BUILD
  	extern "C" int main (int argc, char **argv)
#else
	int main (int argc, char **argv)
#endif
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
#ifndef BUILDING_PSP_EBOOT
	int status = initSDL(&window, &renderer, &font);
#else
	int status = initSDL(&window, &renderer);
#endif

	if (status != 0)
	return 1;

	// Init the sound library
	status = SoundFX::initMixerLibrary();
	if (status != 0)
		return 1;

	status = SoundFX::loadLaserSound();
	if (status != 0)
		return 1;

	status = SoundFX::loadMusic();
	if (status != 0)
		return 1;

	SoundFX::startMusic();

	// Create the background and the ship
	std::string bgImagePath = getResourcePath() + "img/" + levels[level];
	background = BackgroundPtr(new Background(renderer, bgImagePath));

	if (background.get()->isTextureNull())
	{
		std::cout << "The background texture is nullptr" << std::endl;
	}

#ifndef BUILDING_PSP_EBOOT
	// Create the menuscreen
	menuScreen = MenuScreenPtr(new MenuScreen(renderer, font));
#endif

	std::string shipImagePath = getResourcePath() + "img/ship.png";
	std::string bulletImagePath = getResourcePath() + "img/bullet_strip.png";
	ship = ShipPtr(new Ship(renderer, shipImagePath, bulletImagePath));

#ifndef BUILDING_PSP_EBOOT
	// Create the score table
	scoreTable = StatsPtr(new Stats(renderer, font));
#endif

	// Create the enemy array
	std::string asteroidImagePath = getResourcePath() + "img/asteroid.png";

#ifndef BUILDING_PSP_EBOOT
	SDL_Texture *asteroidTexture = IMG_LoadTexture(renderer, asteroidImagePath.c_str());
#else
	// In IMG_loadTexture is an SDL 2.0 function in the SDL_image exteions.
	// I revert to the old API here
	SDL_Surface *asteroidSurface = IMG_Load(asteroidImagePath.c_str());
	SDL_Texture *asteroidTexture = SDL_CreateTextureFromSurface(renderer, asteroidSurface);
	SDL_FreeSurface(asteroidSurface);
#endif

	Asteroid::asteroidTexture = asteroidTexture; // Set the asteroid texture

	for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
	{
		asteroids.push_back(AsteroidPtr(new Asteroid(renderer)));

#ifndef BUILDING_PSP_EBOOT
		asteroids.back().get()->setStats(scoreTable.get()); // Add the stats table to the asteroid
#endif
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
				doGameOverCase(gameEvent, lastAsteroidTime);
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

	// Cleanup everything. The ship, background and enemies will clean themselves
#ifndef BUILDING_PSP_EBOOT
	TTF_CloseFont(font);
	TTF_Quit();
#endif
	SDL_DestroyTexture(asteroidTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SoundFX::shutDownMixerLibrary();
	SDL_Quit();

	renderer = NULL;
	window = NULL;
#ifndef BUILDING_PSP_EBOOT
	font = NULL;
#endif

	return 0;
}

void doMenuCase(int &event, unsigned int &lastAsteroidTime)
{
	if (event == KEY_P_PRESSED)
	{
		// Reset the positions of the asteroids
		for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
			asteroids[i].get()->reset();

#ifndef BUILDING_PSP_EBOOT
		// Turn on the scoring table
		scoreTable.get()->reset();
		scoreTable.get()->activate();
#else
		currentScore.asteroids = 0;
		currentScore.lives = NUMBER_PLAYER_LIVES;
#endif

		state = GAME;
		return; // Move to the game state
	}

	// Fire an asteroid if enough time has passed
	if (lastAsteroidTime + ASTEROID_INTERVAL <= SDL_GetTicks())
	{
#ifdef USE_CPP_RANDOM
		ASTEROID_INTERVAL = distr(eng);
#else
		int randomNum = rand() % ASTEROID_INTERVAL_RANGE_MIN + (ASTEROID_INTERVAL_RANGE_MAX + 1);
		ASTEROID_INTERVAL = randomNum;
#endif
		lastAsteroidTime = SDL_GetTicks();
		activateAsteroid();
	}

	// In the menu case, we just display the asteroids moving across the screen
	// and display the "press space to play button"

	updateEnemies(asteroids);

#ifndef BUILDING_PSP_EBOOT
	drawEntities(renderer, NULL, background.get(), asteroids, NULL, menuScreen.get());
#else
	drawEntities(renderer, NULL, background.get(), asteroids);
#endif
}

void doGameCase(int &event, int &fireEvent, unsigned int &lastAsteroidTime)
{

#ifndef BUILDING_PSP_EBOOT
	// Check the stats class to see if the game has ended
	if (scoreTable.get()->checkIsGameOver())
	{
		// Turn off the score table
		scoreTable.get()->deactivate();

		// Set the menu screens' text
		std::string gameOverText;
		if (scoreTable.get()->playerWon())
			gameOverText = "You won!";
		else
			gameOverText = "You lost";
		menuScreen.get()->setText(gameOverText);

		// Move to the game over state
		state = GAME_OVER;

		// Quick hack because the event was passed by reference. If the user doesn't press anthing
		// the game over screen will never be played because the event will be P_PRESSED which will just automatically
		// start the next game
		event = NONE;
		return;
	}
#else
	if (currentScore.asteroids == NUMBER_ASTEROIDS_TO_WIN
			|| currentScore.lives <= 0)
	{
		state = GAME_OVER;
		event = NONE;
		return;
	}
#endif

	// Fire an asteroid if enough time has passed
	if (lastAsteroidTime + ASTEROID_INTERVAL <= SDL_GetTicks())
	{
#ifdef USE_CPP_RANDOM
		ASTEROID_INTERVAL = distr(eng);
#else
		int randomNum = rand() % ASTEROID_INTERVAL_RANGE_MIN + (ASTEROID_INTERVAL_RANGE_MAX + 1);
		ASTEROID_INTERVAL = randomNum;
#endif
		lastAsteroidTime = SDL_GetTicks();
		activateAsteroid();
	}

	// Update the player ship and the asteroid belt
	ship.get()->update(event, fireEvent);
	updateEnemies(asteroids);

	// Check collisions and draw stuff
#ifndef BUILDING_PSP_EBOOT
	handleCollisions(ship.get(), asteroids, scoreTable.get());
	drawEntities(renderer, ship.get(), background.get(), asteroids, scoreTable.get(), NULL);
#else
	handleCollisions(ship.get(), asteroids);
	drawEntities(renderer, ship.get(), background.get(), asteroids);
#endif


}

void doGameOverCase(int &event, unsigned int &lastAsteroidTime)
{
	if (event == KEY_P_PRESSED)
	{
		// Reset the positions of the asteroids
		for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
			asteroids[i].get()->reset();

		// Reset the ship
		ship.get()->reset();

#ifndef BUILDING_PSP_EBOOT
		// Turn on the scoring table
		scoreTable.get()->reset();
		scoreTable.get()->activate();
#else
		currentScore.asteroids = 0;
		currentScore.lives = NUMBER_PLAYER_LIVES;
#endif

		state = GAME;

		// Another hack; we don't want to return because otherwise the new score table
		// won't be redrawn
		// return; // Move to the game state
	}

	// Fire an asteroid if enough time has passed
	if (lastAsteroidTime + ASTEROID_INTERVAL <= SDL_GetTicks())
	{
#ifdef USE_CPP_RANDOM
		ASTEROID_INTERVAL = distr(eng);
#else
		int randomNum = rand() % ASTEROID_INTERVAL_RANGE_MIN + (ASTEROID_INTERVAL_RANGE_MAX + 1);
		ASTEROID_INTERVAL = randomNum;
#endif
		lastAsteroidTime = SDL_GetTicks();
		activateAsteroid();
	}

	ship.get()->update(NONE, NONE);
	updateEnemies(asteroids);

#ifndef BUILDING_PSP_EBOOT
	drawEntities(renderer, ship.get(), background.get(), asteroids, scoreTable.get(), menuScreen.get());
#else
	drawEntities(renderer, ship.get(), background.get(), asteroids);
#endif
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

#ifndef BUILDING_PSP_EBOOT
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
#else
int initSDL(SDL_Window **window, SDL_Renderer **renderer)
	{
		SDL_Window *tempWin = NULL;
		SDL_Renderer *tempRen = NULL;

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
			std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
			return 1;
		}

		// Create the window that will hold our game
		tempWin = SDL_CreateWindow("Rocket Shooter!", 100, 100, GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
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
#endif

void updateEnemies(std::vector<AsteroidPtr> &asteroids)
{
	for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
		asteroids[i].get()->update();
}

#ifndef BUILDING_PSP_EBOOT
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
						currAsteroid.get()->deactivate();
						currBullet.get()->deactivate();

						// record the score
						scoreTable->shootAsteroid();
						break;
					}
				}
			}
		}
	}
#else
	void handleCollisions(Ship *player, std::vector<AsteroidPtr> &asteroids)
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
						currAsteroid.get()->deactivate();
						currBullet.get()->deactivate();
						break;
					}
				}
			}
		}
	}
#endif

#ifndef BUILDING_PSP_EBOOT
	void drawEntities(SDL_Renderer *renderer, Ship *player, 
						Background *bg, std::vector<AsteroidPtr> &asteroids, 
						Stats *scoreTable, MenuScreen *menuScreen)
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

		if (scoreTable != nullptr)
		{
			scoreTable->draw();
		}

		if (menuScreen != nullptr)
		{
			menuScreen->draw();
		}

		// Present everything to be rendered
		SDL_RenderPresent(renderer);
	}
#else
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
#endif

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