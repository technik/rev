//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 21st, 2012
//----------------------------------------------------------------------------------------------------------------------
// Shader

#include "shader.h"
#include <driver3d/driver3d.h>
#include <videoDriver/videoDriver.h>

namespace rev {
	video::Shader::Manager* video::Shader::sManager = 0;
}	// namespace rev

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	Shader* Shader::factory(const std::pair<VtxShader*, PxlShader*>& _key)
	{
		return VideoDriver::getDriver3d()->createShader(_key.first, _key.second);
	}
}	// namespace video
}	// namespace rev