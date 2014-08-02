#ifndef STATS_HPP
#define STATS_HPP

#include <SDL.h> // For the renderer
#include <SDL_ttf.h> // For the fonts

#include "RSConstants.hpp"
/**
  * This is the class for keeping track
  * of the score in the game.
  */
class Stats
{
public:

	Stats(SDL_Renderer *renderer, TTF_Font *font);
	~Stats();

	void loseLife();
	void shootAsteroid();

	bool checkIsGameOver() { return mLives <= 0; }
	int getScore() { return mAsteroids; }

	void draw();

private:

	int mLives;
	int mAsteroids;

	SDL_Color mFGColor;
	// SDL_Color bgColor; // We're using blended mode so we don't have a background color

	SDL_Renderer *mRenderer;
	TTF_Font *mFont; 
	SDL_Texture *mTexture;
	SDL_Rect mTextLocation;

	void setText(); // This function will update the text to be displayed with the font
};

#endif