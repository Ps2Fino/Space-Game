#include "App.hpp"

#ifdef __APPLE__
#include <dispatch/dispatch.h> // for gcd on the Mac
#endif

int main (int argc, char **argv)
{
	App myApp;
	myApp.Execute();
	return myApp.Loop();
	
}