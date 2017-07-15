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

	namespace {
		//-------------------------------------------------------------------
		void drawRect(cv::Mat& _img, cv::Point2i _center, float _size, cv::Scalar _clr) {
			float halfSize = _size * 0.5f;
			cv::Point2i a = cv::Point2i(int(_center.x - halfSize), int(_center.y - halfSize));
			cv::Point2i b = cv::Point2i(int(_center.x + halfSize), int(_center.y + halfSize));
			cv::rectangle(_img, a, b, _clr);
		}
	}	// anonymous namespace

	//------------------------------------------------------------------------------------------------------------------
	HeadTracker::HeadTracker(const std::string& _datasetName, bool _noVideo) {
		string csvFileName = _datasetName + ".csv";
		mImu = new ImuProvider();
		if (!mImu->init(csvFileName))
			return;
		if (_noVideo)
			return;
		mVideoSrc = new ImageProvider();
		string videoFileName = _datasetName + ".mp4";
		if (!mVideoSrc->init(videoFileName))
			return;
		mDev2View.col(0) = Vec3f::zAxis();
		mDev2View.col(1) = -Vec3f::xAxis();
		mDev2View.col(2) = -Vec3f::yAxis();
	}

	//------------------------------------------------------------------------------------------------------------------
	bool HeadTracker::update(float _dt) {
		mCam.predict(_dt);
		// Update with accelerometer info
		return updateImu(_dt);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool HeadTracker::updateImu(float _dt) {
		if (!mImu->update(_dt)) {
			return false;
		}
		while (mImu->available()) {
			// Get raw sensor data (all in device space)
			Vec3f dsAccel, dsAngRate;
			if (!mImu->getData(dsAccel, dsAngRate))
				return false;
			// Transform data to view space. This transform is fixed, and assumed to carry no uncertainty, so we can
			// perform it outside the kalman filter.
			Vec3f vsAccel = mDev2View * dsAccel;
			Vec3f vsRate = mDev2View * dsAngRate;
			// Fuse sensor data
			mCam.update(vsAccel, vsRate);
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool HeadTracker::updateVideo(float _dt) {
		if (!mVideoSrc)
			return true;
		mVideoSrc->update(_dt);
		if (mVideoSrc->available()) {
			cv::Mat reduced;
			cv::Mat frame;
			if (!mVideoSrc->getFrame(reduced))
				return false;
			cvtColor(reduced, frame, CV_BGR2GRAY);
			// --- Update map features & cam
			// --- Find corners
			std::vector<cv::Point2i>	corners;
			cv::goodFeaturesToTrack(frame, corners, 25, 0.01, 5.0);
			for (const auto& c : corners) {
				math::Vec2f ssPoint = mVideoSrc->toClipSpace(c);
				math::Vec3f ssRayDir{ ssPoint.x, ssPoint.y, 1.f };
				math::Ray cornerRay = { mCam.pos(), mCam.view().rotate(ssRayDir.normalized()) };
				// --- Update hypotheses
				bool matchedCorner = false;
				for (auto& h : mOpenHypotheses) {
					if (h.matches(c)) // If this corner can match an old hypothesis
					{
						matchedCorner = true;
						h.update(cornerRay);
						drawRect(reduced, c, 5.f, cv::Scalar(0.0, 255.0, 0.0));
						break;
					}
				}
				// --- Generate hypotheses
				if (!matchedCorner) {
					drawRect(reduced, c, 5.f, cv::Scalar(0, 0, 255));
					mOpenHypotheses.push_back(PartialHypothesis(cornerRay, c));
				}
			}
			cv::imshow("frame", reduced);
		}
		return true;
	}
}