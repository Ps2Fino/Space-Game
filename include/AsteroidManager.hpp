#ifndef ASTEROID_MANAGER_HPP
#define ASTEROID_MANAGER_HPP

#include <SDL.h>
#include <vector>

#include "RSConstants.hpp"
#include "Asteroid.hpp"

class AsteroidManager
{
public:

	AsteroidManager(SDL_Renderer *renderer);
	~AsteroidManager();

	void update(); // Call this to update the modifiers whenever the player kills a certain number of asteroids
	void reset();
	void draw(); // Just really calls draw on all the asteroids

	std::vector<AsteroidPtr>& getAsteroids() { return mAsteroids; } // Return the asteoroids as a const vector

private:
	
	void init(SDL_Renderer *renderer); // init method rather than polluting the constructor
	void activateAsteroid(); // Activates an asteroid to place in the game
	
	int mAsteroidSpawnInterval;
	int mAsteroidMinInterval;
	int mAsteroidMaxInterval;
	int mAsteroidMinVelocityInterval;
	int mAsteroidMaxVelocityInterval;
	int mNextAsteroidIntervalWaitTime;
	int mLastAsteroidTime;
	
	SDL_Texture *mAsteroidTexture; // The shared texture for the asteroids
	std::vector<AsteroidPtr> mAsteroids; // The manager should look after the asteroids	
};

typedef boost::shared_ptr<AsteroidManager> AsteroidManagerPtr;

#endif
