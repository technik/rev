//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------

#include "HeadTracker.h"
#include <opencv2/opencv.hpp>
#include "ImuProvider.h"

using namespace std;
using namespace rev::math;

namespace rev {
	//------------------------------------------------------------------------------------------------------------------
	HeadTracker::HeadTracker(const std::string& _datasetName) {
		string csvFileName = _datasetName + ".csv";
		mImu = new ImuProvider();
		if (!mImu->init(csvFileName))
			return;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool HeadTracker::update(float _dt) {
		mImuDt += _dt;
		if (!mImu->update(_dt)) {
			return false;
		}
		mCam.predict(_dt);
		if (mImu->available()) {
			// Get raw sensor data
			Vec3f accel, angRate;
			if (!mImu->getData(accel, angRate))
				return false;
			// Fuse sensor data
			mCam.update(accel, angRate, mImuDt);
			mImuDt = 0.f;
		}
		return true;
	}
}