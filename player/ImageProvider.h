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

		bool getFrame(cv::Mat& _dst);

		math::Vec2f toClipSpace(const cv::Point2i& _imageSpacePoint) {
			cv::Size halfImageSize = workSize / 2;
			// Center coordinates, change y axis direction
			math::Vec2f centeredPoint = { float(_imageSpacePoint.x - halfImageSize.width), float(halfImageSize.height - _imageSpacePoint.y) };
			float halfImgWidth = float(halfImageSize.width);
			return centeredPoint / halfImgWidth;
		}

	private:
		cv::Size			workSize;
		cv::VideoCapture	mSrc;
		cv::Mat				mReducedImg;
		float				mTimeSinceLastFrame = 0.f;

		static constexpr float SAMPLE_PERIOD = 0.033f;
	};

}	// namespace rev