////////////////////////////////////////////////////////////////////////////////
// Revolution Engine game client
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on June 10th, 2012
////////////////////////////////////////////////////////////////////////////////
// Revolution Engine Game client

#include "revGameClient.h"

//----------------------------------------------------------------------------------------------------------------------
void rev::game::SGameClient::create()
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
}

//--------------------------------------------------------------------------------
bool CRevGameClient::update()
{
	return SGameClient::update();
}