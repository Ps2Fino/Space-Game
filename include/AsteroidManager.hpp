#ifndef ASTEROID_MANAGER_HPP
#define ASTEROID_MANAGER_HPP

#include "RSConstants.hpp"
#include "Stats.hpp"
#include "Asteroid.hpp"

class AsteroidManager
{
	GameManager(SDL_renderer *renderer, Stats *stats);
	~GameManager();

	void update(); // Call this to update the modifiers whenever the player kills a certain number of asteroids
	const vector<AsteroidPtr>& getAsteroids() { return mAsteroids; } // Return the asteoroids as a const vector

private:
	
	void init(SDL_Renderer *renderer); // init method rather than polluting the constructor
	void activateAsteroid(); // Activates an asteroid to place in the game
	
	int mAsteroidSpawnInterval;
	int mWavesDefeated;
	
	int mAsteroidMinInterval;
	int mAsteroidMaxInterval;
	int mAsteroidMinVelocityInterval;
	int mAsteroidMaxVelocityInterval;
	int mNextAsteroidIntervalWaitTime;
	
	SDL_Texture *mAsteroidTexture; // The shared texture for the asteroids
	Stats *mGameScore; // Use this to check whether or not we should update the game modifiers
	std::vector<AsteroidPtr> mAsteroids; // The manager should look after the asteroids	
};

#endif