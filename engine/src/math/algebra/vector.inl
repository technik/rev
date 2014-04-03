//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Generic mathematical vector
#ifndef _REV_MATH_ALGEBRA_VECTOR_INL_
#define _REV_MATH_ALGEBRA_VECTOR_INL_

namespace rev {
	namespace math {
		
		//--------------------------------------------------------------------------------------------------------------
		template<typename T_>
		bool Vector<T_, 2>::operator==(const Vector2<T_, 2>& _v) {
			return _v.x == x && _v.y == y;
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename T_>
		bool Vector<T_, 2>::operator!=(const Vector2<T_, 2>& _v) {
			return _v.x != x || _v.y != y;
		}

	}	// namespace math
}	// namespace rev

#endif // _REV_MATH_ALGEBRA_VECTOR_INL_