#include "Ship.hpp"

Ship::Ship(SDL_Renderer *renderer, std::string &imagePath,
				int width, int height, int x, int y) 
					: Sprite(renderer, imagePath, width, height, x, y), mVelocity(DEFAULT_VELOCITY)
{
}

Ship::~Ship()
{
}

void Ship::update(EVENT ev)
{
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

	// Update the box we live in
	mShape.x = mX_pos;
	mShape.y = mY_pos;
}

void Ship::draw()
{
	// Just draw the surface at the current co-ordinates
	SDL_RenderCopy(mRenderer, mTex, NULL, &mShape);
}