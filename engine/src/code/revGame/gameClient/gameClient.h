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
#ifdef WIN32
	class CWindowsHandler;
#endif // WIN32

	class CGameClient
	{
	public:
		/// Constructor: Initializes basic game services
		CGameClient			();
		virtual ~CGameClient	(); ///< Destroy the application


		virtual	bool	update	();	// Update the game, returns false when the
									// The game requests exit
	private:
#ifdef WIN32
		CWindowsHandler * mWindowsHandler;
#endif // WIN32
	};

}	// namespace game
}	// namespace rev

#endif // _REV_REVGAME_GAMECLIENT_GAMECLIENT_H_
