#include "Sprite.hpp"
#include <string>
#include <iostream>

#include <exception>

class RendererException : public std::exception
{
	virtual const char * what() const throw()
	{
		return "I can't exist without a renderer";
	}
} rendererException;

/* This is the implementation of the sprite class */

Sprite::Sprite(SDL_Renderer *renderer, std::string &surface_image_path)
{
	if (renderer == nullptr)
	{
		throw rendererException;
	}

	// Just call load surface to load the texture for us
	mRenderer = renderer;
	load_surface(renderer, surface_image_path, false);
}

Sprite::~Sprite()
{
	SDL_DestroyTexture(mTex);
}

void Sprite::load_surface(SDL_Renderer *renderer, std::string &surface_image_path, bool replace)
{
	if ((mTex != nullptr && replace)
		|| mTex == nullptr)
		mTex = IMG_LoadTexture(renderer, surface_image_path.c_str());
}