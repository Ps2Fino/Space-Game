#include "App.hpp"

int main (int argc, char **argv)
{
	App myApp;
	myApp.Execute();

	// Start the game loop
	return myApp.Loop();
}