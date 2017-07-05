//----------------------------------------------------------------------------------------------------------------------
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <opencv2/opencv.hpp>
#include <string>

namespace rev {

	//--------------------------------------------------------------------
	class ImageProvider {
	public:
		bool init(const std::string& _fileName);

		void update(float _dt) {
			mTimeSinceLastFrame += _dt;
		}

		bool available() const {
			return mTimeSinceLastFrame >= SAMPLE_PERIOD;
		}

		bool getFrame(cv::Mat& _dst) {
			mSrc >> _dst;
			mTimeSinceLastFrame = 0.f;
			return !_dst.empty();
		}

	private:
		cv::VideoCapture	mSrc;
		float				mTimeSinceLastFrame = 0.f;

		static constexpr float SAMPLE_PERIOD = 0.033f;
	};

}	// namespace rev