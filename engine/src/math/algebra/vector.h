//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Generic mathematical vector
#ifndef _REV_MATH_ALGEBRA_VECTOR_H_
#define _REV_MATH_ALGEBRA_VECTOR_H_

namespace rev {
	namespace math {

		// A vector of dimension _n
		template<typename T_, unsigned n_>
		class Vector {
		public:
			/// 666 TODO: Implemente generic dimension vector
			inline T_&			operator[](unsigned _n)			{ return m[_n]; }
			inline const T_&	operator[](unsigned _n) const	{ return m[_n]; }
		public:
			T_ m[n_];
		};

		// 2D vector
		template<typename T_>
		class Vector<T_, 2> {
		public:
			T_ x, y;
		public:
			// Construction
			Vector() = default;
			Vector(const Vector&) = default;
			Vector(const T_& _x, const T_& _y) : x(_x), y(_y) {}

			// Comparison
			bool operator==(const Vector&) const;
			bool operator!=(const Vector&) const;
		};

		// 3D vector
		template<typename T_>
		class Vector<T_, 3> {
		public:
			T_ x, y, z;
		public:
			// Construction
			Vector() = default;
			Vector(const Vector&) = default;
			Vector(const T_& _x, const T_& _y, const T_& _z) : x(_x), y(_y), z(_z) {}

			// Comparison
			bool operator==(const Vector&) const;
			bool operator!=(const Vector&) const;
		};

		template<typename T_>
		using Vector2 = Vector<T_, 2>;
		template<typename T_>
		using Vector3 = Vector<T_, 3>;
		template<typename T_>
		using Vector4 = Vector<T_, 4>;

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
		
	}	// namespace math
}	// namespace rev

#include "vector.inl"

#endif // _REV_MATH_ALGEBRA_VECTOR_H_