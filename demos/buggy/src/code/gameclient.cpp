////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buggy Demo
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On November 27th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Engine headers
#include <revCore/math/noise.h>
#include <revGame/gui/guiPanel.h>
#include <revGame/gui/webPanel/webPanel.h>
#include <revGame/scene/mesh/meshGenerator.h>
#include <revGame/scene/object/staticObject.h>
#include <revVideo/color/color.h>
#include <revVideo/font/font.h>
#include <revVideo/material/basic/diffuseTextureMaterial.h>
#include <revVideo/scene/model/staticModel.h>
#include <revVideo/texture/texture.h>
#include <revVideo/video.h>
#include <revVideo/videoDriver/videoDriver.h>
#include <revVideo/viewport/viewport.h>

// Demo headers
#include "gameclient.h"

#include "buggy/buggy.h"
#include "camera/aerialCamera.h"

using namespace rev;
using namespace rev::video;
using namespace rev::game;

//----------------------------------------------------------------------------------------------------------------------
void SGameClient::create()
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
	SVideo::get()->driver()->setBackgroundColor(CColor::LIGHT_BLUE);

	CWebPanel * testWeb = new CWebPanel(320, 240);
	testWeb->runHtml("Hello from an HTML panel");

	CStaticModel::registerResource(CMeshGenerator::terrain(125, 512, 50.f), "terrain");
	IMaterial::registerResource(new CDiffuseTextureMaterial("sand.png"), "sand");
	new CStaticObject("terrain", "sand");
}

//----------------------------------------------------------------------------------------------------------------------
bool CBuggyGameClient::update()
{
	mBuggy->update();
	mCamera->update();
	return SGameClient::update();
}