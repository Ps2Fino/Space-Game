#ifndef LIFE_HPP
#define LIFE_HPP

#include <SDL.h>
#include <SDL_image.h>
#include "Destructible.hpp"
#include "boost/shared_ptr.hpp"

/**
 * This class grants the player a random number of lives if shot successfully
 */

class Life : public Destructible
{
public:

	Life(SDL_Renderer *renderer, int width = LIFE_WIDTH, int height = LIFE_HEIGHT,
			int x = 0, int y = 0);
	~Life();

	void activate(int xPos = 0, int yPos = 0);
	void update(int ev1 = 0, int ev2 = 0);
	void reset();
	void draw();
	int getValue() { return mValue; }

	// All lives look the same in the game (this can change in future iterations)
	static SDL_Texture *lifeTexture;

private:
	int mValue;
};

typedef boost::shared_ptr<Life> LifePtr;

#endif

