//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// on September 25th, 2011
//----------------------------------------------------------------------------------------------------------------------
// Linear algebra's quaternion classes

#ifndef _REV_MATH_ALGEBRA_QUATERION_H_
#define _REV_MATH_ALGEBRA_QUATERION_H_

// Standard headers
#include <cmath>

// Engine headers
#include "matrix.h"
#include "vector.h"
#include "../numericTraits.h"

namespace rev { namespace math {

	template<typename Number_>
	class Quaternion
	{
	public:
		// Constructors
		Quaternion	()	{}
		Quaternion	(Number_ _x, Number_ _y, Number_ _z, Number_ _w): x(_x), y(_y), z(_z), w(_w)	{}
		Quaternion	(const Vector3<Number_>& _axis, const Number_ _radians);
		Quaternion	(const Vector3<Number_>& _rotationVector);
		Quaternion	(const Matrix3x3<Number_>& _matrix);
		Quaternion	(const Matrix3x4<Number_>& _matrix);

		// Operators
		Quaternion	operator *	(const Quaternion& _q) const;

		// Other operations
		Vector3<Number_>	rotate		(const Vector3<Number_>& _v) const;
		Quaternion	inverse		() const;

		// Useful quaternions
		static Quaternion	identity();

		Number_ x;
		Number_ y;
		Number_ z;
		Number_ w;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Useful typedefs
	typedef Quaternion<float>	Quatf;
	typedef Quaternion<double>	Quatd;

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementation
	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Quaternion<N_>::Quaternion(const Vector3<N_>& _axis, const N_ _radians)
	{
		w = cos(_radians * 0.5f);
		N_ sinus = (N_)sin(_radians * 0.5f);
		x = _axis.x * sinus;
		y = _axis.y * sinus;
		z = _axis.z * sinus;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Quaternion<N_>::Quaternion(const Vector3<N_>& _rotationVector)
	{
		N_ halfRadians = _rotationVector.norm() * 0.5f;
		Vector3<N_> axis  = _rotationVector.normalized();
		w = cos(halfRadians);
		N_ sinus = aqrt(NumericTraits<N_>::one() - w*w); // sin(halfRadians);
		x = axis.x * sinus;
		y = axis.y * sinus;
		z = axis.z * sinus;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Quaternion<N_>::Quaternion(const Matrix3x3<N_>& _matrix)
	{
		// Find the largest diagonal element of _m
		if(_m[0][0] > _m[1][1]) {
			if(_m[0][0] > _m[2][2]) { // _m00 is the largest diagonal element
				N_ r = sqrt( NumericTraits<N_>::one() + _m[0][0] - _m[1][1] - _m[2][2] );
				N_ inv2r = 0.5f / r;
				w = (_m[2][1] - _m[1][2]) * inv2r;
				x = 0.5f * r;
				y = (_m[0][1] + _m[1][0]) * inv2r;
				z = (_m[2][0] + _m[0][2]) * inv2r;
			} else { // _m22 is the largest diagonal element
				N_ r = sqrt( NumericTraits<N_>::one() + _m[2][2] - _m[0][0] - _m[1][1] );
				N_ inv2r = 0.5f / r;
				w = (_m[1][0] - _m[0][1]) * inv2r;
				z = 0.5f * r;
				x = (_m[2][0] + _m[0][2]) * inv2r;
				y = (_m[1][2] + _m[2][1]) * inv2r;
			}
		} else {
			if(_m[1][1] > _m[2][2]) { // _m11 is the largest diagonal element
				N_ r = sqrt( NumericTraits<N_>::one() + _m[1][1] - _m[2][2] - _m[0][0] );
				N_ inv2r = 0.5f / r;
				w = (_m[0][1] - _m[1][0]) * inv2r;
				y = 0.5f * r;
				z = (_m[1][2] + _m[2][1]) * inv2r;
				x = (_m[0][1] + _m[1][0]) * inv2r;
			} else { // _m22 is the largest diagonal element
				N_ r = sqrt( NumericTraits<N_>::one() + _m[2][2] - _m[0][0] - _m[1][1] );
				N_ inv2r = 0.5f / r;
				w = (_m[1][0] - _m[0][1]) * inv2r;
				z = 0.5f * r;
				x = (_m[2][0] + _m[0][2]) * inv2r;
				y = (_m[1][2] + _m[2][1]) * inv2r;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Quaternion<N_>::Quaternion(const Matrix3x4<N_>& _matrix)
	{	// Find the largest diagonal element of _m
		if(_matrix[0][0] > _matrix[1][1]) {
			if(_matrix[0][0] > _matrix[2][2]) { // _m00 is the largest diagonal element
				N_ r = sqrt( 1 + _matrix[0][0] - _matrix[1][1] - _matrix[2][2] );
				N_ inv2r = 0.5f / r;
				w = (_matrix[2][1] - _matrix[1][2]) * inv2r;
				x = 0.5f * r;
				y = (_matrix[0][1] + _matrix[1][0]) * inv2r;
				z = (_matrix[2][0] + _matrix[0][2]) * inv2r;
			} else {// _m22 is the largest diagonal element
				N_ r = sqrt( 1 + _matrix[2][2] - _matrix[0][0] - _matrix[1][1] );
				N_ inv2r = 0.5f / r;
				w = (_matrix[1][0] - _matrix[0][1]) * inv2r;
				z = 0.5f * r;
				x = (_matrix[2][0] + _matrix[0][2]) * inv2r;
				y = (_matrix[1][2] + _matrix[2][1]) * inv2r;
			}
		} else {
			if(_matrix[1][1] > _matrix[2][2]) { // _m11 is the largest diagonal element
				N_ r = sqrt( 1 + _matrix[1][1] - _matrix[2][2] - _matrix[0][0] );
				N_ inv2r = 0.5f / r;
				w = (_matrix[0][1] - _matrix[1][0]) * inv2r;
				y = 0.5f * r;
				z = (_matrix[1][2] + _matrix[2][1]) * inv2r;
				x = (_matrix[0][1] + _matrix[1][0]) * inv2r;
			} else { // _m22 is the largest diagonal element
				N_ r = sqrt( 1 + _matrix[2][2] - _matrix[0][0] - _matrix[1][1] );
				N_ inv2r = 0.5f / r;
				w = (_matrix[1][0] - _matrix[0][1]) * inv2r;
				z = 0.5f * r;
				x = (_matrix[2][0] + _matrix[0][2]) * inv2r;
				y = (_matrix[1][2] + _matrix[2][1]) * inv2r;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Quaternion<N_> Quaternion<N_>::operator*(const Quaternion<N_>& _q) const
	{
		return Quaternion<N_>(
			w*_q.x + x*_q.w + y*_q.z - z*_q.y,	// x
			w*_q.y - x*_q.z + y*_q.w + z*_q.x,	// y
			w*_q.z + x*_q.y - y*_q.x + z*_q.w,	// z
			w*_q.w - x*_q.x - y*_q.y - z*_q.z	// w
			);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector3<N_> Quaternion<N_>::rotate(const Vector3<N_>& _v) const
	{
		N_ a2 = w*w;
		N_ b2 = x*x;
		N_ c2 = y*y;
		N_ d2 = z*z;
		N_ ab = w*x;
		N_ ac = w*y;
		N_ ad = w*z;
		N_ bc = x*y;
		N_ bd = x*z;
		N_ cd = y*z;
		return Vec3<N_>(
			(a2+b2-c2-d2)*_v.x+2.0f*((bc-ad)*_v.y+(bd+ac)*_v.z),	// x
			(a2-b2+c2-d2)*_v.y+2.0f*((cd-ab)*_v.z+(bc+ad)*_v.x),	// y
			(a2-b2-c2+d2)*_v.z+2.0f*((bd-ac)*_v.x+(cd+ab)*_v.y)		// z
			);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Quaternion<N_> Quaternion<N_>::inverse() const
	{
		return Quaternion<N_>(-x, -y, -z, w);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Quaternion<N_> Quaternion<N_>::identity()
	{
		return Quaternion<N_>(
			N_(0.f),
			N_(0.f),
			N_(0.f),
			N_(1.f));
	}

}	// namespace math
}	// namespace rev

#endif // _REV_REVCORE_MATH_QUATERION_H_
