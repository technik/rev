//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 12th, 2012
//----------------------------------------------------------------------------------------------------------------------
// OpenGL 2.0 Shader

#ifndef _REV_VIDEO_TYPES_SHADER_OPENGL21_SHADEROPENGL21_H_
#define _REV_VIDEO_TYPES_SHADER_OPENGL21_SHADEROPENGL21_H_

#include "../shader.h"

namespace rev { namespace video
{
	// Forward declarations
	class Driver3dOpenGL21;
	class PxlShader;
	class VtxShader;
	class VtxShaderOpenGL21;

	class ShaderOpenGL21 : public Shader
	{
	public:
		ShaderOpenGL21(unsigned _id, const VtxShaderOpenGL21* _vtxShader);
		~ShaderOpenGL21();

		unsigned					id					() const;
		int							getUniformLocation	(const char* _uniform) const;
		const VtxShaderOpenGL21*	vtxShader			() const;

	private:
		unsigned					mId;
		const VtxShaderOpenGL21*	mVtxShader;
	};

	//-----------------------------------------------------------------------------------
	// Inline implementations
	//-----------------------------------------------------------------------------------
	inline unsigned ShaderOpenGL21::id() const
	{
		return mId;
	}

	
	//-----------------------------------------------------------------------------------
	inline const VtxShaderOpenGL21* ShaderOpenGL21::vtxShader() const
	{
		return mVtxShader;
	}

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_TYPES_SHADER_OPENGL21_SHADEROPENGL21_H_
