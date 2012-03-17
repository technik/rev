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
	// 3 components vector interface
	class CVec3
	{
	public:
		// Construction, copy and destruction
		CVec3			();								///< Default constructor
		explicit CVec3	(TReal _val);					///< Single value constructor
		CVec3			(const CVec2& _xy, TReal _z);	///< Vec2 + z constructor
		CVec3			(TReal _x, TReal _y, TReal _z);	///< Three component constructor
		CVec3			(const CVec3& _v);				///< Copy constructor
		CVec3& operator=(const CVec3& _v);				///< Assignment operator
		~CVec3			();								///< Destructor

		// Equality and inequality
		bool	operator==	(const CVec3& _v) const;	///< Equality comparison
		bool	operator!=	(const CVec3& _v) const;	///< Inequality comparison

		// Adition and substraction
		CVec3	operator+	(const CVec3& _v) const;	///< Addition operator
		CVec3&	operator+=	(const CVec3& _v);			///< Self addition operator
		CVec3	operator-	(const CVec3& _v) const;	///< Substraction operator
		CVec3&	operator-=	(const CVec3& _v);			///< Self substraction operator




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
		static const CVec3 xAxis;
		static const CVec3 yAxis;
		static const CVec3 zAxis;

		// Components
		TReal x;
		TReal y;
		TReal z;
	};

	//--------------------------------------------------------------------------
	// CVec3 Inline implementation
	//--------------------------------------------------------------------------
	inline CVec3::CVec3()
	{}

	//--------------------------------------------------------------------------
	inline CVec3::CVec3(TReal _val)
		:x(_val)
		,y(_val)
		,z(_val)
	{}

	//--------------------------------------------------------------------------
	inline CVec3::CVec3(const CVec2& _xy, TReal _z)
		:x(_xy.x)
		,y(_xy.y)
		,z(_z)
	{}

	//--------------------------------------------------------------------------
	inline CVec3::CVec3(TReal _x, TReal _y, TReal _z)
		:x(_x)
		,y(_y)
		,z(_z)
	{}

	//--------------------------------------------------------------------------
	inline CVec3::CVec3(const CVec3& _v)
		:x(_v.x)
		,y(_v.y)
		,z(_v.z)
	{}

	//--------------------------------------------------------------------------
	inline CVec3& CVec3::operator=(const CVec3& _v)
	{
		x=_v.x;
		y=_v.y;
		z=_v.z;
		return *this;
	}

	//--------------------------------------------------------------------------
	inline CVec3::~CVec3()
	{}

	//--------------------------------------------------------------------------
	inline bool CVec3::operator==(const CVec3& _v) const
	{
		return x==_v.x
			&& y==_v.y
			&& z==_v.z;
	}

	//--------------------------------------------------------------------------
	inline bool CVec3::operator!=(const CVec3& _v) const
	{
		return x!=_v.x
			|| y!=_v.y
			|| z!=_v.z;
	}

	//--------------------------------------------------------------------------
	inline CVec3 CVec3::operator+(const CVec3& _v) const
	{
		return CVec3(x+_v.x, y+_v.y, z+_v.z);
	}

	//--------------------------------------------------------------------------
	inline CVec3& CVec3::operator+=(const CVec3& _v)
	{
		x+=_v.x;
		y+=_v.y;
		z+=_v.z;
		return *this;
	}

	//--------------------------------------------------------------------------
	inline CVec3 CVec3::operator-(const CVec3& _v) const
	{
		return CVec3(x-_v.x, y-_v.y, z-_v.z);
	}

	//--------------------------------------------------------------------------
	inline CVec3& CVec3::operator-=(const CVec3& _v)
	{
		x-=_v.x;
		y-=_v.y;
		z-=_v.z;
		return *this;
	}

}	//namespace rev

#endif // _REV_REVCORE_MATH_VECTOR_H_
