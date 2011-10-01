////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, video driver interface
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 25th, 2011
////////////////////////////////////////////////////////////////////////////////
// Matrix

#ifndef _REV_REVCORE_MATH_MATRIX_H_
#define _REV_REVCORE_MATH_MATRIX_H_

#include "revCore/types.h"

namespace rev
{
//----------------------------------------------------------------------------------------------------------------------
// Forward declarations
template <typename _T> class TMat34;
template <typename _T> class TQuaternion;
template <typename _T> class TVec3;

//----------------------------------------------------------------------------------------------------------------------
// Matrix 3x3
template<typename _T>
class TMat3
{
public:
	// Constructors
	TMat3()	{}
	TMat3(const TQuaternion<_T>& _q);
	TMat3(const TMat34<_T>& _mat);

public:
	_T m[3][3];
};

//----------------------------------------------------------------------------------------------------------------------
// Matrix 3x4
template<typename _T>
class TMat34
{
public:
	// Constructors
	TMat34()	{}
	TMat34(const TMat3<_T>& _mat)
	{
		for(unsigned int row = 0; row < 3; ++row)
		{
			for(unsigned int column = 0; column < 4; ++column)
			{
				m[row][column] = _mat.m[row][column];
			}
		}
	}
	TMat34(const TQuaternion<_T>& _q, const TVec3<_T>& _v);

	// Identity
	static TMat34<_T> identity()
	{
		TMat4 mtx;
		for(unsigned int row = 0; row < 3; ++row)
		{
			for(unsigned int column = 0; column < 4; ++column)
			{
				if(column == row)
					mtx.m[row][column] = 1.f;
				else
					mtx.m[row][column] = 0.f;
			}
		}
		return mtx;
	}

	// Mathematical operations
	TMat34<_T> operator * (const TMat34& _b) const
	{
		TMat34<_T> ab;
		for(unsigned row = 0; row < 3; ++row)
		{
			for(unsigned column = 0; column < 4; ++ column)
			{
				ab.m[row][column] = m[row][0]*_b.m[0][column]
								+ m[row][1]*_b.m[1][column]
								+ m[row][2]*_b.m[2][column];
			}
		}
		ab.m[0][3] += m[0][3];
		ab.m[1][3] += m[1][3];
		ab.m[2][3] += m[2][3];
		return ab;
	}

	TVec3<_T> operator * (const TVec3<_T>& _b) const
	{
		return TVec3<_T>(
			m[0][0] * _b.x + m[0][1] * _b.y + m[0][2] * _b.z + m[0][3],
			m[1][0] * _b.x + m[1][1] * _b.y + m[1][2] * _b.z + m[1][3],
			m[2][0] * _b.x + m[2][1] * _b.y + m[2][2] * _b.z + m[2][3]
		);
	}

	void inverse(TMat34<_T>& _dst) const
	{
		 // Assume mat34 are always affine transforms
		for(unsigned row = 0; row < 3; ++row)
		{
			for(unsigned column = 0; column < 3; ++column)
			{
				_dst.m[row][column] = m[column][row];
			}
			_dst.m[row][3] = - (_dst.m[row][0] * m[0][3] + _dst.m[row][1] * m[1][3] + _dst.m[row][2] * m[2][3]);
		}
	}

public:
	_T m[3][4];
};

//----------------------------------------------------------------------------------------------------------------------
// Matrix 4x4
template<typename _T>
class TMat4
{
public:
	// Identity
	static TMat4 identity()
	{
		TMat4 mtx;
		for(unsigned int row = 0; row < 4; ++row)
		{
			for(unsigned int column = 0; column < 4; ++column)
			{
				if(column == row)
					mtx.m[row][column] = 1.f;
				else
					mtx.m[row][column] = 0.f;
			}
		}
		return mtx;
	}

	TMat4<_T> operator * (const TMat34<_T>& _b) const
	{
		TMat4<_T> dst;
		for(unsigned row = 0; row < 4; ++row)
		{
			for(unsigned column = 0; column < 4; ++column)
			{
				dst.m[row][column]	= m[row][0] * _b.m[0][column]
									+ m[row][1] * _b.m[1][column]
									+ m[row][2] * _b.m[2][column];
			}
			dst.m[row][4] += m[row][4];
		}
		return dst;
	}

public:
	_T m[4][4];
};

//----------------------------------------------------------------------------------------------------------------------
// typedefs
typedef TMat3<TReal>	CMat3;
typedef TMat3<float>	CMat3f;
typedef TMat3<double>	CMat3d;

typedef TMat34<TReal>	CMat34;
typedef TMat34<float>	CMat34f;
typedef TMat34<double>	CMat34d;

typedef TMat4<TReal>	CMat4;
typedef TMat4<float>	CMat4f;
typedef TMat4<double>	CMat4d;

//----------------------------------------------------------------------------------------------------------------------
// Inline implementations
//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
TMat3<_T>::TMat3(const TQuaternion<_T>& _q)
{
	_T a2 = _q.w*_q.w;
	_T b2 = _q.x*_q.x;
	_T c2 = _q.y*_q.y;
	_T d2 = _q.z*_q.z;
	_T ab = 2*_q.w*_q.x;
	_T ac = 2*_q.w*_q.y;
	_T ad = 2*_q.w*_q.z;
	_T bc = 2*_q.x*_q.y;
	_T bd = 2*_q.x*_q.z;
	_T cd = 2*_q.y*_q.z;
	m[0][0] = a2+b2-c2-d2;
	m[0][1] = bc-ad;
	m[0][2] = bd+ac;
	m[1][0] = bc+ad;
	m[1][1] = a2-b2+c2-d2;
	m[1][2] = cd-ab;
	m[2][0] = bd-ac;
	m[2][1] = cd+ab;
	m[2][2] = a2-b2-c2+d2;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
TMat3<_T>::TMat3(const TMat34<_T>& _m)
{
	for(unsigned row = 0; row < 3; ++row)
	{
		for(unsigned column = 0; column < 4; ++column)
		{
			m[row][column] = m[row][column];
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
template<typename _T>
TMat34<_T>::TMat34(const TQuaternion<_T>& _q, const TVec3<_T>& _v)
{
	// Rotation
	_T a2 = _q.w*_q.w;
	_T b2 = _q.x*_q.x;
	_T c2 = _q.y*_q.y;
	_T d2 = _q.z*_q.z;
	_T ab = 2*_q.w*_q.x;
	_T ac = 2*_q.w*_q.y;
	_T ad = 2*_q.w*_q.z;
	_T bc = 2*_q.x*_q.y;
	_T bd = 2*_q.x*_q.z;
	_T cd = 2*_q.y*_q.z;
	m[0][0] = a2+b2-c2-d2;
	m[0][1] = bc-ad;
	m[0][2] = bd+ac;
	m[1][0] = bc+ad;
	m[1][1] = a2-b2+c2-d2;
	m[1][2] = cd-ab;
	m[2][0] = bd-ac;
	m[2][1] = cd+ab;
	m[2][2] = a2-b2-c2+d2;
	// Translation
	m[0][3] = _v.x;
	m[1][3] = _v.y;
	m[2][3] = _v.z;
}

}	// namespace rev

#endif // _REV_REVCORE_MATH_MATRIX_H_
