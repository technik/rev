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

#include "matrixBase.h"
#include "vector.h"

namespace rev {
	namespace math {

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_, bool col_major_ = true>
		struct Matrix
			: public MatrixBase
			<
				rows_,
				cols_,
				MatrixDenseStorage
				<
					T_,
					rows_,
					cols_,
					col_major_,
					Matrix<T_, rows_, cols_, col_major_>
				>
			>
		{
			using Base = MatrixBase<
				rows_,
				cols_,
				MatrixDenseStorage
				<
					T_,
					rows_,
					cols_,
					col_major_,
					Matrix
				>
			>;
			using Derived = Matrix;

			// Basic construction
			Matrix() = default;
			Matrix(const Matrix&) = default;
			Matrix(std::initializer_list<T_> _l) 
				: Base(_l)
			{}
			template<typename Other_>
			Matrix(const Other_& _x)
				: Base(_x)
			{}

			// Smarter construction
			static Matrix identity()	{ Matrix m; m.setIdentity(); return m; }
			static Matrix zero()		{ Matrix m; m.setZero(); return m; }
			static Matrix ones()		{ Matrix m; m.setOnes(); return m; }
		};

		//------------------------------------------------------------------------------------------------------------------
		template<size_t m_, size_t n_, size_t l_, typename S1_, typename S2_>
		auto operator*(
			const MatrixBase<m_,n_,S1_>& _a,
			const MatrixBase<n_,l_,S2_>& _b
			) -> Matrix<typename S1_::Element,m_,l_,S1_::is_col_major>
		{
			using Result = Matrix<typename S1_::Element,m_,l_,S1_::is_col_major>;
			Result result;
			for(auto i = 0; i < m_; ++i) { // for each row in _a
				for(auto k = 0; k < l_; ++k) { // for each column in _b
					result(i,k) = typename Result::Element(0);
					for(auto j = 0; j < n_; ++j) { // for each element in this row/col
						result(i,k) += _a(i,j) * _b(j,k);
					}
				}
			}
			return result;
		}

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
		// Returns a frustum matrix that maps into a screen space of x,t -> [-1,1], z -> [0,1], right handed with +x to the
		// right, +y up and +z pointing out from the screen.
		template<typename Number_>
		inline Matrix44<Number_> frustumMatrix(
			Number_ yFovRad,
			Number_ aspectRatio,
			Number_ n, // Near clip
			Number_ f) // Far clip
		{
			assert(yFovRad < math::Constants<Number_>::pi);
			assert(aspectRatio > 0);
			// Precomputations
			auto yFocalLength = 1 / std::tan(yFovRad / 2);
			auto xFocalLength = yFocalLength / aspectRatio;
#if 0 // Old OpenGL convention
			auto A = (f+n)/(n-f);
			auto B = 2*f*n/(n-f);
#else
			/*
			auto invRange = 1 / (f - n);
			auto A = n * invRange;
			auto B = -A * f;*/
			Number_ A = 0;
			Number_ B = n;
#endif
			return Matrix44<Number_>({
				xFocalLength, 0, 0, 0,
				0, yFocalLength, 0, 0,
				0, 0,			 A, B,
				0, 0,			-1, 0
			});
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename Number_>
		inline Matrix44<Number_> orthographicMatrix(
			const Vector<Number_,2>& _size,
			Number_ _near, Number_ _far)
		{
			return Matrix44<Number_>({
				2 / _size.x(), 0,                0,             0,
				0,             2 / _size.y(),	 0,				0,
				0,             0,		 2 / (_near-_far),      (_near+_far)/(_near-_far),
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