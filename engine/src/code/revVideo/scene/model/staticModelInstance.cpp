////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 3rd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static model instance

#include "staticModelInstance.h"
#include "staticModel.h"

#include "revCore/node/node.h"
#include "revCore/resourceManager/passiveResourceManager.h"
#include "revCore/resourceManager/resourceManager.h"
#include "revVideo/camera/camera.h"
#include "revVideo/material/material.h"
#include "revVideo/material/materialInstance.h"
#include "revVideo/scene/renderableInstance.h"
#include "revVideo/video.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CStaticModelInstance::CStaticModelInstance(const char * _modelName, const char * _material)
	{
		mModel = CStaticModel::manager()->get(string(_modelName));
		IRenderableInstance::setRenderable(mModel);
		IMaterial * material = IMaterial::manager()->get(string(_material));
		setMaterial(material);
		IMaterial::manager()->release(material);
	}

	//------------------------------------------------------------------------------------------------------------------
	CStaticModelInstance::CStaticModelInstance(CStaticModel * _model, CMaterialInstance * _material)
		:mModel(_model)
	{
		setMaterialInstance(_material);
	}

	//------------------------------------------------------------------------------------------------------------------
	CStaticModelInstance::~CStaticModelInstance()
	{
		CStaticModel::manager()->release(mModel);
	}

}	// namespace video
}	// namespace rev