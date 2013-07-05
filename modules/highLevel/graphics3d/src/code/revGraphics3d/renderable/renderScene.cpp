//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/2
//----------------------------------------------------------------------------------------------------------------------
// render scene

#include "renderable.h"
#include "renderScene.h"

namespace rev { namespace graphics3d {

	//------------------------------------------------------------------------------------------------------------------
	// Static data
	//----------------------------------------------------------------------------------------------------------------------
	RenderScene* RenderScene::sInstance = nullptr;

	//----------------------------------------------------------------------------------------------------------------------
	RenderScene* RenderScene::get()
	{
		if(nullptr == sInstance)
			sInstance = new RenderScene();
		return sInstance;
	}

	//----------------------------------------------------------------------------------------------------------------------
	void RenderScene::add(Renderable* _x)
	{
		mRenderQueue.push_back(_x);
	}

	//----------------------------------------------------------------------------------------------------------------------
	void RenderScene::remove(Renderable* _x)
	{
		for(auto element = mRenderQueue.begin(); element != mRenderQueue.end(); ++element)
		{
			if(*element == _x)
			{
				mRenderQueue.erase(element);
			}
		}
	}

	//----------------------------------------------------------------------------------------------------------------------
	void RenderScene::render()
	{
		for(auto element : mRenderQueue )
			element->render();
	}
	
}	// namespace graphics3d
}	// namespace rev