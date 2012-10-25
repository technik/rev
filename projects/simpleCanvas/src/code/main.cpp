//----------------------------------------------------------------------------------------------------------------------
// Rev Generic template
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On October 23rd, 2012
//----------------------------------------------------------------------------------------------------------------------
// Entry point

#include <iostream>
#include <revVideo/videoDriver/videoDriver.h>

int main (int //_argc
	, const char** //_argv
	)
{
	rev::video::VideoDriver::startUp();
	rev::video::VideoDriver::shutDown();

	int a;
	std::cin >> a;
	return 0;
}