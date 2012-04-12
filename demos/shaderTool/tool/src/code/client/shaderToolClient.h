////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, Shader tool
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 12th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Client

#ifndef _SHADERTOOL_CLIENT_SHADERTOOLCLIENT_H_
#define _SHADERTOOL_CLIENT_SHADERTOOLCLIENT_H_

#include <revGame/gameClient/gameClient.h>

class ShaderToolClient : public rev::game::CGameClient
{
public:
	ShaderToolClient();

	bool update();
};

#endif // _SHADERTOOL_CLIENT_SHADERTOOLCLIENT_H_