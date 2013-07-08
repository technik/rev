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
	void RenderScene::render(int _mvp)
	{
		video::Driver3d* driver = video::VideoDriver::getDriver3d();
		Mat44f proj = Mat44f::frustrum(1.6f, 1.333f, 1.f, 1000.f);
		for(auto element : mRenderQueue )
		{
			driver->setUniform(_mvp, proj * element->m);
			element->render();
		}
	}
	
}	// namespace graphics3d
}	// namespace rev