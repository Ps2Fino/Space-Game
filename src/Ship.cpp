#include "Ship.hpp"

Ship::Ship(SDL_Renderer *renderer, std::string &imagePath,
				int width, int height, int x, int y) 
					: Sprite(renderer, width, height, x, y), 
					mVelocity(SHIP_VELOCITY)
{
	loadTexture(renderer, imagePath);

	// Set the default boundary
	setMovementBoundary(0, GAME_WINDOW_HEIGHT);

} // See the Sprite class

Ship::~Ship()
{
	SDL_DestroyTexture(mTex);

} // See the Sprite class

void Ship::setMovementBoundary(int top, int bottom)
{
	mTopBoundary = top;
	mBottomBoundary = bottom - mShape.h;
}

void Ship::reset()
{
	setPosition(SHIP_START_POSITION_X, SHIP_START_POSITION_Y);
}

void Ship::update(int ev1, int ev2)
{
	/**
	  * TODO: This is the ship classes update
	  * method. Implement this to move the ship.
	  * The ship should be able to move
	  * in one-dimension only: specifically vertically
	  */
}

void Ship::draw()
{
	// Just draw the surface at the current co-ordinates
	SDL_RenderCopy(mRenderer, mTex, NULL, &mShape);
}