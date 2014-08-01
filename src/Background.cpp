#include "Background.hpp"

#ifdef __APPLE__
#include <dispatch/dispatch.h> // for gcd on the Mac
#endif

Background::Background(SDL_Renderer *renderer, std::string &imagePath) 
					: Sprite(renderer, imagePath)
{
} // See the Sptite class

Background::~Background()
{
} // See the Sprite class

void Background::update(EVENT ev)
{
	// The background doesn't update.......for now ;-)
}

void Background::draw()
{	
	if (mRenderer == nullptr)
	{
		std::cout << "The renderer is null" << std::endl;
	}

	if (mTex == nullptr)
	{
		std::cout << "The textur is null" << std::endl;
	}

#ifdef __APPLE__
	std::cout << "In the apple clause" << std::endl;
	std::cout << "About to draw the background" << std::endl;
	dispatch_async(dispatch_get_main_queue(), ^{
		SDL_RenderCopy(this->mRenderer, this->mTex, NULL, &this->mShape);
	});
#endif
}