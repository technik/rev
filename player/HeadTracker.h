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
		HeadTracker(const std::string& _datasetName, bool _noVideo);

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
			static constexpr size_t NEstimates = 50;
			static constexpr float depthStep = 10.f;
			PartialHypothesis(const math::Ray& _ray, const FeatureDesc& _desc) {
				hypotheses.resize(NEstimates);
				for (size_t i = 0; i < NEstimates; ++i) {
					float depth = i*depthStep;
					hypotheses[i].pos = _ray.origin + _ray.direction * depth;
					hypotheses[i].variance = math::Vec3f(1.f) * depthStep;
				}
			}

			void update(const math::Ray&) {
				// 666 TODO
			}

			bool matches(const cv::Point2i& _corner) const {
				math::Vec2f dist(float(_corner.x - descriptor.x), float(_corner.y - descriptor.y));
				return dist.norm() < 60.f;
			}

			bool isComplete() const; // True when all hypotheses have collapsed to a single point
			MapFeature&	collapsedEstimate() const;

			FeatureDesc					descriptor;
			std::vector<PointEstimate>	hypotheses;
		};

		ImuProvider*	mImu = nullptr;
		ImageProvider*	mVideoSrc = nullptr;
		CamEstimate		mCam;
		math::Mat33f	mDev2View; // Transforms from device space (IMU space) to View space
		std::vector<PartialHypothesis>	mOpenHypotheses;
	};

}
