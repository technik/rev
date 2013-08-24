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

	//----------------------------------------------------------------------------------------------------------------------
	void RenderScene::traverse(const math::Vec3f& _camPos, std::function<void (const Renderable*)>_fn) const
	{
		traverse(_camPos, _fn, [](const Renderable*){return true;});
	}

	//----------------------------------------------------------------------------------------------------------------------
	void LinearRenderScene::traverse(const math::Vec3f&, std::function<void (const Renderable*)> _fn, std::function<bool (const Renderable*)> _filter) const
	{
		for(auto element : mRenderQueue )
			if(_filter(element))
				_fn(element);
	}

	//----------------------------------------------------------------------------------------------------------------------
	void LinearRenderScene::add(const Renderable* _x)
	{
		mRenderQueue.push_back(_x);
	}

	//----------------------------------------------------------------------------------------------------------------------
	void LinearRenderScene::remove(const Renderable* _x)
	{
		for(auto element = mRenderQueue.begin(); element != mRenderQueue.end(); ++element)
		{
			if(*element == _x)
			{
				mRenderQueue.erase(element);
			}
		}
	}
	
}	// namespace graphics3d
}	// namespace rev