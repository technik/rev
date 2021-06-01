//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2021 Carmelo J Fdez-Aguera
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
// //-----------------------------------------------------------------------------------------------
// Generic mathematical vector
#pragma once

#include <algorithm>
#include <cassert>
#include <concepts>
#include <math/linear.h>
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

			explicit Vector(T _x)
			{
				for (size_t i = 0; i < n; ++i)
					m[i] = _x;
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

			Vector operator+(Scalar auto x) const
			{
				Vector res;
				for (size_t i = 0; i < n; ++i)
				{
					res[i] = x + m[i];
				}
				return res;
			}

			Vector operator-(Scalar auto x) const
			{
				Vector res;
				for (size_t i = 0; i < n; ++i)
				{
					res[i] = m[i] - x;
				}
				return res;
			}

			Vector operator*(Scalar auto x) const
			{
				Vector res;
				for (size_t i = 0; i < n; ++i)
				{
					res[i] = x * m[i];
				}
				return res;
			}

			Vector operator*(Vector x) const
			{
				Vector res;
				for (size_t i = 0; i < n; ++i)
				{
					res[i] = x[i] * m[i];
				}
				return res;
			}

			template<class T2>
			Vector<T2, n> cast() const {
				Vector<T2, n> castedResult;
				for (size_t i = 0; i < n; ++i)
					castedResult[i] = T2(m[i]);
				return castedResult;
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

		// Specializations
		template<>
		struct Vector<float,3> : MatrixView<float, 3, 1, Vector<float, 3>>
		{
			template<class Derived>
			using VectorExpr = MatrixExpr<float, 3, 1, Derived>;

			// Basic construction
			Vector() = default;
			Vector(const Vector&) = default;

			template<class Other>
			Vector(const VectorExpr<Other>& x)
			{
				for (size_t i = 0; i < 3; ++i)
					m[i] = x(i, 0);
			}

			template<class OtherT>
			Vector(const MatrixExpr<float, 2, 1, OtherT>& x, float f)
			{
				for (size_t i = 0; i < 2; ++i)
				{
					m[i] = x[i];
				}
				m[2] = f;
			}

			Vector(float _s)
				: m{ _s, _s, _s }
			{
			}

			Vector(float _x, float _y, float _z)
				: m{ _x, _y, _z }
			{
			}

			Vector(std::initializer_list<float> _l)
			{
				auto x = _l.begin();
				for (size_t i = 0; i < 3; ++i)
					m[i] = *x++;
			}

			float& operator[]	(size_t i) { return m[i]; }
			float	operator[] 	(size_t i) const { return m[i]; }
			float& operator()(size_t i, size_t) { return m[i]; }
			float	operator()(size_t i, size_t) const { return m[i]; }

			float x() const { return m[0]; }
			float y() const { return m[1]; }
			float z() const { return m[2]; }

			float& x() { return m[0]; }
			float& y() { return m[1]; }
			float& z() { return m[2]; }

			Vector operator+(Scalar auto x) const
			{
				return Vector(x + m[0], x + m[1], x + m[2]);
			}

			Vector operator-(Scalar auto x) const
			{
				return Vector(x - m[0], x - m[1], x - m[2]);
			}

			Vector operator*(float x) const
			{
				return Vector(x * m[0], x * m[1], x * m[2]);
			}

			Vector operator*(int x) const
			{
				return Vector(x * m[0], x * m[1], x * m[2]);
			}

			Vector operator*(Vector x) const
			{
				return { m[0] * x[0], m[1] * x[1], m[2] * x[2] };
			}

			void operator+=(Vector x)
			{
				m[0] += x[0];
				m[1] += x[1];
				m[2] += x[2];
			}

			// Compile time accessors
			template<size_t i>
			float& getComponent()
			{
				static_assert(i < 3);
				return m[i];
			}

			template<size_t i>
			float getComponent() const
			{
				static_assert(i < 3);
				return m[i];
			}

			float* data() { return m; }
			const float* data() const { return m; }

		private:
			float m[3];
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
		typedef Vector<int32_t, 2>	Vec2i;
		typedef Vector<float, 2>	Vec2f;
		typedef Vector<double, 2>	Vec2d;

		typedef Vector3<unsigned>	Vec3u;
		typedef Vector3<uint8_t>	Vec3u8;
		typedef Vector3<int32_t>	Vec3i;
		typedef Vector3<float>		Vec3f;
		typedef Vector3<double>		Vec3d;

		typedef Vector4<unsigned>	Vec4u;
		typedef Vector4<uint8_t>	Vec4u8;
		typedef Vector4<int32_t>	Vec4i;
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
        template<class T>
        Vector<T, 3> cross(const Vector<T, 3>& a, const Vector<T, 3>& b)
        {
            Vector<T, 3> result;
            result.x() = a.y()*b.z() - a.z()*b.y();
            result.y() = a.z()*b.x() - a.x()*b.z();
            result.z() = a.x()*b.y() - a.y()*b.x();
            return result;
        }

		//--------------------------------------------------------------------------------------------------------------
		__forceinline Vec3f operator*(Scalar auto x, const Vec3f& v)
		{
			return v*x;
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

		//--------------------------------------------------------------------------------------------------------------
		// Pixar's method for orthonormal basis generation
		inline void branchlessONB(const Vec3f& n, Vec3f& b1, Vec3f& b2)
		{
			float sign = copysignf(1.0f, n.z());
			const float a = -1.0f / (sign + n.z());
			const float b = n.x() * n.y() * a;
			b1 = Vec3f(1.0f + sign * n.x() * n.x() * a, sign * b, -sign * n.x());
			b2 = Vec3f(b, sign + n.y() * n.y() * a, -n.y());
		}
		
	}	// namespace math
}	// namespace rev
