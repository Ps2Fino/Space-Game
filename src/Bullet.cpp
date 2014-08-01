#include "Bullet.hpp"

Bullet::Bullet(SDL_Renderer *renderer, SDL_Texture *bulletTexture, 
			int type, int width, int height, int x, int y)
				: Sprite(renderer, width, height, x, y)
{
	mVelocity = 5;
	mIsActive = false;

	// Create the source rectangle for the texture
	switch (type)
	{
		case 0:
			mTextureRegion.x = 0;
			break;

		case 1:
			mTextureRegion.x = 95;
			break;

		case 2:
			mTextureRegion.x = 190;

		default:
			mTextureRegion.x = 0; // Default to the first bullet
	}
	mTextureRegion.y = 0;
	mTextureRegion.w = BULLET_WIDTH;
	mTextureRegion.h = BULLET_HEIGHT;

	sharedTexture = bulletTexture;
}

Bullet::~Bullet()
{
} // No need to do anything. The ship should delete anything important

void Bullet::activate(int xPos, int yPos)
{
	if (!mIsActive)
	{
		mIsActive = true;
		mX_pos = xPos;
		mY_pos = yPos;
	}

}

void Bullet::update(GAME_EVENT ev)
{
	// Couldn't be simpler :-)
	if (mIsActive)
	{
		mX_pos += 1 * mVelocity;
		if (mX_pos > 640)
			mIsActive = false;
	}
}

void Bullet::draw()
{
	// The difference here is that we draw a particular region of the texture
	// std::cout << "The bullet shape: X: " << mShape.x
	// 			<< "\tY: " << mShape.y
	// 			<< "\tWidth: " << mShape.w
	// 			<< "\tHeight: " << mShape.h
	// 			 << std::endl;
	if (mIsActive)
		SDL_RenderCopy(mRenderer, sharedTexture, &mTextureRegion, &mShape);
}