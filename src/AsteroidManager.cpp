#include "AsteroidManager.hpp"

GameManager::GameManager(SDL_Renderer *renderer, Stats *stats) 
	: mAsteroidSpawnInterval(20), mWavesDefeated(0)
{	
	mStats = stats
	mAsteroidMinInterval = ASTEROID_INTERVAL_RANGE_MIN;
	mAsteroidMaxInterval = ASTEROID_INTERVAL_RANGE_MAX;
	mAsteroidMinVelocityInterval = 1;
	mAsteroidMaxVelocityInterval = 3; // This range will rise by 2 every wave
	mNextAsteroidIntervalWaitTime = 0;
	
	init(renderer);
}

GameManager::~GameManager()
{
	// Delete the asteroid texture
	SDL_DestroyTexture(mAsteroidTexture);
	mAsteroidTexture = NULL;
} // Stats is none of my concern!

void GameManager::init(SDL_Renderer *renderer)
{	
	// Create the list of asteroids
	std::string asteroidImagePath = getResourcePath() + "img/asteroid.png";
	mAsteroidTexture = IMG_LoadTexture(renderer, asteroidImagePath.c_str());
	Asteroid::asteroidTexture = asteroidTexture; // Set the asteroid texture
	
	// Create a set of at most X number of asteroids
	for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
	{
		mAsteroids.push_back(AsteroidPtr(new Asteroid(renderer)));
		mAsteroids.back().get()->setStats(mGameScore.get()); // Add the stats table to the asteroid
	}
}

void GameManager::update()
{
	if (mGameScore->getScore() % 10)
	{
		mWavesDefeated++; // Defeated a wave. Record it
		
		SDL_Log("Defeated a wave! Current wave number is %d", mWavesDefeated + 1);
		
		mAsteroidMaxInterval = 1000 - (mAsteroidSpawnInterval * (mWavesDefeated + 1));
		if (mWavesDefeated % 2)
			mAsteroidMaxVelocityInterval++; // Raise the max velocity every two waves
		else if (mWavesDefeated % 4)
			mAsteroidMinVelocityInterval++; // Raise the min velocity every 4 waves
	}
	
	// Fire an asteroid if enough time has passed
	if (lastAsteroidTime + mNextAsteroidIntervalWaitTime <= SDL_GetTicks())
	{
		mNextAsteroidIntervalWaitTime = 
				rand() % ASTEROID_INTERVAL_RANGE_MIN + (ASTEROID_INTERVAL_RANGE_MAX + 1);
		lastAsteroidTime = SDL_GetTicks();
		activateAsteroid();
	}
	
	// Update my container (ie update the stats and the asteroids array
	for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
		asteroids[i].get()->update();
}

void GameManager::activateAsteroid()
{
	for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
	{
		Asteroid *currAsteroid = asteroids[i].get();
		if (!currAsteroid->checkIsActivated())
		{
			// Pick a random velocity to apply to the asteroid
			int randomVelocity = 
					rand() % mAsteroidMinVelocityInterval + (mAsteroidMaxVelocityInterval + 1);
			currAsteroid.setVelocity(randomVelocity);
			currAsteroid->activate();
			break;
		}
	}
}