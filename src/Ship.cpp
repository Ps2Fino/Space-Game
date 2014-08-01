#include "Ship.hpp"

Ship::Ship(SDL_Renderer *renderer, std::string &imagePath, std::string &bulletImagePath,
				int width, int height, int x, int y) 
					: Sprite(renderer, width, height, x, y), 
					mVelocity(DEFAULT_SHIP_VELOCITY)
					// mBullet(new Bullet(renderer))
{
	loadTexture(renderer, imagePath, false);

	// Let's create our bullet here
	mBulletTexture = IMG_LoadTexture(renderer, bulletImagePath.c_str());
	for (int i = 0; i < NUMBER_BULLETS; ++i)
	{
		mBullets.push_back(BulletPointer(new Bullet(renderer)));
		mBullets.back().get()->sharedTexture = mBulletTexture;
	}
} // See the Sprite class

Ship::~Ship()
{
	SDL_DestroyTexture(mTex);
	SDL_DestroyTexture(mBulletTexture);

} // See the Sprite class

void Ship::setMovementBoundary(int top, int bottom)
{
	mTopBoundary = top;
	mBottomBoundary = bottom - mShape.h;
}

void Ship::fireBullet()
{
	for (int i = 0; i < NUMBER_BULLETS; ++i)
	{
		Bullet *currBullet = mBullets[i].get();
		if (!currBullet->checkIsActivated())
		{
			currBullet->activate(mX_pos, mY_pos - 10);
			break;
		}
	}
}

void Ship::update(GAME_EVENT ev)
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

	// Call update on the bullet
	// mBullet.get()->update();
	for (int i = 0; i < NUMBER_BULLETS; ++i)
		mBullets[i].get()->update();
}

void Ship::draw()
{
	// Just draw the surface at the current co-ordinates
	SDL_RenderCopy(mRenderer, mTex, NULL, &mShape);
	for (int i = 0; i < NUMBER_BULLETS; ++i)
		mBullets[i].get()->draw();
}