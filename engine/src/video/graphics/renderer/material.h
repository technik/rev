//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Generic material
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_MATERIAL_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_MATERIAL_H_

#include <video/basicTypes/color.h>

namespace rev {
	namespace video {

		class Texture;

		class Material {
		public:
			Texture* mDiffMap;
			Color mDiffuse;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_MATERIAL_H_