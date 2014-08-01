#include "App.hpp"

App::App()
{
	mRunning = true;
}

App::~App()
{
	// delete mPlayer;
	// delete mBg;

	// // Cleanup
	// SDL_DestroyRenderer(mRenderer);
	// SDL_DestroyWindow(mWindow);
	// SDL_Quit();

	// mPlayer = NULL;
	// mBg = NULL;
	// mRenderer = NULL;
	// mWindow = NULL;
}

void App::CleanUp()
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
void App::Init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
	}

	// Create the window that will hold our game
	mWindow = SDL_CreateWindow("Rocket Shooter!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (mWindow == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
	}

	// Next, we need a renderer to render the assets to the screen
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (mRenderer == nullptr)
	{
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(mWindow);
		SDL_Quit();
	}

	InitGameObjects();

	// install the timer
	mTimer = SDL_AddTimer(30, LoopTimer, this);
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

Uint32 App::LoopTimer(Uint32 interval, void *param)
{
    // Create a user event to call the game loop.
    SDL_Event event;
    
    event.type = SDL_USEREVENT;
    event.user.code = RUN_GAME_LOOP;
    event.user.data1 = 0;
    event.user.data2 = 0;
    
    SDL_PushEvent(&event);
    
    return interval;
}

void App::EventLoop()
{
	ProcessInput();
}

void App::ProcessInput()
{
	SDL_Event event;
	EVENT myEvent; // For controlling the ship
	while (mRunning && SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_USEREVENT:
				HandleUserEvent(&event);
				break;

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
}

void App::HandleUserEvent(SDL_Event *event)
{
	switch (event->user.code)
	{
		case RUN_GAME_LOOP:
			UpdateScene();
			break;

		default:
			break;
	}
}

void App::ProcessEvents()
{
	// TODO: Implement this
}

void App::UpdateScene()
{
	SDL_RenderClear(mRenderer);

	mBg->draw();
	mPlayer->draw();

	// TODO: Draw other game objects here

	SDL_RenderPresent(mRenderer);
}