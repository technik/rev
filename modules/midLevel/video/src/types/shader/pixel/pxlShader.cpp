//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 24th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Pixel Shader

#include "pxlShader.h"

#include <driver3d/driver3d.h>
#include <videoDriver/videoDriver.h>

namespace rev
{
	video::PxlShader::Manager * video::PxlShader::sManager = 0;
}	// namespace rev

namespace rev { namespace video
{
	PxlShader * PxlShader::factory(const std::string& _fileName)
	{
		return VideoDriver::getDriver3d()->createPxlShader(_fileName.c_str());
	}
}	// namespace video
}	// namespace rev
