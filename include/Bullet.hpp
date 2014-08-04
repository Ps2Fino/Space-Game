#ifndef BULLET_HPP
#define BULLET_HPP

#include "RSConstants.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include "boost/shared_ptr.hpp"
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
	void update(int ev1 = 0, int ev2 = 0);
	void draw();

	static SDL_Texture *bulletTexture; // Shared bullet texture
};

typedef boost::shared_ptr<Bullet> BulletPtr;

#endif