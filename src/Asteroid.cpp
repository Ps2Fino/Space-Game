#include "Asteroid.hpp"
#include "RSConstants.hpp"

// Initialize the static texture variable
SDL_Texture* Asteroid::asteroidTexture = NULL;

#include <cstdlib> // For generating random numbers

Asteroid::Asteroid(SDL_Renderer *renderer)
				: Destructible(renderer, ASTEROID_WIDTH, ASTEROID_HEIGHT, -ASTEROID_WIDTH, 0)
{
	// We've 8 to choose from
	mTextureRegion.x = 64 * 3; // Lets pick the 3rd row across on the sprite sheet
	mTextureRegion.y = 64 * 5; // Lets pick the 5th column down on the sprite sheet
	mTextureRegion.w = ASTEROID_WIDTH;
	mTextureRegion.h = ASTEROID_HEIGHT;

	mVelocity = ASTEROID_VELOCITY;
	mIsActive = true;
}

Asteroid::~Asteroid()
{
} // No need to do anything. The program (probaly main somewhere) should delete anything important

void Asteroid::activate(int xPos, int yPos)
{
	if (!mIsActive)
	{
		mIsActive = true;
		mX_pos = GAME_WINDOW_WIDTH + ASTEROID_WIDTH; // The edge of the game sceen plus the width of the asteroid sprite

	    int randomY = rand() % ((GAME_WINDOW_HEIGHT - ASTEROID_HEIGHT) + 1);
	    mY_pos = randomY;

		mShape.x = mX_pos;
		mShape.y = mY_pos;
	}
}

void Asteroid::update(int ev1, int ev2)
{
	if (mIsActive)
	{
		mX_pos -= 1 * mVelocity;
		mShape.x = mX_pos;
		if (mX_pos < (-1 * ASTEROID_WIDTH))
		{
			deactivate();
		}
	}
}

void Asteroid::reset()
{
	mX_pos = GAME_WINDOW_WIDTH + ASTEROID_WIDTH;
	mY_pos = 0;
	mShape.x = mX_pos;
	mShape.y = mY_pos;
	mIsActive = false;
}

void Asteroid::draw()
{
	if (mIsActive)
	{
		// std::cout << "Asteroid shape: X:" << mShape.x
		//   << "\tY: " << mShape.y
		//   << "\tWidth: " << mShape.w
		//   << "\tHeight: " << mShape.h << std::endl;
		SDL_RenderCopy(mRenderer, asteroidTexture, &mTextureRegion, &mShape);
	}
}