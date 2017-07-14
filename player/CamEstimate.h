//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <math/algebra/quaternion.h>
#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>
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
			x = Eigen::VectorXf::Zero(9);
			P = Eigen::MatrixXf::Identity(9, 9)*100.f;
			Q = Eigen::MatrixXf::Identity(9, 9) * 0.1f;
			R = Eigen::MatrixXf::Identity(3, 3) * 0.1f;
			I = Eigen::MatrixXf::Identity(9, 9);
			F = Eigen::MatrixXf::Identity(9, 9);
			H = Eigen::MatrixXf::Zero(3, 9);
			H.block<3, 3>(0, 6) = Eigen::Matrix3f::Identity();
		}

		void predict(float _dt) {
			// Recompute state dependent matrices
			for (size_t i = 0; i < 6; ++i)
				F(i, 3 + i) = _dt;
			// Perform actual prediction using those matrices
			x = F*x;
			P = F*P*F.transpose() + Q;
		}

		void update(const math::Vec3f& _accel, const math::Vec3f& _rate) {
			// Construct observation vector from data
			//math::Quatf rateQ(_rate);
			Eigen::Vector3f z(_accel.x, _accel.y, _accel.z);// , rateQ.x, rateQ.y, rateQ.z, rateQ.w;
			Eigen::VectorXf y = z - H*x; // Observation error
			S = H*P*H.transpose() + R;
			K = P*H.transpose()*S.inverse();
			x = x + K*y;
			P = (I - K*H)*P;
			// Log data for analysis
			std::ofstream stateLog("camLog.csv", std::ios::app);
			stateLog << pos().x << ", " << pos().y << ", " << pos().z << "\n";
		}

		const math::Vec3f& pos() const { return *((math::Vec3f*)x.data()); }
		math::Quatf rot() const { return math::Quatf::identity(); }

		math::Mat34f view() const { return math::Mat34f(rot(), pos()); }

	private:
		// --- Kalman filter matrices
		// State
		Eigen::VectorXf	x; // Current state estimate
		Eigen::MatrixXf P; // State uncertainty
		// Model
		Eigen::MatrixXf F; // Model transition function
		Eigen::MatrixXf Q; // Model transition noise
		Eigen::MatrixXf H; // Observation matrix
		Eigen::MatrixXf R; // Measurement noise
		// Temporary matrices
		Eigen::MatrixXf I;
		Eigen::MatrixXf S;
		Eigen::MatrixXf K;
	};
}
