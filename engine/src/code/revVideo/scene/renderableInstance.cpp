////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 30th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// renderable scene

#include "renderableInstance.h"

#include "revCore/node/node.h"
#include "revVideo/camera/camera.h"
#include "revVideo/video.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	void IRenderableInstance::setEnviroment	(const ICamera * _cam) const
	{
		// Set model-view-projection matrix
		IVideoDriver * driver = SVideo::get()->driver();
		int mvpUniformId = driver->getUniformId("modelViewProj");
		CMat4 viewProj = _cam->viewProj(); // _cam->projMatrix();
		driver->setUniform(mvpUniformId, viewProj * node()->transform());
	}
}	// namespace video
}	// namespace rev