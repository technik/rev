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
	class CMaterialInstance;
	class ICamera;
	class IMaterial;
	class IRenderable;

	class IRenderableInstance: public IComponent
	{
	public:
		// Constructor and destructor
		inline IRenderableInstance			(IRenderable * _renderable = 0, CMaterialInstance * _material = 0)
			:mMaterialInstance(_material)
			,mRenderable(_renderable)
		{
			CVideoScene::defaultScene()->addRenderable(this);
			mScene = CVideoScene::defaultScene();
		}
		virtual ~IRenderableInstance		()
		{
			setMaterialInstance(0);
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
		virtual void		setEnvironment	() const;
		CMaterialInstance*	materialInstance() const { return mMaterialInstance; }
	protected:
		void				setRenderable	(IRenderable * _renderable) { mRenderable = _renderable; }
		void				setMaterial		(IMaterial * _material);
		void				setMaterialInstance (CMaterialInstance * _instance);

	private:
		CMaterialInstance*	mMaterialInstance;
		IRenderable * mRenderable;
		CVideoScene * mScene;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SCENE_RENDERABLEINSTANCE_H_
