//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#ifndef _REV_VIDEO_GRAPHICS_STATICRENDERMESH_H_
#define _REV_VIDEO_GRAPHICS_STATICRENDERMESH_H_

#include <cstdint>
#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>
#include <video/graphics/driver/graphicsDriver.h>

namespace rev {
	namespace video {
		struct StaticRenderMesh {
			GLuint			vbo; ///< vertex data
			uint16_t*		indices = nullptr;
			uint16_t		nIndices = 0;

		public:
			StaticRenderMesh(uint16_t nVertices, const math::Vec3f*	vertexData, uint16_t nIndices, const uint16_t* indices) {
				glGenBuffers(1,&vbo);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, nVertices*sizeof(math::Vec3f), vertexData, GL_STATIC_DRAW);
			}

			~StaticRenderMesh() {
				if(vbo)
					glDeleteBuffers(1,&vbo);
				if(indices)
					delete[] indices;
			}
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_STATICRENDERMESH_H_