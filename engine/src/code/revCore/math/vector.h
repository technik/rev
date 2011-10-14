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
	class CVec3
	{
	public:
		// Constructors
		CVec3()	{}
		CVec3(TReal _x, TReal _y, TReal _z): x(_x), y(_y), z(_z) {}

		// Mathematical operators
		CVec3 operator + (const CVec3& _v) const;

		// Common vectors
		static const CVec3 zero;

		// Components
		TReal x;
		TReal y;
		TReal z;
	};

	typedef TVec2<TReal>	CVec2;
	typedef TVec2<float>	CVec2f;
	typedef TVec2<double>	CVec2d;

}	//namespace rev

#endif // _REV_REVCORE_MATH_VECTOR_H_
