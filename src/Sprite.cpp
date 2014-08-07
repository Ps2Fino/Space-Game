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

Sprite::Sprite(SDL_Renderer *renderer, int width, int height, int x, int y) 
			: mX_pos(x), mY_pos(y), mRenderer(renderer)
{
	// Set our size
	setSize(width, height);
	setPosition(mX_pos, mY_pos);
}

Sprite::~Sprite()
{
}

void Sprite::loadTexture(SDL_Renderer *renderer, std::string &surface_image_path)
{
#ifndef BUILDING_PSP_EBOOT
	mTex = IMG_LoadTexture(renderer, surface_image_path.c_str());
#else
	// In IMG_loadTexture is an SDL 2.0 function in the SDL_image exteions.
	// I revert to the old API here
	SDL_Surface *tempSurface = IMG_Load(surface_image_path.c_str());
	mTex = SDL_CreateTextureFromSurface(renderer, tempSurface);
	SDL_FreeSurface(tempSurface);
#endif

	if (mTex == nullptr || mTex == NULL)
	{
		std::cout << "The texture is null" << std::endl;
		throw textureException;
	}
}