#include "AsteroidManager.hpp"
#include "Utils.hpp"

#define ABS_MIN_INTERVAL 300
#define ABS_MAX_VELOCITY 8
#define ABS_MIN_VELOCITY 6

AsteroidManager::AsteroidManager(SDL_Renderer *renderer, Stats *stats)
	: mAsteroidSpawnInterval(20)
{	
	mAsteroidMinInterval = 300;
	mAsteroidMaxInterval = 1000;
	mAsteroidMinVelocityInterval = 1;
	mAsteroidMaxVelocityInterval = 3; // This range will rise by 2 every wave
	mNextAsteroidIntervalWaitTime = 500; // Half a second to begin with
	mGameScore = stats;
	mLastAsteroidTime = 0;
	
	init(renderer);
}

AsteroidManager::~AsteroidManager()
{
	// Delete the asteroid texture
	SDL_DestroyTexture(mAsteroidTexture);
	mAsteroidTexture = NULL;
} // Stats is none of my concern!

void AsteroidManager::init(SDL_Renderer *renderer)
{	
	// Create the list of asteroids
	std::string asteroidImagePath = getResourcePath() + "img/asteroid.png";
	mAsteroidTexture = IMG_LoadTexture(renderer, asteroidImagePath.c_str());
	Asteroid::asteroidTexture = mAsteroidTexture; // Set the asteroid texture
	
	// Create a set of at most X number of asteroids
	for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
		mAsteroids.push_back(AsteroidPtr(new Asteroid(renderer, this)));
}

void AsteroidManager::update()
{
	int waveScore = mGameScore->getWaveScore();
	if ((waveScore > 0)
			&& (waveScore % 10 == 0))
	{
		mGameScore->defeatWave();
		int numWavesDefeated = mGameScore->getNumWavesDefeated();

		// Reduce the wait time unless we've hit rock bottom
		mAsteroidMaxInterval =
				(mAsteroidMaxInterval <= ABS_MIN_INTERVAL) ? mAsteroidMaxInterval = ABS_MIN_INTERVAL
						: 1000 - (mAsteroidSpawnInterval * (numWavesDefeated));

		// Raise the max velocity every wave
		// Raise the min velocity every 2 waves
		mAsteroidMaxVelocityInterval =
			(mAsteroidMaxVelocityInterval >= ABS_MAX_VELOCITY) 
				? ABS_MAX_VELOCITY : mAsteroidMaxVelocityInterval + 1;

		if (numWavesDefeated % 2)
		{
			mAsteroidMinVelocityInterval = 
				(mAsteroidMinVelocityInterval >= ABS_MIN_VELOCITY)
					? ABS_MIN_VELOCITY : mAsteroidMinVelocityInterval + 1;
		}
	}
	
	// Fire an asteroid if enough time has passed
	if (mLastAsteroidTime + mNextAsteroidIntervalWaitTime <= SDL_GetTicks())
	{
		int range = mAsteroidMaxInterval - mAsteroidMinInterval;
		if (range <= 0)
			mNextAsteroidIntervalWaitTime = mAsteroidMinInterval;
		else
			mNextAsteroidIntervalWaitTime = rand() % range + mAsteroidMinInterval;

		mLastAsteroidTime = SDL_GetTicks();
		activateAsteroid();
	}
	
	// Update my container (ie update the stats and the asteroids array
	for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
		mAsteroids[i].get()->update();
}

void AsteroidManager::recordAsteroidMissed()
{
	mGameScore->loseLife();
}

void AsteroidManager::draw()
{
	for (int i=0; i < NUMBER_ASTEROIDS; ++i)
		mAsteroids[i].get()->draw();
}

void AsteroidManager::activateAsteroid()
{
	for (int i = 0; i < NUMBER_ASTEROIDS; ++i)
	{
		Asteroid *currAsteroid = mAsteroids[i].get();
		if (!currAsteroid->checkIsActivated())
		{
//			SDL_Log("Asteroid min vel: %d\tAsteroid max vel: %d\n", mAsteroidMinVelocityInterval,
//						mAsteroidMaxVelocityInterval);
			// Pick a random velocity to apply to the asteroid
			int velocity = 0;
			if (mAsteroidMinVelocityInterval >= mAsteroidMaxVelocityInterval)
			{
				velocity = ABS_MAX_VELOCITY;
			}
			else
			{
				velocity =
					rand() % mAsteroidMaxVelocityInterval + mAsteroidMinVelocityInterval;
			}

			currAsteroid->setVelocity(velocity);

//			SDL_Log("Activating an asteroid with velocity: %d\n", velocity);

			// Pick a random yPosition for the asteroid
			currAsteroid->activate();
			break;
		}
	}
}

void AsteroidManager::reset()
{
	mAsteroidMinInterval = 300;
	mAsteroidMaxInterval = 1000;
	mAsteroidMinVelocityInterval = 1;
	mAsteroidMaxVelocityInterval = 3; // This range will rise by 2 every wave
	mNextAsteroidIntervalWaitTime = 500;
	mLastAsteroidTime = 0;

	for (int i=0; i < NUMBER_ASTEROIDS; ++i)
		mAsteroids[i].get()->reset();
}
