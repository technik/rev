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
			GLuint			vbo = 0; ///< vertex data
			GLuint			vao = 0; ///< vertex array object
			const uint16_t*	indices = nullptr;
			uint16_t		nIndices = 0;

		public:
			StaticRenderMesh(uint16_t _nVertices, const math::Vec3f* _vertexData, uint16_t _nIndices, const uint16_t* _indices) 
				: indices(_indices)
				, nIndices(_nIndices)
			{
				glGenBuffers(1,&vbo);
				glGenVertexArrays(1, &vao);

				glBindVertexArray(vao);

				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, _nVertices * sizeof(math::Vec3f), _vertexData, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
				glEnableVertexAttribArray(0);

				glBindVertexArray(0);
			}

			~StaticRenderMesh() {
				if(vbo)
					glDeleteBuffers(1,&vbo);
				if(vao)
					glDeleteVertexArrays(1, &vao);
				if(indices)
					delete[] indices;
			}
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_STATICRENDERMESH_H_