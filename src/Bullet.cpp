#include "Bullet.hpp"

Bullet::Bullet(SDL_Renderer *renderer, SDL_Texture *texture,
				int type, int x, int y)
				: Sprite(renderer, BULLET_WIDTH, BULLET_HEIGHT, x, y),
				sharedTexture(texture),
				mIsActive(false),
				mVelocity(BULLET_VELOCITY)
{
	// Create the source rectangle for the texture
	switch (type)
	{
		case 0:
			mTextureRegion.x = 0;
			break;

		case 1:
			mTextureRegion.x = 1 * BULLET_WIDTH;
			break;

		case 2:
			mTextureRegion.x = 2 * BULLET_WIDTH;
			break;

		default:
			mTextureRegion.x = 0; // Default to the first bullet
			break;
	}
	mTextureRegion.y = 0;
	mTextureRegion.w = BULLET_WIDTH;
	mTextureRegion.h = BULLET_HEIGHT;
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

		mShape.x = mX_pos;
		mShape.y = mY_pos;
	}

}

void Bullet::update(GAME_EVENT ev)
{
	// Couldn't be simpler :-)
	if (mIsActive)
	{
		mX_pos += 1 * mVelocity;
		mShape.x = mX_pos;
		if (mX_pos > GAME_WINDOW_WIDTH)
			mIsActive = false;
	}
}

void Bullet::draw()
{
	if (mIsActive)
		SDL_RenderCopy(mRenderer, sharedTexture, &mTextureRegion, &mShape);
}