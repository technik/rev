//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Generic material
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_MATERIAL_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_MATERIAL_H_

#include <video/graphics/shader/shader.h>

namespace rev {
	namespace video {

		class Material {
		public:
			Shader::Ptr shader;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_MATERIAL_H_