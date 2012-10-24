//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 21st, 2012
//----------------------------------------------------------------------------------------------------------------------
// Shader

#include "shader.h"
#include <revVideo/driver3d/driver3d.h>
#include <revVideo/videoDriver/videoDriver.h>

namespace rev {
	using namespace video;
	template<>
	Shader::Manager* ManagedResourceBase<Shader, std::pair<VtxShader*, PxlShader*>, ShaderKeyHasher>::sManager = 0;
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