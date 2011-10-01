////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 1st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// camera

// Engine headers
#include "camera.h"

#include "revCore/entity/entity.h"
// #include "revVideo/scene/scene.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	ICamera::ICamera():mScene(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	CMat4 ICamera::viewProj	()	const
	{
		if(getEntity() && getEntity()->transformSource())
		{
			CMat34 invParentWorld;
			this->getEntity()->transformSource()->transform().inverse(invParentWorld);
			return mProjection * invParentWorld;
		}
		else
		{
			return mProjection;
		}
	}
}	// namespace video
}	// namespace rev
