////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 1st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// renderable scene

#ifndef _REV_REVVIDEO_SCENE_VIDEOSCENE_H_
#define _REV_REVVIDEO_SCENE_VIDEOSCENE_H_

#include "rtl/poolset.h"

namespace rev { namespace video
{
	// Forward references
	class IRenderable;

	class CVideoScene
	{
	public:
		// -- Default scene
		static CVideoScene* defaultScene();
		// -- Add renderable
		void addRenderable		(IRenderable * _renderable);
		void removeRenderable	(IRenderable * _renderable);
	private:
		// -- Default scene
		static CVideoScene* sDefaultScene;
		// Scene renderables
		typedef rtl::poolset<IRenderable*>	TRenderableContainer;
		
		TRenderableContainer	mRenderables;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SCENE_VIDEOSCENE_H_
