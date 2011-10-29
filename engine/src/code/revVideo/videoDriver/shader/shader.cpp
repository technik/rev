////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Shaders

#include "shader.h"

#include "revVideo/video.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	// Static data definitions
	CShader::managerT * CShader::sManager = 0;

	//------------------------------------------------------------------------------------------------------------------
	CShader::managerT * CShader::manager()
	{
		if(0 == sManager)
			sManager = new managerT();
		return sManager;
	}

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
