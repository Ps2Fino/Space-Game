#include "App.hpp"

#ifdef __APPLE__
#include <dispatch/dispatch.h> // for gcd on the Mac
#endif

int main (int argc, char **argv)
{
	App myApp;
	myApp.Execute();
#ifdef __APPLE__
	dispatch_async(dispatch_get_main_queue(), ^{
		myApp.Loop();
	});
#endif
	return 0;
	
}