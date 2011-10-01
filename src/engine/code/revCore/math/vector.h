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
		TVec2()	{}
		TVec2(_T _x, _T _y): x(_x), y(_y)	{}
		_T	x;
		_T	y;
	};

	//--------------------------------------------------------------------------
	// 3 components vector
	template<typename _T>
	class TVec3
	{
	public:
		// Constructors
		TVec3()	{}
		TVec3(_T _x, _T _y, _T _z): x(_x), y(_y), z(_z) {}
		// Common vectors
		static const TVec3<_T> zero;
		_T x;
		_T y;
		_T z;
	};

	typedef TVec2<TReal>	CVec2;
	typedef TVec2<float>	CVec2f;
	typedef TVec2<double>	CVec2d;

	typedef TVec3<TReal>	CVec3;
	typedef TVec3<float>	CVec3f;
	typedef TVec3<double>	CVec3d;

}	//namespace rev

#endif // _REV_REVCORE_MATH_VECTOR_H_
