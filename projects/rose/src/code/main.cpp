//----------------------------------------------------------------------------------------------------------------------
// Rose project
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On October 8th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Entry point

#include <codeTools/log/log.h>
#include <iostream>

using namespace rev;

int main()
{
	codeTools::Log::init();
	revLog() << "Rose: Robotic Simulation Environment\n";
	codeTools::Log::end();
	return 0;
}