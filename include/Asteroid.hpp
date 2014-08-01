#ifndef ASTEROID_HPP
#define ASTEROID_HPP

#include <SDL.h>
#include <SDL_image.h>

#define NUMBER_OF_ASTEROID_SPRITES 8
#define ASTEROID_WIDTH 64
#define ASTEROID_HEIGHT 64

/**
  * An asteroid doens't do much.
  * It just whizzes past the screen
  */
class Asteroid : public Sprite
{
public:

	Asteroid();
	~Asteroid();

	void update(GAME_EVENT event = 0);
	void draw();

private:
	SDL_Texture *sharedTexture; // Again, just like the bullets, an asteroid has a shared texture
};

#endif