////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Terrain Generator
// by Jose Luis Alvarez-Ossorio Rodal (a.k.a. iovo)
// Created on April 26th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <gameClient/gameClient.h>
#include <Windows.h>

int main (int _argc, const char **_argv)
{
	// Unused parameters
	_argc; _argv;

	// Create the game client
	terrainGenerator * client = new terrainGenerator();
	
	// Loop the game
	bool exitGame = false;
	while(!exitGame)
	{
		exitGame = !client->update();
	}
	
#ifdef REV_PROFILER
	system("PAUSE");
#endif // REV_PROFILER
	return 0;
}