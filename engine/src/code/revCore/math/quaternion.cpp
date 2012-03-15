////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on September 26th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// quaternion

#include "matrix.h"
#include "quaternion.h"
#include "vector.h"

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	// Static data definitions
	CQuat CQuat::identity = CQuat(0.f, 0.f, 0.f, 1.f);

	//------------------------------------------------------------------------------------------------------------------
	CQuat::CQuat(const CVec3& _axis, const TReal _radians)
	{
		w = cos(_radians * 0.5f);
		TReal sinus = sin(_radians * 0.5f);
		x = _axis.x * sinus;
		y = _axis.y * sinus;
		z = _axis.z * sinus;
	}

	//------------------------------------------------------------------------------------------------------------------
	CQuat::CQuat(const CVec3& _rotation)
	{
		float halfRadians = _rotation.norm() * 0.5f;
		CVec3 axis  = _rotation.normalized();
		w = cos(halfRadians);
		TReal sinus = sin(halfRadians);
		x = axis.x * sinus;
		y = axis.y * sinus;
		z = axis.z * sinus;
	}

	//------------------------------------------------------------------------------------------------------------------
	CQuat::CQuat(const CMat3& _m)
	{
		// Find the largest diagonal element of _m
		if(_m.m[0][0] > _m.m[1][1])
		{
			if(_m.m[0][0] > _m.m[2][2]) // _m00 is the largest diagonal element
			{
				TReal r = sqrt( 1 + _m.m[0][0] - _m.m[1][1] - _m.m[2][2] );
				TReal inv2r = 0.5f / r;
				w = (_m.m[2][1] - _m.m[1][2]) * inv2r;
				x = 0.5f * r;
				y = (_m.m[0][1] + _m.m[1][0]) * inv2r;
				z = (_m.m[2][0] + _m.m[0][2]) * inv2r;
			}
			else // _m22 is the largest diagonal element
			{
				TReal r = sqrt( 1 + _m.m[2][2] - _m.m[0][0] - _m.m[1][1] );
				TReal inv2r = 0.5f / r;
				w = (_m.m[1][0] - _m.m[0][1]) * inv2r;
				z = 0.5f * r;
				x = (_m.m[2][0] + _m.m[0][2]) * inv2r;
				y = (_m.m[1][2] + _m.m[2][1]) * inv2r;
			}
		}
		else
		{
			if(_m.m[1][1] > _m.m[2][2]) // _m11 is the largest diagonal element
			{
				TReal r = sqrt( 1 + _m.m[1][1] - _m.m[2][2] - _m.m[0][0] );
				TReal inv2r = 0.5f / r;
				w = (_m.m[0][1] - _m.m[1][0]) * inv2r;
				y = 0.5f * r;
				z = (_m.m[1][2] + _m.m[2][1]) * inv2r;
				x = (_m.m[0][1] + _m.m[1][0]) * inv2r;
			}
			else // _m22 is the largest diagonal element
			{
				TReal r = sqrt( 1 + _m.m[2][2] - _m.m[0][0] - _m.m[1][1] );
				TReal inv2r = 0.5f / r;
				w = (_m.m[1][0] - _m.m[0][1]) * inv2r;
				z = 0.5f * r;
				x = (_m.m[2][0] + _m.m[0][2]) * inv2r;
				y = (_m.m[1][2] + _m.m[2][1]) * inv2r;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	CQuat::CQuat(const CMat34& _m)
	{
		// Find the largest diagonal element of _m
		if(_m.m[0][0] > _m.m[1][1])
		{
			if(_m.m[0][0] > _m.m[2][2]) // _m00 is the largest diagonal element
			{
				TReal r = sqrt( 1 + _m.m[0][0] - _m.m[1][1] - _m.m[2][2] );
				TReal inv2r = 0.5f / r;
				w = (_m.m[2][1] - _m.m[1][2]) * inv2r;
				x = 0.5f * r;
				y = (_m.m[0][1] + _m.m[1][0]) * inv2r;
				z = (_m.m[2][0] + _m.m[0][2]) * inv2r;
			}
			else // _m22 is the largest diagonal element
			{
				TReal r = sqrt( 1 + _m.m[2][2] - _m.m[0][0] - _m.m[1][1] );
				TReal inv2r = 0.5f / r;
				w = (_m.m[1][0] - _m.m[0][1]) * inv2r;
				z = 0.5f * r;
				x = (_m.m[2][0] + _m.m[0][2]) * inv2r;
				y = (_m.m[1][2] + _m.m[2][1]) * inv2r;
			}
		}
		else
		{
			if(_m.m[1][1] > _m.m[2][2]) // _m11 is the largest diagonal element
			{
				TReal r = sqrt( 1 + _m.m[1][1] - _m.m[2][2] - _m.m[0][0] );
				TReal inv2r = 0.5f / r;
				w = (_m.m[0][1] - _m.m[1][0]) * inv2r;
				y = 0.5f * r;
				z = (_m.m[1][2] + _m.m[2][1]) * inv2r;
				x = (_m.m[0][1] + _m.m[1][0]) * inv2r;
			}
			else // _m22 is the largest diagonal element
			{
				TReal r = sqrt( 1 + _m.m[2][2] - _m.m[0][0] - _m.m[1][1] );
				TReal inv2r = 0.5f / r;
				w = (_m.m[1][0] - _m.m[0][1]) * inv2r;
				z = 0.5f * r;
				x = (_m.m[2][0] + _m.m[0][2]) * inv2r;
				y = (_m.m[1][2] + _m.m[2][1]) * inv2r;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	CQuat CQuat::operator*(const CQuat& _q) const
	{
		return CQuat(
			w*_q.x + x*_q.w + y*_q.z - z*_q.y,	// x
			w*_q.y - x*_q.z + y*_q.w + z*_q.x,	// y
			w*_q.z + x*_q.y - y*_q.x + z*_q.w,	// z
			w*_q.w - x*_q.x - y*_q.y - z*_q.z	// w
			);
	}

	//------------------------------------------------------------------------------------------------------------------
	CVec3 CQuat::rotate(const CVec3& _v) const
	{
		TReal a2 = w*w;
		TReal b2 = x*x;
		TReal c2 = y*y;
		TReal d2 = z*z;
		TReal ab = w*x;
		TReal ac = w*y;
		TReal ad = w*z;
		TReal bc = x*y;
		TReal bd = x*z;
		TReal cd = y*z;
		return CVec3(
			(a2+b2-c2-d2)*_v.x+2.0f*((bc-ad)*_v.y+(bd+ac)*_v.z),	// x
			(a2-b2+c2-d2)*_v.y+2.0f*((cd-ab)*_v.z+(bc+ad)*_v.x),	// y
			(a2-b2-c2+d2)*_v.z+2.0f*((bd-ac)*_v.x+(cd+ab)*_v.y)		// z
			);
	}

	//------------------------------------------------------------------------------------------------------------------
	CQuat CQuat::inverse() const
	{
		return CQuat(-x, -y, -z, w);
	}
}	// namespace rev
