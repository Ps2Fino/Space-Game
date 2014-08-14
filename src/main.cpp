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
#include "Stats.hpp"
#include "SoundFX.hpp"
#include "MenuScreen.hpp"
#include "AsteroidManager.hpp"
#include "LifeManager.hpp"

///////////////////////////////////////////
//// Core functions for the state machine /
///////////////////////////////////////////

void doMenuCase(int &event, unsigned int &lastAsteroidTime);
void doGameCase(int &event, int &fireEvent, unsigned int &lastAsteroidTime);
void doGameOverCase(int &event, unsigned int &lastAsteroidTime);

///////////////////////////////////////////
//// Core functions for main //////////////
///////////////////////////////////////////

int initSDL(SDL_Window **window, SDL_Renderer **renderer, TTF_Font **font);

void handleInput(int &playerEvent, int &fireEvent);

// Simple collision checker for everything
void handleCollisions(Ship *player, AsteroidManager *asteroidManager,
							LifeManager *lifeManager, Stats *scoreTable);

void drawEntities(SDL_Renderer *renderer, Ship *player, 
					Background *bg, AsteroidManager *asteroidManager,
					LifeManager *lifeManager, Stats *scoreTable, MenuScreen *menuScreen);

int loadLevel(int argc, char **argv);

#ifdef ANDROID_BUILD
#ifdef __cplusplus
	extern "C"
	{
		int handleAndroidLifeCycleEvent(void *userData, SDL_Event *event);
	}
#endif
#endif

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
TTF_Font *font = NULL;

ShipPtr ship;
BackgroundPtr background;
StatsPtr scoreTable;
MenuScreenPtr menuScreen;
AsteroidManagerPtr asteroidManager;
LifeManagerPtr lifeManager;

bool running; // The main program loop variable
#ifdef ANDROID_BUILD
	bool centerTouched = false; // Detect whether or not the screen was touched
	bool isPaused = false;
#endif

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

#ifdef ANDROID_BUILD
	SDL_SetEventFilter(handleAndroidLifeCycleEvent, NULL);
#endif


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

	status = SoundFX::loadMusic();
	if (status != 0)
		return 1;

	// The android life cycle will handle the calls to stop and
	// start the bg music when the application has initialialized but I
	// explicitly start it here
	SoundFX::startMusic();

	// Create the background and the ship
	std::string bgImagePath = getResourcePath() + "img/" + levels[level];
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

	// Create the asteroid manager
	asteroidManager = AsteroidManagerPtr(new AsteroidManager(renderer, scoreTable.get()));

	// Create the life manager
	lifeManager = LifeManagerPtr(new LifeManager(renderer, scoreTable.get()));

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
#ifdef ANDROID_BUILD
		if (isPaused)
		{
			SDL_Delay(300);
			continue;
		}
#endif
		Uint32 frameStartTime = SDL_GetTicks();

		handleInput(gameEvent, fireEvent);

		// Special input. Regardless of what state we are in, this should exit the game
		if (gameEvent == KEY_ESCAPE_PRESSED
			|| gameEvent == GAME_QUIT)
		{
			running = false;
			continue; // Short circuit the rest of the function
		}

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
	TTF_CloseFont(font);
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
#ifndef ANDROID_BUILD
	if (event == KEY_P_PRESSED)
#else
	if (centerTouched)
#endif
	{
		asteroidManager.get()->reset();

		// Turn on the scoring table
		scoreTable.get()->reset();
		scoreTable.get()->activate();

		state = GAME;
		return; // Move to the game state
	}

	// We'll come back to this effect some day
//	// Fire an asteroid if enough time has passed
//	if (lastAsteroidTime + ASTEROID_INTERVAL <= SDL_GetTicks())
//	{
//#ifdef USE_CPP_RANDOM
//		ASTEROID_INTERVAL = distr(eng);
//#else
//		int randomNum = rand() % ASTEROID_INTERVAL_RANGE_MIN + (ASTEROID_INTERVAL_RANGE_MAX + 1);
//		ASTEROID_INTERVAL = randomNum;
//#endif
//		lastAsteroidTime = SDL_GetTicks();
//		activateAsteroid();
//	}

	// In the menu case, we just display the asteroids moving across the screen
	// and display the "press space to play button"
	asteroidManager.get()->update();
	drawEntities(renderer, NULL, background.get(), asteroidManager.get(), NULL, NULL, menuScreen.get());
}

