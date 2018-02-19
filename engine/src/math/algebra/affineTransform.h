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
		AffineTransform inverse() const;

		AffineTransform operator*(const AffineTransform& b) const
		{
			AffineTransform axb;
			for(int i = 0; i < 3; ++i)
			{
				auto& aRow = mMatrix.block<1,3>(i,0);
				for(size_t j = 0; j < 4; ++j)
				{
					auto& bCol = b.mMatrix.block<3,1>(0,j);
					axb.mMatrix(i,j) = aRow.dot(bCol);
				}
				axb.mMatrix(i,3) += mMatrix(i,3);
			}
			return axb;
		}

		auto&		position	() 			{ return mMatrix.col(3); }
		const auto& position	() const	{ return mMatrix.col(3); }

		void		setRotation	(const Quatf& _q)	{ mMatrix.block<3,3>(0,0) = _q.asMatrix(); }
		auto		rotation	()	const	{ return Quatf(mMatrix.block<3,3>(0,0)); }

		auto&		rotationMtx	()			{ return mMatrix.block<3,3>(0,0); }
		const auto& rotationMtx	()	const	{ return mMatrix.block<3,3>(0,0); }

		void rotate		(const Mat33f& _rot);

		const	Mat34f& matrix() const	{ return mMatrix; }
				Mat34f& matrix()		{ return mMatrix; }

	private:
		Mat34f mMatrix;
	};

	//-------------------------------------------------------------------------------------------
	inline void AffineTransform::rotate(const Mat33f& _rot) {
		rotationMtx() = _rot * rotationMtx();
	}

	//-------------------------------------------------------------------------------------------
	inline AffineTransform AffineTransform::inverse() const {
		AffineTransform inv;
		auto& transposeRot = mMatrix.block<3,3>(0,0).transpose();
		inv.mMatrix.block<3,3>(0,0) = transposeRot;
		inv.position() = -(transposeRot * position());
		return inv;
	}

} }	// namespace rev::math
