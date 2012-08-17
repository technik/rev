//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 12th, 2012
//----------------------------------------------------------------------------------------------------------------------
// OpenGL 2.0 Shader

#ifndef _REV_VIDEO_TYPES_SHADER_OPENGL20_SHADER_H_
#define _REV_VIDEO_TYPES_SHADER_OPENGL20_SHADER_H_

#include "../shader.h"

namespace rev { namespace video
{
	// Forward declarations
	class Driver3dOpenGL20;
	class PxlShader;
	class VtxShader;

	class ShaderOpenGL20 : public Shader
	{
	public:
		ShaderOpenGL20(Driver3dOpenGL20*, const VtxShader*, const PxlShader*);
		~ShaderOpenGL20();

		unsigned	id() const { return mId; }

	private:
		unsigned	mId;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_TYPES_SHADER_OPENGL20_SHADER_H_
