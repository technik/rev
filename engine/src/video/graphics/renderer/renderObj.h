//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/13
//----------------------------------------------------------------------------------------------------------------------
// Base renderable object
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_RENDEROBJ_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_RENDEROBJ_H_

#include <cstdint>
#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>

namespace rev {
	namespace video {

		class RenderObj {
		public:
			math::Vec3f*	vertices;
			math::Vec3f*	normals;
			math::Vec2f*	uvs;
			uint16_t		nVertices;
			uint16_t		nIndices;
			uint16_t*		indices;
			math::Mat34f	transform;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_RENDEROBJ_H_