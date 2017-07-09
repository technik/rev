//----------------------------------------------------------------------------------------------------------------------
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <Eigen/Core>
#include <string>
#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>
#include <fstream>

namespace rev
{
	class ImuProvider {
	public:
		bool init(const std::string& _fileName);

		bool update(float _dt) {
			if (!mSrcFile.is_open())
				return false;
			mTimeSinceLastFrame += _dt;
			return true;
		}

		bool available() const {
			return mTimeSinceLastFrame >= SAMPLE_PERIOD;
		}

		bool getData(rev::math::Vec3f& _accel, rev::math::Vec3f& _gyro);

	private:
		std::ifstream		mSrcFile;
		float				mTimeSinceLastFrame = 0.f;
		rev::math::Vec3f	mAccelDrift = rev::math::Vec3f::zero();
		rev::math::Vec3f	mGyroDrift = rev::math::Vec3f::zero();

		rev::math::Mat33f	mImuBase; // Rotation only, orthonormal transform

		static constexpr float	Ca = 0.15f;
		static constexpr float	Cg = 0.05f;
		static constexpr float	SAMPLE_PERIOD = 0.1f;
	};

}	// namespace rev