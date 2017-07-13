//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <core/components/component.h>
#include <math/algebra/matrix.h>
#include "CamEstimate.h"

namespace rev {

	class ImuProvider;
	class ImageProvider;

	class HeadTracker : public core::Component {
	public:
		HeadTracker(const std::string& _datasetName);

		bool update(float _dt);

		math::Mat34f headTransform() const { return mCam.view(); }

	private:
		float mImuDt = 0.f;
		ImuProvider*	mImu = nullptr;
		ImageProvider*	mVideoSrc = nullptr;
		CamEstimate		mCam;
	};

}
