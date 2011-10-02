////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 1st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// camera

// Engine headers
#include "camera.h"

#include "revCore/entity/entity.h"
#include "revVideo/scene/videoScene.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	ICamera::ICamera():mScene(CVideoScene::defaultScene())
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

	//------------------------------------------------------------------------------------------------------------------
	const CMat4& ICamera::projMatrix() const
	{
		return mProjection;
	}

	//------------------------------------------------------------------------------------------------------------------
	CVideoScene* ICamera::scene() const
	{
		return mScene;
	}

	//------------------------------------------------------------------------------------------------------------------
	void ICamera::setScene(CVideoScene* _scn)
	{
		mScene = _scn;
	}
}	// namespace video
}	// namespace rev
