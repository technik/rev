////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 20th, 2011
////////////////////////////////////////////////////////////////////////////////
// Windows handler: Deals with Windows OS

#ifndef _REV_GAME_GAMECLIENT_WINDOWS_WINDOWSHANDLER_H_
#define _REV_GAME_GAMECLIENT_WINDOWS_WINDOWSHANDLER_H_

namespace rev { namespace game
{
	class CWindowsHandler
	{
	public:
		CWindowsHandler();

		void update();
		bool exitGameRequested() const { return mExitGameRequested; }
	private:
		bool mExitGameRequested;
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GAMECLIENT_WINDOWS_WINDOWSHANDLER_H_