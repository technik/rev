//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 12th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Shader

#ifndef _REV_VIDEO_TYPES_SHADER_SHADER_H_
#define _REV_VIDEO_TYPES_SHADER_SHADER_H_

#include <utility>
#include <revCore/resource/managedResource.h>

namespace rev { namespace video
{
	// Forward declarations
	class PxlShader;
	class VtxShader;

	class ShaderKeyHasher
	{
	public:
		size_t operator()	(const std::pair<VtxShader*,PxlShader*>& _key) const
		{
			return ((reinterpret_cast<size_t>(_key.first)&0xff) | (reinterpret_cast<size_t>(_key.second)<<16));
		}
	};

	class Shader : public FactoryManagedResource<Shader, std::pair<VtxShader*,PxlShader*>, ShaderKeyHasher>
	{
	public:
		virtual ~Shader() = 0;

	public:
		static Shader* factory(const std::pair<VtxShader*,PxlShader*>&);
	};

	// Destructor body
	Shader::~Shader() {}

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_TYPES_SHADER_SHADER_H_
