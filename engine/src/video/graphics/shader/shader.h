//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Generic shader interface
#ifndef _REV_VIDEO_GRAPHICS_SHADER_SHADER_H_
#define _REV_VIDEO_GRAPHICS_SHADER_SHADER_H_

#include <core/containers/resourceManager.h>
#include <core/types.h>

namespace rev {
	namespace video {

		class ShaderCreatorPolicy {

		};

		class Shader : core::ManagedResource<core::string, Shader, ShaderCreatorPolicy, ReferenceLink>{
		public:
			virtual ~Shader() = default;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_SHADER_SHADER_H_