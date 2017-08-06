//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Transform source for the component system
#pragma once

#include <core/time/time.h>
#include <game/scene/component.h>
#include <math/algebra/vector.h>
#include "../sceneNode.h"
#include "objTransform.h"

namespace rev { namespace game {

	class Spinner : public Component {
	public:
		void setSpin(const math::Vec3f& rotationVector); // This rotation will be applied per second

		void onCreate() override {
			mTarget = node().getComponent<ObjTransform>();
			assert(mTarget);
		}

		void update() override {
			float dt = core::Time::get()->frameTime();
			mTarget->rotate(math::Quatf(mRotation * dt));
		}

	private:
		ObjTransform* mTarget = nullptr;
		math::Vec3f mRotation = math::Vec3f::zero();
	};

}}
