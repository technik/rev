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
class CMat34;
class CQuat;
class CVec3;

//----------------------------------------------------------------------------------------------------------------------
// Matrix 3x3
class CMat3
{
public:
	// Constructors
	CMat3()	{}
	CMat3(const CQuat& _q);
	CMat3(const CMat34& _mat);

public:
	TReal m[3][3];
};

//----------------------------------------------------------------------------------------------------------------------
// Matrix 3x4
class CMat34
{
public:
	// Constructors
	CMat34()	{}
	CMat34(const CMat3& _mat);
	CMat34(const CVec3& _diagonal);
	CMat34(const CQuat& _q, const CVec3& _v);

	// Identity
	static CMat34 identity;

	// Mathematical operations
	CMat34 operator * (const CMat34& _b) const;

	CVec3 operator * (const CVec3& _b) const;

	void inverse(CMat34& _dst) const;

public:
	TReal m[3][4];
};

//----------------------------------------------------------------------------------------------------------------------
// Matrix 4x4
class CMat4
{
public:
	// Identity
	static CMat4 identity()
	{
		CMat4 mtx;
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

	CMat4 operator * (const CMat34& _b) const
	{
		CMat4 dst;
		for(unsigned row = 0; row < 4; ++row)
		{
			for(unsigned column = 0; column < 4; ++column)
			{
				dst.m[row][column]	= m[row][0] * _b.m[0][column]
									+ m[row][1] * _b.m[1][column]
									+ m[row][2] * _b.m[2][column];
			}
			dst.m[row][3] += m[row][3];
		}
		return dst;
	}

public:
	TReal m[4][4];
};

}	// namespace rev

#endif // _REV_REVCORE_MATH_MATRIX_H_
