//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
//----------------------------------------------------------------------------------------------------------------------
// Keyboard controlled fly-by transform source

#include "flyby.h"

#include <input/keyboard/keyboardInput.h>
using namespace rev::input;

#include "../sceneNode.h"
#include <math/algebra/vector.h>

using namespace rev::math;

namespace rev {
	namespace game {

		//------------------------------------------------------------------------------------------------------------------
		void FlyBy::init() {
			mSrcTransform = node()->component<Transform>();
		}

		//--------------------------------------------------------------------------------------------------------------
		void FlyBy::update(float _dt) {
			static float mult = 1.f;

			KeyboardInput* input = KeyboardInput::get();
			if (input->pressed('V'))
				mult *= 2.f;
			if (input->pressed('B'))
				mult *= 0.5f;
			// Translation
			const float deltaV = 1.f * mult;
			auto velocity = Vec3f::zero();
			if (input->held('D'))			velocity.x() += deltaV;
			if (input->held('A'))			velocity.x() -= deltaV;
			if (input->held('W'))			velocity.y() += deltaV;
			if (input->held('S'))			velocity.y() -= deltaV;
			if (input->held(KeyboardInput::Key::KeyUp))
				velocity.z() += deltaV;
			if (input->held(KeyboardInput::Key::KeyDown))
				velocity.z() -= deltaV;
			auto& transform = mSrcTransform->xForm;
			transform.position() = transform.position() + transform.rotateDirection(velocity) * (_dt * mSpeed);

			// Rotation
			float angSpd = 0.f;
			float deltaG = 0.8f;
			if (input->held(KeyboardInput::Key::KeyRight))	angSpd -= deltaG;
			if (input->held(KeyboardInput::Key::KeyLeft))	angSpd += deltaG;

			transform.rotate(Quatf::fromAxisAngle({0,0,1}, angSpd * _dt));
		}

	}	// namespace game
}	// namespace rev