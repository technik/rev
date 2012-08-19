//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 12th, 2012
//----------------------------------------------------------------------------------------------------------------------
// OpenGL 2.0 Shader

#include "shaderOpenGL21.h"

#include <codeTools/assert/assert.h>
#include <driver3d/openGL21/driver3dOpenGL21.h>
#include <types/shader/pixel/openGL21/pxlShaderOpenGL21.h>
#include <types/shader/vertex/openGL21/vtxShaderOpenGL21.h>
#include <videoDriver/videoDriver.h>

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	ShaderOpenGL21::ShaderOpenGL21(const VtxShader * _vtx, const PxlShader * _pxl)
	{
		// Cast to the openGL version of the shaders
		const VtxShaderOpenGL21 * vtx21 = static_cast<const VtxShaderOpenGL21*>(_vtx);
		const PxlShaderOpenGL21 * pxl21 = static_cast<const PxlShaderOpenGL21*>(_pxl);
		assert(vtx21 != nullptr, "Error: Trying to link an invalid vertex shader");
		assert(pxl21 != nullptr, "Error: Trying to link an invalid pixel shader");

		// Create an OpenGL program and link the shaders
		Driver3dOpenGL21 * driver = static_cast<Driver3dOpenGL21*>(VideoDriver::getDriver3d());
		GLuint program = driver->glCreateProgram();
		driver->glAttachShader(program, vtx21->id());
		driver->glAttachShader(program, pxl21->id());
		driver->glLinkProgram(program);

		mId = program;
	}

	//------------------------------------------------------------------------------------------------------------------
	ShaderOpenGL21::~ShaderOpenGL21()
	{
		Driver3dOpenGL21 * driver = static_cast<Driver3dOpenGL21*>(VideoDriver::getDriver3d());
		driver->glDeleteProgram(mId);
	}

}	// namespace video
}	// namespace rev
