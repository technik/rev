////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Shaders

#include "shader.h"

#include "revVideo/video.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev {
//------------------------------------------------------------------------------------------------------------------
// Static data definitions
TResource<video::CShader*, pair<video::CVtxShader*,video::CPxlShader*> >::managerT*
	TResource<video::CShader*, pair<video::CVtxShader*,video::CPxlShader*> >::sManager = 0;
}	// namespace rev

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CShader::CShader(pair<CVtxShader*, CPxlShader*> _baseShaders)
		:mVtx(_baseShaders.first)
		,mPxl(_baseShaders.second)
	{
		mId = SVideo::get()->driver()->linkShader(mVtx, mPxl);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CShader::setEnviroment() const
	{
		SVideo::get()->driver()->setShader(mId);
	}
}	// namespace video
}	// namespace rev
