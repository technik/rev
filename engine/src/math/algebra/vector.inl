//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Generic mathematical vector
#ifndef _REV_MATH_ALGEBRA_VECTOR_INL_
#define _REV_MATH_ALGEBRA_VECTOR_INL_

#include "vector.h"

namespace rev {
	namespace math {
		
		//--------------------------------------------------------------------------------------------------------------
		template<typename T_>
		bool Vector<T_, 2>::operator==(const Vector& _v) const {
			return _v.x == x && _v.y == y;
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename T_>
		bool Vector<T_, 2>::operator!=(const Vector& _v) const {
			return _v.x != x || _v.y != y;
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename T_>
		Vector<T_, 3>& Vector<T_, 3>::operator=(const Vector& _v) {
			x = _v.x;
			y = _v.y;
			z = _v.z;
			return *this;
		}

	}	// namespace math
}	// namespace rev

#endif // _REV_MATH_ALGEBRA_VECTOR_INL_