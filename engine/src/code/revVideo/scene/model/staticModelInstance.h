////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 3rd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static model instance

#ifndef _REV_REVVIDEO_SCENE_MODEL_STATICMODELINSTANCE_H_
#define _REV_REVVIDEO_SCENE_MODEL_STATICMODELINSTANCE_H_

#include "revVideo/scene/renderableInstance.h"

namespace rev { namespace video
{
	class CStaticModel;

	class CStaticModelInstance: public IRenderableInstance
	{
	public:
		CStaticModelInstance(const char * _modelName, const char * _material);
		CStaticModelInstance(CStaticModel * _model, CMaterialInstance * materialInstance);
		~CStaticModelInstance();

	private:
		CStaticModel * mModel;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SCENE_MODEL_STATICMODELINSTANCE_H_
