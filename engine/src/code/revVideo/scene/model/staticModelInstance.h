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
	// Forward declarations
	class CStaticModel;
	class ICamera;
	class IMaterialInstance;

	class CStaticModelInstance: public IRenderableInstance
	{
	public:
		CStaticModelInstance(const char * _modelName, IMaterialInstance * _material);
		~CStaticModelInstance();

		void setEnviroment (const ICamera * _cam) const;

	private:
		IMaterialInstance * materialInstance() const;

	private:
		CStaticModel* mModel;
		IMaterialInstance * mMaterial;
	};

	//------------------------------------------------------------------------------------------------------------------
	inline IMaterialInstance * CStaticModelInstance::materialInstance() const
	{
		return mMaterial;
	}
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SCENE_MODEL_STATICMODELINSTANCE_H_
