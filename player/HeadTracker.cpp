//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------

#include "HeadTracker.h"
#include <opencv2/opencv.hpp>
#include "ImuProvider.h"
#include "ImageProvider.h"

using namespace std;
using namespace rev::math;

namespace rev {
	//------------------------------------------------------------------------------------------------------------------
	HeadTracker::HeadTracker(const std::string& _datasetName) {
		string csvFileName = _datasetName + ".csv";
		mImu = new ImuProvider();
		if (!mImu->init(csvFileName))
			return;
		mVideoSrc = new ImageProvider();
		string videoFileName = _datasetName + ".mp4";
		if (!mVideoSrc->init(videoFileName))
			return;
	}



	//-------------------------------------------------------------------
	void drawRect(cv::Mat& _img, cv::Point2i _center, float _size, cv::Scalar _clr) {
		float halfSize = _size * 0.5f;
		cv::Point2i a = cv::Point2i(int(_center.x - halfSize), int(_center.y - halfSize));
		cv::Point2i b = cv::Point2i(int(_center.x + halfSize), int(_center.y + halfSize));
		cv::rectangle(_img, a, b, _clr);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool HeadTracker::update(float _dt) {
		mImuDt += _dt;
		if (!mImu->update(_dt)) {
			return false;
		}
		mCam.predict(_dt);
		// Update with accelerometer info
		if (mImu->available()) {
			// Get raw sensor data
			Vec3f accel, angRate;
			if (!mImu->getData(accel, angRate))
				return false;
			// Fuse sensor data
			mCam.update(accel, angRate, mImuDt);
			mImuDt = 0.f;
		}
		// Update with video info
		mVideoSrc->update(_dt);
		if (mVideoSrc->available()) {
			cv::Mat frame;
			if (!mVideoSrc->getFrame(frame))
				return false;
			// --- Update map features & cam
			// --- Update hypotheses
			// --- Find corners
			std::vector<cv::Point2i>	corners;
			cv::goodFeaturesToTrack(frame, corners, 25, 0.01, 5.0);
			for (const auto& c : corners)
				drawRect(frame, c, 5.f, cv::Scalar(255.0, 0.0, 0.0));
			// --- Generate hypotheses
			cv::imshow("frame", frame);
		}
		return true;
	}
}