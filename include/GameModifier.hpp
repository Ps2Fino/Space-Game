#ifndef GAME_MODIFIER_HPP
#define GAME_MODIFIER_HPP

#include "RSConstants.hpp"

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

#endif