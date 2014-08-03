#ifndef BULLET_HPP
#define BULLET_HPP

#include "RSConstants.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include "Destructible.hpp"

/**
  * This class encapsulates a bullet.
  * A bullet has a shared sprite resource for
  * drawing and has a simple update function; move across the screen
  */
class Bullet : public Destructible
{
public:

	Bullet(SDL_Renderer *renderer, int type = 0, int x = 0, int y = 0);
	~Bullet();

	// inherited methods
	void activate(int xPos, int yPos);
	void update(GAME_EVENT event = NONE);
	void draw();

	static SDL_Texture *bulletTexture; // Shared bullet texture
};

typedef boost::shared_ptr<Bullet> BulletPtr;

#endif