////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 9th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Material instance

#include "materialInstance.h"

#include "material.h"

namespace rev { namespace video
{
	//---------------------------------------------------------------------------------------------------------------
	CMaterialInstance::CMaterialInstance(IMaterial * _material):mMaterial(_material)
	{
		mMaterial->getOwnership();
	}

	//---------------------------------------------------------------------------------------------------------------
	CMaterialInstance::~CMaterialInstance()
	{
		mMaterial->release();
	}
}	// namespace video
}	// namespace rev