//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 17th, 2012
//----------------------------------------------------------------------------------------------------------------------
// OpenGL 2.1 Vertex Shader

#ifndef _REV_VIDEO_TYPES_SHADER_VERTEX_OPENGL21_VTXSHADEROPENGL21_H_
#define _REV_VIDEO_TYPES_SHADER_VERTEX_OPENGL21_VTXSHADEROPENGL21_H_

#include "../vtxShader.h"
#include <map>
#include <string>

namespace rev { namespace video
{
	class VtxShaderOpenGL21 : public VtxShader
	{
	public:
		VtxShaderOpenGL21(unsigned _id);

		void									addAttribute	(const char* _attribute);
		const std::map<std::string, unsigned>&	attributes		() const;
		unsigned								getAttributeId	(const char* _attribute) const;

		unsigned id() const;

	private:
		std::map<std::string, unsigned>	mAttributes;
		unsigned mTopAttributeId;
		unsigned mId;
	};

	//-----------------------------------------------------------------------------------
	inline unsigned VtxShaderOpenGL21::id() const
	{ 
		return mId;
	}

	//-----------------------------------------------------------------------------------
	inline const std::map<std::string, unsigned>& VtxShaderOpenGL21::attributes() const
	{
		return mAttributes;
	}

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_TYPES_SHADER_VERTEX_OPENGL21_VTXSHADEROPENGL21_H_
