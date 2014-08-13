#ifndef ASTEROID_HPP
#define ASTEROID_HPP

#include <SDL.h>
#include <SDL_image.h>
#include "Destructible.hpp"
#include "boost/shared_ptr.hpp"

class AsteroidManager;

/**
  * An asteroid doesn't do much.
  * It just whizzes past the screen
  */
class Asteroid : public Destructible
{
public:

	Asteroid(SDL_Renderer *renderer, AsteroidManager *parentManager, int velocity = ASTEROID_VELOCITY);
	~Asteroid();

	void setXPosition(int x) { mX_pos = x; } // Set this to the screen size (eg 640)
	void setVelocity(int velocity) { mVelocity = velocity; } // Set the velocity for the asteroid
	void activate(int xPos = 0, int yPos = 0);

	// inherited methods
	void update(int ev1 = 0, int ev2 = 0);
	void reset();
	void draw();

	static SDL_Texture *asteroidTexture; // Shared asteroid texture

private:
	int mVelocity; // Every asteroid should actually have it's own velocity
	AsteroidManager *mParentManager; // A pointer to the parent
};

typedef boost::shared_ptr<Asteroid> AsteroidPtr; // Again, same trick used here

#endif
