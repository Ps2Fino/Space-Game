#include "Asteroid.hpp"
#include "Stats.hpp"
#include "RSConstants.hpp"

#include "AsteroidManager.hpp"

// Initialize the static texture variable
SDL_Texture* Asteroid::asteroidTexture = NULL;

#ifdef USE_CPP_RANDOM
	#include <random> // For generating random numbers
#else
	#include <cstdlib> // For generating random numbers
#endif

Asteroid::Asteroid(SDL_Renderer *renderer, AsteroidManager *parentManager, int velocity)
				: Destructible(renderer, ASTEROID_WIDTH, ASTEROID_HEIGHT, -ASTEROID_WIDTH, 0),
				mVelocity(velocity)
{
	// We've 8 to choose from
	mTextureRegion.x = 64 * 3; // Lets pick the 3rd row across on the sprite sheet
	mTextureRegion.y = 64 * 5; // Lets pick the 5th column down on the sprite sheet
	mTextureRegion.w = ASTEROID_WIDTH;
	mTextureRegion.h = ASTEROID_HEIGHT;
	mIsActive = true;
	mParentManager = parentManager;
}

Asteroid::~Asteroid()
{
} // No need to do anything. The program (probably main somewhere) should delete anything important

void Asteroid::activate(int xPos, int yPos)
{
	if (!mIsActive)
	{
		// The edge of the game sceen plus the width of the asteroid sprite
		mX_pos = GAME_WINDOW_WIDTH + ASTEROID_WIDTH;

		// Send me somewhere random
		mY_pos = rand() % ((GAME_WINDOW_HEIGHT - ASTEROID_HEIGHT) + 1);

		mShape.x = mX_pos;
		mShape.y = mY_pos;
		mIsActive = true;
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

			// The player should lose a life here
			mParentManager->recordAsteroidMissed();
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
