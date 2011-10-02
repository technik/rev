////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
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
		typedef rtl::poolset<IRenderable*>	TRenderableContainer;
	public:
		// -- Default scene
		static CVideoScene* defaultScene();
		// --  renderables
		void addRenderable		(IRenderable * _renderable);
		void removeRenderable	(IRenderable * _renderable);

		TRenderableContainer&	renderables() { return mRenderables;	}
	private:
		// -- Default scene
		static CVideoScene* sDefaultScene;
		// Scene renderables
		
		TRenderableContainer	mRenderables;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SCENE_VIDEOSCENE_H_
