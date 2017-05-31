//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
//----------------------------------------------------------------------------------------------------------------------
// Keyboard controlled fly-by transform source

#include "flybySrc.h"

#ifndef ANDROID
#include <input/keyboard/keyboardInput.h>

using namespace rev::input;
#endif // !ANDROID
using namespace rev::math;

namespace rev {
	namespace game {

		//--------------------------------------------------------------------------------------------------------------
		bool FlyBySrc::update(float _dt) {
			static float mult = 1.f;
#ifndef ANDROID
			KeyboardInput* input = KeyboardInput::get();
			if (input->pressed(KeyboardInput::eV))
				mult *= 2.f;
			if (input->pressed(KeyboardInput::eB))
				mult *= 0.5f;
			// Translation
			const float deltaV = 1.f * mult;
			Vec3f velocity = Vec3f::zero();
			if (input->held(KeyboardInput::eD))			velocity.x += deltaV;
			if (input->held(KeyboardInput::eA))			velocity.x -= deltaV;
			if (input->held(KeyboardInput::eW))			
				velocity.y += deltaV;
			if (input->held(KeyboardInput::eS))			velocity.y -= deltaV;
			if (input->held(KeyboardInput::eKeyUp))
				velocity.z += deltaV;
			if (input->held(KeyboardInput::eKeyDown))
				velocity.z -= deltaV;
			Vec3f newPos = transform->position() + transform->transform().rotate(velocity) * (_dt * mSpeed);
			transform->setPosition(newPos);

			// Rotation
			float angSpd = 0.f;
			float deltaG = 0.8f;
			if (input->held(KeyboardInput::eKeyRight))	angSpd -= deltaG;
			if (input->held(KeyboardInput::eKeyLeft))	angSpd += deltaG;

			//rotate(Vec3f::zAxis(), angSpd * _dt);
#endif // ANDROID
			return true;
		}

	}	// namespace game
}	// namespace rev