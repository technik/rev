 //----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 8th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Linear algebra's vector classes

#ifndef _REV_MATH_ALGEBRA_MATRIX_H_
#define _REV_MATH_ALGEBRA_MATRIX_H_

#include "vector.h"

namespace rev { namespace math {

	//------------------------------------------------------------------------------------------------------------------
	template<class Number_>
	class Matrix3x3
	{
	public:
		Vector3<Number_>&		operator[]	(unsigned _row)			{ return mRow[_row]; }
		const Vector3<Number_>&	operator[]	(unsigned _row) const	{ return mRow[_row]; }

	private:
		Vector3<Number_>	mRow[3];
	};

	//------------------------------------------------------------------------------------------------------------------
	template<class Number_>
	class Matrix3x4
	{
	public:
		Vector4<Number_>&		operator[]	(unsigned _row)			{ return m[_row]; }
		const Vector4<Number_>&	operator[]	(unsigned _row) const	{ return m[_row]; }

		// Operators

		// Useful matrices
		static Matrix3x4<Number_>		identity	();

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
		//Matrix4x4	operator*	(const Matrix4x4<Number_>& _b) const;
		void		transpose	(Matrix4x4& _transpose) const;

		// Useful matrices
		static Matrix4x4		frustrum	(Number_ _fovRad, Number_ _aspectRatio, Number_ _nearClip, Number_ _farClip);
		static Matrix4x4		identity	();

	private:
		Vector4<Number_>	m[4];
	};

	//------------------------------------------------------------------------------------------------------------------
	// Useful typedefs
	//------------------------------------------------------------------------------------------------------------------
	typedef Matrix3x3<float>	Mat33f;
	typedef Matrix3x4<float>	Mat34f;
	typedef Matrix4x4<float>	Mat44f;

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementation
	//------------------------------------------------------------------------------------------------------------------
	template<typename Number_>
	inline Matrix3x4<Number_> Matrix3x4<Number_>::identity()
	{
		Matrix3x4<Number_> mtx;
		for(unsigned int row = 0; row < 3; ++row)
		{
			for(unsigned int column = 0; column < 4; ++column)
			{
				if(column == row)
					mtx.m[row][column] = NumericTraits<Number_>::one();
				else
					mtx.m[row][column] = NumericTraits<Number_>::zero();
			}
		}
		return mtx;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename Number_>
	inline Matrix4x4<Number_> Matrix4x4<Number_>::frustrum(Number_ _fovRad, Number_ _aspectRatio, Number_ _nearClip, Number_ _farClip)
	{
		// Precomputations
		Number_ focalLength = NumericTraits<Number_>::one() / tan(_fovRad/2.f);
		Number_ invDepthRange = NumericTraits<Number_>::one() / (_farClip - _nearClip);
		Matrix4x4 f;
		// Row 0
		f.m[0][0] = focalLength;
		f.m[0][1] = NumericTraits<Number_>::zero();
		f.m[0][2] = NumericTraits<Number_>::zero();
		f.m[0][3] = NumericTraits<Number_>::zero();
		// Row 1   
		f.m[1][0] = NumericTraits<Number_>::zero();
		f.m[1][1] = NumericTraits<Number_>::zero();
		f.m[1][2] = focalLength * _aspectRatio; // Map height from Z to Y
		f.m[1][3] = NumericTraits<Number_>::zero();
		// Row 2   
		f.m[2][0] = NumericTraits<Number_>::zero();
		f.m[2][1] = (_nearClip + _farClip) * invDepthRange;
		f.m[2][2] = NumericTraits<Number_>::zero();
		f.m[2][3] = -2.f * _farClip * _nearClip * invDepthRange;
		// Row 3, homogeneous component
		f.m[3][0] = NumericTraits<Number_>::zero();
		f.m[3][1] = NumericTraits<Number_>::one(); // Homogeneous component maps to depth
		f.m[3][2] = NumericTraits<Number_>::zero();
		f.m[3][3] = NumericTraits<Number_>::zero();

		return f;
	}
	
	//------------------------------------------------------------------------------------------------------------------
	template<typename Number_>
	inline Matrix4x4<Number_> Matrix4x4<Number_>::identity()
	{
		Matrix4x4 mtx;
		for(unsigned int row = 0; row < 4; ++row)
		{
			for(unsigned int column = 0; column < 4; ++column)
			{
				if(column == row)
					mtx.m[row][column] = NumericTraits<Number_>::one();
				else
					mtx.m[row][column] = NumericTraits<Number_>::zero();
			}
		}
		return mtx;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename Number_>
	inline Matrix4x4<Number_> Matrix4x4<Number_>::operator* (const Matrix3x4<Number_>& _b) const
	{
		Matrix4x4 dst;
		for(unsigned row = 0; row < 4; ++row)
		{
			for(unsigned column = 0; column < 4; ++column)
			{
				dst[row][column]	= m[row][0] * _b[0][column]
									+ m[row][1] * _b[1][column]
									+ m[row][2] * _b[2][column];
			}
			dst[row][3] += m[row][3];
		}
		return dst;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename Number_>
	inline void Matrix4x4<Number_>::transpose(Matrix4x4& _transpose) const
	{
		for(unsigned i = 0; i < 4; ++i)
		{
			for(unsigned j = 0; j < 4; ++j)
			{
				_transpose.m[i][j] = m[j][i];
			}
		}
	}

}	// namespace math
}	// namespace rev

#endif // _REV_MATH_ALGEBRA_MATRIX_H_
