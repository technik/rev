//----------------------------------------------------------------------------------------------------------------------
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "ImuProvider.h"
#include <iostream>

using namespace rev::math;

namespace rev {
	//--------------------------------------------------------------------
	bool ImuProvider::init(const std::string& _fileName) {
		mSrcFile.open(_fileName);
		if (!mSrcFile.is_open()) {
			std::cout << "Error: Unable to open csv file \"" << _fileName << "\"\n";
			return false;
		}
		mImuBase = Mat33f::identity();
		mImuBase.setCol(0, { 0.f, -1.f,  0.f });
		mImuBase.setCol(0, { 0.f,  0.f, -1.f });
		mImuBase.setCol(0, { 1.f,  0.f,  0.f });
		return true;
	}

	//--------------------------------------------------------------------
	bool ImuProvider::getData(Vec3f& _accel, Vec3f& _gyro) {
		if (mSrcFile.eof())
			return false;
		else {
			// Read data from file
			mTimeSinceLastFrame = 0.f;
			char buffer[1024];

			mSrcFile.getline(buffer, 1024);

			float rawData[6];
			char* ptr = buffer;
			for (size_t i = 0; i < 6; ++i) {
				rawData[i] = (float)atof(ptr);
				ptr = strstr(ptr, ",")+1;
			}

			// Filter data
			Vec3f rawAccel = *((Vec3f*)rawData);
			_accel = mImuBase * mAccelFilter(rawAccel);
			Vec3f rawGyro = *((Vec3f*)&rawData[3]);
			_gyro = mImuBase * mGyroFilter(rawGyro);

			return true;
		}
	}
}