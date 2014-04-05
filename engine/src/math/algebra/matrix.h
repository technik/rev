//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Generic matrix
#ifndef _REV_MATH_ALGEBRA_MATRIX_H_
#define _REV_MATH_ALGEBRA_MATRIX_H_

#include "vector.h"

namespace rev {
	namespace math {

		template<typename Number_> class Quaternion;
		template<typename Number_> class Matrix3x4;

		//------------------------------------------------------------------------------------------------------------------
		template<class Number_>
		class Matrix3x3
		{
		public:
			Matrix3x3(){}
			Matrix3x3(const Quaternion<Number_>& _q);
			Matrix3x3(const Matrix3x4<Number_>& _x);

			Vector3<Number_>	operator*	(const Vector3<Number_>&) const;
			Matrix3x3<Number_>	operator*	(const Matrix3x3<Number_>&) const;

			Vector3<Number_>&		operator[]	(unsigned _row)			{ return mRow[_row]; }
			const Vector3<Number_>&	operator[]	(unsigned _row) const	{ return mRow[_row]; }
			Vector3<Number_>		col(unsigned _col) const	{ return Vector3<Number_>(mRow[0][_col], mRow[1][_col], mRow[1][_col]); }

		private:
			Vector3<Number_>	mRow[3];
		};

		//------------------------------------------------------------------------------------------------------------------
		template<class Number_>
		class Matrix3x4
		{
		public:
			Matrix3x4(){}
			Matrix3x4(const Quaternion<Number_>& _q);
			Matrix3x4(const Quaternion<Number_>& _q, const Vector3<Number_>& _v);
			Matrix3x4(const Matrix3x3<Number_>& _x);

			Vector4<Number_>&		operator[]	(unsigned _row)			{ return m[_row]; }
			const Vector4<Number_>&	operator[]	(unsigned _row) const	{ return m[_row]; }
			Vector3<Number_>		col(unsigned _col) const	{ return Vector3<Number_>(m[0][_col], m[1][_col], m[2][_col]); }

			// Operations
			Vector3<Number_>	operator*	(const Vector3<Number_>&) const;
			Matrix3x3<Number_>	operator*	(const Matrix3x3<Number_>&) const;
			Matrix3x4<Number_>	operator*	(const Matrix3x4&) const;
			void				inverse(Matrix3x4<Number_>& _dst) const; // Matrix is treated like an affine transform
			Vector3<Number_>	rotate(const Vector3<Number_>&) const;

			// Useful matrices
			static Matrix3x4<Number_>		identity();

		private:
			Vector4<Number_>	m[3];
		};

		//------------------------------------------------------------------------------------------------------------------
		template<class Number_>
		class Matrix4x4
		{
		public:
			Vector4<Number_>&		operator[]	(unsigned _row)			{ return m[_row]; }
			const Vector4<Number_>&	operator[]	(unsigned _row) const	{ return m[_row]; }

			// Operators
			Matrix4x4	operator*	(const Matrix3x4<Number_>& _b) const;
			Matrix4x4	operator*	(const Matrix4x4<Number_>& _b) const;
			void		transpose(Matrix4x4& _transpose) const;

			// Useful matrices
			static Matrix4x4		frustrum(Number_ _fovRad, Number_ _aspectRatio, Number_ _nearClip, Number_ _farClip);
			static Matrix4x4		ortho(const Vector3<Number_>& _volumeDimensions);
			static Matrix4x4		identity();

		private:
			Vector4<Number_>	m[4];
		};

		//------------------------------------------------------------------------------------------------------------------
		// Useful typedefs
		//------------------------------------------------------------------------------------------------------------------
		typedef Matrix3x3<float>	Mat33f;
		typedef Matrix3x4<float>	Mat34f;
		typedef Matrix4x4<float>	Mat44f;

	}	// namespace math
}	// namespace rev

#include "matrix.inl"

#endif // _REV_MATH_ALGEBRA_MATRIX_H_