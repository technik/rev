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
		// Axis is assumed normalized
		Quaternion	(const Vector3<Number_>& _axis, const Number_ _radians);

		Quaternion	(const Vector3<Number_>& _rotationVector);
		template<typename S_>
		Quaternion	(const MatrixBase<3,3,S_>& _matrix);

		Quaternion fromUnitVectors(const Vector3<Number_>& u, const Vector3<Number_>& v)
		{
			// TODO: This can be made faster by precomputing sqrt(2)
			auto m = sqrt(2 + 2 * u.dot(v));
			Vector3<Number_> w = (1 / m) * u.cross(v);
			return Quaternion(w.x(), w.y(), w.z(), m*0.5f);
		}

		Quaternion from2Vectors(const Vector3<Number_>& u, const Vector3<Number_>& v)
		{
			Vector3<Number_> w = u.cross(v);
			Quaternion q = quat(dot(u, v), w.x, w.y, w.z);
			q.w += length(q);
			return q.normalized();
		}

		Number_ dot(const Quaternion& q) const {
			return w*q.w - x*q.x - y*q.y - z*q.z;
		}

		Quaternion normalized() const {
			auto n = 1.f/norm();
			return Quaternion(x*n,y*n,z*n,w*n);
		}

		Number_ norm() const { return sqrt(dot(*this)); }
		// Operators
		Quaternion	operator *	(const Quaternion& _q) const;

		// Other operations
		Vector3<Number_>	rotate		(const Vector3<Number_>& _v) const;
		Quaternion	inverse		() const;

		Matrix33<Number_> asMatrix() const;

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
		auto half_angle = 0.5f*_radians;
		w = cos(half_angle);
		N_ sinus = sin(half_angle);
		x = _axis.x() * sinus;
		y = _axis.y() * sinus;
		z = _axis.z() * sinus;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Quaternion<N_>::Quaternion(const Vector3<N_>& _rotationVector)
	{
		N_ halfRadians = _rotationVector.norm() * 0.5f;
		Vector3<N_> axis  = _rotationVector.normalized();
		w = cos(halfRadians);
		N_ sinus = aqrt(N_(1) - w*w); // sin(halfRadians);
		x = axis.x() * sinus;
		y = axis.y() * sinus;
		z = axis.z() * sinus;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	template<typename S_>
	inline Quaternion<N_>::Quaternion(const MatrixBase<3,3,S_> & _m)
	{
		//static_assert(typename S_::Element == N_, "Inconsistent number type Quaternion-Matrix");
		N_ tr = _m(0,0) + _m(1,1) + _m(2,2);

		if (tr > 0) {
			N_ r = sqrt(1 + tr);
			N_ inv2r = 0.5f / r;
			w = 0.5f * r;
			x = (_m(2,1) - _m(1,2)) * inv2r;
			y = (_m(0,2) - _m(2,0)) * inv2r;
			z = (_m(1,0) - _m(0,1)) * inv2r;
			return;
		}
		// Find the largest diagonal element of _m
		if(_m(0,0) > _m(1,1)) {
			if(_m(0,0) > _m(2,2)) { // _m00 is the largest diagonal element
				N_ r = sqrt( NumericTraits<N_>::one() + _m(0,0) - _m(1,1) - _m(2,2) );
				N_ inv2r = 0.5f / r;
				w = (_m(2,1) - _m(1,2)) * inv2r;
				x = 0.5f * r;
				y = (_m(0,1) + _m(1,0)) * inv2r;
				z = (_m(2,0) + _m(0,2)) * inv2r;
			} else { // _m22 is the largest diagonal element
				N_ r = sqrt( NumericTraits<N_>::one() + _m(2,2) - _m(0,0) - _m(1,1) );
				N_ inv2r = 0.5f / r;
				w = (_m(1,0) - _m(0,1)) * inv2r;
				z = 0.5f * r;
				x = (_m(2,0) + _m(0,2)) * inv2r;
				y = (_m(1,2) + _m(2,1)) * inv2r;
			}
		} else {
			if(_m(1,1) > _m(2,2)) { // _m11 is the largest diagonal element
				N_ r = sqrt( NumericTraits<N_>::one() + _m(1,1) - _m(2,2) - _m(0,0) );
				N_ inv2r = 0.5f / r;
				w = (_m(0,1) - _m(1,0)) * inv2r;
				y = 0.5f * r;
				z = (_m(1,2) + _m(2,1)) * inv2r;
				x = (_m(0,1) + _m(1,0)) * inv2r;
			} else { // _m22 is the largest diagonal element
				N_ r = sqrt( NumericTraits<N_>::one() + _m(2,2) - _m(0,0) - _m(1,1) );
				N_ inv2r = 0.5f / r;
				w = (_m(1,0) - _m(0,1)) * inv2r;
				z = 0.5f * r;
				x = (_m(2,0) + _m(0,2)) * inv2r;
				y = (_m(1,2) + _m(2,1)) * inv2r;
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
		return Vector3<N_>(
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

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Matrix33<N_> Quaternion<N_>::asMatrix() const
	{
		N_ a2 = w*w;
		N_ b2 = x*x;
		N_ c2 = y*y;
		N_ d2 = z*z;
		N_ ab = 2 * w*x;
		N_ ac = 2 * w*y;
		N_ ad = 2 * w*z;
		N_ bc = 2 * x*y;
		N_ bd = 2 * x*z;
		N_ cd = 2 * y*z;
		Matrix33<N_> m;
		m(0,0) = a2 + b2 - c2 - d2;
		m(0,1) = bc - ad;
		m(0,2) = bd + ac;
		m(1,0) = bc + ad;
		m(1,1) = a2 - b2 + c2 - d2;
		m(1,2) = cd - ab;
		m(2,0) = bd - ac;
		m(2,1) = cd + ab;
		m(2,2) = a2 - b2 - c2 + d2;
		return m;
	}

}	// namespace math
}	// namespace rev

#endif // _REV_REVCORE_MATH_QUATERION_H_
