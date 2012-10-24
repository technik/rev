//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 24th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Vertex Shader

#include "vtxShader.h"

#include <driver3d/driver3d.h>
#include <videoDriver/videoDriver.h>

namespace rev
{
	video::VtxShader::Manager * video::VtxShader::sManager = 0;
}	// namespace rev

namespace rev { namespace video
{
	VtxShader * VtxShader::factory(const std::string& _fileName)
	{
		return VideoDriver::getDriver3d()->createVtxShader(_fileName.c_str());
	}
}	// namespace video
}	// namespace rev
