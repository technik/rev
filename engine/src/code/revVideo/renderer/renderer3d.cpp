////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 28th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 3d renderer

#include "renderer3d.h"

#include <revVideo/video.h>
#include <revVideo/videoDriver/videoDriver.h>

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	void IRenderer3d::setModelMatrix(const CMat34& _model)
	{
		mModelMatrix = _model;
		SVideo::getDriver()->setModelMatrix(_model);
	}
	
	//------------------------------------------------------------------------------------------------------------------
	void IRenderer3d::setViewMatrix(const CMat34& _view)
	{
		mViewMatrix = _view;
		SVideo::getDriver()->setViewMatrix(_view);
	}

	//---------------------------------------------------------------------------------------------------------------
	void IRenderer3d::setProjectionMatrix(const CMat4& _m)
	{
		mProjectionMatrix = _m;
		SVideo::getDriver()->setProjMatrix(_m);
	}
}	// namespace video
}	// namespace rev
