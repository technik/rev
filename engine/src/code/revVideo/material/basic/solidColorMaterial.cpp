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
		mShader = CPxlShader::manager()->get("solidColor.pxl");
		mColorUniformId = SVideo::get()->driver()->getUniformId("color");
	}

	//------------------------------------------------------------------------------------------------------------------
	void CSolidColorMaterial::setEnviroment() const
	{
		SVideo::get()->driver()->setUniform(mColorUniformId, mColor);
	}
}	// namespace video
}	// namespace rev
