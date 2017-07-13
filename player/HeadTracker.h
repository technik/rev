//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <core/components/component.h>
#include <math/algebra/matrix.h>
#include <math/geometry/ray.h>
#include "CamEstimate.h"
#include <opencv2/opencv.hpp>

namespace rev {

	class ImuProvider;
	class ImageProvider;

	class HeadTracker : public core::Component {
	public:
		HeadTracker(const std::string& _datasetName);

		bool update(float _dt);

		math::Mat34f headTransform() const { return mCam.view(); }

	private:
		bool updateImu	(float _dt);
		bool updateVideo(float _dt);

	private:
		struct PointEstimate {
			math::Vec3f	pos;
			math::Vec3f variance;
		};

		struct MapFeature {
			PointEstimate	pos;
			math::Vec3f		normal;
			cv::Mat			imgPatch;
		};

		struct PartialHypothesis {
			typedef cv::Point2i		FeatureDesc;
			PartialHypothesis(const math::Ray& _ray, const FeatureDesc& _startPos);

			cv::Mat			imgPatch;
			std::vector<PointEstimate>	hypotheses;

			void update(const math::Ray&);
			bool matches(const cv::Point2i& _corner) const;

			bool isComplete() const; // True when all hypotheses have collapsed to a single point
			MapFeature&	collapsedEstimate() const;
		};

		float mImuDt = 0.f;
		ImuProvider*	mImu = nullptr;
		ImageProvider*	mVideoSrc = nullptr;
		CamEstimate		mCam;
		vector<PartialHypothesis>	mOpenHypotheses;
	};

}
