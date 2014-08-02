#ifndef ASTEROID_HPP
#define ASTEROID_HPP

#include "RSConstants.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include "boost/shared_ptr.hpp"

#include "Sprite.hpp"

/**
  * An asteroid doens't do much.
  * It just whizzes past the screen
  */
class Asteroid : public Sprite
{
public:

	Asteroid(SDL_Renderer *renderer, SDL_Texture *texture = NULL);
	~Asteroid();

	void setXPosition(int x) { mX_pos = x; } // Set this to the screen size (eg 640)
	int getVelocity() { return mVelocity; }
	bool checkIsActivated() { return mIsActive; }
	void activate(); // Activate an asteroid

	// inherited methods
	void update(GAME_EVENT event = NONE);
	void draw();

	SDL_Texture *sharedTexture; // Again, don't initialise this. It will be passed to the asteroid instance at creation time

private:
	bool mIsActive;
	int mVelocity;
	SDL_Rect mTextureRegion;
};

typedef boost::shared_ptr<Asteroid> AsteroidPtr; // Again, same trick used here

#endif