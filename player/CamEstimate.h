//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <math/algebra/quaternion.h>
#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>
#include <math/filters.h>
#include <Eigen/Core>
#include <Eigen/LU> // For MatrixBase::Inverse
#include <fstream>

namespace rev {

	struct CamEstimate {
		static constexpr size_t dimension = 9;

		CamEstimate()
		{
			// Restart log file
			std::ofstream stateLog("camLog.csv");
			stateLog.close();
			// Init filter state
			math::KalmanFilter::Vector x0 = Eigen::VectorXf::Zero(9);
			math::KalmanFilter::Matrix P0 = Eigen::MatrixXf::Identity(9, 9)*100.f;
			mFilter = math::KalmanFilter(x0, P0);
			// Init model matrices
			Q = Eigen::MatrixXf::Identity(9, 9) * 0.1f; // Transition error
			R = Eigen::MatrixXf::Identity(3, 3) * 0.1f; // Measurement error
			F = Eigen::MatrixXf::Identity(9, 9); // Transition function
			H = Eigen::MatrixXf::Zero(3, 9); // Observation function
			H.block<3, 3>(0, 6) = Eigen::Matrix3f::Identity();
		}

		void predict(float _dt) {
			// Recompute state dependent matrices
			F.block<3, 3>(0, 3) = Eigen::Matrix3f::Identity() * _dt;
			F.block<3, 3>(3, 6) = Eigen::Matrix3f::Identity() * _dt;
			// Perform actual prediction using those matrices
			mFilter.predict(F, Q);
		}

		void update(const math::Vec3f& _accel, const math::Vec3f& _rate) {
			// Construct observation vector from data
			//math::Quatf rateQ(_rate);
			Eigen::Vector3f z(_accel.x, _accel.y, _accel.z);// , rateQ.x, rateQ.y, rateQ.z, rateQ.w;
			// Update filter
			mFilter.update(z, H, R);
			// Log data for analysis
			std::ofstream stateLog("camLog.csv", std::ios::app);
			stateLog << pos().x << ", " << pos().y << ", " << pos().z << ", "
				<< mFilter.X()(3) << ", " << mFilter.X()(4) << ", " << mFilter.X()(5) << ", "
				<< mFilter.X()(6) << ", " << mFilter.X()(7) << ", " << mFilter.X()(8) << "\n";
		}

		const math::Vec3f& pos() const { return *((math::Vec3f*)mFilter.X().data()); }
		math::Quatf rot() const { return math::Quatf::identity(); }

		math::Mat34f view() const { return math::Mat34f(rot(), pos()); }

	private:
		math::KalmanFilter	mFilter;
		// Model definition
		Eigen::MatrixXf F; // Model transition function
		Eigen::MatrixXf Q; // Model transition noise
		Eigen::MatrixXf H; // Observation matrix
		Eigen::MatrixXf R; // Measurement noise
	};
}
