#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include <SDL.h>
#include <SDL_image.h>

#include "Sprite.hpp"

/**
 * This Ship class is a special
 * child of Sprite. It allows the player to move up
 * and down in discrete movements
 */
class Background : public Sprite
{
public:

	Background(SDL_Renderer *renderer, std::string &imagePath);
	~Background();

	void update(GAME_EVENT ev);
	void draw();
};

#endif