#ifndef LIFE_MANAGER_HPP
#define LIFE_MANAGER_HPP

#include <SDL.h>
#include <string>
#include "RSConstants.hpp"
#include "Stats.hpp"
#include "Life.hpp"

/**
 * Just keep track of when to fire a life
 */
class LifeManager
{
public:

	LifeManager(SDL_Renderer *renderer, Stats *stats);
	~LifeManager();
	void update(); // This will just check if we've killed enough asteroids to gain a life spawn
	void drawLife();
 	void shootLife();
 	Life * getLife() { return life.get(); }
 	void reset() { life.get()->reset(); }
 	void activate() { life.get()->activate(); }

private:
	Stats *mGameScore;
	int mAsteroidsToShootForLifeSpawn;
	SDL_Texture *mLifeTexture;
	LifePtr life;
};

typedef boost::shared_ptr<LifeManager> LifeManagerPtr;

#endif

