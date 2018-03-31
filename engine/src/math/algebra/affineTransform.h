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

		AffineTransform operator*(const AffineTransform& b) const
		{
			AffineTransform axb;
			axb.mMatrix = mMatrix*b.mMatrix;
			return axb;
		}

		Vec3f		transformPosition	(const Vec3f& v) const { return mMatrix.block<3,3>(0,0)*v + mMatrix.block<3,1>(0,0); }
		Vec3f		rotateDirection		(const Vec3f& v) const { return mMatrix.block<3,3>(0,0)*v; }

		AffineTransform orthoNormalInverse() const {
			AffineTransform x = AffineTransform::identity();
			x.mMatrix.block<3,3>(0,0) = mMatrix.block<3,3>(0,0).transpose();
			x.mMatrix.block<3,1>(0,3) = - x.mMatrix.block<3,3>(0,0) * mMatrix.block<3,1>(0,3);
			return x;
		}

		auto&		position	() 			{ return mMatrix.block<3,1>(0,3); }
		const auto& position	() const	{ return mMatrix.block<3,1>(0,3); }

		void		setRotation	(const Quatf& _q)	{ mMatrix.block<3,3>(0,0) = Mat33f(_q); }
		auto		rotation	()	const	{ return Quatf(mMatrix.block<3,3>(0,0)); }
		const auto& rotationMtx	()	const	{ return mMatrix.block<3,3>(0,0); }

		void rotate		(const Quatf& _q) { 
			Mat44f rot = Mat44f::identity();
			rot.block<3,3>(0,0) = (Mat33f)_q;
			mMatrix = rot * mMatrix;
		}

		const	Mat44f& matrix() const	{ return mMatrix; }
				Mat44f& matrix()		{ return mMatrix; }

	private:
		Mat44f mMatrix;
	};

} }	// namespace rev::math
