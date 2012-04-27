#include "aerialCamera.h"

#include <revCore/node/node.h>

#include <revVideo/camera/perspectiveCamera.h>

using namespace rev;
using namespace rev::video;

namespace buggyDemo
{
	//------------------------------------------------------------------------------------------------------------------
	CAerialCamera::CAerialCamera( rev::CNode * _target )
	{
		mTarget = _target;
		mCamNode = new CNode();
		mCamNode->setPos(mTarget->position());
		mCamera = new CPerspectiveCamera(45.f, 1.f, CVec2(0.f, 10000.f));
		mCamera->attachTo(mCamNode);
		mCamNode->attachTo(_target);
	}

	CAerialCamera::~CAerialCamera()
	{
		delete mCamNode;
		delete mCamera;
	}

	void CAerialCamera::update()
	{
		// 
		mCamNode->setPos(mTarget->position() + CVec3(0.f,-10.f,0.f));
	}
}