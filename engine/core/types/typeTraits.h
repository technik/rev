//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/Sep/23
//----------------------------------------------------------------------------------------------------------------------
// KdTree structure
#ifndef _REV_MATH_GEOMETRY_KDTREE_H_
#define _REV_MATH_GEOMETRY_KDTREE_H_

#include <math/algebra/vector.h>

namespace rev {

	//-----------------------------------------------------------------------------
	// Type select (Alexandrescu)
	template<class A_, class B_, bool condition_>
	typedef A_ Select;

	template<class A_, class B_>
	typedef B_ Select<A_,B_,false>;

	// Alternate implementation
	template<class A_, class B_, bool condition_>
	using Select<A_,B_,condition_> = A_;

	template<class A_, class B_>
	using Select<A_,B_,false> = B_;

}	// namespace rev

#endif // _REV_MATH_GEOMETRY_KDTREE_H_