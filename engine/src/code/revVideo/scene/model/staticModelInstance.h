////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 3rd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static model instance

#ifndef _REV_REVVIDEO_SCENE_MODEL_STATICMODELINSTANCE_H_
#define _REV_REVVIDEO_SCENE_MODEL_STATICMODELINSTANCE_H_

#include "revVideo/scene/renderable.h"

namespace rev { namespace video
{
	// Forward declarations
	class CStaticModel;
	class IMaterialInstance;

	class CStaticModelInstance: public IRenderable
	{
	public:
		CStaticModelInstance(CStaticModel* _model, IMaterialInstance * _material);
		~CStaticModelInstance();
		// TODO: Add a constructor that recieves the name of the model
		// TODO: Manage ownership of the model

		float*			vertices	() const;
		unsigned short*	triangles	() const;
		int				nTriangles	() const;
		unsigned short* triStrip	() const;
		int				stripLength	() const;
	private:
		IMaterialInstance * material() const;
	private:
		CStaticModel* mModel;
		IMaterialInstance * mMaterial;
	};

	//------------------------------------------------------------------------------------------------------------------
	inline IMaterialInstance * CStaticModelInstance::material() const
	{
		return mMaterial;
	}
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SCENE_MODEL_STATICMODELINSTANCE_H_
