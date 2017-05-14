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

using namespace rev::math;

namespace rev {
	namespace video {
		struct StaticRenderMesh {

			struct VertexFormat {
				enum class NormalFormat {
					none, ///< Contains no information of normal vectors
					//normal2, ///< Contains normal vectors only, encoded in two components
					normal3, ///< Contains normal vectors only, encoded in 3 components
					//n2t2, ///< Contains normal and tangent vectors, encoded in two components each
				};

				NormalFormat	normals;
				uint16_t		uvChannels;

				size_t stride() const {
					size_t t = sizeof(math::Vec3f); // position size
					if(normals == NormalFormat::normal3)
						t += sizeof(math::Vec3f);
					t += uvChannels * sizeof(math::Vec2f);
					return t;
				}
			};

			GLuint			vbo = 0; ///< vertex data
			GLuint			vao = 0; ///< vertex array object
			const uint16_t*	indices = nullptr;
			uint16_t		nIndices = 0;

		public:
			StaticRenderMesh(VertexFormat _format, uint16_t _nVertices, const void* _vertexData, uint16_t _nIndices, const uint16_t* _indices) 
				: indices(_indices)
				, nIndices(_nIndices)
			{
				glGenBuffers(1,&vbo);
				glGenVertexArrays(1, &vao);

				glBindVertexArray(vao);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);

				GLsizei stride = (GLsizei)_format.stride();
				glBufferData(GL_ARRAY_BUFFER, _nVertices * stride, _vertexData, GL_STATIC_DRAW);
				// Bind vertex attribs
				size_t bindOffset = 0;
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)bindOffset);
				glEnableVertexAttribArray(0);
				bindOffset += sizeof(Vec3f);
				// Only 3 component normals are currently supported
				assert(_format.normals == VertexFormat::NormalFormat::none
					|| _format.normals == VertexFormat::NormalFormat::normal3);
				if (_format.normals == VertexFormat::NormalFormat::normal3) {
					glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)bindOffset);
					glEnableVertexAttribArray(1);
					bindOffset += sizeof(Vec3f);
				}
				for (auto i = 0; i <_format.uvChannels; ++i) {
					glVertexAttribPointer(2+i, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)bindOffset);
					glEnableVertexAttribArray(2+i);
					bindOffset += sizeof(Vec2f);
				}
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