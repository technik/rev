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

// #include <revCore/interpreter/parser.h>
// #include <revCore/interpreter/token.h>

using namespace rev;
using namespace rev::video;
using namespace rev::game;

// enum EGrammarRule
// {
// 	eStatement,
// 	eExpression
// };
// 
// enum EToken
// {
// 	eNum,
// 	ePlus,
// };
// 
// CSyntagma expr = { eExpression,	false};
// CSyntagma plus = { ePlus,		true };
// CSyntagma numb = { eNum,		true };

//----------------------------------------------------------------------------------------------------------------------
void SGameClient::create()
{
	sInstance = new CBuggyGameClient();
	sInstance->init();

	// // Tokens to parse
	// rtl::vector<CToken>	tkList;
	// CToken t = {eNum, 0, 0, 0};
	// CToken t2 = {ePlus, 0, 0, 0};
	// tkList.push_back(t);
	// tkList.push_back(t2);
	// tkList.push_back(t);
	// 
	// // -- Grammar
	// CGrammarRule grammar[3];
	// // Statement : Expression
	// grammar[0].from = eStatement;
	// grammar[0].to.push_back(expr);
	// // Expression : Expression + Expression
	// grammar[1].from = eExpression;
	// grammar[1].to.push_back(expr);
	// grammar[1].to.push_back(plus);
	// grammar[1].to.push_back(expr);
	// // Expression : Number
	// grammar[2].from = eExpression;
	// grammar[2].to.push_back(numb);
	// 
	// // -- Create the parser
	// CParser * parser = new CParser(grammar, 3);
	// parser->generateParseTree(tkList);
	// delete parser;
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