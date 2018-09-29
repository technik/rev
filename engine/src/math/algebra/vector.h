//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
// Generic mathematical vector
#pragma once

#include <algorithm>
#include <cassert>
#include "matrixBase.h"

namespace rev {
	namespace math {

        template<typename T, size_t n, class Derived>
        struct VectorExpr
        {
            T operator[](size_t i) const { return static_cast<const Derived&>(*this)[i]; }

            template<size_t i>
            T  getComponent() const
            {
                static_assert(i < n);
                auto& derived = static_cast<const Derived&>(*this);
                return derived.template getComponent<i>();
            }

            T	x()	const { return getComponent<0>(); }
            T	y()	const { return getComponent<1>(); }
            T	z()	const { return getComponent<2>(); }
            T	w()	const { return getComponent<3>(); }
        };

		template<typename T, size_t n>
		struct Vector : VectorExpr<T, n, Vector<T,n>>
			, public MatrixBase
			<
				n,1 ,
				MatrixDenseStorage
				<
					T, n, 1,
					true,
					Vector<T, n>
				>
			>
		{
			using Base = MatrixBase<
				n, 1,
				MatrixDenseStorage
				<
					T, n, 1,
					true,
					Vector<T, n>
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
				static_assert(n >= 2, "Vector is not big enough");
			}

			Vector(T _x, T _y, T _z)
				: Base{ _x, _y, _z }
			{
				static_assert(n >= 3, "Vector is not big enough");
			}

			Vector(T _x, T _y, T _z, T _w)
				: Base{ _x, _y, _z, _w }
			{
				static_assert(n >= 4, "Vector is not big enough");
			}

			// Smarter construction
			static Vector zero()        { Vector m; m.setZero(); return m; }
			static Vector ones()        { Vector m; m.setOnes(); return m; }

            T&	operator[]	(size_t i)       { return m[i]; }
            T	operator[] 	(size_t i) const { return m[i]; }

            using VectorExpr<T, n, Vector<T, n>>::x;
            using VectorExpr<T, n, Vector<T, n>>::y;
            using VectorExpr<T, n, Vector<T, n>>::z;
            using VectorExpr<T, n, Vector<T, n>>::w;

			T&	x()         { return m[0]; }
			T&	y()         { return m[1]; }
			T&	z()         { return m[2]; }
			T&	w()         { return m[3]; }

            // Compile time accessors
            template<size_t i>
            T& getComponent()
            {
                static_assert(i < n);
                return m[i];
            }
            template<size_t i>
            T  getComponent() const
            {
                static_assert(i < n);
                return m[i];
            }
		};

		template<typename T, size_t n, class Derived>
		Vector<T,n> operator/(const VectorExpr<T,n,Derived> v, T k)
		{
			Vector<T,n> result;
			for(size_t i = 0; i < n; ++i)
				result[i] =  v[i] / k;
			return result;
		}

		template<typename T_>
		using Vector2 = Vector<T_,2>;
		template<typename T_>
		using Vector3 = Vector<T_,3>;
		template<typename T_>
		using Vector4 = Vector<T_,4>;

		typedef Vector<unsigned, 2> Vec2u;
		typedef Vector<uint16_t, 2> Vec2u16;
		typedef Vector<int, 2>		Vec2i;
		typedef Vector<float, 2>	Vec2f;
		typedef Vector<double, 2>	Vec2d;

		typedef Vector3<unsigned>	Vec3u;
		typedef Vector3<uint8_t>	Vec3u8;
		typedef Vector3<int>		Vec3i;
		typedef Vector3<float>		Vec3f;
		typedef Vector3<double>		Vec3d;

		typedef Vector4<unsigned>	Vec4u;
		typedef Vector4<uint8_t>	Vec4u8;
		typedef Vector4<int>		Vec4i;
		typedef Vector4<float>		Vec4f;
		typedef Vector4<double>		Vec4d;

        //--------------------------------------------------------------------------------------------------------------
        // Vector-Vector operations
        //--------------------------------------------------------------------------------------------------------------
        // Dot product
        template<class T, size_t n, class Derived1, class Derived2>
        T dot(const VectorExpr<T, n, Derived1>& a, const VectorExpr<T, n, Derived2>& b)
        {
            T result(0);
            for (size_t i = 0; i < n; ++i)
                result += a[i] * b[i];
            return result;
        }

        //--------------------------------------------------------------------------------------------------------------
        template<class T, size_t n>
        Vector<T, n> cross(const Vector<T, n>& a, const Vector<T, n>& b)
        {
            Vector<T, n> result;
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
