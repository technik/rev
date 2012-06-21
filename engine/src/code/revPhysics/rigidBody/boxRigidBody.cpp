////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on June 18th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Box Rigid Body

#include "boxRigidBody.h"

namespace rev { namespace physics
{
	//------------------------------------------------------------------------------------------------------------------
	CBoxRigidBody::CBoxRigidBody(const CVec3& _size, float _mass)
		:CRigidBody(_mass)
		,mHalfSize(0.5f*_size)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	float CBoxRigidBody::rayCast(const CVec3& _origin, const CVec3& _uDir) const
	{
		// Transform ray origin to local space
		CVec3 pos = _origin - position();
		CVec3 dir(_uDir.x / mHalfSize.x, _uDir.y / mHalfSize.y, _uDir.z / mHalfSize.z);
		// Intersection test (adapatation of Smit's method)
		// X Axis
		float tmin, tmax;
		float divx = 1.f / _uDir.x;
		if(divx >= 0.f)
		{
			tmin = -(mHalfSize.x + pos.x) * divx;
			tmax = (mHalfSize.x - pos.x) * divx;
		}
		else
		{
			tmin = (mHalfSize.x - pos.x) * divx;
			tmax = -(mHalfSize.x + pos.x) * divx;
		}
		// Y Axis
		float tymin, tymax;
		float divy = 1.f / _uDir.y;
		if(divy >= 0.f)
		{
			tymin = -(mHalfSize.y + pos.y) * divy;
			tymax = (mHalfSize.y - pos.y) * divy;
		}
		else
		{
			tymin = (mHalfSize.y - pos.y) * divy;
			tymax = -(mHalfSize.y + pos.y) * divy;
		}
		// Check ray
		if((tmin > tymax) || (tymin > tmax))
			return -1.f;
		tmin = tmin>tymin?tmin:tymin; // Take the largest minimun
		tmax = tmax<tymax?tmax:tymax; // and the smallest maximun
		// Z Axis
		float tzmin, tzmax;
		float divz = 1.f / _uDir.z;
		if(divz >= 0.f)
		{
			tzmin = -(mHalfSize.z + pos.z) * divz;
			tzmax = (mHalfSize.z - pos.z) * divz;
		}
		else
		{
			tzmin = (mHalfSize.z - pos.z) * divz;
			tzmax = -(mHalfSize.z + pos.z) * divz;
		}
		// Check ray
		if((tmin > tzmax) || (tzmin > tmax))
			return -1.f;
		return tmin>tzmin?tmin:tzmin; // return the largest minimun
	}

}	// namespace physics
}	// namespace rev
