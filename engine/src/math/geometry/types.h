//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#ifndef _REV_MATH_GEOMETRY_TYPES_H_
#define _REV_MATH_GEOMETRY_TYPES_H_

#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>

namespace rev {
	namespace math {

		struct BBox {
			Vec3f min, max;

			BBox() : min(0.f), max(0.f) {}
			BBox(const Vec3f& _size) : min(_size*-0.5f), max(_size*0.5f) {}
			BBox(const Vec3f& _min, const Vec3f& _max) : min(_min), max(_max) {}

			void merge(const Vec3f& _point);

			Vec3f size() const { return max - min; }
			Vec3f center() const { return (max+min)*0.5f; }
		};

		struct Frustum {
			Frustum(float _aspectRatio, float _fov, float _near, float _far);

			float aspectRatio() const;
			float fov() const;
			float near() const;
			float far() const;
			float centroid() const;

			const Mat44f& projection() const;
			const Vec3f*	vertices() const;

		private:
			float mAspectRatio;
			float mFov;
			float mNear;
			float mFar;
			Vec3f mVertices[8];
			Mat44f mProjection;
		};

} }	// namespace rev::math

#endif // _REV_MATH_GEOMETRY_TYPES_H_