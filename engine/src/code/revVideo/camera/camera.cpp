////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 1st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// camera

// Engine headers
#include "camera.h"

#include "revCore/node/node.h"
#include "revVideo/scene/videoScene.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	ICamera::ICamera():mScene(CVideoScene::defaultScene())
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	CMat34 ICamera::viewMatrix () const
	{
		if(node())
		{
			return node()->transform();
		}
		else
		{
			return CMat34::identity;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	CMat4 ICamera::viewProj	()	const
	{
		if(node())
		{
			CMat34 invParentWorld;
			node()->transform().inverse(invParentWorld);
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
