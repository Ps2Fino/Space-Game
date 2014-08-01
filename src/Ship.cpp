#include "Ship.hpp"

#ifdef __APPLE__
#include <dispatch/dispatch.h> // for gcd on the Mac
#endif

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
	if (mRenderer == nullptr)
	{
		std::cout << "The renderer is null" << std::endl;
	}

	if (mTex == nullptr)
	{
		std::cout << "The textur is null" << std::endl;
	}
#ifdef __APPLE__
	std::cout << "In the apple clause" << std::endl;
	std::cout << "About to draw the Ship" << std::endl;
	dispatch_async(dispatch_get_main_queue(), ^{
		SDL_RenderCopy(this->mRenderer, this->mTex, NULL, &this->mShape);
	});
#endif
}