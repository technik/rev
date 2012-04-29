////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 1st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// renderable scene

#include "videoScene.h"

#include <revCore/codeTools/profiler/profiler.h>

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
	void CVideoScene::reserve(unsigned _nRenderables)
	{
		mRenderables.reserve(_nRenderables);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoScene::addRenderable(IRenderableInstance * _renderable)
	{
		//codeTools::CProfileFunction("CVideoScene::addRenderable");
		mRenderables.push_back(_renderable);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoScene::removeRenderable(IRenderableInstance * _renderable)
	{
		for(TRenderableContainer::iterator i = mRenderables.begin(); i != mRenderables.end(); ++i)
		{
			if(*i == _renderable)
			{
				mRenderables.erase(i);
				return;
			}
		}
	}
}	// namespace video
}	// namespace rev