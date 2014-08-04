#ifndef DESTRUCTIBLE_HPP
#define DESTRUCTIBLE_HPP

#include "Sprite.hpp"

/**
  * Tis is an abstract class that represents
  * a destructibele. A destructible is any entity in the
  * game that only lives for a short time and is destructible
  * eg bullets, asteroids etc.
  */
class Destructible : public Sprite
{
public:

	Destructible(SDL_Renderer *renderer, int width = 0,
					int height = 0, int x = 0, int y = 0);
	virtual ~Destructible();

	int getVelocity() { return mVelocity; }
	bool checkIsActivated() { return mIsActive; }
	void deactivate() { mIsActive = false; }

	virtual void activate(int xPos = 0, int yPos = 0) =0;

	// inherited methods
	virtual void update(int ev1 = 0, int ev2 = 0) =0;
	virtual void reset() =0;
	virtual void draw() =0;

protected:
	bool mIsActive;
	int mVelocity;
	SDL_Rect mTextureRegion;
};

#endif