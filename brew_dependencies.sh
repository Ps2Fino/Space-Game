#
# Use this script (with homebrew) to install the required libraries for cmake
# Any issues, make sure to run $(brew doctor) as I personally had an
# issue with Xcode not being up to date, and therefore not being able to build
# C programs
#
# @author Daniel J. Finnegan
# @date October 2015
#

brew install cmake
brew install SDL2
brew install SDL2_Image
brew install SDL_Ttf
brew install SDL2_Mixer --with-libvorbis # Some of the audio assets are oggs, so need the vorbis codec
brew install boost
