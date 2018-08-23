//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
// Generic mathematical vector
#pragma once

#include <cassert>
#include "matrixBase.h"

namespace rev {
	namespace math {

		template<typename T, size_t n_>
		struct Vector
			: public MatrixBase
			<
				n_,1 ,
				MatrixDenseStorage
				<
					T, n_, 1,
					true,
					Vector<T, n_>
				>
			>
		{
			using Base = MatrixBase<
				n_, 1,
				MatrixDenseStorage
				<
					T, n_, 1,
					true,
					Vector<T, n_>
				>
			>;
			// Basic construction
			Vector() = default;
			Vector(const Vector&) = default;
			Vector(std::initializer_list<T> _l)
				: Base(_l)
			{
			}

			template<typename Other_>
			Vector(const Other_& _x)
				: Base(_x)
			{
			}

			Vector(T _x, T _y)
				: Base{ _x, _y }
			{
				static_assert(n_ >= 2, "Vector is not big enough");
			}

			Vector(T _x, T _y, T _z)
				: Base{ _x, _y, _z }
			{
				static_assert(n_ >= 3, "Vector is not big enough");
			}

			Vector(T _x, T _y, T _z, T _w)
				: Base{ _x, _y, _z, _w }
			{
				static_assert(n_ >= 4, "Vector is not big enough");
			}

			// Smarter construction
			static Vector zero()        { Vector m; m.setZero(); return m; }
			static Vector ones()        { Vector m; m.setOnes(); return m; }

			T&	x()         { return namedElement<0>(); }
			T	x()	const   { return namedElement<0>(); }
			T&	y()         { return namedElement<1>(); }
			T	y()	const   { return namedElement<1>(); }
			T&	z()         { return namedElement<2>(); }
			T	z()	const   { return namedElement<2>(); }
			T&	w()         { return namedElement<3>(); }
			T	w()	const   { return namedElement<3>(); }

        private:

            template<size_t i>
            T& namedElement()
            {
                return (*this)(i);
            }
            template<size_t i>
            T  namedElement() const
            {
                return (*this)(i);
            }
		};

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
