//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <math/algebra/matrix.h>
#include <math/algebra/quaternion.h>
#include <math/algebra/vector.h>

namespace rev { namespace math {

	struct AffineTransform {
	public:
		AffineTransform& operator=(const AffineTransform& _x) {
			mMatrix = _x.mMatrix;
			return *this;
		}

		static AffineTransform identity() { AffineTransform t; t.mMatrix.setIdentity(); return t; }

		auto position	() 			{ return mMatrix.col(3); }
		auto position	() const	{ return mMatrix.col(3); }

		auto rotation	()			{ return mMatrix.block<3,3>(0,0); }
		auto rotation	()	const	{ return mMatrix.block<3,3>(0,0); }

		void rotate		(const Mat33f& _rot);

		const	Mat34f& matrix() const	{ return mMatrix; }
				Mat34f& matrix()		{ return mMatrix; }

	private:
		Mat34f mMatrix;
	};

	//-------------------------------------------------------------------------------------------
	inline void AffineTransform::rotate(const Mat33f& _rot) {
		rotation() = _rot * rotation();
	}

} }	// namespace rev::math
