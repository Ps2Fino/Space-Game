#ifndef SOUND_FX_HPP
#define SOUND_FX_HPP

#include <SDL.h>
#include <SDL_mixer.h>

#include "RSConstants.hpp"

/**
  * This is the class that wraps
  * around the SDL mixer library
  * for playing sound effects 
  * during the game
  */
class SoundFX
{
public:

	static int initMixerLibrary();
	static bool checkIsReady() { return isReady; }
	static int loadLaserSound();
	static void playLaserSound();
	static void shutDownMixerLibrary();

private:

	static bool isReady;
	static Mix_Chunk *laserBulletSound; // The sound effect for the ship bullet
};

#endif