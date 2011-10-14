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

		// Buffers
		virtual float*			vertices	() const = 0;
		// Indices
		virtual unsigned short*	triangles	() const = 0;
		virtual int				nTriangles	() const = 0;
		virtual unsigned short* triStrip	() const = 0;
		virtual int				stripLength	() const = 0;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SCENE_RENDERABLE_H_
