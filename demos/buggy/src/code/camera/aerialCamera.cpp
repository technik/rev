#include "aerialCamera.h"

#include <revCore/node/node.h>

#include <revVideo/camera/perspectiveCamera.h>

using namespace rev;
using namespace rev::video;

namespace buggyDemo
{
	//------------------------------------------------------------------------------------------------------------------
	CAerialCamera::CAerialCamera( const rev::CNode * _target )
	{
		mTarget = _target;
		mCamNode = new CNode();
		mCamNode->setPos(mTarget->position());
		mCamera = new CPerspectiveCamera(45, 480.f/800.f, CVec2(1.f, 100.f));
	}

	CAerialCamera::~CAerialCamera()
	{
		delete mCamNode;
		delete mCamera;
	}

	void CAerialCamera::update()
	{
		mCamNode->setPos(mTarget->position());
	}
}