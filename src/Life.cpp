#include "Life.hpp"
#include "RSConstants.hpp"

SDL_Texture* Life::lifeTexture = NULL;

Life::Life(SDL_Renderer *renderer, int width, int height, int x, int y)
	: Destructible(renderer, LIFE_WIDTH, LIFE_HEIGHT, 0, 0)
{
	// Give it a random value when it is spawned
	mValue = rand() % 5 + 1;
//	mValue = 1;
}

Life::~Life()
{
} // Main will handle our shared texture

void Life::activate(int x, int y)
{
	// First, pick whether we want to float up or down
	int direction = rand() % 1 + 1;
	if (direction % 1)
	{
		mVelocity = -1 * LIFE_VELOCITY;
		mY_pos = GAME_WINDOW_HEIGHT;
	}
	else
	{
		mVelocity = LIFE_VELOCITY;
		mY_pos = 0;
	}

	// Now pick the spawn location
	mX_pos = rand() % (GAME_WINDOW_WIDTH - LIFE_WIDTH);
	if (mX_pos < SHIP_WIDTH) mX_pos += SHIP_WIDTH; // Stop the heart appearing off the screen

	// Set the position
	setPosition(mX_pos, mY_pos);

	// And we're off!!!
	mIsActive = true;
}

void Life::update(int ev1, int ev2)
{
	if (mIsActive)
	{
		mY_pos += mVelocity;
		mShape.y = mY_pos;
		if (mY_pos + LIFE_HEIGHT <= 0)
			deactivate();
	}
}

void Life::reset()
{
	mVelocity = 0;
	mY_pos = -1 * LIFE_WIDTH;
	mX_pos = 0;
	mShape.x = mX_pos;
	mShape.y = mY_pos;
	
	// Give it a random value
	mValue = rand() % 5 + 1;
//	mValue = 1;
	mIsActive = false;
}

void Life::draw()
{
	if (mIsActive)
	{
		SDL_RenderCopy(mRenderer, lifeTexture, NULL, &mShape);
	}
}
