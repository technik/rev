////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 30th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// renderable scene

#include "renderableInstance.h"

#include "revCore/node/node.h"
#include "revVideo/camera/camera.h"
#include "revVideo/renderer/renderer3d.h"
#include "revVideo/video.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	void IRenderableInstance::setEnviroment() const
	{
		// Set model-view-projection matrix
		IVideoDriver * driver = SVideo::get()->driver();
		int mvpUniformId = driver->getUniformId("modelViewProj");
		IRenderer3d * renderer = SVideo::get()->renderer();
		renderer->setModelMatrix(node()->transform());
		driver->setUniform(mvpUniformId, renderer->viewProjMatrix() * node()->transform());
	}
}	// namespace video
}	// namespace rev