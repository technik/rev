////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on September 26th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// quaternion

#include "quaternion.h"

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	// Static data definitions
	template<>
	CQuat CQuat::identity = CQuat(0.f, 0.f, 0.f, 1.f);
}	// namespace rev
