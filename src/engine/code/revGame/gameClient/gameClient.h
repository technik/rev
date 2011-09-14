////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 20th, 2011
////////////////////////////////////////////////////////////////////////////////
// basic game client

#ifndef _REV_REVGAME_GAMECLIENT_GAMECLIENT_H_
#define _REV_REVGAME_GAMECLIENT_GAMECLIENT_H_

namespace rev	{	namespace game
{

	class CGameClient
	{
	public:
		//--------------------------------------------------------------------------
		// Basic interface
		//--------------------------------------------------------------------------
		virtual	void	init	();	// Initialise the game
		virtual	bool	update	();	// Update the game, returns false when the
									// The game requests exit
		virtual	void	end		();	// End the game
	};

}	// namespace game
}	// namespace rev

#endif // _REV_REVGAME_GAMECLIENT_GAMECLIENT_H_
