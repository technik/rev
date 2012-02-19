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
		/// Constructor: Initializes basic game services
		CGameClient			();
		virtual ~CGameClient	(); ///< Destroy the application


		virtual	bool	update	();	// Update the game, returns false when the
									// The game requests exit
	};

}	// namespace game
}	// namespace rev

#endif // _REV_REVGAME_GAMECLIENT_GAMECLIENT_H_
