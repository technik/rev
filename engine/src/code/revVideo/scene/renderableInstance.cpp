////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 30th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// renderable scene

#include "renderableInstance.h"

#include <revCore/codeTools/profiler/profiler.h>
#include <revCore/node/node.h>
#include <revVideo/camera/camera.h>
#include <revVideo/material/material.h>
#include <revVideo/renderer/renderer3d.h>
#include <revVideo/video.h>
#include <revVideo/videoDriver/videoDriver.h>

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	IRenderableInstance::IRenderableInstance(IRenderable * _renderable, CMaterialInstance * _material)
		:mMaterialInstance(_material)
		,mRenderable(_renderable)
		,mScene(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void IRenderableInstance::setEnvironment() const
	{
		// Set model matrix
		IRenderer3d * renderer = SVideo::get()->renderer();
		renderer->setModelMatrix(node()->transform());
	}

	//------------------------------------------------------------------------------------------------------------------
	void IRenderableInstance::setMaterial(IMaterial * _material)
	{
		setMaterialInstance(_material->getInstance());
	}

	//------------------------------------------------------------------------------------------------------------------
	void IRenderableInstance::setMaterialInstance(CMaterialInstance * _instance)
	{
		if( 0 != mMaterialInstance )
		{
			delete mMaterialInstance;
		}
		mMaterialInstance = _instance;
	}
}	// namespace video
}	// namespace rev