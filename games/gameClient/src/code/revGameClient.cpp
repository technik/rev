////////////////////////////////////////////////////////////////////////////////
// Revolution Engine game client
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on June 10th, 2012
////////////////////////////////////////////////////////////////////////////////
// Revolution Engine Game client

#include "revGameClient.h"

#include <revCore/file/file.h>
#include <revCore/node/node.h>
#include <revCore/time/time.h>
#include <revCore/variant/variant.h>
#include <revInput/keyboardInput/keyboardInput.h>
#include <revInput/touchInput/touchInput.h>
#include <revScript/script.h>

using namespace rev;
using namespace rev::input;
using namespace rev::script;

//----------------------------------------------------------------------------------------------------------------------
void game::SGameClient::create()
{
	sInstance = new CRevGameClient();
	sInstance->init();
}

//--------------------------------------------------------------------------------
CRevGameClient::CRevGameClient()
{
}

//--------------------------------------------------------------------------------
CRevGameClient::~CRevGameClient()
{
}

//--------------------------------------------------------------------------------
void CRevGameClient::init()
{
	// Run script entry point
	CFile entryFile("main.rsc");
	CScript entryScript(entryFile.textBuffer());
	entryScript.run();

	// Camera configuration
	CFile cameraConfigFile("camera.rsc");
	CScript cameraScript (cameraConfigFile.textBuffer());
	cameraScript.run();
	CVariant temp;
	CScript::getVar("cameraSpeed", temp);
	mCamSpeed = temp.asFloat(2.f);
	CScript::getVar("cameraTurbo", temp);
	mCamTurboFactor = temp.asFloat(1.5f);
	CScript::getVar("cameraTurnSpeed", temp);
	mCamTurnSpeed = temp.asFloat(1.5f);
}

//--------------------------------------------------------------------------------
bool CRevGameClient::update()
{
	updateCamera();
	return true;
}

//------------------------------------------------------------------------------------------------------------------
void CRevGameClient::updateCamera()
{
	float deltaTime = STime::get()->frameTime();
	SKeyboardInput * keyboard = SKeyboardInput::get();
	CNode * camNode = camera3d()->node();
	// Camera turbo
	float camSpeed = mCamSpeed * (keyboard->held(SKeyboardInput::eShift))?mCamTurboFactor:1.f;
	// Camera displacement
	if(keyboard->held(SKeyboardInput::eKeyUp))
		camNode->move(deltaTime * CVec3::yAxis * camSpeed);
	if(keyboard->held(SKeyboardInput::eKeyDown))
		camNode->move(-deltaTime * CVec3::yAxis * camSpeed);
	if(keyboard->held(SKeyboardInput::eD))
		camNode->move(deltaTime * CVec3::xAxis * camSpeed);
	if(keyboard->held(SKeyboardInput::eA))
		camNode->move(-deltaTime * CVec3::xAxis * camSpeed);
	if(keyboard->held(SKeyboardInput::eW))
		camNode->move(deltaTime * CVec3::zAxis * camSpeed);
	if(keyboard->held(SKeyboardInput::eS))
		camNode->move(-deltaTime * CVec3::zAxis * camSpeed);
	// Camera rotation
	if(keyboard->held(SKeyboardInput::eKeyLeft))
		camNode->rotate(CVec3::zAxis, deltaTime);
	if(keyboard->held(SKeyboardInput::eKeyRight))
		camNode->rotate(CVec3::zAxis, -deltaTime);
}
