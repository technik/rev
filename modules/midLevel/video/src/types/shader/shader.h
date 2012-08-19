//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 12th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Shader

#ifndef _REV_VIDEO_TYPES_SHADER_SHADER_H_
#define _REV_VIDEO_TYPES_SHADER_SHADER_H_

#include <utility>
#include <resource/managedResource.h>

namespace rev { namespace video
{
	// Forward declarations
	class PxlShader;
	class VtxShader;

	class Shader : public ManagedResource<Shader, std::pair<VtxShader*,PxlShader*>, true>
	{
	public:
		virtual ~Shader() = 0 {};

	public:
		static Shader* factory(const std::pair<VtxShader*,PxlShader*>&);
	};

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_TYPES_SHADER_SHADER_H_
