#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

/**
 * Declare an event enum. This is how the sprites know
 * how to update
 */

 enum EVENT {
 	NONE,
 	UP,
 	DOWN,
 	LEFT,
 	// RIGHT, // For the moment, we can't ever move to the right of the screen
 };

/**
  * The Sprite class is an abstract class
  * It has an XY position as well as an SDL_Surface
  * that it uses to represent itself visually
  */
class Sprite
{
public:
	Sprite(SDL_Renderer *renderer, std::string &surface_image_path, 
				int width = 640, int height = 480, int x = 0, int y = 0);
	~Sprite();

	void load_surface(SDL_Renderer *renderer, std::string &surface_image_path, bool replace);

	void setSize(int width, int height) { mShape.w = width; mShape.h = height; } // set the size of the rect
	SDL_Rect getSize() { return mShape; } // return the rect for the sprite

	virtual void update(EVENT ev) =0; // pure virtual method for moving the sprite
	virtual void draw() =0; // This is the function to call in the render part of the game loop

protected:
	SDL_Texture *mTex; // visual representation
	SDL_Rect mShape; // This is the shape of the sprite
	int mX_pos, mY_pos; // position of the sprite

	// All sprites need a rendering context
	SDL_Renderer *mRenderer;
};

#endif