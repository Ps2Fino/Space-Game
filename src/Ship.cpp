#include "Ship.hpp"

#include <random> // For random bullet colors
#include "SoundFX.hpp" // For the bullet sounds

Ship::Ship(SDL_Renderer *renderer, std::string &imagePath, std::string &bulletImagePath,
				int width, int height, int x, int y) 
					: Sprite(renderer, width, height, x, y), 
					mVelocity(SHIP_VELOCITY), bulletInterval(BULLET_INTERVAL),
					lastBulletTime(0), quickFire(true)
{
	loadTexture(renderer, imagePath);

	// Grab a uni distro generator
	std::random_device rd; // obtain a random number from hardware
    std::mt19937 eng(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, 2); // define the range

	// Let's create our bullet here
	mBulletTexture = IMG_LoadTexture(renderer, bulletImagePath.c_str());
	Bullet::bulletTexture = mBulletTexture; // Set the shared texture for all the bullets
	for (int i = 0; i < NUMBER_BULLETS; ++i)
	{
		// Vary the bullet colors
		int bulletColor = distr(eng);
		mBullets.push_back(BulletPtr(new Bullet(renderer, bulletColor))); // Make yellow bullets
	}

	// Set the default boundary
	setMovementBoundary(0, GAME_WINDOW_HEIGHT);

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
	if (quickFire)
	{
		if (lastBulletTime + bulletInterval <= SDL_GetTicks())
		{
			lastBulletTime = SDL_GetTicks();
			for (int i = 0; i < NUMBER_BULLETS; ++i)
			{
				Bullet *currBullet = mBullets[i].get();
				if (!currBullet->checkIsActivated())
				{
					currBullet->activate(mX_pos, mY_pos - 10);
					// Play the sound effect
					SoundFX::playLaserSound();
					break;
				}
			}
		}
	}
	else
	{
		Bullet *currBullet = mBullets[0].get(); // We always only fire the first bullet if its not active already
		if (!currBullet->checkIsActivated())
		{
			currBullet->activate(mX_pos, mY_pos - 10);
			SoundFX::playLaserSound();
		}
	}
}

void Ship::update(int ev1, int ev2)
{
	// std::cout << "Ship::update GAME_EVENT ev = " << ev << std::endl;
	int tempY = mY_pos;

	switch(ev1)
	{
		case KEY_UP_PRESSED:
			mY_pos -= 1 * mVelocity;
			break;

		case KEY_DOWN_PRESSED:
			mY_pos += 1 * mVelocity;
			break;

		default:
			break;
	}

	if (ev2 == KEY_SPACE_PRESSED)
		fireBullet();

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

	// std::cout << "Ship::update called successfully" << std::endl;
}

void Ship::draw()
{
	// Just draw the surface at the current co-ordinates
	SDL_RenderCopy(mRenderer, mTex, NULL, &mShape);
	for (int i = 0; i < NUMBER_BULLETS; ++i)
		mBullets[i].get()->draw();
}