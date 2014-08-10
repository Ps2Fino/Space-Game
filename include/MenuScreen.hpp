#ifndef MENU_SCREEN_HPP
#define MENU_SCREEN_HPP

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "RSConstants.hpp"
#include "boost/shared_ptr.hpp"

/**
  * This is the class for rendering the menu screen
  * It just displays "Press space to play"
  */
class MenuScreen
{
public:

	MenuScreen(SDL_Renderer *renderer, TTF_Font *font);
	~MenuScreen();

	void draw();

	void setText(std::string &text); // This function will update the text to be displayed with the font

private:

	SDL_Color mFGColor;
	// SDL_Color bgColor; // We're using blended mode so we don't have a background color

	SDL_Renderer *mRenderer;
	TTF_Font *mFont; 
	SDL_Texture *mTexture;
	SDL_Rect mTextLocation;

};

typedef boost::shared_ptr<MenuScreen> MenuScreenPtr;

#endif