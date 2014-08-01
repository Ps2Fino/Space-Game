#ifndef BULLET_HPP
#define BULLET_HPP

#include <SDL.h>
#include <SDL_image.h>

#include "Sprite.hpp"

#define BULLET_WIDTH 95
#define BULLET_HEIGHT 68
#define DEFAULT_BULLET_VELOCITY 10

/**
  * This class encapsulates a bullet.
  * A bullet has a shared sprite resource for
  * drawing and has a simple update function; move across the screen
  */
class Bullet : public Sprite
{
public:

	Bullet(SDL_Renderer *renderer, SDL_Texture *bulletTexture = NULL, 
				int type = 0, int width = BULLET_WIDTH, int height = BULLET_HEIGHT,
				int x = 320, int y = 240);
	~Bullet();

	int getVelocity() { return mVelocity; }
	bool checkIsActive() { return mIsActive; }
	void activate(int xPos, int yPos); // Activate a bullet at a spcific position

	// inherited methods
	void update(GAME_EVENT event = NONE);
	void draw();

	SDL_Texture *sharedTexture; // This pointer should never be initiated by the bullet! It is a shared resource

private:

	bool mIsActive;
	int mVelocity;
	SDL_Rect mTextureRegion;
};

#endif