//----------------------------------------------------------------------------------------------------------------------
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <Eigen/Core>
#include <string>
#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>
#include <math/filters.h>
#include <fstream>

namespace rev
{
	class ImuProvider {
	public:
		ImuProvider()
			: mAccelFilter({ 0.f,0.f,0.f }, ACCEL_CUTOFF_FREQ, SAMPLE_PERIOD)
			, mGyroFilter({ 0.f,0.f,0.f }, GYRO_CUTOFF_FREQ, SAMPLE_PERIOD)
		{}

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
		// Filters 
		static constexpr float	SAMPLE_PERIOD = 0.1f;
		static constexpr float	ACCEL_CUTOFF_FREQ = 2.f; ///< Accelerometer filter's cut off frequency (1/s)
		static constexpr float	GYRO_CUTOFF_FREQ = 2.5f; ///< Accelerometer filter's cut off frequency (1/s)
		typedef rev::math::Vec3f	Vec3;
		
		rev::math::LowPassFilter1st<Vec3>	mAccelFilter;
		rev::math::LowPassFilter1st<Vec3>	mGyroFilter;

		// Data source
		std::ifstream		mSrcFile;
		float				mTimeSinceLastFrame = 0.f;

		rev::math::Mat33f	mImuBase; // Rotation only, orthonormal transform


//		static constexpr float	Ca = 0.15f;
//		static constexpr float	Cg = 0.05f;
	};

}	// namespace rev