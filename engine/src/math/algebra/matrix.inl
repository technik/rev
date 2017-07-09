//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Generic matrix
#ifndef _REV_MATH_ALGEBRA_MATRIX_INL_
#define _REV_MATH_ALGEBRA_MATRIX_INL_

#include <cmath>
#include "matrix.h"

namespace rev {
	namespace math {

		//------------------------------------------------------------------------------------------------------------------
		template<typename Number_>
		inline Matrix3x3<Number_>::Matrix3x3(const Matrix3x4<Number_>& _x)
		{
			for (unsigned i = 0; i < 3; ++i) {
				for (unsigned j = 0; j < 3; ++j)
					m[i][j] = _x[i][j];
			}
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename Number_>
		inline Matrix3x3<Number_> Matrix3x3<Number_>::identity()
		{
			Matrix3x3<Number_> mtx;
			for (unsigned int row = 0; row < 3; ++row)
			{
				for (unsigned int column = 0; column < 3; ++column)
				{
					if (column == row)
						mtx.m[row][column] = Number_(1.f);
					else
						mtx.m[row][column] = Number_(0.f);
				}
			}
			return mtx;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename Number_>
		inline Vector3<Number_> Matrix3x3<Number_>::operator*(const Vector3<Number_>& _v) const
		{
			Vector3<Number_>	v;
			for (unsigned i = 0; i < 3; ++i)
				v[i] = _v[0] * m[i][0] + _v[1] * m[i][1] + _v[2] * m[i][2];
			return v;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename Number_>
		inline Matrix3x3<Number_> Matrix3x3<Number_>::operator*(const Matrix3x3<Number_>& _b) const
		{
			Matrix3x3<Number_>	ab;
			for (unsigned i = 0; i < 3; ++i)
				for (unsigned j = 0; j < 3; ++j)
					ab[i][j] = m[i][0] * _b[0][j] + m[i][1] * _b[1][j] + m[i][2] * _b[2][j];
			return ab;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename Number_>
		inline Matrix3x4<Number_>::Matrix3x4(const Quaternion<Number_>& _q)
		{
			Number_ a2 = _q.w*_q.w;
			Number_ b2 = _q.x*_q.x;
			Number_ c2 = _q.y*_q.y;
			Number_ d2 = _q.z*_q.z;
			Number_ ab = 2 * _q.w*_q.x;
			Number_ ac = 2 * _q.w*_q.y;
			Number_ ad = 2 * _q.w*_q.z;
			Number_ bc = 2 * _q.x*_q.y;
			Number_ bd = 2 * _q.x*_q.z;
			Number_ cd = 2 * _q.y*_q.z;
			m[0][0] = a2 + b2 - c2 - d2;
			m[0][1] = bc - ad;
			m[0][2] = bd + ac;
			m[1][0] = bc + ad;
			m[1][1] = a2 - b2 + c2 - d2;
			m[1][2] = cd - ab;
			m[2][0] = bd - ac;
			m[2][1] = cd + ab;
			m[2][2] = a2 - b2 - c2 + d2;
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
			Number_ ab = 2 * _q.w*_q.x;
			Number_ ac = 2 * _q.w*_q.y;
			Number_ ad = 2 * _q.w*_q.z;
			Number_ bc = 2 * _q.x*_q.y;
			Number_ bd = 2 * _q.x*_q.z;
			Number_ cd = 2 * _q.y*_q.z;
			m[0][0] = a2 + b2 - c2 - d2;
			m[0][1] = bc - ad;
			m[0][2] = bd + ac;
			m[1][0] = bc + ad;
			m[1][1] = a2 - b2 + c2 - d2;
			m[1][2] = cd - ab;
			m[2][0] = bd - ac;
			m[2][1] = cd + ab;
			m[2][2] = a2 - b2 - c2 + d2;
			// Translation
			m[0][3] = _v[0];
			m[1][3] = _v[1];
			m[2][3] = _v[2];
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename Number_>
		inline Matrix3x4<Number_>::Matrix3x4(const Matrix3x3<Number_>& _x)
		{
			for (unsigned i = 0; i < 3; ++i) {
				for (unsigned j = 0; j < 3; ++j)
					m[i][j] = _x[i][j];
				m[i][3] = Number_(0.f);
			}
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename Number_>
		inline Vector3<Number_> Matrix3x4<Number_>::operator*(const Vector3<Number_>& _v) const
		{
			Vector3<Number_>	v;
			for (unsigned i = 0; i < 3; ++i)
				v[i] = _v[0] * m[i][0] + _v[1] * m[i][1] + _v[2] * m[i][2] + m[i][3];
			return v;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename Number_>
		inline Matrix3x3<Number_> Matrix3x4<Number_>::operator*(const Matrix3x3<Number_>& _b) const
		{
			Matrix3x3<Number_>	ab;
			for (unsigned i = 0; i < 3; ++i)
			for (unsigned j = 0; j < 3; ++j)
				ab[i][j] = m[i][0] * _b[0][j] + m[i][1] * _b[1][j] + m[i][2] * _b[2][j];
			return ab;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename Number_>
		inline Matrix3x4<Number_> Matrix3x4<Number_>::operator*(const Matrix3x4<Number_>& _b) const
		{
			Matrix3x4<Number_> ab;
			for (unsigned row = 0; row < 3; ++row)
			for (unsigned column = 0; column < 4; ++column)
				ab[row][column] = m[row][0] * _b[0][column]
				+ m[row][1] * _b[1][column]
				+ m[row][2] * _b[2][column];
			ab[0][3] += m[0][3];
			ab[1][3] += m[1][3];
			ab[2][3] += m[2][3];
			return ab;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename Number_>
		inline Matrix3x4<Number_> Matrix3x4<Number_>::inverse() const
		{
			// Affine transformation's inverse
			//     | A ··· b |         | A^-1 ··· -(A^-1)b |
			// m = | 0 ··· 1 |, m^-1 = |  0   ···     1    |
			// Since A is indeed orthonormal, A^-1 = transpose(A)
			Matrix3x4<Number_> dst;
			for (unsigned int row = 0; row < 3; ++row)
			{
				Vector4<Number_>& dstRow = dst[row];
				for (unsigned int col = 0; col < 3; ++col)
					dstRow[col] = m[col][row]; // Transpose the core matrix
				dstRow[3] = -(dstRow[0] * m[0][3] + dstRow[1] * m[1][3] + dstRow[2] * m[2][3]); // Invert the translation
			}
			return dst;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename Number_>
		inline Vector3<Number_>	Matrix3x4<Number_>::rotate(const Vector3<Number_>& _v) const
		{
			Vector3<Number_> result;
			for (unsigned i = 0; i < 3; ++i)
			{
				result[i] = m[i][0] * _v[0] + m[i][1] * _v[1] + m[i][2] * _v[2];
			}
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename Number_>
		inline Matrix3x4<Number_> Matrix3x4<Number_>::identity()
		{
			Matrix3x4<Number_> mtx;
			for (unsigned int row = 0; row < 3; ++row)
			{
				for (unsigned int column = 0; column < 4; ++column)
				{
					if (column == row)
						mtx.m[row][column] = Number_(1.f);
					else
						mtx.m[row][column] = Number_(0.f);
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
			Number_ focalLength = Number_(1.f) / tan(_fovRad / 2.f);
			Number_ invDepthRange = Number_(1.f) / (_farClip - _nearClip);
			Matrix4x4 f;
			// Row 0
			f.m[0][0] = focalLength;
			f.m[0][1] = Number_(0.f);
			f.m[0][2] = Number_(0.f);
			f.m[0][3] = Number_(0.f);
			// Row 1   
			f.m[1][0] = Number_(0.f);
			f.m[1][1] = Number_(0.f);
			f.m[1][2] = focalLength * _aspectRatio; // Map height from Z to Y
			f.m[1][3] = Number_(0.f);
			// Row 2   
			f.m[2][0] = Number_(0.f);
			f.m[2][1] = (_nearClip + _farClip) * invDepthRange;
			f.m[2][2] = Number_(0.f);
			f.m[2][3] = -2.f * _farClip * _nearClip * invDepthRange;
			// Row 3, homogeneous component
			f.m[3][0] = Number_(0.f);
			f.m[3][1] = Number_(1.f); // Homogeneous component maps to depth
			f.m[3][2] = Number_(0.f);
			f.m[3][3] = Number_(0.f);

			return f;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename Number_>
		inline Matrix4x4<Number_> Matrix4x4<Number_>::ortho(const Vector3<Number_>& _volumeDimensions)
		{
			Matrix4x4 mtx;
			for (unsigned int row = 0; row < 4; ++row)
			{
				for (unsigned int column = 0; column < 4; ++column)
				{
					if (column == row)
						mtx.m[row][column] = row==3 ? Number_(1.f) : 2 * Number_(1.f) / _volumeDimensions[row];
					else
						mtx.m[row][column] = Number_(0.f);
				}
			}
			return mtx;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename T_>
		inline Matrix4x4<T_> Matrix4x4<T_>::identity()
		{
			Matrix4x4 mtx;
			for (unsigned int row = 0; row < 4; ++row)
			{
				for (unsigned int column = 0; column < 4; ++column)
				{
					if (column == row)
						mtx.m[row][column] = T_(1.f);
					else
						mtx.m[row][column] = T_(0.f);
				}
			}
			return mtx;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename Number_>
		inline Matrix4x4<Number_> Matrix4x4<Number_>::operator* (const Matrix3x4<Number_>& _b) const
		{
			Matrix4x4 dst;
			for (unsigned row = 0; row < 4; ++row)
			{
				for (unsigned column = 0; column < 4; ++column)
				{
					dst[row][column] = m[row][0] * _b[0][column]
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
			for (unsigned row = 0; row < 4; ++row)
			{
				for (unsigned column = 0; column < 4; ++column)
				{
					dst[row][column] = m[row][0] * _b[0][column]
						+ m[row][1] * _b[1][column]
						+ m[row][2] * _b[2][column];
					+m[row][3] * _b[3][column];
				}
			}
			return dst;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename Number_>
		inline void Matrix4x4<Number_>::transpose(Matrix4x4& _transpose) const
		{
			for (unsigned i = 0; i < 4; ++i)
			{
				for (unsigned j = 0; j < 4; ++j)
				{
					_transpose.m[i][j] = m[j][i];
				}
			}
		}

	}	// namespace math
}	// namespace rev

#endif // _REV_MATH_ALGEBRA_MATRIX_INL_