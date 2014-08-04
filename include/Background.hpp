#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "RSConstants.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include "boost/shared_ptr.hpp"
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

	bool isTextureNull() { return mTex == nullptr || mTex == NULL; }

	void update(int ev1 = 0, int ev2 = 0);
	void draw();
};

typedef boost::shared_ptr<Background> BackgroundPtr;

#endif