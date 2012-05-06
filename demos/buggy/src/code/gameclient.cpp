////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buggy Demo
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On November 27th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Engine headers
#include <revVideo/color/color.h>
#include <revVideo/video.h>
#include <revVideo/videoDriver/videoDriver.h>
#include <revVideo/viewport/viewport.h>

// Demo headers
#include "gameclient.h"

#include "buggy/buggy.h"
#include "camera/aerialCamera.h"

using namespace rev;
using namespace rev::video;

//----------------------------------------------------------------------------------------------------------------------
void rev::game::SGameClient::create()
{
	sInstance = new CBuggyGameClient();
	sInstance->init();
}

//----------------------------------------------------------------------------------------------------------------------
CBuggyGameClient::CBuggyGameClient()
{
}

//----------------------------------------------------------------------------------------------------------------------
CBuggyGameClient::~CBuggyGameClient()
{
	//delete mCamera;
	//delete mBuggy;
	//delete mViewport;
}

//----------------------------------------------------------------------------------------------------------------------
void CBuggyGameClient::init()
{
	// Create the buggy
	mBuggy = new buggyDemo::CBuggy();
	// Create a camera
	mCamera = new buggyDemo::CAerialCamera(mBuggy->node());
	SVideo::get()->driver()->setBackgroundColor(CColor::GREEN);
}

//----------------------------------------------------------------------------------------------------------------------
bool CBuggyGameClient::update()
{
	mBuggy->update();
	mCamera->update();
	return SGameClient::update();
}