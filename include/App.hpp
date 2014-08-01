#ifndef APP_HPP
#define APP_HPP

#include <SDL.h>
#include <SDL_image.h> // For loading textures from pngs
#include <iostream>

#include "Utils.hpp"
#include "Sprite.hpp"
#include "Ship.hpp"
#include "Background.hpp"

/**
  * This is the main class for handling the
  * SDL inititation code and the main game loop
  */
class App
{
public:

	App();
	~App();

	int Execute();
	int Loop();

private:

	void InitGameObjects(); // create the ship and the array of enemies

	int ProcessInput();
	int ProcessEvents();
	int UpdateScene();

	Ship *mPlayer;
	Background *mBg;

	SDL_Window *mWindow; // The window to house the game
	SDL_Renderer *mRenderer; // The rendering context

	bool mRunning; // The game loop flag
};

#endif