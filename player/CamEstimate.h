//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <math/algebra/quaternion.h>
#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>

namespace rev {

	struct CamEstimate {
		math::Vec3f pos; // World space position
		math::Vec3f vel; // World space velocity
		math::Quatf rot;
		static constexpr size_t dimension = 10;

		CamEstimate()
			: pos(math::Vec3f::zero())
			, vel(math::Vec3f::zero())
			, rot(math::Quatf::identity())
		{}

		void predict(float _dt) {
			pos += vel*_dt;
		}

		void update(const math::Vec3f& _accel, const math::Vec3f& _rate, float _dt) {
			vel = vel + (rot.rotate(_accel)) * _dt;
			auto rotStep = math::Quatf(_rate * _dt);
			rot = rotStep * rot;
			//std::ofstream stateLog("camLog.csv", ios::app);
			//stateLog << pos.x << ", " << pos.y << ", " << pos.z << ", " << vel.x << ", " << vel.y << ", " << vel.z << "\n";
		}

		math::Mat34f view() const { return math::Mat34f(rot, pos); }
	};
}
