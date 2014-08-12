#include "GameManager.hpp"

GameManager::GameManager(Stats *stats) 
	: mAsteroidSpawnInterval(20), mWavesDefeated(0)
{	
	mStats = stats;
	mAsteroidMinInterval = ASTEROID_INTERVAL_RANGE_MIN;
	mAsteroidMaxInterval = ASTEROID_INTERVAL_RANGE_MAX;
	mAsteroidMinVelocityInterval = 1;
	mAsteroidMaxVelocityInterval = 3; // This range will rise by 2 every wave
	mNumberOfLives = NUMBER_PLAYER_LIVES;
	mBulletVelocity = BULLET_VELOCITY;
	mShipVelocity = SHIP_VELOCITY;
	mNextAsteroidIntervalWaitTime = 0;
	
	// Create the list of asteroids
}

GameManager::~GameManager()
{
	// Delete the asteroid texture
} // Nothing to do really

void GameManager::update()
{
	if (mGameScore->getScore() % 10)
	{
		mWavesDefeated++; // Defeated a wave. Record it
		mAsteroidMaxInterval = 1000 - (mAsteroidSpawnInterval * (mWavesDefeated + 1));
		if (mWavesDefeated % 2)
			mAsteroidMaxVelocityInterval++; // Raise the max velocity every two waves
		else if (mWavesDefeated % 4)
			mAsteroidMinVelocityInterval++; // Raise the min velocity every 4 waves
		
		// Compute the
		// Fire off another asteroid if the wait time has expired
	}
}