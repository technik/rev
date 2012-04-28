////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Terrain Generator
// by Jose Luis Alvarez-Ossorio Rodal (a.k.a. iovo)
// Created on April 26th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include "gameClient.h"
#include <revCore/math/vector.h>
#include <revCore/node/node.h>
#include <revCore/time/time.h>
#include <revCore/resourceManager/resourceManager.h>
#include <revCore/resourceManager/passiveResourceManager.h>
#include <revInput/keyboardInput/keyboardInput.h>
#include <revVideo/camera/perspectiveCamera.h>
#include <revVideo/video.h>
#include <revVideo/videoDriver/videoDriver.h>
#include <revVideo/viewport/viewport.h>
#include <revVideo/material/basic/diffuseTextureMaterial.h>
#include <revVideo/material/basic/solidColorMaterial.h>
#include <revVideo/scene/model/staticModel.h>
#include <revGame/scene/mesh/meshGenerator.h>
#include <revGame/scene/object/staticObject.h>

using namespace rev;
using namespace rev::game;
using namespace rev::input;
using namespace rev::video;
CStaticObject * cubos[10000];
float heightmap[10000];

//----------------------------------------------------------------------------------------------------------------------
terrainGenerator::terrainGenerator()
{
	// Register resources
	IMaterial::manager()->registerResource(new CDiffuseTextureMaterial("grass.png"), "block");
	CStaticModel::manager()->registerResource( CMeshGenerator::box(CVec3(1.f, 1.f, 1.f)), "block");

	genHeightmap();
	for(int i=0; i<100; ++i)
	{
		for(int j=0; j<100; ++j)
		{
			cubos[i+100*j] = new CStaticObject("block", "block", CVec3(i*1.0f,j*1.0f,heightmap[i+100*j]));
		}
		
	}

	
}

//-------------------------------------------------------------------------------------------------------------------
void terrainGenerator::updateCamera(float _time)
{
	TReal fwdStep = 3.f * _time;
	TReal upStep = 1.f * _time;
	TReal sideStep = 3.f * _time;
	TReal turnStep = 1.f * _time;
	SKeyboardInput * keyboard = SKeyboardInput::get();
	CNode * camNode = camera3d()->node();
	if(keyboard->held(SKeyboardInput::eKeyLeft))
		camNode->move(CVec3(-sideStep, 0.f, 0.f));
	if(keyboard->held(SKeyboardInput::eKeyRight))
		camNode->move(CVec3(sideStep, 0.f, 0.f));
	if(keyboard->held(SKeyboardInput::eKeyUp))
		camNode->move(CVec3(0.f, 0.f, upStep));
	if(keyboard->held(SKeyboardInput::eKeyDown))
		camNode->move(CVec3(0.f, 0.f, -upStep));
	if(keyboard->held(SKeyboardInput::eW))
		camNode->move(CVec3::yAxis * fwdStep);
	if(keyboard->held(SKeyboardInput::eS))
		camNode->move(CVec3::yAxis * -fwdStep);
	if(keyboard->held(SKeyboardInput::eA))
		camNode->rotate(CVec3::zAxis, turnStep);
	if(keyboard->held(SKeyboardInput::eD))
		camNode->rotate(CVec3::zAxis, -turnStep);
}

//----------------------------------------------------------------------------------------------------------------------
terrainGenerator::~terrainGenerator()
{
	// Intentionally blank
}

//----------------------------------------------------------------------------------------------------------------------

void terrainGenerator::genHeightmap()
{
	for(int i=0; i<100; ++i)
	{
		for(int j=0; j<100; ++j)
		{
			heightmap[i+100*j] = (i*i-j*j+j)/500.0f;
		}
		
	}
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
	updateCamera(STime::get()->frameTime());

	return true;
}