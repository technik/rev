////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 3rd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static model instance

#include "staticModelInstance.h"
#include "staticModel.h"

#include <revCore/codeTools/profiler/profiler.h>
#include <revCore/node/node.h>
#include <revVideo/camera/camera.h>
#include <revVideo/material/material.h>
#include <revVideo/material/materialInstance.h>
#include <revVideo/scene/renderableInstance.h>
#include <revVideo/video.h>
#include <revVideo/videoDriver/videoDriver.h>

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CStaticModelInstance::CStaticModelInstance(const char * _modelName, const char * _material)
	{
		// Model
		mModel = CStaticModel::get(_modelName);
		mModel->getOwnership();
		CStaticModel::release(_modelName);
		IRenderableInstance::setRenderable(mModel);
		// Material
		IMaterial * material = IMaterial::get(_material);
		setMaterial(material);
		IMaterial::release(material); // Relinquish materail ownership
	}

	//------------------------------------------------------------------------------------------------------------------
	CStaticModelInstance::CStaticModelInstance(CStaticModel * _model, CMaterialInstance * _material)
		:mModel(_model)
	{
		IRenderableInstance::setRenderable(mModel);
		setMaterialInstance(_material);
	}

	//------------------------------------------------------------------------------------------------------------------
	CStaticModelInstance::~CStaticModelInstance()
	{
		mModel->release();
	}

}	// namespace video
}	// namespace rev