////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Terrain Generator
// by Jose Luis Alvarez-Ossorio Rodal (a.k.a. iovo)
// Created on April 26th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _TERRAINGEN_GAMECLIENT_GAMECLIENT_H_
#define _TERRAINGEN_GAMECLIENT_GAMECLIENT_H_

#include <revCore/math/vector.h>
#include <revGame/gameClient/gameClient.h>

// Forward declarations
namespace rev
{
	class CNode;
	namespace video { class CPerspectiveCamera; }
}	// namespace rev

class terrainGenerator : public rev::game::CGameClient
{
public:
	// Class constructor
	terrainGenerator(); // Here goes the code to initialize the game.
	// Class destructor
	~terrainGenerator(); // This get's called when you exit the game.

	// Update the game
	bool	update(); // Here goes the code for the main loop of the game.
	// Returns false to exit the game.
	// Returns true otherways.

	void		initPools();
	rev::CNode*	createCube(const rev::CVec3& _pos);

	void updateCamera(float _time);

	void genHeightmap();

	void growSquare(int xMin, int xMax, int yMin, int yMax, unsigned char growth);

private:
};

#endif // _TERRAINGEN_GAMECLIENT_GAMECLIENT_H_