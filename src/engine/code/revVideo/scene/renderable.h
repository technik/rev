////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 2nd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// renderable

#ifndef _REV_REVVIDEO_SCENE_RENDERABLE_H_
#define _REV_REVVIDEO_SCENE_RENDERABLE_H_

#include "revCore/component/component.h"
#include "revVideo/scene/videoScene.h"

namespace rev { namespace video
{
	class IRenderable: public IComponent
	{
	public:
		IRenderable()
		{
			CVideoScene::defaultScene()->addRenderable(this);
		}
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SCENE_RENDERABLE_H_
