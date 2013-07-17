//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/15
//----------------------------------------------------------------------------------------------------------------------
// Keyboard controlled fly-by camera

#include "flyByCamera.h"

#include <revCore/time/time.h>
#include <revGame/core/transform/sceneNode.h>
#include <revInput/keyboard/keyboardInput.h>

using namespace rev::input;
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
		float dt = Time::get()->frameTime();
	 	KeyboardInput* input = KeyboardInput::get();
		// Translation
		Vec3f velocity = Vec3f::zero();
		if(input->held(KeyboardInput::eD))			velocity.x += 2.f;
	 	if(input->held(KeyboardInput::eA))			velocity.x -= 2.f;
		if(input->held(KeyboardInput::eW))			velocity.y += 2.f;
	 	if(input->held(KeyboardInput::eS))			velocity.y -= 2.f;
		if(input->held(KeyboardInput::eKeyUp))		
			velocity.z += 2.f;
	 	if(input->held(KeyboardInput::eKeyDown))	
			velocity.z -= 2.f;
		mNode->move(velocity * dt);
		// Rotation
		float angSpd = 0.f;
		if(input->held(KeyboardInput::eKeyRight))	angSpd -= 0.3f;
	 	if(input->held(KeyboardInput::eKeyLeft))	angSpd += 0.3f;
		mNode->rotate(Vec3f::zAxis(), angSpd * dt);

		mCam.setView(mNode->transform());
	}

}	// namespace game
}	// namespace rev