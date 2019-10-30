//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
// Generic mathematical vector
#pragma once

#include <algorithm>
#include <cassert>
#include "matrixView.h"

namespace rev {
	namespace math {

		template<typename T, size_t n>
		struct Vector : MatrixView<T, n, 1, Vector<T,n>>
		{
			template<class Derived>
			using VectorExpr = MatrixExpr<T, n, 1, Derived>;

			// Basic construction
			Vector() = default;
			Vector(const Vector&) = default;

			template<class Other>
			Vector(const VectorExpr<Other>& x)
			{
				for (size_t i = 0; i < n; ++i)
					m[i] = x(i,0);
			}

			template<class OtherT>
			Vector(const MatrixExpr<T, n-1, 1, OtherT>& x, float f)
			{
				for (size_t i = 0; i < n - 1; ++i)
				{
					m[i] = x[i];
				}
				m[n - 1] = f;
			}

			Vector(T _x, T _y)
				: m{ _x, _y }
			{
				static_assert(n >= 2, "Vector is not big enough");
			}

			Vector(T _x, T _y, T _z)
				: m{ _x, _y, _z }
			{
				static_assert(n >= 3, "Vector is not big enough");
			}

			Vector(T _x, T _y, T _z, T _w)
				: m{ _x, _y, _z, _w }
			{
				static_assert(n >= 4, "Vector is not big enough");
			}

			Vector(std::initializer_list<T> _l)
			{
				auto x = _l.begin();
				for (size_t i = 0; i < n; ++i)
					m[i] = *x++;
			}

            T&	operator[]	(size_t i)       { return m[i]; }
            T	operator[] 	(size_t i) const { return m[i]; }
			T&	operator()(size_t i, size_t)		{ return m[i]; }
			T	operator()(size_t i, size_t) const	{ return m[i]; }

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

			T* data() { return m; }
			const T* data() const { return m; }

		private:
			T m[n];
		};

		template<class T, size_t n, class Derived>
		using VectorExpr = MatrixExpr<T, n, 1, Derived>;

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
		Vector<T, n> abs(const Vector<T, n>& a)
		{
			Vector<T, n> result;
			for(size_t i = 0; i < n; ++i)
				result[i] = std::abs(a[i]);
			return result;
		}

        //--------------------------------------------------------------------------------------------------------------
		template<class T, size_t n, class Derived>
        T squaredNorm(const VectorExpr<T, n, Derived>& a)
        {
            return dot(a, a);
        }

        //--------------------------------------------------------------------------------------------------------------
		template<class T, size_t n, class Derived>
        T norm(const VectorExpr<T, n, Derived>& a)
        {
            return std::sqrt(squaredNorm(a));
        }

        //--------------------------------------------------------------------------------------------------------------
        template<class T, size_t n, class Derived>
        Vector<T,n> normalize(const VectorExpr<T,n,Derived>& a)
        {
            return a * (1 / norm(a));
        }
		
	}	// namespace math
}	// namespace rev
