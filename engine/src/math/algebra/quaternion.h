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

	template<typename T>
	class UnitQuaternion
	{
	public:
		// Constructors
		UnitQuaternion	()	{}
		UnitQuaternion	(T _x, T _y, T _z, T _w):
			m({_x, _y, _z, _w})
		{}
		template<typename S_>
		UnitQuaternion	(const MatrixBase<3,3,S_>& _matrix);
		static UnitQuaternion	identity();

		// Accessors
		const T_& x() const { return m[0]; }
		const T_& y() const { return m[1]; }
		const T_& z() const { return m[2]; }
		const T_& w() const { return m[3]; }

		// Useful constructors
		/// \param axis is assumed to be normalized
		UnitQuaternion fromAxisAngle(const Vector3<T>& axis, T _radians)
		{
			auto half_sin = sin(_radians); // Using sine(theta/2) instead of cosine preserves the sign.
			m.block<3,1>(0,0) = axis*half_sin;
			w() = std::sqrt(1-half_sin*half_sin);
		}

		UnitQuaternion fromUnitVectors(const Vector3<T>& u, const Vector3<T>& v)
		{
			// TODO: This can be made faster by precomputing sqrt(2)
			auto m = sqrt(2 + 2 * u.dot(v));
			Vector3<T> w = (1 / m) * u.cross(v);
			return UnitQuaternion(w.x(), w.y(), w.z(), m*0.5f);
		}

		UnitQuaternion from2Vectors(const Vector3<T>& u, const Vector3<T>& v)
		{
			Vector3<T> w = u.cross(v);
			Quaternion q = quat(dot(u, v), w.x, w.y, w.z);
			q.w += length(q);
			return q.normalized();
		}

		UnitQuaternion conjugate() const
		{
			return Quaternion(-x(), -y(), -z(), w());
		}

		// Operators
		UnitQuaternion	operator *	(const Quaternion& _q) const;

		// Other operations
		Vector3<T>	rotate		(const Vector3<T>& _v) const;

		// Useful quaternions

	private:
		T_& x() { return m[0]; }
		T_& y() { return m[1]; }
		T_& z() { return m[2]; }
		T_& w() { return m[3]; }

		Vector4<T> m;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Useful typedefs
	typedef UnitQuaternion<float>	Quatf;
	typedef UnitQuaternion<double>	Quatd;

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementation
	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	template<typename S_>
	inline UnitQuaternion<N_>::UnitQuaternion(const MatrixBase<3,3,S_> & _m)
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
				N_ r = sqrt( 1 + _m(0,0) - _m(1,1) - _m(2,2) );
				N_ inv2r = 0.5f / r;
				w = (_m(2,1) - _m(1,2)) * inv2r;
				x = 0.5f * r;
				y = (_m(0,1) + _m(1,0)) * inv2r;
				z = (_m(2,0) + _m(0,2)) * inv2r;
			} else { // _m22 is the largest diagonal element
				N_ r = sqrt( 1 + _m(2,2) - _m(0,0) - _m(1,1) );
				N_ inv2r = 0.5f / r;
				w = (_m(1,0) - _m(0,1)) * inv2r;
				z = 0.5f * r;
				x = (_m(2,0) + _m(0,2)) * inv2r;
				y = (_m(1,2) + _m(2,1)) * inv2r;
			}
		} else {
			if(_m(1,1) > _m(2,2)) { // _m11 is the largest diagonal element
				N_ r = sqrt( 1 + _m(1,1) - _m(2,2) - _m(0,0) );
				N_ inv2r = 0.5f / r;
				w = (_m(0,1) - _m(1,0)) * inv2r;
				y = 0.5f * r;
				z = (_m(1,2) + _m(2,1)) * inv2r;
				x = (_m(0,1) + _m(1,0)) * inv2r;
			} else { // _m22 is the largest diagonal element
				N_ r = sqrt( 1 + _m(2,2) - _m(0,0) - _m(1,1) );
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
	inline UnitQuaternion<N_> UnitQuaternion<N_>::operator*(const Quaternion<N_>& _q) const
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
	inline Vector3<N_> UnitQuaternion<N_>::rotate(const Vector3<N_>& _v) const
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
			(a2+b2-c2-d2)*_v.x+2*((bc-ad)*_v.y+(bd+ac)*_v.z),	// x
			(a2-b2+c2-d2)*_v.y+2*((cd-ab)*_v.z+(bc+ad)*_v.x),	// y
			(a2-b2-c2+d2)*_v.z+2*((bd-ac)*_v.x+(cd+ab)*_v.y)	// z
			);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline UnitQuaternion<N_> UnitQuaternion<N_>::identity()
	{
		return Quaternion<N_>(0,0,0,1);
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
