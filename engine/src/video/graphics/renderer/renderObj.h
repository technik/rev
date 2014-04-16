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
			math::Vec3f*	vertices = nullptr;
			math::Vec3f*	normals = nullptr;
			math::Vec2f*	uvs = nullptr;
			uint16_t		nVertices = 0;
			uint16_t		nIndices = 0;
			uint16_t*		triStrip = nullptr;
			uint16_t		stripLength = 0;
			uint16_t*		indices = nullptr;
			math::Mat34f	transform;

			//----------------------------------------------------------------------------------------------------------
			void setVertexData(uint16_t _nVertices, math::Vec3f* _vertices
				, math::Vec3f* _normals, math::Vec2f* _uvs) {
				vertices = _vertices;
				normals = _normals;
				uvs = _uvs;
				nVertices = _nVertices;
			}

			//----------------------------------------------------------------------------------------------------------
			void setFaceIndices(uint16_t _nIndices, uint16_t* _indices, bool _strip = false) {
				if (_strip) {
					stripLength = _nIndices;
					triStrip = _indices;
				} else {
					nIndices = _nIndices;
					indices = _indices;
				}
			}

		};
	}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_GRAPHICS_RENDERER_RENDEROBJ_H_