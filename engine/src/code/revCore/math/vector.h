////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, video driver interface
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////
// vectors

#ifndef _REV_REVCORE_MATH_VECTOR_H_
#define _REV_REVCORE_MATH_VECTOR_H_

#include <cmath>

// #include "mathUtility.h"

#include <revCore/types.h>

namespace rev
{
	//--------------------------------------------------------------------------
	// 2 components vector
	class CVec2
	{
	public:
		CVec2()	{}
		CVec2(TReal _x, TReal _y): x(_x), y(_y)	{}
		TReal	x;
		TReal	y;

		CVec2	operator * (TReal _k) const { return CVec2(x*_k, y*_k);}

		// Useful vectors
		static const CVec2 zero;
	};

	//--------------------------------------------------------------------------
	// 3 components vector
	class CVec3
	{
	public:
		// Constructors
		CVec3()	{}
		CVec3(TReal _x, TReal _y, TReal _z): x(_x), y(_y), z(_z) {}
		~CVec3(){}
		// Mathematical operators
		bool	operator==	(const CVec3& _x) const
		{
			return x==_x.x && y==_x.y &&z==_x.z;
		}
		CVec3	operator + (const CVec3& _v) const
		{
			return CVec3(x+_v.x,y+_v.y,z+_v.z);
		}
		CVec3	operator - (const CVec3& _v) const
		{
			return CVec3(x-_v.x,y-_v.y,z-_v.z);
		}
		CVec3	operator * (TReal k) const { return CVec3(x*k, y*k, z*k); }
		TReal	operator * (const CVec3& _x) const { return x*_x.x+y*_x.y+z*_x.z; }
		CVec3	operator ^ (const CVec3& b) const 
		{
			return CVec3(
				y*b.z - z*b.y,
				z*b.x - x*b.z,
				x*b.y - y*b.x);
		}

		TReal norm() const { return sqrt((*this)*(*this)); }
		CVec3 normalized() const { return *this * (1.f/norm()); }

		// Common vectors
		static const CVec3 zero;

		// Components
		TReal x;
		TReal y;
		TReal z;
	};

}	//namespace rev

#endif // _REV_REVCORE_MATH_VECTOR_H_
