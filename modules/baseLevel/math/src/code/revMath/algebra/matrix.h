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
		Vector3<Number_>		col			(unsigned _col) const	{ return Vector3<Number_>(mRow[0][_col],mRow[1][_col],mRow[1][_col]);}

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
		Vector3<Number_>		col			(unsigned _col) const	{ return Vector3<Number_>(m[0][_col],m[1][_col],m[2][_col]);}

		// Operations
		Vector3<Number_>	operator*	(const Vector3<Number_>&) const;
		Matrix3x3<Number_>	operator*	(const Matrix3x3<Number_>&) const;
		Matrix3x4<Number_>	operator*	(const Matrix3x4&) const;
		void				inverse	(Matrix3x4<Number_>& _dst) const; // Matrix is treated like an affine transform
		Vector3<Number_>	rotate		(const Vector3<Number_>& ) const;

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
		Matrix4x4	operator*	(const Matrix4x4<Number_>& _b) const;
		void		transpose	(Matrix4x4& _transpose) const;

		// Useful matrices
		static Matrix4x4		frustrum	(Number_ _fovRad, Number_ _aspectRatio, Number_ _nearClip, Number_ _farClip);
		static Matrix4x4		ortho		(const Vector3<Number_>& _volumeDimensions);
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

	//------------------------------------------------------------------------------------------------------------------
	template<typename Number_>
	inline Matrix3x4<Number_>::Matrix3x4(const Quaternion<Number_>& _q)
	{
		Number_ a2 = _q.w*_q.w;
		Number_ b2 = _q.x*_q.x;
		Number_ c2 = _q.y*_q.y;
		Number_ d2 = _q.z*_q.z;
		Number_ ab = 2*_q.w*_q.x;
		Number_ ac = 2*_q.w*_q.y;
		Number_ ad = 2*_q.w*_q.z;
		Number_ bc = 2*_q.x*_q.y;
		Number_ bd = 2*_q.x*_q.z;
		Number_ cd = 2*_q.y*_q.z;
		m[0][0] = a2+b2-c2-d2;
		m[0][1] = bc-ad;
		m[0][2] = bd+ac;
		m[1][0] = bc+ad;
		m[1][1] = a2-b2+c2-d2;
		m[1][2] = cd-ab;
		m[2][0] = bd-ac;
		m[2][1] = cd+ab;
		m[2][2] = a2-b2-c2+d2;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename Number_>
	inline Matrix3x4<Number_>::Matrix3x4(const Quaternion<Number_>& _q, const Vector3<Number_>& _v)
	{
		// Rotation
		Number_ a2 = _q.w*_q.w;
		Number_ b2 = _q.x*_q.x;
		Number_ c2 = _q.y*_q.y;
		Number_ d2 = _q.z*_q.z;
		Number_ ab = 2*_q.w*_q.x;
		Number_ ac = 2*_q.w*_q.y;
		Number_ ad = 2*_q.w*_q.z;
		Number_ bc = 2*_q.x*_q.y;
		Number_ bd = 2*_q.x*_q.z;
		Number_ cd = 2*_q.y*_q.z;
		m[0][0] = a2+b2-c2-d2;
		m[0][1] = bc-ad;
		m[0][2] = bd+ac;
		m[1][0] = bc+ad;
		m[1][1] = a2-b2+c2-d2;
		m[1][2] = cd-ab;
		m[2][0] = bd-ac;
		m[2][1] = cd+ab;
		m[2][2] = a2-b2-c2+d2;
		// Translation
		m[0][3] = _v[0];
		m[1][3] = _v[1];
		m[2][3] = _v[2];
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename Number_>
	inline Matrix3x4<Number_>::Matrix3x4(const Matrix3x3<Number_>& _x)
	{
		for(unsigned i = 0; i < 3; ++i) {
			for(unsigned j = 0; j < 3; ++j)
				m[i][j] = _x[i][j];
			m[i][3] = NumericTraits<Number_>::zero();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename Number_>
	inline Vector3<Number_> Matrix3x4<Number_>::operator*(const Vector3<Number_>& _v) const
	{
		Vector3<Number_>	v;
		for(unsigned i = 0; i < 3; ++i)
			v[i] = _v[0]*m[i][0] + _v[1]*m[i][1] + _v[2]*m[i][2];
		return v;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename Number_>
	inline Matrix3x3<Number_> Matrix3x4<Number_>::operator*(const Matrix3x3<Number_>& _b) const
	{
		Matrix3x3<Number_>	ab;
		for(unsigned i = 0; i < 3; ++i)
			for(unsigned j = 0; j < 3; ++j)
				ab[i][j] = m[i][0]*_b[0][j] + m[i][1]*_b[1][j] + m[i][2]*_b[2][j];
		return ab;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename Number_>
	inline Matrix3x4<Number_> Matrix3x4<Number_>::operator*(const Matrix3x4<Number_>& _b) const
	{
		Matrix3x4<Number_> ab;
		for(unsigned row = 0; row < 3; ++row)
			for(unsigned column = 0; column < 4; ++ column)
				ab[row][column] = m[row][0]*_b[0][column]
								+ m[row][1]*_b[1][column]
								+ m[row][2]*_b[2][column];
		ab[0][3] += m[0][3];
		ab[1][3] += m[1][3];
		ab[2][3] += m[2][3];
		return ab;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename Number_>
	inline void Matrix3x4<Number_>::inverse(Matrix3x4<Number_>& _dst) const
	{
		// Affine transformation's inverse
		//     | A ··· b |         | A^-1 ··· -(A^-1)b |
		// m = | 0 ··· 1 |, m^-1 = |  0   ···     1    |
		// Since A is indeed orthonormal, A^-1 = transpose(A)
		for(unsigned int row = 0; row < 3; ++row)
		{
			Vector4<Number_>& dstRow = _dst[row];
			for(unsigned int col = 0; col < 3; ++col)
				dstRow[col] = m[col][row]; // Transpose the core matrix
			dstRow[3] = - (dstRow[0] * m[0][3] + dstRow[1] * m[1][3] + dstRow[2] * m[2][3]); // Invert the translation
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<typename Number_>
	inline Vector3<Number_>	Matrix3x4<Number_>::rotate(const Vector3<Number_>& _v) const
	{
		Vector3<Number_> result;
		for(unsigned i = 0; i < 3; ++i)
		{
			result[i] = m[i][0]*_v[0]+m[i][1]*_v[1]+m[i][2]*_v[2];
		}
		return result;
	}

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
	// Matrix 4x4

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
	inline Matrix4x4<Number_> Matrix4x4<Number_>::ortho(const Vector3<Number_>& _volumeDimensions)
	{
		Matrix4x4 mtx;
		for(unsigned int row = 0; row < 4; ++row)
		{
			for(unsigned int column = 0; column < 4; ++column)
			{
				if(column == row)
					mtx.m[row][column] = row>3?NumericTraits<Number_>::one():2*NumericTraits<Number_>::one()/_volumeDimensions[row];
				else
					mtx.m[row][column] = NumericTraits<Number_>::zero();
			}
		}
		return mtx;
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
	inline Matrix4x4<Number_> Matrix4x4<Number_>::operator* (const Matrix4x4<Number_>& _b) const
	{
		Matrix4x4 dst;
		for(unsigned row = 0; row < 4; ++row)
		{
			for(unsigned column = 0; column < 4; ++column)
			{
				dst[row][column]	= m[row][0] * _b[0][column]
									+ m[row][1] * _b[1][column]
									+ m[row][2] * _b[2][column];
									+ m[row][3] * _b[3][column];
			}
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
