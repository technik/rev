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

#include <revCore/fsm/regExpFsm.h>

using namespace rev;
using namespace rev::video;
using namespace rev::game;

//----------------------------------------------------------------------------------------------------------------------
void SGameClient::create()
{
	sInstance = new CBuggyGameClient();
	sInstance->init();

	rev::CRegExpFsm	empty("");
	empty.accepts("", 0);
	empty.accepts("hello", 4);

	rev::CRegExpFsm a("a");
	a.accepts("", 0);
	a.accepts("a", 1);
	a.accepts("b", 1);
	a.accepts("aaa", 3);

	rev::CRegExpFsm dis("a|b+");
	dis.accepts("", 0);
	dis.accepts("a", 1);
	dis.accepts("b", 1);
	dis.accepts("c", 1);
	dis.accepts("abbab", 5);
	
	rev::CRegExpFsm exclude("[^a]");
	exclude.accepts("", 0);
	exclude.accepts("b", 1);
	exclude.accepts("a", 1);
	exclude.accepts("bb", 2);

	rev::CRegExpFsm range("[0-9]+");
	range.accepts("", 0);
	range.accepts("0", 1);
	range.accepts("1", 1);
	range.accepts("9", 1);
	range.accepts("a", 1);
	range.accepts("123", 3);

	rev::CRegExpFsm floating("-?[0-9]+.[0-9]+f?");
	floating.accepts("-",1);
	floating.accepts("1.57f", 3);
	floating.accepts("-.2", 3);
	floating.accepts("-138.222f", 8);
	floating.accepts("-.", 2);
	floating.accepts("1111.222", 8);
	floating.accepts("-112", 3);
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
	testWeb->loadPage("testPage.html");
	// testWeb->runHtml("Hello from an HTML panel");

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