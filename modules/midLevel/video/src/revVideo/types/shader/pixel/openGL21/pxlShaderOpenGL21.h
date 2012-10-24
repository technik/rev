//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 17th, 2012
//----------------------------------------------------------------------------------------------------------------------
// OpenGL 2.1 Pixel Shader

#ifndef _REV_VIDEO_TYPES_SHADER_PIXEL_OPENGL21_PXLSHADEROPENGL21_H_
#define _REV_VIDEO_TYPES_SHADER_PIXEL_OPENGL21_PXLSHADEROPENGL21_H_

#include "../pxlShader.h"

namespace rev { class File; }

namespace rev { namespace video
{
	class PxlShaderOpenGL21 : public PxlShader
	{
	public:
		PxlShaderOpenGL21(unsigned _id);
		unsigned id() const { return mId; }

	private:
		unsigned mId;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_TYPES_SHADER_PIXEL_OPENGL21_PXLSHADEROPENGL21_H_
