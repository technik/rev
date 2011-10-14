////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 1st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Orthographic camera

#include "orthoCamera.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	COrthoCamera::COrthoCamera(const CVec2& _apperture, TReal _zMin, TReal _zMax)
	{
		mProjection = CMat4::identity();
		mProjection.m[0][0] = 2.f/_apperture.x;
		mProjection.m[1][1] = 2.f/_apperture.y;
		mProjection.m[2][2] = -2.f/(_zMax - _zMin);
		mProjection.m[2][3] = (_zMin + _zMax)/(_zMax-_zMin);
	}

	//------------------------------------------------------------------------------------------------------------------
	void COrthoCamera::setApperture(const CVec2& _apperture)
	{
		mProjection.m[0][0] = 2.f/_apperture.x;
		mProjection.m[1][1] = 2.f/_apperture.y;
	}

	//------------------------------------------------------------------------------------------------------------------
	void COrthoCamera::setZRange(TReal _zMin, TReal _zMax)
	{
		mProjection.m[2][2] = -2.f/(_zMax - _zMin);
		mProjection.m[2][3] = (_zMin + _zMax)/(_zMax-_zMin);
	}
}	// namespace video
}	// namespace rev
