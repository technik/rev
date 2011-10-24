////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on October 1st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Matrix

#include "matrix.h"
#include "quaternion.h"
#include "vector.h"

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	// Static definitions
	CMat34 CMat34::identity = CMat34(CVec3(1.f,1.f,1.f));

	//------------------------------------------------------------------------------------------------------------------
	CMat3::CMat3(const CQuat& _q)
	{
		TReal a2 = _q.w*_q.w;
		TReal b2 = _q.x*_q.x;
		TReal c2 = _q.y*_q.y;
		TReal d2 = _q.z*_q.z;
		TReal ab = 2*_q.w*_q.x;
		TReal ac = 2*_q.w*_q.y;
		TReal ad = 2*_q.w*_q.z;
		TReal bc = 2*_q.x*_q.y;
		TReal bd = 2*_q.x*_q.z;
		TReal cd = 2*_q.y*_q.z;
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

	//------------------------------------------------------------------------------------------------------------------
	CMat3::CMat3(const CMat34& _m)
	{
		for(unsigned row = 0; row < 3; ++row)
		{
			for(unsigned column = 0; column < 4; ++column)
			{
				m[row][column] = _m.m[row][column];
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	CMat34::CMat34(const CMat3& _mat)
	{
		for(unsigned int row = 0; row < 3; ++row)
		{
			for(unsigned int column = 0; column < 4; ++column)
			{
				m[row][column] = _mat.m[row][column];
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	CMat34::CMat34(const CVec3& _diagonal)
	{
		for(unsigned int row = 0; row < 3; ++row)
		{
			for(unsigned int column = 0; column < 4; ++column)
			{
				m[row][column] = 0.f;
			}
		}
		m[0][0] = _diagonal.x;
		m[1][1] = _diagonal.y;
		m[2][2] = _diagonal.z;
	}

	//------------------------------------------------------------------------------------------------------------------
	CMat34::CMat34(const CQuat& _q, const CVec3& _v)
	{
		// Rotation
		TReal a2 = _q.w*_q.w;
		TReal b2 = _q.x*_q.x;
		TReal c2 = _q.y*_q.y;
		TReal d2 = _q.z*_q.z;
		TReal ab = 2*_q.w*_q.x;
		TReal ac = 2*_q.w*_q.y;
		TReal ad = 2*_q.w*_q.z;
		TReal bc = 2*_q.x*_q.y;
		TReal bd = 2*_q.x*_q.z;
		TReal cd = 2*_q.y*_q.z;
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

	//------------------------------------------------------------------------------------------------------------------
	CMat34 CMat34::operator * (const CMat34& _b) const
	{
		CMat34 ab;
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

	//------------------------------------------------------------------------------------------------------------------
	CVec3 CMat34::operator * (const CVec3& _b) const
	{
		return CVec3(
			m[0][0] * _b.x + m[0][1] * _b.y + m[0][2] * _b.z + m[0][3],
			m[1][0] * _b.x + m[1][1] * _b.y + m[1][2] * _b.z + m[1][3],
			m[2][0] * _b.x + m[2][1] * _b.y + m[2][2] * _b.z + m[2][3]
		);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CMat34::inverse(CMat34& _dst) const
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
}	// namespace rev
