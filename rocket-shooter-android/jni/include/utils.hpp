#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <SDL.h>
#include <string>

static std::string getResourcePath(const std::string &subDir = "")
{
#ifdef _WIN32
	const char	PATH_SEP = '\\'
#else
	const char PATH_SEP = '/';
#endif

	static std::string baseRes;
	if (baseRes.empty())
	{
		char *basePath = SDL_GetBasePath();
		if (basePath)
		{
			baseRes = basePath;
			SDL_free(basePath);
		}
		else
		{
			std::cout << "Error: Couldn't find the base path using SDL" << SDL_GetError() << std::endl;
			return "";
		}

		// Do a reverse search for the bin direcory and append the resources folder to it
		size_t pos = baseRes.rfind("bin");
		baseRes = baseRes.substr(0, pos) + "resources" + PATH_SEP;
	}

	return subDir.empty() ? baseRes : baseRes + subDir + PATH_SEP;
}

#endif