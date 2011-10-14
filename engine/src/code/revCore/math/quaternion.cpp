////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on September 26th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// quaternion

#include "quaternion.h"
#include "vector.h"

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	// Static data definitions
	template<>
	CQuat CQuat::identity = CQuat(0.f, 0.f, 0.f, 1.f);

	//----------------------------------------------------------------------------------------------------------------------
	template<typename _T>
	CVec3 TQuaternion<_T>::rotate(const CVec3& _v) const
	{
			_T a2 = w*w;
			_T b2 = x*x;
			_T c2 = y*y;
			_T d2 = z*z;
			_T ab = w*x;
			_T ac = w*y;
			_T ad = w*z;
			_T bc = x*y;
			_T bd = x*z;
			_T cd = y*z;
			return CVec3(
				(a2+b2-c2-d2)*_v.x+2.0f*((bc-ad)*_v.y+(bd+ac)*_v.z),	// x
				(a2-b2+c2-d2)*_v.y+2.0f*((cd-ab)*_v.z+(bc+ad)*_v.x),	// y
				(a2-b2-c2+d2)*_v.z+2.0f*((bd-ac)*_v.x+(cd+ab)*_v.y)		// z
				);
	}
}	// namespace rev
