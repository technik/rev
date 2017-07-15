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
		static constexpr size_t StateDim = 9 + 7; // Dimension of stored space
		static constexpr size_t ObserDim = 3 + 3; // Dimension of observations

		CamEstimate()
		{
			// Restart log file
			std::ofstream stateLog("camLog.csv");
			stateLog.close();
			// Init filter state
			math::KalmanFilter::Vector x0 = Eigen::VectorXf::Zero(StateDim);
			x0(12) = 1; // So we make initial rotation into a unit quaternion
			math::KalmanFilter::Matrix P0 = Eigen::MatrixXf::Identity(StateDim, StateDim)*100.f;
			P0.block<3, 3>(0, 0) = Eigen::Matrix3f::Identity() * 0.001f; // We are sure we start on 0 position (by definition)
			mFilter = math::KalmanFilter(x0, P0);
			// Init model matrices
			Q = Eigen::MatrixXf::Identity(StateDim, StateDim) * 100.f; // Transition error
			R = Eigen::MatrixXf::Identity(ObserDim, ObserDim) * 0.00001f; // Measurement error
			F = Eigen::MatrixXf::Identity(StateDim, StateDim); // Transition function
			H = Eigen::MatrixXf::Zero(ObserDim, StateDim); // Observation function
		}

		void predict(float _dt) {
			// Recompute state dependent and time-dependent matrices
			F.block<3, 3>(0, 3) = Eigen::Matrix3f::Identity() * _dt; // x' = x + v*dt
			F.block<3, 3>(3, 6) = Eigen::Matrix3f::Identity() * _dt; // v' = v + a*dt
			math::Vec3f wsRate = *reinterpret_cast<const math::Vec3f*>(mFilter.X().bottomRows(3).data());
			if(wsRate.norm() > 1e-4f) { // Too small rates just produce numeric noise, so we just skip them
				math::Quatf dRot = math::Quatf(wsRate * _dt);
				Eigen::Matrix4f rotMat;
				rotMat <<	dRot.w, dRot.x, dRot.y,-dRot.z,
							dRot.w,-dRot.x, dRot.y, dRot.z,
							dRot.w, dRot.x,-dRot.y, dRot.z,
							dRot.w,-dRot.x,-dRot.y,-dRot.z;
				F.block<4, 4>(9, 9) = rotMat; // q' = (rate*dt) * q
			}
			else {
				F.block<4, 4>(9, 9) = Eigen::Matrix4f::Identity();
			}

			// Perform actual prediction using those matrices
			mFilter.predict(F, Q);
		}

		void update(const math::Vec3f& _accel, const math::Vec3f& _rate) {
			// Construct observation vector from data
			math::Quatf rateQ(_rate);
			Eigen::VectorXf z(ObserDim);
			z << _accel.x, _accel.y, _accel.z, _rate.x, _rate.y, _rate.z;
			// Update observation function H.
			//First three columns transform world space acceleration (as stored in X)
			// into view space acceleration (which can be observed)
			math::Mat33f viewRot(rot());
			Eigen::Matrix3f viewRotEigen;
			viewRotEigen << Eigen::Vector3f(viewRot[0][0], viewRot[1][0], viewRot[2][0]),
				Eigen::Vector3f(viewRot[0][1], viewRot[1][1], viewRot[2][1]),
				Eigen::Vector3f(viewRot[0][2], viewRot[1][2], viewRot[2][2]);
			H.block<3, 3>(0,6) = viewRotEigen;
			// Next for rows transform ws angular rate into view space. Same transformation, same matrix.
			H.block<3, 3>(3, 13) = viewRotEigen;
			// Update filter
			mFilter.update(z, H, R);
			// Log data for analysis
			std::ofstream stateLog("camLog.csv", std::ios::app);
			stateLog << pos().x << ", " << pos().y << ", " << pos().z << ", "
				<< mFilter.X()(3) << ", " << mFilter.X()(4) << ", " << mFilter.X()(5) << ", "
				<< mFilter.X()(6) << ", " << mFilter.X()(7) << ", " << mFilter.X()(8) << "\n";
		}

		const math::Vec3f& pos() const { return *((math::Vec3f*)mFilter.X().data()); }
		const math::Quatf& rot() const { return *((math::Quatf*)&mFilter.X().data()[9]); }

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
