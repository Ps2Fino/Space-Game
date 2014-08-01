#ifndef SHIP_HPP
#define SHIP_HPP

#include <SDL.h>
#include <SDL_image.h>

#include "Sprite.hpp"

#define DEFAULT_VELOCITY 5

/**
 * This Ship class is a special
 * child of Sprite. It allows the player to move up
 * and down in discrete movements
 */
class Ship : public Sprite
{
public:

	Ship(SDL_Renderer *renderer, std::string &imagePath,
			int width = 64, int height = 48,
			int x = 20, int y = 240);
	~Ship();

	void update(EVENT ev);
	void draw();

	int getVelocity() { return mVelocity; }
	void setVelocity(int vel) { mVelocity = vel; }

	void setMovementBoundary(int top, int bottom); // This is the most the ship can move up and down

private:
	int mVelocity;
	int mTopBoundary, mBottomBoundary;
};

#endif