#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "RSConstants.hpp" // Handy constants for the game

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

/**
 * Declare an event enum. This is how the sprites know
 * how to update
 */

 enum GAME_EVENT {
 	NONE,
 	UP,
 	DOWN,
 	LEFT,
 	FIRE,
 	// RIGHT, // For the moment, we can't ever move to the right of the screen
 };

/**
  * The Sprite class is an abstract class
  * It has an XY position as well as an SDL_Surface
  * that it uses to represent itself visually
  *
  * The class can load textures itself and store. Alternatively,
  * the texture can just be set (many instances sharing the same texture)
  */
class Sprite
{
public:
	Sprite(SDL_Renderer *renderer, int width = 0, 
				int height = 0, int x = 0, int y = 0);
	virtual ~Sprite();

	void loadTexture(SDL_Renderer *renderer, std::string &surface_image_path, bool replace);
	void setTexture(SDL_Texture *texture) { mTex = texture; }

	void setSize(int width, int height) { mShape.w = width; mShape.h = height; } // set the size of the rect
	SDL_Rect getSize() { return mShape; } // return the rect for the sprite

	void setPosition(int xPos, int yPos) { mShape.x = xPos; mShape.y = yPos; }
	SDL_Rect getPosition() { return mShape; }

	virtual void update(GAME_EVENT ev  = NONE) =0; // pure virtual method for moving the sprite
	virtual void draw() =0; // This is the function to call in the render part of the game loop

protected:
	SDL_Texture *mTex; // visual representation
	SDL_Rect mShape; // This is the shape of the sprite
	int mX_pos, mY_pos; // position of the sprite

	// All sprites need a rendering context
	SDL_Renderer *mRenderer;
};

#endif