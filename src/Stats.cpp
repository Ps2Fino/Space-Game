#include "Stats.hpp"

#include <string>
#include <sstream>

Stats::Stats(SDL_Renderer *renderer, TTF_Font *font) : mLives(NUMBER_PLAYER_LIVES),
				mAsteroids(0), mIsActive(false), mWavesDefeated(0), mCurrWaveScore(0),
				mFGColor({255, 0, 0}), mRenderer(renderer), mFont(font)
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

void Stats::defeatWave()
{
	SDL_Log("Defeated a wave! Current wave number is %d", mWavesDefeated + 1);
	mWavesDefeated++;
	mCurrWaveScore = 0;
}

void Stats::shootAsteroid()
{
	if (mIsActive)
	{
		mAsteroids++;
		mCurrWaveScore++;
		setText();
	}
}

void Stats::addLives(int numToAdd)
{
	if (mIsActive)
	{
		mLives += numToAdd;
		setText();
	}
}

void Stats::reset()
{
	mAsteroids = 0;
	mLives = NUMBER_PLAYER_LIVES;
	mWavesDefeated = 0;
	mCurrWaveScore = 0;
	setText(); // Reset the text
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
