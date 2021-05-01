//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// Generic matrix
#pragma once
#include "matrixView.h"

namespace rev {
	namespace math {

		//------------------------------------------------------------------------------------------------------------------
		template<class T, size_t m, size_t n>
		struct Matrix : MatrixView < T, m, n, Matrix<T, m, n>>
		{
			// Basic construction
			Matrix() = default;
			Matrix(const Matrix&) = default;
			template<typename Other>
			Matrix(const MatrixExpr<T,m,n,Other>& _x)
			{
				for (size_t i = 0; i < m; ++i)
					for (size_t j = 0; j < n; ++j)
						(*this)(i, j) = _x(i, j);
			}

			Matrix(std::initializer_list<T> _l)
			{
				auto x = _l.begin();
				for (size_t i = 0; i < m; ++i)
					for (size_t j = 0; j < n; ++j)
						(*this)(i, j) = *x++;
			}

			Matrix& operator=(const Matrix&) = default;
			using MatrixView<T, m, n, Matrix<T, m, n>>::operator=;

			// Smarter construction
			static Matrix identity()	{ Matrix res; res.setIdentity(); return res; }
			static Matrix zero()		{ Matrix res; res.setZero(); return res; }
			static Matrix ones()		{ Matrix res; res.setOnes(); return res; }

			// Generic component accessor.
			inline T operator()(size_t i, size_t j) const
			{
				return m_data[i][j];
			}
			inline T& operator()(size_t i, size_t j)
			{
				return m_data[i][j];
			}

			inline T* data() { return &m_data[0][0]; }
			inline const T* data() const { return &m_data[0][0]; }

			static constexpr bool is_col_major = false;
		private:
			T m_data[m][n];
		};

		//------------------------------------------------------------------------------------------------------------------
		// Useful aliases
		template<typename T_>
		using Matrix22 = Matrix<T_,2,2>;
		template<typename T_>
		using Matrix33 = Matrix<T_,3,3>;
		template<typename T_>
		using Matrix34 = Matrix<T_,3,4>;
		template<typename T_>
		using Matrix44 = Matrix<T_,4,4>;

		using Mat22f = Matrix22<float>;
		using Mat33f = Matrix33<float>;
		using Mat34f = Matrix34<float>;
		using Mat44f = Matrix44<float>;

		//------------------------------------------------------------------------------------------------------------------
		// Product specialization for matrices
		template<class T, size_t m, size_t l, size_t n>
		Matrix<T, m, n> operator*(const Matrix<T, m, l>& a, const Matrix<T, l, n>& b)
		{
			Matrix<T, m, n> result;
			for (int i = 0; i < m; ++i)
			{
				for (int j = 0; j < n; ++j)
				{
					result(i, j) = a(i, 0) * b(0, j);
					for (int k = 1; k < l; ++k)
						result(i, j) += a(i, k) * b(k, j);
				}
			}

			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename Number_>
		inline Matrix44<Number_> frustumMatrix(
			Number_ yFovRad,
			Number_ aspectRatio,
			Number_ n, // Near clip
			Number_ f) // Far clip
		{
			// Precomputations
			auto yFocalLength = 1 / std::tan(yFovRad / 2);
			auto xFocalLength = yFocalLength / aspectRatio;
			auto B = 2*n;
			// P * (0,0,-n,1) = (0,0,-n,-n) = (0,0,n,n)
			// P.z/w = 1;
			// P * (0,0,-i,1) = (0,0,-n,-i); z/w = 0
			return Matrix44<Number_>({
				xFocalLength, 0, 0, 0,
				0, yFocalLength, 0, 0,
				0, 0,			 0, n,
				0, 0,			-1, 0
			});
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename T, class Expr>
		inline Matrix44<T> orthographicMatrix(
			const MatrixExpr<T,2,1,Expr>& _size,
			T _near, T _far)
		{
			T invRange = 1 / (_near - _far);
			return Matrix44<T>({
				2 / _size(0,0), 0,                0,             0,
				0,             2 / _size(1,0),	 0,				0,
				0,             0,		 -2 * invRange,      (_near+_far) * invRange,
				0,             0,                0,             1
				});
		}

		//------------------------------------------------------------------------------------------------------------------
		inline float affineTransformDeterminant(const Mat44f& x)
		{
			auto det =
				x(0,0) * (x(1,1)*x(2,2)-x(2,1)*x(1,2)) -
				x(0,1) * (x(1,0)*x(2,2)-x(2,0)*x(1,2)) +
				x(0,2) * (x(1,0)*x(2,1)-x(2,0)*x(1,1));
			return det;
		}

	}	// namespace math
}	// namespace rev