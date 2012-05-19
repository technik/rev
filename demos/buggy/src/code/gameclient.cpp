////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buggy Demo
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On November 27th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Engine headers
#include <revCore/math/noise.h>
#include <revCore/node/node.h>
#include <revGame/gui/guiPanel.h>
#include <revGame/gui/webPanel/webPanel.h>
#include <revGame/scene/mesh/meshGenerator.h>
#include <revGame/scene/object/staticObject.h>
#include <revVideo/color/color.h>
#include <revVideo/font/font.h>
#include <revVideo/material/basic/diffuseTextureMaterial.h>
#include <revVideo/material/basic/scriptedMaterial.h>
#include <revVideo/scene/model/quad.h>
#include <revVideo/scene/model/staticModel.h>
#include <revVideo/scene/renderableInstance.h>
#include <revVideo/scene/videoScene.h>
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

	CVec2 screenSize = SVideo::getDriver()->screenSize();

	// Create the background sky
	// -------------------------------------------------------
	CQuad * bgQuad = new CQuad(screenSize);
	CNode * skyNode = new CNode();
	CVideoScene * bgScene = new CVideoScene();
	CScriptedMaterial * skyMaterial = new CScriptedMaterial("skyMaterial.rsc");
	IRenderableInstance * sky = new IRenderableInstance(bgQuad, skyMaterial->getInstance());
	sky->setScene(bgScene);
	sky->attachTo(skyNode);
	CViewport * bgViewport = new CViewport(CVec2::zero, screenSize, -1.f);
	COrthoCamera * skyCam = new COrthoCamera(screenSize, -1.f, 1.f);
	bgViewport->setCamera(skyCam);
	skyCam->setScene(bgScene);
	skyCam->attachTo(skyNode);
	// -------------------------------------------------------

	CWebPanel * testWeb = new CWebPanel(unsigned(screenSize.x)-40, unsigned(screenSize.y)-30);
	testWeb->loadPage("testPage.html");

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