//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Engine's default player's entry point
#include "racer.h"

//--------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv)
{
	rev::Racer game;
	game.run(_argc, _argv);

	return 0;
}