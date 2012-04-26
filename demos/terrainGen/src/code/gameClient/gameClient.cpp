////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Terrain Generator
// by Jose Luis Alvarez-Ossorio Rodal (a.k.a. iovo)
// Created on April 26th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "gameClient.h"
#include <revCore/math/vector.h>
#include <revCore/node/node.h>
#include <revInput/keyboardInput/keyboardInput.h>
#include <revVideo/camera/perspectiveCamera.h>
#include <revVideo/video.h>
#include <revVideo/videoDriver/videoDriver.h>
#include <revVideo/viewport/viewport.h>

using namespace rev;
using namespace rev::game;
using namespace rev::input;
using namespace rev::video;

//----------------------------------------------------------------------------------------------------------------------
terrainGenerator::terrainGenerator()
	:mCamera(0)
{
	// Create the camera
	mCamera = new CPerspectiveCamera(45.f, 1.5f, CVec2(0.f, 100000.f));
	CNode * camNode = new CNode();
	camNode->addComponent(mCamera);
	// Create a viewport and attach the camera to it
	CViewport * view = new CViewport(rev::CVec2::zero, SVideo::getDriver()->screenSize(), 0.f);
	view->setCamera(mCamera);
}

//----------------------------------------------------------------------------------------------------------------------
terrainGenerator::~terrainGenerator()
{
	// Intentionally blank
}

//----------------------------------------------------------------------------------------------------------------------
bool terrainGenerator::update()
{
	// CGameClient::update calls some internal methods to keep the engine running;
	if(!CGameClient::update()) // If it returns false, it means the engine intends to close the game
		return false; // So we close the gamet

	// Exit if someone pressed ESC
	SKeyboardInput * keyboard = SKeyboardInput::get();	// This lets you get input from the keyboard
	if(keyboard->pressed(SKeyboardInput::eEscape)) // If pressed ESC
		return false; // Exit the game

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Enter your code below
	return true;
}