//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 18th, 2012
//----------------------------------------------------------------------------------------------------------------------
// OpenGL 2.1 Pixel Shader

#include "vtxShaderOpenGL21.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	VtxShaderOpenGL21::VtxShaderOpenGL21(unsigned _id)
		:mId(_id)
		,mTopAttributeId(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void VtxShaderOpenGL21::addAttribute(const char* _attribute)
	{
		mAttributes[std::string(_attribute)] = mTopAttributeId;
		++mTopAttributeId;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned VtxShaderOpenGL21::getAttributeId(const char* _attribute) const
	{
		auto iter = mAttributes.find(std::string(_attribute));
		if(iter != mAttributes.end())
		{
			return iter->second;
		}
		return unsigned(-1);
	}

}	// namespace video
}	// namespace rev