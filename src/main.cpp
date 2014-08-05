#include <SDL.h>
#include <SDL_image.h> // For loading textures from pngs
#include <iostream>

#include "RSConstants.hpp"
#include "boost/shared_ptr.hpp" // For smart pointers and the like
#include "Utils.hpp"
#include "Sprite.hpp"
#include "Ship.hpp"
#include "Background.hpp"

///////////////////////////////////////////
//// Core functions for main //////////////
///////////////////////////////////////////

int initSDL(SDL_Window **window, SDL_Renderer **renderer);
void handleInput(int &playerEvent, int &fireEvent);
void drawEntities(SDL_Renderer *renderer, Ship *player, Background *bg);
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

bool running; // The main program loop variable
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
	ship = ShipPtr(new Ship(renderer, shipImagePath));

#ifdef LOGGING_FPS
	int countedFrames = 0;
	Uint32 fpsStartTime = SDL_GetTicks();
#endif

	int gameEvent = NONE;
	int fireEvent = NONE;
	running = true;

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

		/**
		  * TODO: Your task here is to make the ship move up and down
		  */

		  // Draw the current frame
		  drawEntities(renderer, ship.get(), background.get());

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

	// Cleanup everything. The ship and background will clean themselves
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

void drawEntities(SDL_Renderer *renderer, Ship *ship, Background *bg)
{
	// Now draw the frame
	SDL_RenderClear(renderer);

	if (bg != nullptr)
	{
		bg->draw();
	}

	if (ship != nullptr)
	{
		ship->draw();
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
	/**
	  * TODO: This function is part implemented to
	  * allow the application to close when ESC is pressed.
	  * Complete it to grab the latest event from SDL in order
	  * to move the ship. Remember that the gameEvent and fireEvent
	  * variables are passed by reference
	  */

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						gameEvent = KEY_ESCAPE_PRESSED;
						break;

					default:
						break;
				}
				break;

			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						gameEvent = KEY_RELEASED;
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