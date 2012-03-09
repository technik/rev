////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on November 4th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Solid color material

#include "raymarchingMaterial.h"

#include "revCore/resourceManager/resourceManager.h"
#include "revCore/codeTools/log/log.h"
#include "revCore/time/time.h"
#include "revVideo/video.h"
#include "revVideo/videoDriver/shader/pxlShader.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CRaymarchingMaterial::CRaymarchingMaterial():mTime(0.f)
	{
		mShader = CPxlShader::manager()->get("raymarching.pxl");
	}

	//------------------------------------------------------------------------------------------------------------------
	CRaymarchingMaterial::~CRaymarchingMaterial()
	{
		CPxlShader::manager()->release(mShader);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CRaymarchingMaterial::setEnvironment() const
	{
		float dt = STime::get()->frameTime();
		mTime +=dt;
		while(mTime > 4.f)
			mTime -= 4.f;
		IVideoDriver * driver = SVideo::get()->driver();
		int uniformId = driver->getUniformId("time");
		float clampTime = 0.5f*mTime - 1.0f;
		clampTime = clampTime > 0.f ? clampTime : -clampTime;
		driver->setUniform(uniformId, 2.f*clampTime-1.f);
	}

}	// namespace video
}	// namespace rev
