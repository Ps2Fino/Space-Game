#include "Ship.hpp"

Ship::Ship(SDL_Renderer *renderer, std::string &imagePath,
				int width, int height, int x, int y) 
					: Sprite(renderer, imagePath, width, height, x, y), mVelocity(DEFAULT_VELOCITY)
{
}

Ship::~Ship()
{
}

void Ship::setMovementBoundary(int top, int bottom)
{
	mTopBoundary = top;
	mBottomBoundary = bottom - mShape.h;
}

void Ship::update(EVENT ev)
{
	int tempY = mY_pos;
	switch(ev)
	{
		case UP:
			mY_pos -= 1 * mVelocity;
			break;

		case DOWN:
			mY_pos += 1 * mVelocity;
			break;

		default:
			break;
	}

	// Update the box we live in based on the boundary set
	if (mY_pos > mTopBoundary
			&& mY_pos < mBottomBoundary)
	{
		mShape.y = mY_pos;
	}
	else
	{
		mY_pos = tempY;
	}
}

void Ship::draw()
{
	// Just draw the surface at the current co-ordinates
	SDL_RenderCopy(mRenderer, mTex, NULL, &mShape);
}