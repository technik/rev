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
	void CVideoScene::reserve(unsigned _nRenderables)
	{
		mRenderables.reserve(_nRenderables);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoScene::addRenderable(IRenderableInstance * _renderable)
	{
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