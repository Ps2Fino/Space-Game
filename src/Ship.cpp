#include "Ship.hpp"

Ship::Ship(int width, int height, SDL_Renderer *renderer, std::string &imagePath) : Sprite(renderer, imagePath)
{
	setSize(width, height);
	mX_pos = 20;
	mY_pos = 240;
}

Ship::~Ship()
{
}

void Ship::update(EVENT ev)
{
	switch(ev)
	{
		case UP:
			mY_pos -= 10;
			break;

		case DOWN:
			mY_pos += 10;
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