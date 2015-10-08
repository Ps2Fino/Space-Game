Space-Game
==========

A small game in SDL. Why not sure :-)

Uses CMake. Beware though, the CMakeLists.txt file is optimized for Mac really (I haven't had the time to try cross compilation for windows)
The easiest way is to install homebrew, then run the brew_dependencies.sh script to install everything needed.
After this, run the compile.sh script to build the project.
This works on a new OSX install, so it should work right off the bat on your machine.

Dependencies
============

+ SDL 2.0.3 (>= 2.0 should be fine)
+ SDL Image (SDL_image.h)
+ SDL TrueType Font (SDL_ttf.h)
+ SDL Mixer (SDL_mixer)
+ Boost (a modified version of boost containing just the smart pointer header-only library and its dependencies is included with the source)

Android Port
============

The latest version of the repo contains an android source project for building the game on android devices supporting SDK >= 10

I've placed local symlinks to the projects source files (better than duplicating files). What you have to do yourself is the following:

1. Download the source distributions of the dependencies listed above (note boost is included!)
2. Create a symlink for each of the SDL distros inside rocket-shooter-android/jni/ and then note the following:
	1. SDL 2.0.3 should compile fine by itself.
	2. For SDL_image you need to disable jpeg and wbep loading inside the SDL_imageXXX/Android.mk file
	3. For SDL_ttf you need to download the freetype library (http://sourceforge.net/projects/freetype/files/) and unpack it to the root of the SDL_imageXXX directory.
	4. For SDL_mixer you need to disable the mikmod, modplug and mp3 smpeg plugins. I stuck with OGG

After doing all this, cd into the rocket-shooter-android directory and run ndk-build (you have it installed right?).
The ndk will then build the entire application, including the SDL dependencies and will create a nice apk inside rocket-shooter-android/bin.
Use adb or Eclipse or human-machine telepathy to install to your device. I don't think the app will run on an emulator but you can knock yourself out.

If all that sounds like too much hard work, there is a signed binary in the rocket-shooter-android/bin directory. Just dump that to your device and you should be able to install it

Cheers

Dan
