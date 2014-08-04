#include "Background.hpp"

Background::Background(SDL_Renderer *renderer, std::string &imagePath) 
					: Sprite(renderer)
{
	// Load our texture
	// std::cout << "Calling load texture in background with path = " << imagePath << std::endl;
	loadTexture(renderer, imagePath);
	mShape.w = GAME_WINDOW_WIDTH;
	mShape.h = GAME_WINDOW_HEIGHT;
} // See the Sptite class

Background::~Background()
{
	// std::cout << "The background destructor was called" << std::endl;
	SDL_DestroyTexture(mTex);
} // See the Sprite class

void Background::update(int ev, int ev2)
{
	// The background doesn't update.......for now ;-)
}

void Background::draw()
{
	// std::cout << "Background::draw function was called" << std::endl;

	// Just draw the surface at the current co-ordinates
	if (mTex == nullptr)
	{
		std::cout << "The background texture is nullptr" << std::endl;
	}

	// std::cout << "mShape.x = " << mShape.x << ", mShape.y = " << mShape.y << std::endl;

	SDL_RenderCopy(mRenderer, mTex, NULL, &mShape);

	// std::cout << "SDL_RenderCopy called" << std::endl;	
}