#include "SoundFX.hpp"

#include <string>
#include "Utils.hpp" // for getResourcePath

bool SoundFX::isReady = false;
Mix_Chunk* SoundFX::laserBulletSound = nullptr;
Mix_Music *SoundFX::bgMusic = nullptr;

int SoundFX::initMixerLibrary()
{
	// init ogg/vorbis support
	int res = Mix_Init(MIX_INIT_OGG);
	if ((res & MIX_INIT_OGG) != MIX_INIT_OGG)
	{
		std::cout << "Couldn't init the mixer with OGG support" << std::endl;
		return 1;
	}

	// Init the sound engine
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048))
	{
		std::cout << "SDL_Mixer Error: " << Mix_GetError() << std::endl;
		return 1;
	}

	isReady = true;
	return 0;
}

void SoundFX::shutDownMixerLibrary()
{
	if (Mix_PlayingMusic() != 0)
		stopMusic();

	Mix_FreeChunk(laserBulletSound);
	Mix_FreeMusic(bgMusic);
	laserBulletSound = NULL;
	bgMusic = NULL;
	Mix_Quit();
}

int SoundFX::loadLaserSound()
{
	std::string laserBulletPath = getResourcePath() + "sounds/laser.ogg";
	laserBulletSound = Mix_LoadWAV(laserBulletPath.c_str());
	if (laserBulletSound == nullptr)
	{
		std::cout << "Couldn't load the laser sound: " << Mix_GetError() << std::endl;
		return 1;
	}

	return 0;
}

int SoundFX::loadMusic()
{
	std::string bgMusicPath = getResourcePath() + "sounds/music.ogg";
	bgMusic = Mix_LoadMUS(bgMusicPath.c_str());
	if (bgMusic == nullptr)
	{
		std::cout << "Couldn't load the laser sound: " << Mix_GetError() << std::endl;
		return 1;
	}

	return 0;
}

int SoundFX::startMusic()
{
	if (Mix_PlayingMusic() == 0)
	{
		if (Mix_PlayMusic(bgMusic, -1) == -1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return 0;
}

void SoundFX::stopMusic()
{
	Mix_HaltMusic();
}

void SoundFX::playLaserSound()
{
	if (laserBulletSound != nullptr)
	{
		Mix_PlayChannel(-1, laserBulletSound, 0);
	}
}