#include "LifeManager.hpp"
#include "Utils.hpp"

LifeManager::LifeManager(SDL_Renderer *renderer, Stats *stats)
{
	// Load the texture for the life
	std::string lifeImagePath = getResourcePath() + "img/small_heart.png";
	mLifeTexture = IMG_LoadTexture(renderer, lifeImagePath.c_str());
	Life::lifeTexture = mLifeTexture; // Set the asteroid texture

	// Load the life
	life = LifePtr(new Life(renderer));

	// Set the initial target
	mAsteroidsToShootForLifeSpawn = 5;

	// Set the gamescore reference
	mGameScore = stats;
}

LifeManager::~LifeManager()
{
	SDL_DestroyTexture(mLifeTexture);
	mLifeTexture = NULL;
} // Just clean up

void LifeManager::update()
{
	int score = mGameScore->getScore();
	if (score >= mAsteroidsToShootForLifeSpawn)
	{
		life.get()->reset();
		life.get()->activate();

		// Set the new target
		int target = rand() % 10 + 1; // Blow up between 1 and 10 asteroids
//		int target = 5;
		mAsteroidsToShootForLifeSpawn += target;

		SDL_Log("The new target is: %d\n", mAsteroidsToShootForLifeSpawn);
	}

	life.get()->update();
}

void LifeManager::shootLife()
{
	if (life.get()->checkIsActivated())
	{
		int numLivesToAdd = life.get()->getValue();
		mGameScore->addLives(numLivesToAdd);
		life.get()->reset();
	}
}

void LifeManager::drawLife()
{
	life.get()->draw();
}
