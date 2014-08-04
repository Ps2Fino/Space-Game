#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "MenuScreen.hpp"
#include "RSConstants.hpp"

#include <string>
#include <sstream>

MenuScreen::MenuScreen(SDL_Renderer *renderer, TTF_Font *font) 
							: mFGColor({255, 255, 255}), mRenderer(renderer), mFont(font)
{
	// Init the texture for the score
	std::string text = MENU_SCREEN_TEXT;
	setText(text);

	// Set the textlocation
	mTextLocation.x = MENU_SCREEN_TEXT_LOCATION_X;
	mTextLocation.y = MENU_SCREEN_TEXT_LOCATION_Y;
	mTextLocation.w = MENU_SCREEN_TEXT_WIDTH;
	mTextLocation.h = MENU_SCREEN_TEXT_HEIGHT;
}

MenuScreen::~MenuScreen()
{
	SDL_DestroyTexture(mTexture);
}

void MenuScreen::setText(std::string &text)
{
	SDL_Surface *newSurface = 
		TTF_RenderText_Blended(mFont, text.c_str(), mFGColor);
	mTexture = SDL_CreateTextureFromSurface(mRenderer, newSurface);
	SDL_FreeSurface(newSurface);
}

void MenuScreen::draw()
{
	SDL_RenderCopy(mRenderer, mTexture, NULL, &mTextLocation);
}