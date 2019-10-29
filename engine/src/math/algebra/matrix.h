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
#include "matrixExpr.h"
#include "matrixView.h"

namespace rev {
	namespace math {

		//------------------------------------------------------------------------------------------------------------------
		template<class T, size_t m, size_t n>
		struct Matrix
			: MatrixExpr < T, m, n, Matrix<T, m, n>>
			, MatrixView < T, m, n, Matrix<T, m, n>>
		{
			// Basic construction
			Matrix() = default;
			Matrix(const Matrix&) = default;
			template<typename Other>
			Matrix(const MatrixView<T,m,n,Other>& _x)
			{
				for (size_t i = 0; i < m; ++i)
					for (size_t j = 0; j < n; ++j)
						(*this)(i, j) = _x(i, j);
			}

			Matrix& operator(const Matrix&) = default;

			template<class Other>
			Matrix& operator=(const MatrixExpr<T, m, n, Other>& x)
			{
				for (size_t i = 0; i < m; ++i)
					for (size_t j = 0; j < n; ++j)
						(*this)(i, j) = x(i, j);
				return *this;
			}

			// Smarter construction
			static Matrix identity()	{ Matrix res; res.setIdentity(); return res; }
			static Matrix zero()		{ Matrix res; res.setZero(); return res; }
			static Matrix ones()		{ Matrix res; res.setOnes(); return res; }

			// Generic component accessor.
			T operator()(size_t i, size_t j) const { return data[i][j]; }
			T& operator()(size_t i, size_t j) { return data[i][j]; }

			// Component accessor for when you know the component index at compile time.
			template<size_t i, size_t j>
			T  coefficient() const
			{
				static_assert(i < m);
				static_assert(j < n);
				return data[i][j];;
			}

			template<size_t i, size_t j>
			T& coefficient()
			{
				static_assert(i < m);
				static_assert(j < n);
				return data[i][j];;
			}

		private:
			T data[m][n];
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
			auto A = (f+n)/(n-f);
			auto B = 2*f*n/(n-f);
			return Matrix44<Number_>({
				xFocalLength, 0, 0, 0,
				0, yFocalLength, 0, 0,
				0, 0,			 A, B,
				0, 0,			-1, 0
			});
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename T, class Expr>
		inline Matrix44<T> orthographicMatrix(
			const MatrixExpr<T,2,1,Expr>& _size,
			T _near, T _far)
		{
			return Matrix44<T>({
				2 / _size[0,0], 0,                0,             0,
				0,             2 / _size[1,0],	 0,				0,
				0,             0,		 2 / (_far-_near),      (_near+_far)/(_near-_far),
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