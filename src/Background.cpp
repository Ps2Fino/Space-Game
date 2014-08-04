#include "Background.hpp"
#include "RSConstants.hpp"

Background::Background(SDL_Renderer *renderer, std::string &imagePath) 
					: Sprite(renderer)
{
	// Load our texture
	loadTexture(renderer, imagePath);
	mShape.w = GAME_WINDOW_WIDTH;
	mShape.h = GAME_WINDOW_HEIGHT;
} // See the Sptite class

Background::~Background()
{
	SDL_DestroyTexture(mTex);
} // See the Sprite class

void Background::update(int ev, int ev2)
{
} // The background doesn't update.......for now ;-)

void Background::reset()
{
} // Reset what? :-)

void Background::draw()
{
	// Just draw the surface at the current co-ordinates
	if (mTex == nullptr)
		std::cout << "The background texture is nullptr" << std::endl;
	else
		SDL_RenderCopy(mRenderer, mTex, NULL, &mShape);
}