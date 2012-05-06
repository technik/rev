////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 1st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// renderable scene

#ifndef _REV_REVVIDEO_SCENE_VIDEOSCENE_H_
#define _REV_REVVIDEO_SCENE_VIDEOSCENE_H_

#include <vector.h>

namespace rev { namespace video
{
	// Forward references
	class IRenderableInstance;

	class CVideoScene
	{
	public:
		typedef rtl::vector<IRenderableInstance*>	TRenderableContainer; // Should be a pool?
	public:
		// --  renderables
		void reserve			(unsigned _nRenderables);
		void addRenderable		(IRenderableInstance * _renderable);
		void removeRenderable	(IRenderableInstance * _renderable);

		TRenderableContainer&	renderables() { return mRenderables;	}
	private:
		// Scene renderables
		TRenderableContainer	mRenderables;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SCENE_VIDEOSCENE_H_
