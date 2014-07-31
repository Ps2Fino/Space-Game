#ifndef SHIP_HPP
#define SHIP_HPP

#include <SDL.h>
#include <SDL_image.h>

#include "Sprite.hpp"

/**
 * This Ship class is a special
 * child of Sprite. It allows the player to move up
 * and down in discrete movements
 */
class Ship : public Sprite
{
public:

	Ship(int width, int height, SDL_Renderer *renderer, std::string &imagePath);
	~Ship();

	void update(EVENT ev);
	void draw();

private:
	// TODO: Implement this
};

#endif