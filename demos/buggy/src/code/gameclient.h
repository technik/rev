////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buggy Demo
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On November 26th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _BUGGYDEMO_GAMECLIENT_H_
#define _BUGGYDEMO_GAMECLIENT_H_

#include "revGame/gameclient/gameclient.h"

namespace buggyDemo
{
	class CBuggy;
	class CAerialCamera;
}	// namespace buggyDemo

class CBuggyGameClient : public rev::game::SGameClient
{
public:
	CBuggyGameClient();
	~CBuggyGameClient();

	void init	();
	bool update	();	///< Update the game
private:
	// The demo contains: A landscape, a buggy car, a simple GUI with a controller and a camera
	buggyDemo::CBuggy *			mBuggy;
	buggyDemo::CAerialCamera *	mCamera;
};

#endif // _BUGGYDEMO_GAMECLIENT_H_