void doGameCase(int &event, int &fireEvent, unsigned int &lastAsteroidTime)
{
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

		// Turn off the life if its showing
		lifeManager.get()->deactivateLife();

		// Move to the game over state
		state = GAME_OVER;

		// Quick hack because the event was passed by reference. If the user doesn't press anthing
		// the game over screen will never be played because the event will be P_PRESSED which will just automatically
		// start the next game
		event = NONE;
		return;
	}

	// Update the player ship and the asteroid belt
	ship.get()->update(event, fireEvent);
	asteroidManager.get()->update();
	lifeManager.get()->update();

	// Check collisions here
	handleCollisions(ship.get(), asteroidManager.get(), lifeManager.get(), scoreTable.get());

	// Draw stuff
	drawEntities(renderer, ship.get(), background.get(),
					asteroidManager.get(), lifeManager.get(), scoreTable.get(), NULL);
}

void doGameOverCase(int &event, unsigned int &lastAsteroidTime)
{
#ifndef ANDROID_BUILD
	if (event == KEY_P_PRESSED)
#else
	if (centerTouched) // if (centerTouched = true) is a fun bug. See if you can figure out the logic :-)
#endif
	{
		// Reset the asteroid belt
		asteroidManager.get()->reset();

		// Reset the ship
		ship.get()->reset();

		// Turn on the scoring table
		scoreTable.get()->reset();
		scoreTable.get()->activate();

		state = GAME;

		// Another hack; we don't want to return because otherwise the new score table
		// won't be redrawn
		// return; // Move to the game state
	}

	// Fire an asteroid if enough time has passed
//	if (lastAsteroidTime + ASTEROID_INTERVAL <= SDL_GetTicks())
//	{
//#ifdef USE_CPP_RANDOM
//		ASTEROID_INTERVAL = distr(eng);
//#else
//		int randomNum = rand() % ASTEROID_INTERVAL_RANGE_MIN + (ASTEROID_INTERVAL_RANGE_MAX + 1);
//		ASTEROID_INTERVAL = randomNum;
//#endif
//		lastAsteroidTime = SDL_GetTicks();
//		activateAsteroid();
//	}

	ship.get()->update(NONE, NONE);
	asteroidManager.get()->update();
	drawEntities(renderer, ship.get(), background.get(),
				 asteroidManager.get(), NULL, scoreTable.get(),
				 menuScreen.get());
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

#ifndef ANDROID_BUILD
	// Create the window that will hold our game
	tempWin = SDL_CreateWindow("Rocket Shooter!", 100, 100,
									GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT, 0); // SDL_WINDOW_SHOWN is ignored
#else
	tempWin = SDL_CreateWindow(NULL, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
									0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
#endif
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

#ifdef ANDROID_BUILD
	SDL_RenderSetLogicalSize(tempRen, 640, 480);
#endif

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

void handleCollisions(Ship *player, AsteroidManager *asteroidManager,
						LifeManager *lifeManager, Stats *scoreTable)
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

					// record the score
					scoreTable->shootAsteroid();
					break;
				}
			}
		}
	}


	//////////////////////////////////////////////////////////////////////
	////// Detect the life hit ///////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	// Loop over all the bullets and check for collisions against the life
	SDL_Rect lifeRect = lifeManager->getLife()->getSize();
	std::vector<BulletPtr> bullets = player->getBullets();

	for (std::vector<BulletPtr>::iterator bulletIt = bullets.begin(); bulletIt != bullets.end(); ++bulletIt)
	{
		BulletPtr currBullet = *bulletIt;
		if (!currBullet.get()->checkIsActivated())
			continue;

		SDL_Rect bulletRect = currBullet.get()->getSize();

		// Check for a collision with the life
		// Check for collision with an asteroid
		if (lifeRect.x >= bulletRect.x
				&& lifeRect.x <= bulletRect.x + BULLET_COLLISION_WIDTH)
		{
			if (lifeRect.y >= bulletRect.y - ASTEROID_OFFSET
					&& lifeRect.y <= bulletRect.y + BULLET_COLLISION_HEIGHT)
			{
				static int numCalls = 0;
				SDL_Log("The bullet shot a life %d time(s)\n", numCalls++);
				// Add the lives
				lifeManager->shootLife();
				break;
			}
		}
	}
}

