#include "Destructible.hpp"

Destructible::Destructible(SDL_Renderer *renderer,
							int width, int height, int x, int y)
				: Sprite(renderer, width, height, x, y)
{
	mIsActive = false;
	mVelocity = 0;
}

Destructible::~Destructible()
{
}