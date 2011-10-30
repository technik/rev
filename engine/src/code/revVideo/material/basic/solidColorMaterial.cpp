////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 28th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Solid color material

#include "solidColorMaterial.h"
#include "revVideo/video.h"
#include "revVideo/videoDriver/videoDriver.h"
#include "revVideo/videoDriver/shader/pxlShader.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CSolidColorMaterial::CSolidColorMaterial(const CColor& _color): mColor(_color)
	{
		if(0 == mShader)
			mShader = CPxlShader::manager()->get("solidColor.pxl");
		else
			mShader->get();
	}
	
	//------------------------------------------------------------------------------------------------------------------
	CSolidColorMaterial::~CSolidColorMaterial()
	{
		CPxlShader::manager()->release(mShader);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CSolidColorMaterial::setEnviroment() const
	{
		int colorUniformId = SVideo::get()->driver()->getUniformId("color");
		SVideo::get()->driver()->setUniform(colorUniformId, mColor);
	}
}	// namespace video
}	// namespace rev
