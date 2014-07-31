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

class TextureException : public std::exception
{
	virtual const char * what() const throw()
	{
		return "A texture could not be loaded";
	}
} textureException;

/* This is the implementation of the sprite class */

Sprite::Sprite(SDL_Renderer *renderer, std::string &surface_image_path,
					int width, int height, int x, int y) : mX_pos(x), mY_pos(y)
{
	if (renderer == nullptr)
	{
		throw rendererException;
	}

	// Just call load surface to load the texture for us
	mRenderer = renderer;
	load_surface(renderer, surface_image_path, false);

	// Set our size
	setSize(width, height);
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

	if (mTex == nullptr)
	{
		throw textureException;
	}
}