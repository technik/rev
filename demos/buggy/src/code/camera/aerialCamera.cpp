#include "aerialCamera.h"

#include <revCore/node/node.h>
#include <revCore/time/time.h>
#include <revGame/gameClient/gameClient.h>
#include <revInput/keyboardInput/keyboardInput.h>
#include <revVideo/camera/perspectiveCamera.h>

using namespace rev;
using namespace rev::input;
using namespace rev::video;

namespace buggyDemo
{
	//------------------------------------------------------------------------------------------------------------------
	CAerialCamera::CAerialCamera( rev::CNode * _target )
	{
		mTarget = _target;
		mCamera = rev::game::SGameClient::get()->camera3d();
		mCamNode = mCamera->node();
		mCamNode->attachTo(_target);
		mCamNode->setPos(mTarget->position() + CVec3(0.f,-10.f,2.f));
	}

	//------------------------------------------------------------------------------------------------------------------
	CAerialCamera::~CAerialCamera()
	{
		delete mCamNode;
		delete mCamera;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CAerialCamera::update()
	{
		move();
	}

	//------------------------------------------------------------------------------------------------------------------
	void CAerialCamera::move()
	{
		float time = STime::get()->frameTime();
		TReal fwdStep = 3.f * time;
		TReal upStep = 1.f * time;
		TReal sideStep = 3.f * time;
		TReal turnStep = 1.f * time;
		SKeyboardInput * keyboard = SKeyboardInput::get();
		if(keyboard->held(SKeyboardInput::eKeyLeft))
			mCamNode->move(CVec3(-sideStep, 0.f, 0.f));
		if(keyboard->held(SKeyboardInput::eKeyRight))
			mCamNode->move(CVec3(sideStep, 0.f, 0.f));
		if(keyboard->held(SKeyboardInput::eKeyUp))
			mCamNode->move(CVec3(0.f, 0.f, upStep));
		if(keyboard->held(SKeyboardInput::eKeyDown))
			mCamNode->move(CVec3(0.f, 0.f, -upStep));
		if(keyboard->held(SKeyboardInput::eW))
			mCamNode->move(CVec3::yAxis * fwdStep);
		if(keyboard->held(SKeyboardInput::eS))
			mCamNode->move(CVec3::yAxis * -fwdStep);
		if(keyboard->held(SKeyboardInput::eA))
			mCamNode->rotate(CVec3::zAxis, turnStep);
		if(keyboard->held(SKeyboardInput::eD))
			mCamNode->rotate(CVec3::zAxis, -turnStep);
	}

}