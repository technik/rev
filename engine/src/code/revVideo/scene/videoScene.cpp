////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 1st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// renderable scene

#include "videoScene.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	// Static data definitions
	CVideoScene * CVideoScene::sDefaultScene = 0;

	//------------------------------------------------------------------------------------------------------------------
	CVideoScene * CVideoScene::defaultScene()
	{
		if(!sDefaultScene)
			sDefaultScene = new CVideoScene();
		return sDefaultScene;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoScene::addRenderable(IRenderableInstance * _renderable)
	{
		mRenderables.push(_renderable);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoScene::removeRenderable(IRenderableInstance * _renderable)
	{
		mRenderables.erase(_renderable);
	}
}	// namespace video
}	// namespace rev