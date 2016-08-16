//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#ifndef _REV_MATH_GEOMETRY_TYPES_H_
#define _REV_MATH_GEOMETRY_TYPES_H_

#include <math/algebra/vector.h>

namespace rev {
	namespace math {

		struct BBox {
			Vec3f min, max;

			BBox() : min(0.f), max(0.f) {}
			BBox(const Vec3f& _size) : min(_size*-0.5f), max(_size*0.5f) {}
			BBox(const Vec3f& _min, const Vec3f& _max) : min(_min), max(_max) {}

			Vec3f size() const { return max - min; }
		};

} }	// namespace rev::math

#endif // _REV_MATH_GEOMETRY_TYPES_H_