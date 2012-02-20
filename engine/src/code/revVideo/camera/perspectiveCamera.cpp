////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on November 27th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Perspective camera
#include <math.h>

#include "perspectiveCamera.h"

#include "revCore/math/vector.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CPerspectiveCamera::CPerspectiveCamera (TReal _fov, TReal _invRatio, const CVec2& _clipDistances)
	{
		// Precomputations
		TReal horFactor = 1.f / cos(3.14159f * _fov / 180.f); // One over cosine of the field of view
		TReal verFactor = horFactor * _invRatio;
		TReal invDepthRange = 1.f / (_clipDistances.y - _clipDistances.x);
		// Row 0
		mProjection.m[0][0] = horFactor;
		mProjection.m[0][1] = 0.f;
		mProjection.m[0][2] = 0.f;
		mProjection.m[0][3] = 0.f;
		// Row 1   
		mProjection.m[1][0] = 0.f;
		mProjection.m[1][1] = 0.f;
		mProjection.m[1][2] = verFactor; // Map height from Z to Y
		mProjection.m[1][3] = 0.f;
		// Row 2   
		mProjection.m[2][0] = 0.f;
		mProjection.m[2][1] = 2.f * invDepthRange;
		mProjection.m[2][2] = 0.f;
		mProjection.m[2][3] = - (_clipDistances.x + _clipDistances.y) * invDepthRange;
		// Row 3, homogeneous component
		mProjection.m[3][0] = 0.f;
		mProjection.m[3][1] = 1.f; // Homogeneous component maps to depth
		mProjection.m[3][2] = 0.f;
		mProjection.m[3][3] = 0.f;
	}

}	// namespace video
}	// namespace rev
