#include <SDL.h>
#include <SDL_image.h> // For loading textures from pngs
#include <iostream>
#include <vector> // For the std::vector data structure
#include <random> // For random bullet colors
#include <SDL_ttf.h> // For drawing text to the screen

#include "boost/shared_ptr.hpp" // For smart pointers and the like
#include "Utils.hpp"
#include "Sprite.hpp"
#include "Ship.hpp"
#include "Background.hpp"
#include "Bullet.hpp"
#include "Asteroid.hpp"

int initSDL(SDL_Window **window, SDL_Renderer **renderer, TTF_Font **font);

void handleInput(bool &running, GAME_EVENT &playerEvent, bool &fire);
void updateEnemies(std::vector<AsteroidPtr> &asteroids); // Update the vector of asteroids
void handleCollisions(Ship &player, std::vector<AsteroidPtr> &asteroids); // Simple collision checker for everything
void drawEntities(SDL_Renderer *renderer, Ship &player, 
					Background &bg, std::vector<AsteroidPtr> &asteroids, SDL_Texture *statsTexture);

void activateAsteroid();
int loadLevel(int argc, char **argv);

const char *levels[4] = {
	"lv1-bg.png",
	"lv2-bg.png",
	"lv3-bg.png",
	"lv4-bg.png"
};

typedef struct stats {
	int num_lives;
	int num_asteroids_shot;
} Stats;

// Global variable for the list of asteroids. This is bad practice, don't do this. My excuse is its 01:32 am
std::vector<AsteroidPtr> asteroids;

// The text location for the statistics text (game score)
SDL_Rect textLocation = { GAME_WINDOW_WIDTH - 350, 20, 300, 50 };

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
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	TTF_Font *font = NULL;
	int status = initSDL(&window, &renderer, &font);
	if (status != 0)
		return 1;

	// Create the font foreground and background colors, and the text location
	SDL_Color fgColor = { 0, 0, 0 };
	SDL_Color bgColor = { 255, 255, 255 };

	// Default surface for the text
	SDL_Surface *statsSurface = TTF_RenderText_Shaded(font, "5 lives. 0 asteroids shot",
																fgColor, bgColor);
	SDL_Texture *statsTexture = SDL_CreateTextureFromSurface(renderer, statsSurface);

	// Create the background and the ship
	std::string bgImagePath = getResourcePath() + "img/" + levels[level];
	std::cout << "Playing level: " << bgImagePath << std::endl;
	Background levelBG(renderer, bgImagePath);

	std::string shipImagePath = getResourcePath() + "img/ship.png";
	std::string bulletImagePath = getResourcePath() + "img/bullet_strip.png";
	Ship player(renderer, shipImagePath, bulletImagePath);

	// Create the enemy array
	std::string asteroidImagePath = getResourcePath() + "img/asteroid.png";
	SDL_Texture *asteroidTexture = IMG_LoadTexture(renderer, asteroidImagePath.c_str());
	Asteroid::asteroidTexture = asteroidTexture; // Set the asteroid texture
	for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
		asteroids.push_back(AsteroidPtr(new Asteroid(renderer)));

	// Init the stats struct
	Stats score;
	score.num_lives = 0;
	score.num_asteroids_shot = 0;

	// Some variables necessary for the game loop
	bool running = true;
	GAME_EVENT gameEvent; // For controlling the ship
	bool fire = false;

	// Have a delay for firing bullets
	const signed int BULLET_INTERVAL = 300;
	unsigned int lastBulletTime = 0;

	// Have a variable delay for the asteroids too
	std::random_device rd; // obtain a random number from hardware
    std::mt19937 eng(rd()); // seed the generator
    std::uniform_int_distribution<> distr(300, 1000); // define the range
	signed int ASTEROID_INTERVAL = distr(eng); // Random time interval
	unsigned int lastAsteroidTime = 0;

	while (running)
	{		
		// handleInput(running, gameEvent);
		handleInput(running, gameEvent, fire);

		// Fire a bullet if enough time has passed
		if (fire && (lastBulletTime + BULLET_INTERVAL <= SDL_GetTicks()))
		{
			lastBulletTime = SDL_GetTicks();
			player.fireBullet();
		}

		// Fire an asteroid if enough time has passed
		if (lastAsteroidTime + ASTEROID_INTERVAL <= SDL_GetTicks())
		{
			ASTEROID_INTERVAL = distr(eng);
			lastAsteroidTime = SDL_GetTicks();
			activateAsteroid();
		}

		// Update the player shio and the asteroid belt
		player.update(gameEvent);
		updateEnemies(asteroids);

		// Check collisions here
		handleCollisions(player, asteroids);

		// Draw stuff
		drawEntities(renderer, player, levelBG, asteroids, statsTexture);
	}

	// Cleanup everything. The ship, background and enemies will clean themselves
	SDL_FreeSurface(statsSurface);
	SDL_DestroyTexture(statsTexture);
	TTF_CloseFont(font);
	SDL_DestroyTexture(asteroidTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();

	renderer = NULL;
	window = NULL;
	font = NULL;
	statsSurface = NULL;
	statsTexture = NULL;

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

int initSDL(SDL_Window **window, SDL_Renderer **renderer, TTF_Font **font)
{
	SDL_Window *tempWin = NULL;
	SDL_Renderer *tempRen = NULL;
	TTF_Font *tempFont = NULL;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	if(TTF_Init() != 0) {
	    std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
	    SDL_Quit();
	    return 1;
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

	// Next we init the font
	std::string fontString = getResourcePath() + "fonts/tiptoe.ttf";
	tempFont = TTF_OpenFont(fontString.c_str(), TEXT_FONT_SIZE);
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

	return 0;
}

void updateEnemies(std::vector<AsteroidPtr> &asteroids)
{
	for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
		asteroids[i].get()->update();
}

void handleCollisions(Ship &player, std::vector<AsteroidPtr> &asteroids)
{
	for (std::vector<AsteroidPtr>::iterator asteroidIt = asteroids.begin(); asteroidIt != asteroids.end(); ++asteroidIt)
	{
		AsteroidPtr currAsteroid = *asteroidIt;

		if (!currAsteroid.get()->checkIsActivated())
			continue; // An inactive asteroid can't be hit anyways

		SDL_Rect asteroidRect = currAsteroid.get()->getSize();
		std::vector<BulletPtr> bullets = player.getBullets();
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
					break;
				}
			}
		}
	}
}

void drawEntities(SDL_Renderer *renderer, Ship &player, 
					Background &bg, std::vector<AsteroidPtr> &asteroids, SDL_Texture *statsTexture)
{
	// Now draw the frame
	SDL_RenderClear(renderer);

	bg.draw();
	
	for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
		asteroids[i].get()->draw();

	player.draw();

	// Draw the stats too
	SDL_RenderCopy(renderer, statsTexture, NULL, &textLocation);

	// Present everything to be rendered
	SDL_RenderPresent(renderer);
}

/**
  * This game is very simple. It only accepts
  * 4 buttons as input: Up, down, space bar and escape
  * This function checks which one was pressed and acts accordingly
  */
void handleInput(bool &running, GAME_EVENT &playerEvent, bool &fire)
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
						fire = true;
						break;
				}
				break;

			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
					case SDLK_SPACE:
						fire = false;
						break;

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