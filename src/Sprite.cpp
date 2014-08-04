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
	mTex = IMG_LoadTexture(renderer, surface_image_path.c_str());

	if (mTex == nullptr || mTex == NULL)
	{
		std::cout << "The texture is null" << std::endl;
		throw textureException;
	}

	std::cout << "Load texture called successfully" << std::endl;
}