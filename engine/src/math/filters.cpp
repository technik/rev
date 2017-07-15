//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include "filters.h"
#include <Eigen/LU> // Required for linking against Matrix::Inverse

namespace rev {
	namespace math {

		//----------------------------------------------------------------------------------------------
		KalmanFilter::KalmanFilter(const Vector& _X, const Matrix& _P) 
			: x(_X)
			, P(_P)
			, I(Matrix::Identity(_X.rows(), _X.rows()))
		{
			// Intentionally blank
		}

		//----------------------------------------------------------------------------------------------
		void KalmanFilter::predict(const Matrix& F, const Matrix& Q) {
			assert(F.rows() == F.cols());
			assert(F.cols() == x.rows());
			assert(Q.rows() == Q.cols());
			assert(Q.rows() == P.rows());

			x = F*x;
			P = F*P*F.transpose() + Q;
		}

		//----------------------------------------------------------------------------------------------
		void KalmanFilter::update(const Vector& z, const Matrix& H, const Matrix& R) {
			assert(z.rows() == H.rows());
			assert(R.rows() == H.rows());

			y = z - H*x; // Observed error
			S = H*P*H.transpose() + R; // Uncertainty of the observed error
			K = P*H.transpose()*S.inverse(); // State update matrix
			// Actual state update
			x = x + K*y;
			P = (I - K*H)*P;
		}
	}
}