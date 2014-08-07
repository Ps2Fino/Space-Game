#ifndef ASTEROID_HPP
#define ASTEROID_HPP

#include <SDL.h>
#include <SDL_image.h>
#include "Destructible.hpp"
#include "boost/shared_ptr.hpp"

#ifndef BUILDING_PSP_EBOOT
	class Stats; // Forward declaration
#endif

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
	void update(int ev1 = 0, int ev2 = 0);
	void reset();
	void draw();

#ifndef BUILDING_PSP_EBOOT
	void setStats(Stats *stats) { mScoreTable = stats; }
#endif

	static SDL_Texture *asteroidTexture; // Shared asteroid texture

private:
#ifndef BUILDING_PSP_EBOOT
	Stats *mScoreTable; // A pointer to the stats so that when an asteroid passes the player it can take a life
#endif
};

typedef boost::shared_ptr<Asteroid> AsteroidPtr; // Again, same trick used here

#endif