void drawEntities(SDL_Renderer *renderer, Ship *player, 
					Background *bg, AsteroidManager *asteroidManager,
					LifeManager *lifeManager, Stats *scoreTable, MenuScreen *menuScreen)
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

	if (scoreTable != nullptr)
	{
		scoreTable->draw();
	}

	if (lifeManager != nullptr)
	{
		lifeManager->drawLife();
	}

	if (menuScreen != nullptr)
	{
		menuScreen->draw();
	}

	// Present everything to be rendered
	SDL_RenderPresent(renderer);
}

/**
  * This game is very simple. It only accepts
  * 4 buttons as input: Up, down, space bar and escape
  * This function checks which one was pressed and acts accordingly
  */
#ifndef ANDROID_BUILD
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
#else
	void handleInput(int &gameEvent, int &fireEvent)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
//			SDL_Log("The finger index ID: %d", event.tfinger.fingerId);
			switch(event.type)
			{
				case SDL_FINGERDOWN:
					// If we touched the location of one of the soft buttons,
					// then trigger the movement for the ship
					if (event.tfinger.y < 0.5
							&& event.tfinger.x <= 0.3)
					{
						gameEvent = KEY_UP_PRESSED;
					}

					if (event.tfinger.y >= 0.5
								&& event.tfinger.x <= 0.3)
					{
						gameEvent = KEY_DOWN_PRESSED;
					}

					if (event.tfinger.y >= 0.7
								&& event.tfinger.x >= 0.7)
					{
						fireEvent = KEY_SPACE_PRESSED;
					}

					// Touching the center of the screen will activate the menu transitions
					if (event.tfinger.y >= 0.4
							&& event.tfinger.y < 0.6
							&& event.tfinger.x >= 0.4
							&& event.tfinger.x < 0.6)
					{
						centerTouched = true;
					}

					break;

				case SDL_FINGERUP:
					if (event.tfinger.y < 0.5
							&& event.tfinger.x <= 0.3)
					{
						gameEvent = KEY_RELEASED;
					}

					if (event.tfinger.y >= 0.5
								&& event.tfinger.x <= 0.3)
					{
						gameEvent = KEY_RELEASED;
					}

					if (event.tfinger.y >= 0.7
								&& event.tfinger.x >= 0.7)
					{
						fireEvent = KEY_RELEASED;
					}

					centerTouched = false;
					break;

				case SDL_QUIT:
					gameEvent = GAME_QUIT;
					break;

				default:
					break;
			}
		}
	}
#endif

/**
 * The following functions
 * are only used in the android build
 * of the game
 */
#ifdef ANDROID_BUILD

#ifdef __cplusplus
	extern "C"
	{
		int handleAndroidLifeCycleEvent(void *userData, SDL_Event *event)
		{
			SDL_Log("Handling the life cycle event");
			switch(event->type)
			{
				case SDL_APP_WILLENTERFOREGROUND:
					SDL_Log("Entering foreground in lifecycle");
					SoundFX::startMusic();
					isPaused = false; // Unpause the game
					break;

				case SDL_APP_WILLENTERBACKGROUND:
					SDL_Log("Entering background in lifecycle");
					SoundFX::stopMusic();
					isPaused = true;
					break;

				case SDL_APP_TERMINATING:
					SDL_Log("Terminating in lifecycle");
					SoundFX::stopMusic();
					isPaused = true;
					running = false;
					break;

				default:
					break;
			}

			return 1;
		}

		// Declare some JNI functions
		#include <jni.h>

		JNIEXPORT void JNICALL Java_org_dandan_rocket_MyActivity_pauseMusic(JNIEnv*, jobject);
		JNIEXPORT void JNICALL Java_org_dandan_rocket_MyActivity_resumeMusic(JNIEnv*, jobject);
		JNIEXPORT void JNICALL Java_org_dandan_rocket_MyActivity_stopMusic(JNIEnv*, jobject);

		JNIEXPORT void JNICALL Java_org_dandan_rocket_MyActivity_stopMusic(JNIEnv *env, jobject obj)
		{
			SoundFX::stopMusic();
		}

		JNIEXPORT void JNICALL Java_org_dandan_rocket_MyActivity_pauseMusic(JNIEnv *env, jobject obj)
		{
			SoundFX::pauseMusic();
		}

		JNIEXPORT void JNICALL Java_org_dandan_rocket_MyActivity_resumeMusic(JNIEnv *env, jobject obj)
		{
			SoundFX::resumeMusic();
		}
	}
#endif
#endif
