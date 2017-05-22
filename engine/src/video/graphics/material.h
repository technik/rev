//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Generic material
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_MATERIAL_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_MATERIAL_H_

#include <video/graphics/shader/shader.h>
#include <string>

namespace rev {
	namespace video {

		class Texture;

		class Material {
		public:
			static Material* loadFromFile(const std::string& _fileName);
			Shader::Ptr shader;

			Texture* albedo;
			Texture* physics;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_MATERIAL_H_