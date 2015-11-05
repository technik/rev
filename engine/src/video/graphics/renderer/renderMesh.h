//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Base renderable object
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_RENDERMESH_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_RENDERMESH_H_

#include <cstdint>
#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>

namespace rev {
	namespace video {

		class RenderMesh {
		public:
			math::Vec3f*	vertices = nullptr;
			math::Vec3f*	normals = nullptr;
			math::Vec2f*	uvs = nullptr;
			uint16_t		nVertices = 0;
			uint16_t		nIndices = 0;
			uint16_t*		triStrip = nullptr;
			uint16_t		stripLength = 0;
			uint16_t*		indices = nullptr;

			math::Vec3f bbMin, bbMax; // Bounding box

			//----------------------------------------------------------------------------------------------------------
			void setVertexData(uint16_t _nVertices, math::Vec3f* _vertices
				, math::Vec3f* _normals, math::Vec2f* _uvs, const math::Vec3f& _min, const math::Vec3f& _max) {
				vertices = _vertices;
				normals = _normals;
				uvs = _uvs;
				nVertices = _nVertices;
				bbMin = _min;
				bbMax = _max;
			}

			//----------------------------------------------------------------------------------------------------------
			void setVertexData(uint16_t _nVertices, math::Vec3f* _vertices
				, math::Vec3f* _normals, math::Vec2f* _uvs) {
				if(!_vertices)
					return;
				math::Vec3f lmin = _vertices[0];
				math::Vec3f lmax = _vertices[0];
				for (unsigned i = 1; i < _nVertices; ++i) {
					math::Vec3f& v = _vertices[i];
					lmin = min(lmin, v);
					lmax = max(lmax, v);
				}
				setVertexData(_nVertices, _vertices, _normals, _uvs, lmin, lmax);
			}

			//----------------------------------------------------------------------------------------------------------
			void setFaceIndices(uint16_t _nIndices, uint16_t* _indices, bool _strip = false) {
				if (_strip) {
					stripLength = _nIndices;
					triStrip = _indices;
				}
				else {
					nIndices = _nIndices;
					indices = _indices;
				}
			}

		};
	}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_GRAPHICS_RENDERER_RENDERMESH_H_