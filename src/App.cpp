#include "App.hpp"

App::App()
{
}

App::~App()
{
	delete mPlayer;
	delete mBg;

	// Cleanup
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();

	mPlayer = NULL;
	mBg = NULL;
	mRenderer = NULL;
	mWindow = NULL;
}

/**
  * This method is responsible for inititiating the SDL 
  */
void App::Execute()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return;
	}

	// Create the window that will hold our game
	mWindow = SDL_CreateWindow("Rocket Shooter!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (mWindow == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return;
	}

	// Next, we need a renderer to render the assets to the screen
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (mRenderer == nullptr)
	{
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(mWindow);
		SDL_Quit();
		return;
	}

	InitGameObjects();
	return;
}

void App::InitGameObjects()
{
	// Load the first level for the game
	std::string bgImagePath = getResourcePath() + "lv1-bg.png";
	mBg = new Background(mRenderer, bgImagePath);

	// Create a little ship sprite and animate it
	std::string shipImagePath = getResourcePath() + "ship.png";
	mPlayer = new Ship(mRenderer, shipImagePath);
}

int App::ProcessInput()
{
	SDL_Event event;
	EVENT myEvent; // For controlling the ship
	std::cout << "About to call SDL_PollEvent" << std::endl;
	while (SDL_PollEvent(&event))
	{
		std::cout << "Looping on poll event" << std::endl;
		switch(event.type)
		{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_UP:
						myEvent = UP;
						break;

					case SDLK_DOWN:
						myEvent = DOWN;
						break;

					case SDLK_ESCAPE:
						mRunning = false;
						break;
				}
				break;

			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
					default:
						myEvent = NONE;
						break;
				}
				break;

			case SDL_QUIT:
				mRunning = false;
				break;

			default:
				break;
		}
	}
	mPlayer->update(myEvent);
	return 0;
}

int App::ProcessEvents()
{
	// TODO: Implement this
	return 0;
}

int App::UpdateScene()
{
	SDL_RenderClear(mRenderer);

	mBg->draw();
	mPlayer->draw();	

	// TODO: Draw other game objects here

	SDL_RenderPresent(mRenderer);
	return 0;
}

void App::Loop()
{
	std::cout << "The main loop was entered" << std::endl;

	mRunning = true;
	while (mRunning)
	{
		ProcessInput();
		std::cout << "Input processed" << std::endl;
		ProcessEvents();
		std::cout << "Events processed" << std::endl;
		UpdateScene();
		std::cout << "Scene updated" << std::endl;
	}
	return;
}