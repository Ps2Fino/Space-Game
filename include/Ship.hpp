#ifndef SHIP_HPP
#define SHIP_HPP

#include <SDL.h>
#include <SDL_image.h>
#include <vector> // For the bullets
#include "boost/smart_ptr.hpp" // smart pointers for the bullets

#include "Sprite.hpp"
#include "Bullet.hpp"

#define DEFAULT_SHIP_VELOCITY 5
#define NUMBER_BULLETS 5

/**
 * This Ship class is a special
 * child of Sprite. It allows the player to move up
 * and down in discrete movements
 */
class Ship : public Sprite
{
public:

	Ship(SDL_Renderer *renderer, std::string &imagePath, std::string &bulletImagePath,
			int width = 64, int height = 48,
			int x = 20, int y = 240);
	~Ship();

	void update(GAME_EVENT ev);
	void draw();

	int getVelocity() { return mVelocity; }
	void setVelocity(int vel) { mVelocity = vel; }
	void fireBullet();

	void setMovementBoundary(int top, int bottom); // This is the most the ship can move up and down

private:
	int mVelocity;
	int mTopBoundary, mBottomBoundary;

	// An array of Bullets
	SDL_Texture *mBulletTexture;
	boost::scoped_ptr<Bullet> mBullet;
};

#endif