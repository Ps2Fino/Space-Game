#ifndef SHIP_HPP
#define SHIP_HPP

#include "RSConstants.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <vector> // For the bullets
#include "boost/shared_ptr.hpp" // smart pointers for the bullets

#include "Sprite.hpp"
#include "Bullet.hpp"

/**
 * This Ship class is a special
 * child of Sprite. It allows the player to move up
 * and down in discrete movements
 */
class Ship : public Sprite
{
public:

	Ship(SDL_Renderer *renderer, std::string &imagePath, std::string &bulletImagePath,
			int width = SHIP_WIDTH, int height = SHIP_HEIGHT,
			int x = SHIP_START_POSITION_X, int y = SHIP_START_POSITION_Y);
	~Ship();

	void update(GAME_EVENT ev);
	void draw();

	int getVelocity() { return mVelocity; }
	void setVelocity(int vel) { mVelocity = vel; }
	void fireBullet();
	std::vector<BulletPtr> getBullets() { return mBullets; }

	void setMovementBoundary(int top, int bottom); // This is the most the ship can move up and down

private:
	int mVelocity;
	int mTopBoundary, mBottomBoundary;

	// The texture to be shared across all the bullets
	SDL_Texture *mBulletTexture;

	// An array of Bullets
	std::vector<BulletPtr> mBullets;
};

typedef boost::shared_ptr<Ship> ShipPtr;

#endif