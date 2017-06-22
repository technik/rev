//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Generic material
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_MATERIAL_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_MATERIAL_H_

#include "effect.h"
#include <math/algebra/vector.h>
#include <string>
#include <vector>
#include <video/graphics/shader/shader.h>

namespace rev {
	namespace video {

		class Texture;

		class Material {
		public:
			static Material* loadFromFile(const std::string& _fileName);

			template<typename T_>
			using Param = std::pair<std::string, T_>;

			Effect*							mEffect;
			std::vector<Param<Texture*>>	mMaps;
			std::vector<Param<float>>		mFloats;
			std::vector<Param<math::Vec3f>>	mVec3s;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_MATERIAL_H_