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

			BBox() : min(Vec3f::zero()), max(Vec3f::zero()) {}
			BBox(const Vec3f& _size) : min(_size*-0.5f), max(_size*0.5f) {}
			BBox(const Vec3f& _min, const Vec3f& _max) : min(_min), max(_max) {}

			void merge(const Vec3f& _point) {
				max = this->max.cwiseMax(_point);
				min = this->min.cwiseMin(_point);
			}

			Vec3f size() const { return max - min; }
			float radius() const { return size().norm(); }
			Vec3f center() const { return (max+min)*0.5f; }
		};

		struct Frustum {
			Frustum() = default; // Allow empty construction
			Frustum(float _aspectRatio, float _fov, float _near, float _far) 
				:mAspectRatio(_aspectRatio)
				,mFov(_fov)
				,mNear(_near)
				,mFar(_far)
			{
				mProjection = frustrumMatrix(_fov, _aspectRatio, _near, _far);

				float tangent = tan(_fov / 2.f);
				Vec3f minVert = Vec3f(tangent, 1.f, tangent/_aspectRatio)*_near;
				Vec3f maxVert = Vec3f(tangent, 1.f, tangent/_aspectRatio)*_far;

				mVertices[0] = minVert;
				mVertices[1] = Vec3f(-minVert.x(), minVert.y(), minVert.z());
				mVertices[2] = Vec3f(-minVert.x(), minVert.y(), -minVert.z());
				mVertices[3] = Vec3f(minVert.x(), minVert.y(), -minVert.z());
				mVertices[4] = maxVert;
				mVertices[5] = Vec3f(-maxVert.x(), maxVert.y(), maxVert.z());
				mVertices[6] = Vec3f(-maxVert.x(), maxVert.y(), -maxVert.z());
				mVertices[7] = Vec3f(maxVert.x(), maxVert.y(), -maxVert.z());
			}

			Frustum(const Vec3f& size) // Rectangular
				: mAspectRatio(size.x()/size.y())
				, mFov(0.f)
				, mNear(-0.5f*size.y())
				, mFar(-0.5f*size.y())
			{
				math::Vec3f hSize = size*0.5f;
				math::Vec3f minVert = math::Vec3f(hSize.x(), -hSize.y(), hSize.z());
				math::Vec3f maxVert = math::Vec3f(hSize.x(), hSize.y(), hSize.z());

				mVertices[0] = minVert;
				mVertices[1] = Vec3f(-minVert.x(), minVert.y(), minVert.z());
				mVertices[2] = Vec3f(-minVert.x(), minVert.y(), -minVert.z());
				mVertices[3] = Vec3f(minVert.x(), minVert.y(), -minVert.z());
				mVertices[4] = maxVert;
				mVertices[5] = Vec3f(-maxVert.x(), maxVert.y(), maxVert.z());
				mVertices[6] = Vec3f(-maxVert.x(), maxVert.y(), -maxVert.z());
				mVertices[7] = Vec3f(maxVert.x(), maxVert.y(), -maxVert.z());

				mProjection = orthographicMatrix({size.x(), size.y()}, mNear, mFar);
			}

			float aspectRatio	() const { return mAspectRatio; }
			float fov			() const { return mFov; }
			float nearPlane		() const { return mNear; }
			float farPlane		() const { return mFar; }
			float centroid		() const { return 0.5f*(mNear+mFar); }

			const Mat44f& projection() const { return mProjection; }
			const Vec3f*	vertices() const { return mVertices; }

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