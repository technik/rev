//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 17th, 2012
//----------------------------------------------------------------------------------------------------------------------
// OpenGL 2.1 Vertex Shader

#ifndef _REV_VIDEO_TYPES_SHADER_VERTEX_OPENGL21_VTXSHADEROPENGL21_H_
#define _REV_VIDEO_TYPES_SHADER_VERTEX_OPENGL21_VTXSHADEROPENGL21_H_

#include "../vtxShader.h"

namespace rev { namespace video
{
	class VtxShaderOpenGL21 : public VtxShader
	{
	public:
		VtxShaderOpenGL21(unsigned _id);
		unsigned id() const { return mId; }

	private:
		unsigned mId;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_TYPES_SHADER_VERTEX_OPENGL21_VTXSHADEROPENGL21_H_
