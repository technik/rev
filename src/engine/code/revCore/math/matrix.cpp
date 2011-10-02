////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 1st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Matrix

#include "matrix.h"
#include "vector.h"

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	// Static definitions
	TMat34<TReal> TMat34<TReal>::identity = TMat34<TReal>(TVec3<TReal>(1.f,1.f,1.f));
}	// namespace rev
