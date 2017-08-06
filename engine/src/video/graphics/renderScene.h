//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Struct containing all information from a scene that is relevant for render (objects, lights, etc)
#ifndef _REV_VIDEO_GRAPHICS_RENDERSCENE_H_
#define _REV_VIDEO_GRAPHICS_RENDERSCENE_H_

#include <vector>
#include <math/algebra/vector.h>
#include <video/basicTypes/camera.h>

namespace rev {
	namespace video {

		class RenderObj;

		struct RenderScene {
			std::vector<RenderObj*>	objects;
			math::Vec3f lightDir;
			math::Vec3f lightClr;
			Camera* camera;
		};
		
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERSCENE_H_
