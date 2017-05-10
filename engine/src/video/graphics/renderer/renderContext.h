//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Struct containing information for a specific render set up
#ifndef _REV_VIDEO_GRAPHICS_RENDERSCENE_H_
#define _REV_VIDEO_GRAPHICS_RENDERSCENE_H_

namespace rev {
	namespace video {

		class Camera;

		struct RenderContext {
			Camera* cam;
		};

	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERSCENE_H_
