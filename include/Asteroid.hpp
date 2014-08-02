#ifndef ASTEROID_HPP
#define ASTEROID_HPP

#include "RSConstants.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include "boost/shared_ptr.hpp"
#include "Stats.hpp"
#include "Destructible.hpp"

/**
  * An asteroid doesn't do much.
  * It just whizzes past the screen
  */
class Asteroid : public Destructible
{
public:

	Asteroid(SDL_Renderer *renderer);
	~Asteroid();

	void setXPosition(int x) { mX_pos = x; } // Set this to the screen size (eg 640)
	void activate(int xPos = 0, int yPos = 0);

	// inherited methods
	void update(GAME_EVENT event = NONE);
	void draw();

	void setStats(Stats *stats) { mScoreTable = stats; }

	static SDL_Texture *asteroidTexture; // Shared asteroid texture

private:
	Stats *mScoreTable; // A pointer to the stats so that when an asteroid passes the player it can take a life
};

typedef boost::shared_ptr<Asteroid> AsteroidPtr; // Again, same trick used here

#endif