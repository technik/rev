//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 12th, 2012
//----------------------------------------------------------------------------------------------------------------------
// OpenGL 2.0 Shader

#include "shaderOpenGL21.h"

#include <revCore/codeTools/assert/assert.h>
#include <revVideo/driver3d/openGL21/driver3dOpenGL21.h>
#include <revVideo/types/shader/pixel/openGL21/pxlShaderOpenGL21.h>
#include <revVideo/types/shader/vertex/openGL21/vtxShaderOpenGL21.h>
#include <revVideo/videoDriver/videoDriver.h>

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	ShaderOpenGL21::ShaderOpenGL21(unsigned _id)
		:mId(_id)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	ShaderOpenGL21::~ShaderOpenGL21()
	{
		Driver3dOpenGL21 * driver = static_cast<Driver3dOpenGL21*>(VideoDriver::getDriver3d());
		driver->glDeleteProgram(mId);
	}

}	// namespace video
}	// namespace rev
