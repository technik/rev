//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 17th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Pixel Shader

#ifndef _REV_VIDEO_TYPES_SHADER_PIXEL_PXLSHADER_H_
#define _REV_VIDEO_TYPES_SHADER_PIXEL_PXLSHADER_H_

#include <string>

#include <resource/managedResource.h>

namespace rev { namespace video
{
	class PxlShader : public FactoryManagedResource<PxlShader,std::string>
	{
	public:
		static PxlShader* factory(const std::string&);
	};

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_TYPES_SHADER_PIXEL_PXLSHADER_H_
