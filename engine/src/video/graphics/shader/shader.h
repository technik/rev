//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Generic shader interface
#ifndef _REV_VIDEO_GRAPHICS_SHADER_SHADER_H_
#define _REV_VIDEO_GRAPHICS_SHADER_SHADER_H_

#include <core/resources/creator/virtualConstructor.h>
#include <core/resources/ownership/refLink.h>
#include <core/resources/resourceManager.h>
#include <core/types.h>

namespace rev {
	namespace video {

		class Shader
			: public core::ManagedResource<core::string, Shader, 
				core::VirtualConstructor<Shader,core::string>, core::RefLink>{
		public:
			virtual ~Shader() = default;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_SHADER_SHADER_H_