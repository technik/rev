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
		,mSpeed(1.f)
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
		static float mult = 1.f;
		// TODO: Move
		float dt = Time::get()->frameTime();
		KeyboardInput* input = KeyboardInput::get();
		if(input->pressed(KeyboardInput::eV))
			mult *= 2.f;
		if(input->pressed(KeyboardInput::eB))
			mult *= 0.5f;
		// Translation
		const float deltaV = 1.f * mult;
		Vec3f velocity = Vec3f::zero();
		if(input->held(KeyboardInput::eD))			velocity.x += deltaV;
	 	if(input->held(KeyboardInput::eA))			velocity.x -= deltaV;
		if(input->held(KeyboardInput::eW))			velocity.y += deltaV;
	 	if(input->held(KeyboardInput::eS))			velocity.y -= deltaV;
		if(input->held(KeyboardInput::eKeyUp))		
			velocity.z += deltaV;
	 	if(input->held(KeyboardInput::eKeyDown))	
			velocity.z -= deltaV;
		mNode->move(velocity * mSpeed *dt);
		// Rotation
		float angSpd = 0.f;
		float deltaG = 0.8f;
		if(input->held(KeyboardInput::eKeyRight))	angSpd -= deltaG;
	 	if(input->held(KeyboardInput::eKeyLeft))	angSpd += deltaG;
		mNode->rotate(Vec3f::zAxis(), angSpd * dt);

		mCam.setView(mNode->transform());
	}

}	// namespace game
}	// namespace rev