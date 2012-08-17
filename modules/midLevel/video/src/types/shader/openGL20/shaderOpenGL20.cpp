//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 12th, 2012
//----------------------------------------------------------------------------------------------------------------------
// OpenGL 2.0 Shader

#include "shaderOpenGL20.h"

#include <codeTools/assert/assert.h>
#include <driver3d/openGL20/driver3dOpenGL20.h>
#include <types/shader/pixel/openGL20/pxlShaderOpenGL20.h>
#include <types/shader/vertex/openGL20/vtxShaderOpenGL20.h>

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	ShaderOpenGL20::ShaderOpenGL20(Driver3dOpenGL20* _driver, const VtxShader * _vtx, const PxlShader * _pxl)
		:Shader(_vtx,_pxl)
	{
		// Cast to the openGL version of the shaders
		const VtxShaderOpenGL20 * vtx20 = static_cast<const VtxShaderOpenGL20*>(_vtx);
		const PxlShaderOpenGL20 * pxl20 = static_cast<const PxlShaderOpenGL20*>(_pxl);
		assert(vtx20 != nullptr, "Error: Trying to link an invalid vertex shader");
		assert(pxl20 != nullptr, "Error: Trying to link an invalid pixel shader");

		// Create an OpenGL program and link the shaders
		GLuint program = _driver->glCreateProgram();
		_driver->glAttachShader(program, vtx20->id());
		_driver->glAttachShader(program, pxl20->id());
		_driver->glLinkProgram(program);

		mId = program;
	}

}	// namespace video
}	// namespace rev
