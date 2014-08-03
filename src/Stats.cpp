#include "Stats.hpp"

#include <string>
#include <sstream>

Stats::Stats(SDL_Renderer *renderer, TTF_Font *font) : mLives(NUMBER_PLAYER_LIVES),
				mAsteroids(0), mIsActive(true), mFGColor({255, 0, 0}), mRenderer(renderer), mFont(font)
{
	// Init the texture for the score
	setText();

	// Set the textlocation
	mTextLocation.x = SCORE_TEXT_LOCATION_X;
	mTextLocation.y = SCORE_TEXT_LOCATION_Y;
	mTextLocation.w = SCORE_TEXT_WIDTH;
	mTextLocation.h = SCORE_TEXT_HEIGHT;
}

Stats::~Stats()
{
	SDL_DestroyTexture(mTexture);
}

void Stats::loseLife()
{
	if (mIsActive)
	{
		mLives--;
		setText();
	}
}

void Stats::shootAsteroid()
{
	if (mIsActive)
	{
		mAsteroids++;
		setText();
	}
}

void Stats::setText()
{
	std::ostringstream stream;
	stream << mLives << " lives " << mAsteroids << " asteroids shot";
	std::string textString = stream.str();
	SDL_Surface *newSurface = 
		TTF_RenderText_Blended(mFont, textString.c_str(), mFGColor);
	mTexture = SDL_CreateTextureFromSurface(mRenderer, newSurface);
	SDL_FreeSurface(newSurface);
}

void Stats::draw()
{
	SDL_RenderCopy(mRenderer, mTexture, NULL, &mTextLocation);
}