//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/2
//----------------------------------------------------------------------------------------------------------------------
// render scene

#include "renderable.h"
#include "renderScene.h"

#include <revMath/algebra/matrix.h>
#include <revVideo/driver3d/driver3d.h>
#include <revVideo/videoDriver/videoDriver.h>

using namespace rev::math;

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
	void RenderScene::add(const Renderable* _x)
	{
		mRenderQueue.push_back(_x);
	}

	//----------------------------------------------------------------------------------------------------------------------
	void RenderScene::remove(const Renderable* _x)
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
	void RenderScene::traverse(std::function<void (const Renderable*)>_fn) const
	{
		for(auto element : mRenderQueue )
			_fn(element);
	}
	
}	// namespace graphics3d
}	// namespace rev