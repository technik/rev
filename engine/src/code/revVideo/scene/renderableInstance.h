////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 30th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// renderable scene

#ifndef _REV_REVVIDEO_SCENE_RENDERABLEINSTANCE_H_
#define _REV_REVVIDEO_SCENE_RENDERABLEINSTANCE_H_

#include "revCore/component/component.h"
#include "revVideo/scene/videoScene.h"

namespace rev { namespace video
{
	// Forward declaration
	class ICamera;
	class IMaterialInstance;
	class IRenderable;

	class IRenderableInstance: public IComponent
	{
	public:
		// Constructor and destructor
		inline IRenderableInstance(IRenderable * _renderable = 0):mRenderable(_renderable)
		{
			CVideoScene::defaultScene()->addRenderable(this);
			mScene = CVideoScene::defaultScene();
		}
		virtual ~IRenderableInstance()
		{
			mScene->removeRenderable(this);
		}

		// Interface
		inline IRenderable *		renderable			() const { return mRenderable; }
		virtual void				setEnviroment		(const ICamera * _cam) const = 0;
		virtual IMaterialInstance * materialInstance	() const = 0;
	protected:
		inline void					setRenderable		(IRenderable * _renderable) { mRenderable = _renderable; }
	private:
		IRenderable * mRenderable;
		CVideoScene * mScene;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SCENE_RENDERABLEINSTANCE_H_
