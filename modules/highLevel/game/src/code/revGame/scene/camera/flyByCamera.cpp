//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/15
//----------------------------------------------------------------------------------------------------------------------
// Keyboard controlled fly-by camera

#include "flyByCamera.h"

#include <revGame/core/transform/sceneNode.h>

using namespace rev::math;

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	FlyByCamera::FlyByCamera(float _fov, float _aspectRatio, float _near, float _far)
		:mCam(Mat44f::frustrum(_fov, _aspectRatio, _near, _far))
		,mNode(new SceneNode)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	FlyByCamera::~FlyByCamera()
	{
		delete mNode;
	}

	//------------------------------------------------------------------------------------------------------------------
	void FlyByCamera::update()
	{
		// TODO: Move
		mCam.setView(mNode->transform());
	}

}	// namespace game
}	// namespace rev