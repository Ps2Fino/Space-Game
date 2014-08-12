#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include "RSConstants.hpp"
#include "Stats.hpp"
#include "Asteroid.hpp"

class GameManager
{
	GameManager(Stats *stats);
	~GameManager();

	void update(); // Call this to update the modifiers whenever the player kills a certain number of asteroids

private:
	int mAsteroidSpawnInterval;
	int mWavesDefeated;
	
	int mAsteroidMinInterval;
	int mAsteroidMaxInterval;
	int mAsteroidMinVelocityInterval;
	int mAsteroidMaxVelocityInterval;
	
	int mNextAsteroidIntervalWaitTime;
	
	int mNumberOfLives;
	int mBulletVelocity;
	int mShipVelocity;
	
	SDL_Texture mAsteroidTexture; // The shared texture for the asteroids
	StatsPtr mGameScore; // Use this to check whether or not we should update the game modifiers
	std::vector<AsteroidPtr> asteroids; // The manager should look after the asteroids	
};

#endif