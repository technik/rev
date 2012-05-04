////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 20th, 2011
////////////////////////////////////////////////////////////////////////////////
// basic game client

#ifndef _REV_REVGAME_GAMECLIENT_GAMECLIENT_H_
#define _REV_REVGAME_GAMECLIENT_GAMECLIENT_H_

#include <revVideo/camera/orthoCamera.h>
#include <revVideo/camera/perspectiveCamera.h>
#include <revVideo/viewport/viewport.h>

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
	protected:
		video::CPerspectiveCamera *	camera3d() { return m3dCamera; }
		video::COrthoCamera*		camera2d() { return m2dCamera; }
	private:
		void	initEngineSystems();
		void	initBasic3d();
		void	initBasic2d();
	private:
		video::CPerspectiveCamera*	m3dCamera;
		video::COrthoCamera*		m2dCamera;
		video::CViewport*			mViewport;

#ifdef WIN32
		CWindowsHandler * mWindowsHandler;
#endif // WIN32
	};

	//------------------------------------------------------------
	// Implement this function to create your own game client
	CGameClient * createGameClient();
	//------------------------------------------------------------

}	// namespace game
}	// namespace rev

#endif // _REV_REVGAME_GAMECLIENT_GAMECLIENT_H_
