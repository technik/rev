////////////////////////////////////////////////////////////////////////////////
// Revolution Engine game client
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on June 10th, 2012
////////////////////////////////////////////////////////////////////////////////
// Revolution Engine Game client

#include <revGame/gameClient/gameClient.h>

class CRevGameClient : public rev::game::SGameClient
{
public:
	CRevGameClient();
	~CRevGameClient();

	void init	();
	bool update	();	///< Update the game

private:
	void updateCamera();

	float mCamSpeed;
	float mCamTurboFactor;
	float mCamTurnSpeed;
};