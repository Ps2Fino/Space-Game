class GameModifier
{
	GameModifier(bool defModifiers=true);
	~GameModifier();

	void update(); // Call this to update the modifiers whenever the player kills a certain number of asteroids

private:
	int asteroidMinInterval;
	int asteroidMaxInterval;
	int numberOfLives;
	int bulletVelocity;
	int shipVelocity;
	int asteroidVelocity;
};

GameModifier::GameModifier(bool defModifiers)
{
	if (defModifiers)
	{
		asteroidMinInterval = ASTEROID_INTERVAL_RANGE_MIN;
		asteroidMaxInterval = ASTEROID_INTERVAL_RANGE_MAX;
	}
}