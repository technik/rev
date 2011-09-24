////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, video driver interface
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////
// vectors

#ifndef _REV_REVCORE_MATH_VECTOR_H_
#define _REV_REVCORE_MATH_VECTOR_H_

#include "revCore/types.h"

namespace rev
{
	//--------------------------------------------------------------------------
	// 2 components vector
	template<typename _T>
	class TVec2
	{
	public:
		_T	x;
		_T	y;
	};

	//--------------------------------------------------------------------------
	class CVec2: public TVec2<TReal>
	{
	};

}	//namespace rev

#endif // _REV_REVCORE_MATH_VECTOR_H_
