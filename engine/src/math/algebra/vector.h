//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
// Generic mathematical vector
#ifndef _REV_MATH_ALGEBRA_VECTOR_H_
#define _REV_MATH_ALGEBRA_VECTOR_H_

#include <cassert>
#include "matrix.h"

namespace rev {
	namespace math {

		template<typename T_, size_t n_>
		using Vector = Matrix<T_, n_, 1>;

		template<typename T_>
		using Vector2 = Vector<T_,2>;
		template<typename T_>
		using Vector3 = Vector<T_,3>;
		template<typename T_>
		using Vector4 = Vector<T_,4>;

		typedef Vector<unsigned, 2> Vec2u;
		typedef Vector<int, 2>		Vec2i;
		typedef Vector<float, 2>	Vec2f;
		typedef Vector<double, 2>	Vec2d;

		typedef Vector3<unsigned>	Vec3u;
		typedef Vector3<int>		Vec3i;
		typedef Vector3<float>		Vec3f;
		typedef Vector3<double>		Vec3d;

		typedef Vector4<unsigned>	Vec4u;
		typedef Vector4<int>		Vec4i;
		typedef Vector4<float>		Vec4f;
		typedef Vector4<double>		Vec4d;

        //--------------------------------------------------------------------------------------------------------------
        // Vector-Vector operations
        //--------------------------------------------------------------------------------------------------------------
        // Dot product
        template<class Mat, size_t n>
        auto dot(const MatrixBase<n, 1, Mat>& a, const MatrixBase<n, 1, Mat>& b) -> typename Mat::Element
        {
            typename Mat::Element result(0);
            for (size_t i = 0; i < n; ++i)
                result += a[i] * b[i];
            return result;
        }

        //--------------------------------------------------------------------------------------------------------------
        template<class Mat, size_t n>
        auto cross(const MatrixBase<n, 1, Mat>& a, const MatrixBase<n, 1, Mat>& b) -> MatrixBase<n, 1, Mat>
        {
            Derived result;
            result.x() = a.y()*b.z() - a.z()*b.y();
            result.y() = a.z()*b.x() - a.x()*b.z();
            result.z() = a.x()*b.y() - a.y()*b.x();
            return result;
        }

        //--------------------------------------------------------------------------------------------------------------
        template<class T, size_t n>
        T squaredNorm(const Vector<T, n>& a)
        {
            return dot(a, a);
        }

        //--------------------------------------------------------------------------------------------------------------
        template<class T, size_t n>
        T norm(const Vector<T, n>& a)
        {
            return std::sqrt(squaredNorm(a));
        }

        //--------------------------------------------------------------------------------------------------------------
        template<class T, size_t n>
        Vector<T,n> normalize(const Vector<T,n>& a)
        {
            return a * (1 / norm(a));
        }
		
	}	// namespace math
}	// namespace rev

#endif // _REV_MATH_ALGEBRA_VECTOR_H_