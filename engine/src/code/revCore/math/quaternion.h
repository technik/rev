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

namespace rev {

template<typename _T> class TVec3;
template<typename _T> class TMat3;
template<typename _T> class TMat34;

template<typename _T>
class TQuaternion
{
public:
	// Constructors
	TQuaternion	()	{}
	TQuaternion (_T _x, _T _y, _T _z, _T _w): x(_x), y(_y), z(_z), w(_w)	{}
	TQuaternion	(const TVec3<_T>& _axis, const _T _radians);
	TQuaternion	(const TMat3<_T>& _matrix);
	TQuaternion	(const TMat34<_T>& _matrix);

	// Operators
	TQuaternion<_T>	operator *	(const TQuaternion<_T>& _q) const;

	// Other operations
	TVec3<_T>		rotate		(const TVec3<_T>& _v) const;
	TQuaternion<_T>	inverse		() const;

	// Useful quaternions
	static TQuaternion<_T>	identity;

	_T x;
	_T y;
	_T z;
	_T w;
};

typedef TQuaternion<TReal>	CQuat;
typedef TQuaternion<float>	CQuatf;
typedef TQuaternion<double>	CQuatd;

//----------------------------------------------------------------------------------------------------------------------
// Quaternion implementation
//----------------------------------------------------------------------------------------------------------------------

template<typename _T>
TQuaternion<_T>::TQuaternion(const TVec3<_T>& _axis, const _T _radians)
{
		w = cos(_radians * 0.5f);
		_T sinus = sin(_radians * 0.5f);
		x = _axis.x * sinus;
		y = _axis.y * sinus;
		z = _axis.z * sinus;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
TQuaternion<_T>::TQuaternion(const TMat3<_T>& _m)
{
	// Find the largest diagonal element of _m
	if(_m[0][0] > _m[1][1])
	{
		if(_m[0][0] > _m[2][2]) // _m00 is the largest diagonal element
		{
			_T r = sqrt( 1 + _m[0][0] - _m[1][1] - _m[2][2] );
			_T inv2r = 0.5f / r;
			w = (_m[2][1] - _m[1][2]) * inv2r;
			x = 0.5f * r;
			y = (_m[0][1] + _m[1][0]) * inv2r;
			z = (_m[2][0] + _m[0][2]) * inv2r;
		}
		else // _m22 is the largest diagonal element
		{
			_T r = sqrt( 1 + _m[2][2] - _m[0][0] - _m[1][1] );
			_T inv2r = 0.5f / r;
			w = (_m[1][0] - _m[0][1]) * inv2r;
			z = 0.5f * r;
			x = (_m[2][0] + _m[0][2]) * inv2r;
			y = (_m[1][2] + _m[2][1]) * inv2r;
		}
	}
	else
	{
		if(_m[1][1] > _m[2][2]) // _m11 is the largest diagonal element
		{
			_T r = sqrt( 1 + _m[1][1] - _m[2][2] - _m[0][0] );
			_T inv2r = 0.5f / r;
			w = (_m[0][1] - _m[1][0]) * inv2r;
			y = 0.5f * r;
			z = (_m[1][2] + _m[2][1]) * inv2r;
			x = (_m[0][1] + _m[1][0]) * inv2r;
		}
		else // _m22 is the largest diagonal element
		{
			_T r = sqrt( 1 + _m[2][2] - _m[0][0] - _m[1][1] );
			_T inv2r = 0.5f / r;
			w = (_m[1][0] - _m[0][1]) * inv2r;
			z = 0.5f * r;
			x = (_m[2][0] + _m[0][2]) * inv2r;
			y = (_m[1][2] + _m[2][1]) * inv2r;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
TQuaternion<_T>::TQuaternion(const TMat34<_T>& _m)
{
	// Find the largest diagonal element of _m
	if(_m.m[0][0] > _m.m[1][1])
	{
		if(_m.m[0][0] > _m.m[2][2]) // _m00 is the largest diagonal element
		{
			_T r = sqrt( 1 + _m.m[0][0] - _m.m[1][1] - _m.m[2][2] );
			_T inv2r = 0.5f / r;
			w = (_m.m[2][1] - _m.m[1][2]) * inv2r;
			x = 0.5f * r;
			y = (_m.m[0][1] + _m.m[1][0]) * inv2r;
			z = (_m.m[2][0] + _m.m[0][2]) * inv2r;
		}
		else // _m22 is the largest diagonal element
		{
			_T r = sqrt( 1 + _m.m[2][2] - _m.m[0][0] - _m.m[1][1] );
			_T inv2r = 0.5f / r;
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
			_T r = sqrt( 1 + _m.m[1][1] - _m.m[2][2] - _m.m[0][0] );
			_T inv2r = 0.5f / r;
			w = (_m.m[0][1] - _m.m[1][0]) * inv2r;
			y = 0.5f * r;
			z = (_m.m[1][2] + _m.m[2][1]) * inv2r;
			x = (_m.m[0][1] + _m.m[1][0]) * inv2r;
		}
		else // _m22 is the largest diagonal element
		{
			_T r = sqrt( 1 + _m.m[2][2] - _m.m[0][0] - _m.m[1][1] );
			_T inv2r = 0.5f / r;
			w = (_m.m[1][0] - _m.m[0][1]) * inv2r;
			z = 0.5f * r;
			x = (_m.m[2][0] + _m.m[0][2]) * inv2r;
			y = (_m.m[1][2] + _m.m[2][1]) * inv2r;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
TQuaternion<_T> TQuaternion<_T>::operator*(const TQuaternion<_T>& _q) const
{
	return TQuaternion<_T>(
		w*_q.x + x*_q.w + y*_q.z - z*_q.y,	// x
		w*_q.y - x*_q.z + y*_q.w + z*_q.x,	// y
		w*_q.z + x*_q.y - y*_q.x + z*_q.w,	// z
		w*_q.w - x*_q.x - y*_q.y - z*_q.z	// w
		);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
TVec3<_T> TQuaternion<_T>::rotate(const TVec3<_T>& _v) const
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
		return TVec3<_T>(
			(a2+b2-c2-d2)*_v.x+2.0f*((bc-ad)*_v.y+(bd+ac)*_v.z),	// x
			(a2-b2+c2-d2)*_v.y+2.0f*((cd-ab)*_v.z+(bc+ad)*_v.x),	// y
			(a2-b2-c2+d2)*_v.z+2.0f*((bd-ac)*_v.x+(cd+ab)*_v.y)		// z
			);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
TQuaternion<_T> TQuaternion<_T>::inverse() const
{
	return TQuaternion<_T>(-x, -y, -z, w);
}

}	// namespace rev

#endif // _REV_REVCORE_MATH_QUATERION_H_
