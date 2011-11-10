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
		inline IRenderableInstance			(IRenderable * _renderable = 0, IMaterialInstance * _material = 0)
			:mRenderable(_renderable)
			,mMaterialInstance(_material)
		{
			CVideoScene::defaultScene()->addRenderable(this);
			mScene = CVideoScene::defaultScene();
		}
		virtual ~IRenderableInstance		()
		{
			mScene->removeRenderable(this);
		}

		// Scene management
		void				setScene(CVideoScene * _scn)
		{
			if( 0 != mScene)
				mScene->removeRenderable(this);
			_scn->addRenderable(this);
			mScene = _scn;
		}

		// Interface
		inline IRenderable*	renderable		() const { return mRenderable; }
		virtual void		setEnviroment	(const ICamera * _cam) const;
		IMaterialInstance*	materialInstance() const { return mMaterialInstance; }
	protected:
		void				setRenderable	(IRenderable * _renderable) { mRenderable = _renderable; }

		IMaterialInstance*	mMaterialInstance;
	private:
		IRenderable * mRenderable;
		CVideoScene * mScene;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SCENE_RENDERABLEINSTANCE_H_
