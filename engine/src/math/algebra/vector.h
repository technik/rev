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
		template<typename T_, unsigned _n>
		class Vector {
			/// 666 TODO: Implemente generic dimension vector
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

		typedef Vector<unsigned, 2> Vec2u;
		typedef Vector<int, 2>		Vec2i;
		typedef Vector<float, 2>	Vec2f;
		typedef Vector<double, 2>	Vec2d;
		
	}	// namespace math
}	// namespace rev

#include "vector.inl"

#endif // _REV_MATH_ALGEBRA_VECTOR_H_