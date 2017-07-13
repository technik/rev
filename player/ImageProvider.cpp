//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "ImageProvider.h"
#include <iostream>

namespace rev {
	bool ImageProvider::init(const std::string& _fileName) {
		workSize = cv::Size(640, 360);
		cv::namedWindow("frame");
		mSrc.open(_fileName);
		if (!mSrc.isOpened()) {
			std::cout << "Error: Unable to open video \"" << _fileName << "\"\n";
			return false;
		}
		return true;
	}

	bool ImageProvider::getFrame(cv::Mat& _dst) {
		cv::Mat frame;
		mSrc >> frame;
		mTimeSinceLastFrame = 0.f;
		if (frame.empty())
			return false;
		
		cv::resize(frame, mReducedImg, workSize);
		cvtColor(mReducedImg, _dst, CV_BGR2GRAY);
		return true;
	}
}