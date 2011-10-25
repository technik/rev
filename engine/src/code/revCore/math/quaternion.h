////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, video driver interface
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 25th, 2011
////////////////////////////////////////////////////////////////////////////////
// Quaternion

#ifndef _REV_REVCORE_MATH_QUATERION_H_
#define _REV_REVCORE_MATH_QUATERION_H_

// Standard headers
#include <cmath>

// Engine headers
#include "revCore/types.h"

namespace rev
{

// Forward declarations
class CVec3;
class CMat3;
class CMat34;

class CQuat
{
public:
	// Constructors
	CQuat	()	{}
	CQuat	(TReal _x, TReal _y, TReal _z, TReal _w): x(_x), y(_y), z(_z), w(_w)	{}
	CQuat	(const CVec3& _axis, const TReal _radians);
	CQuat	(const CMat3& _matrix);
	CQuat	(const CMat34& _matrix);

	// Operators
	CQuat	operator *	(const CQuat& _q) const;

	// Other operations
	CVec3	rotate		(const CVec3& _v) const;
	CQuat	inverse		() const;

	// Useful quaternions
	static CQuat	identity;

	TReal x;
	TReal y;
	TReal z;
	TReal w;
};

}	// namespace rev

#endif // _REV_REVCORE_MATH_QUATERION_H_
