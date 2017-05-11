//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/16
//----------------------------------------------------------------------------------------------------------------------
// Basic procedurally generated meshes
#ifndef _REV_GAME_GEOMETRY_PROCEDURAL_BASIC_H_
#define _REV_GAME_GEOMETRY_PROCEDURAL_BASIC_H_

#include <functional>

#include <math/algebra/vector.h>
#include <video/graphics/staticRenderMesh.h>

namespace rev {
	namespace game {

		class Procedural {
		public:
			static video::StaticRenderMesh* plane(const math::Vec2f& _size);
			static video::StaticRenderMesh* box(const math::Vec3f& _size);
			static video::StaticRenderMesh* geoSphere(float _radius, uint16_t _nMeridians, uint16_t _nParallels);
			///\Parameters:
			// - n: number of divisions along y axis
			// - m: number of divisions along x axis
			// - height: function to compute the height at a given index
			static video::StaticRenderMesh* heightField(uint16_t _n, uint16_t _m,
			 	const math::Vec2f& _size, std::function<float(const math::Vec2u& _idx)> _height);

		};
	}
}

#endif // _REV_GAME_GEOMETRY_PROCEDURAL_BASIC_H_