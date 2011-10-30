////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 3rd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static model instance

#include "staticModelInstance.h"
#include "staticModel.h"

#include "revCore/node/node.h"
#include "revVideo/camera/camera.h"
#include "revVideo/scene/renderableInstance.h"
#include "revVideo/video.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CStaticModelInstance::CStaticModelInstance(const char * _modelName, IMaterialInstance * _material)
		:mMaterial(_material)
	{
		mModel = CStaticModel::manager()->get(string(_modelName));
		IRenderableInstance::setRenderable(mModel);
	}

	//------------------------------------------------------------------------------------------------------------------
	CStaticModelInstance::~CStaticModelInstance()
	{
		CStaticModel::manager()->release(mModel);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CStaticModelInstance::setEnviroment (const ICamera * _cam) const
	{
		// Set model-view-projection matrix
		IVideoDriver * driver = SVideo::get()->driver();
		int mvpUniformId = driver->getUniformId("modelViewProj");
		CMat4 viewProj = _cam->projMatrix();
		driver->setUniform(mvpUniformId, viewProj * node()->transform());
	}

}	// namespace video
}	// namespace rev