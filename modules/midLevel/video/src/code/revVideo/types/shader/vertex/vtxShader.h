//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 17th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Vertex Shader

#ifndef _REV_VIDEO_TYPES_SHADER_VERTEX_VTXSHADER_H_
#define _REV_VIDEO_TYPES_SHADER_VERTEX_VTXSHADER_H_

#include <string>

#include <revCore/resource/managedResource.h>

namespace rev { namespace video
{
	class VtxShader : public FactoryManagedResource<VtxShader, std::string>
	{
	public:
		static VtxShader * factory(const std::string&);
	};

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_TYPES_SHADER_VERTEX_VTXSHADER_H_
