#ifndef APP_HPP
#define APP_HPP



/**
  * This is the main class for handling the
  * SDL inititation code and the main game loop
  */
class App
{
public:

	App();
	~App();

	void Init();
	void EventLoop();
	void CleanUp();

	static Uint32 LoopTimer(Uint32 interval, void *param);

private:

	void InitGameObjects(); // create the ship and the array of enemies
	void InstallLoopTimer(); // Internal method for installing the rendering timer
	void HandleUserEvent(SDL_Event *event); // Handles the render and any other user event

	void ProcessInput();
	void ProcessEvents();
	void UpdateScene();

	Ship *mPlayer;
	Background *mBg;

	SDL_Window *mWindow; // The window to house the game
	SDL_Renderer *mRenderer; // The rendering context
	SDL_TimerID mTimer; // The timer to fire the rendering events

	bool mRunning; // The game loop flag
};

const int RUN_GAME_LOOP = 1;

#endif