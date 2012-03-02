////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buggy Demo
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On November 26th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _BUGGYDEMO_GAMECLIENT_H_
#define _BUGGYDEMO_GAMECLIENT_H_

#include "revGame/gameclient/gameclient.h"

namespace rev { namespace video
{
	class CViewport;
}	// namespace video
}	// namespace rev

namespace buggyDemo
{
	class CBuggy;
	class CAerialCamera;
}	// namespace buggyDemo

class CBuggyGameClient : public rev::game::CGameClient
{
public:
	CBuggyGameClient();
	~CBuggyGameClient();
	bool update	();	///< Update the game
private:
	// The demo contains: A landscape, a buggy car, a simple GUI with a controller and a camera
	buggyDemo::CBuggy *			mBuggy;
	buggyDemo::CAerialCamera *	mCamera;
	rev::video::CViewport*		mViewport;
};

#endif // _BUGGYDEMO_GAMECLIENT_H_
